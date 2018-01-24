/****************************************************************************
 **
 ** Copyright (C) 2017, 2018 The Qst project.
 **
 ** Contact: https://github.com/rweickelt/qst
 **
 ** $BEGIN_LICENSE$
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.

 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.

 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **
 ** $END_LICENSE$
****************************************************************************/

#include <QtCore/QGlobalStatic>
#include <QtCore/QEventLoop>

#include "console.h"
#include "rochostcontroller.h"
#include "qst.h"

namespace {
    QMap<QString, QPointer<RocHostController> > controllers;
}

RocHostController::RocHostController(const QString& port) : QObject()
{
    m_connected = false;
    m_currentObject = NULL;
    m_port = port;
    m_unansweredPings = 0;

    QObject::connect(
                &m_socket, &StpSocket::readyRead,
                this, &RocHostController::onReadyRead
    );

    QObject::connect(
                &m_pingTimer, &QTimer::timeout,
                this, &RocHostController::onPingTimerTick);
    m_pingTimer.setSingleShot(false);
}


RocHostController* RocHostController::instance(const QString& port)
{
    Q_UNUSED(port);
    if (!controllers.contains(port))
    {
        controllers.insert(port, QPointer<RocHostController>(new RocHostController(port)));
    }
    RocHostController* host = controllers.value(port).data();
    return host;
}

quint32 RocHostController::registerObject(RocHostObject* object)
{
    Q_ASSERT(object != NULL);

    if (!isConnected() && !m_socket.connectToTarget(m_port))
    {
        QST_ERROR_AND_EXIT(m_socket.errorString());
    }

    for(int retries = 2; (!isConnected() && (retries > 0)); retries--)
    {
        roc::MessageHeader resetMessage;
        resetMessage.type = roc::Reset;
        resetMessage.payloadLength = 0;
        m_socket.sendMessage(QByteArray(reinterpret_cast<char*>(&resetMessage), sizeof(roc::MessageHeader)));
        if (waitForReply(900))
        {
            setConnected(true);
        }
    }
    if (!isConnected())
    {
        QST_ERROR_AND_EXIT("Target does not respond.");
    }

    m_currentObject = object;
    QByteArray payloadBuffer(reinterpret_cast<char*>(object), sizeof(object));
    QByteArray headerBuffer(sizeof(roc::MessageHeader), Qt::Uninitialized);
    roc::MessageHeader* request = reinterpret_cast<roc::MessageHeader*>(headerBuffer.data());
    request->type = roc::Construct;
    request->classId = object->classId();
    request->payloadLength = payloadBuffer.length();
    m_socket.sendMessage(headerBuffer + payloadBuffer);

    if (!waitForReply(900))
    {
        QST_ERROR_AND_EXIT("Target does not respond.");
    }
    Q_ASSERT(m_ids.contains(object));
    return m_ids.value(object, 0);
}


void RocHostController::unregisterObject(const RocHostObject* object)
{
    roc::MessageHeader request;
    request.type = roc::Destruct;
    request.classId = object->classId();
    request.payloadLength = 0;

    m_socket.sendMessage(QByteArray(reinterpret_cast<char*>(&request), sizeof(roc::MessageHeader)));

    m_objects.remove(object->targetId());
    m_ids.remove(object);
}

void RocHostController::sendMessage(const QByteArray &message)
{
    Q_ASSERT(isConnected());
    m_socket.sendMessage(message);
}

void RocHostController::sendMessage(RocHostObject* object, quint8 methodId, const QByteArray& data, roc::MessageType type)
{
    Q_ASSERT(isConnected());
    Q_ASSERT(m_ids.contains(object));

    roc::MessageHeader rocHeader;
    rocHeader.objectId = m_ids.value(object);
    rocHeader.methodId = methodId;
    rocHeader.payloadLength = data.length();
    rocHeader.type = type;

    QByteArray rocMessage(reinterpret_cast<char*>(&rocHeader), sizeof(roc::MessageHeader));
    rocMessage.append(data);

    m_socket.sendMessage(rocMessage);
}

void RocHostController::onReadyRead()
{
    while (m_socket.messagesAvailable())
    {
        QByteArray message = m_socket.readNextMessage();
        roc::MessageHeader header;
        memcpy(&header, message.data(), sizeof(roc::MessageHeader));
        Q_ASSERT(header.payloadLength == (message.length() - sizeof(roc::MessageHeader)));
        message.remove(0, sizeof(roc::MessageHeader));
        if (header.type == roc::CallMethod)
        {
            QByteArray& data = message;
            RocHostObject* object = m_objects.value(header.objectId, NULL);
            Q_ASSERT_X(object != NULL, "MpiHost", qPrintable(QString("No object found for %1").arg(header.objectId)));
            object->processFromTarget(header.methodId, data);
        }
        else if (header.type == roc::ObjectId)
        {
            m_objects.insert(header.objectId, m_currentObject);
            m_ids.insert(m_currentObject, header.objectId);
            m_currentObject->m_targetId = header.objectId;
            m_currentObject = NULL;
            m_loop.exit(0);
        }
        else if (header.type == roc::StateMessage)
        {
            if (header.state == roc::Ready)
            {
                m_loop.exit(0);
            }
        }
        else if (header.type == roc::Pong)
        {
            Q_ASSERT(m_unansweredPings > 0);
            m_unansweredPings--;
        }
    }
}

bool RocHostController::waitForReply(int milliseconds)
{
    QTimer timer;
    timer.connect(&timer, &QTimer::timeout,
                  [=]() { m_loop.exit(1); });
    timer.setSingleShot(true);
    timer.start(milliseconds);
    return (m_loop.exec() == 0);
}

void RocHostController::setConnected(bool connected)
{
    Q_ASSERT(connected != m_connected);
    if (connected)
    {
        m_pingTimer.start(517);
    }
    else
    {
        m_pingTimer.stop();
    }
    m_connected = true;
}

bool RocHostController::isConnected() const
{
    return m_connected;
}

void RocHostController::onPingTimerTick()
{
    Q_ASSERT(isConnected());

    if (m_unansweredPings > 2)
    {
        QST_ERROR_AND_EXIT("Connection to probe lost");
    }

    roc::MessageHeader header;
    header.objectId = 0;
    header.methodId = 0;
    header.payloadLength = 0;
    header.type = roc::Ping;

    QByteArray message(reinterpret_cast<char*>(&header), sizeof(roc::MessageHeader));
    m_unansweredPings++;
    m_socket.sendMessage(message);
}


