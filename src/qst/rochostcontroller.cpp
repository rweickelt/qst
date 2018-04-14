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

#include <unistd.h>

namespace {
    QMap<QString, QPointer<RocHostController> > controllers;
}

RocHostController::RocHostController(const QString& port) : QObject()
{
    m_state = Disconnected;
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
    m_pingTimer.setTimerType(Qt::PreciseTimer);
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

    if (m_state == Disconnected)
    {
        if (m_socket.connectToTarget(m_port))
        {
            m_state = Standby;
        }
        else
        {
            QST_ERROR_AND_EXIT(m_socket.errorString());
        }
    }

    if (m_state == Standby)
    {
        for(int retries = 2; ((m_state != Connected) && (retries > 0)); retries--)
        {
            if (retries > 2)
            {
                qDebug() << "Retry connect " << m_port;
            }
            bool success = sendMessageBlocking(roc::Connect, (uint32_t)0, QByteArray(), 900);
            if (success)
            {
                m_state = Connected;
                m_pingTimer.start(ConnectionTimeoutIntervalMs);
            }
        }
        if (m_state != Connected)
        {
            QST_ERROR_AND_EXIT("Target does not respond.");
        }
    }

    m_currentObject = object;
    bool success = sendMessageBlocking(roc::Construct, object->classId(), QByteArray(), 900);
    if (!success)
    {
        QST_ERROR_AND_EXIT("Target does not respond.");
    }
    Q_ASSERT(m_ids.contains(object));
    return m_ids.value(object, 0);
}


void RocHostController::unregisterObject(const RocHostObject* object)
{
    Q_ASSERT(m_objects.contains(object->targetId()));

    bool success = sendMessageBlocking(roc::Destruct, object->targetId(), QByteArray(), 500);
    if (!success)
    {
        QST_ERROR_AND_EXIT("Target does not respond.");
    }

    m_objects.remove(object->targetId());
    m_ids.remove(object);

    if (m_objects.size() == 0)
    {
        m_pingTimer.stop();
        m_socket.disconnectFromTarget();
        m_state = Disconnected;
    }
}


bool RocHostController::sendMessageBlocking(
        uint8_t messageType, RocHostObject* object, const QByteArray& data, int milliseconds)
{
    Q_ASSERT(m_ids.contains(object));
    return sendMessageBlocking(messageType, object->targetId(), data, milliseconds);
}

bool RocHostController::sendMessageBlocking(
        uint8_t messageType, uint32_t objectId, const QByteArray& data, int milliseconds)
{
    Q_ASSERT((m_state == Standby) || (m_state == Connected));
    Q_ASSERT(data.length() <= UINT16_MAX);

//    qDebug() << "Send blocking " << messageType << " port " << m_port;
    roc::MessageHeader header;
    header.type = messageType;
    header.objectId = objectId;
    header.payloadLength = data.length();
    header.transactionId = roc::Blocking;

    QByteArray message(reinterpret_cast<const char*>(&header), sizeof(roc::MessageHeader));
    message.append(data);
    m_socket.sendMessage(message, stp::RocMessage);

    QTimer timer;
    timer.connect(&timer, &QTimer::timeout,
                  [=]() { m_loop.exit(1); });
    timer.setSingleShot(true);
    timer.start(milliseconds);
    return (m_loop.exec() == 0);
}

void RocHostController::sendMessageNonBlocking(uint8_t messageType, uint32_t objectId, const QByteArray& data)
{
    Q_ASSERT((m_state == Standby) || (m_state == Connected));
    Q_ASSERT(data.length() <= UINT16_MAX);

//    qDebug() << "Send non-blocking " << messageType << " port " << m_port;

    roc::MessageHeader header;
    header.type = messageType;
    header.objectId = objectId;
    header.payloadLength = data.length();
    header.transactionId = roc::NonBlocking;

    QByteArray message(reinterpret_cast<const char*>(&header), sizeof(roc::MessageHeader));
    message.append(data);
    m_socket.sendMessage(message, stp::RocMessage);
}

void RocHostController::onReadyRead()
{
    while (m_socket.messagesAvailable())
    {
        QByteArray message = m_socket.readNextMessage();
        if (message.length() == 0)
        {
            // Ignore stp-only messages that should not be passed through
            // from stpsocket. This nifty error should be fixed one day in
            // StpSocket.
            continue;
        }
        roc::MessageHeader header;
        memcpy(&header, message.constData(), sizeof(roc::MessageHeader));
        Q_ASSERT(header.payloadLength == (message.length() - sizeof(roc::MessageHeader)));

        if (header.type != roc::Pong) {
//        qDebug() << "Receiving " << header.type << " port " << m_port;
        }

        // There might be a blocking request
        if (m_loop.isRunning() && (header.transactionId == roc::Blocking))
        {
            m_loop.exit(0);
        }

        if (header.type == roc::ObjectId)
        {
            m_objects.insert(header.objectId, m_currentObject);
            m_ids.insert(m_currentObject, header.objectId);
            m_currentObject->m_targetId = header.objectId;
            m_currentObject = NULL;
        }
        else if (header.type == roc::StatusOk)
        {
        }
        else if (header.type == roc::StatusError)
        {
            QST_ERROR_AND_EXIT("Unexpected error happened on target.");
        }
        else if (header.type == roc::Pong)
        {
            Q_ASSERT(m_unansweredPings > 0);
            m_unansweredPings--;
        }
        else
        {
            RocHostObject* object = m_objects.value(header.objectId, nullptr);
            Q_ASSERT_X(object != nullptr, "RocHostController", qPrintable(QString("No object found for %1").arg(header.objectId)));
            QByteArray& data = message.remove(0, sizeof(roc::MessageHeader));
            object->processFromTarget(header.type, data);
        }
    }
}

void RocHostController::onPingTimerTick()
{
    Q_ASSERT(m_state == Connected);

    if (m_unansweredPings > 2)
    {
        QST_ERROR_AND_EXIT("Connection to probe lost");
    }

    m_unansweredPings++;
    sendMessageNonBlocking(roc::Ping);
}

