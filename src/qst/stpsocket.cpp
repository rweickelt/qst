/****************************************************************************
 **
 ** Copyright (C) 2017-2018 The Qst project.
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

#include "stpsocket.h"
#include <protocols/stp.h>
#include <protocols/roc.h>

#include <QtCore/QThread>


StpSocket::StpSocket()
{
    m_rxState = RxDelimiter;
    m_rxBytesNeeded = 1;
    m_rxBuffer.reserve(1024);
    m_connected = false;

    connect(&m_port, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, &StpSocket::onUartError);

}

StpSocket::~StpSocket()
{
    m_port.close();
}


bool StpSocket::connectToTarget(const QString& port)
{
    m_port.setPortName(port);
    m_port.setBaudRate(115200);

    if (!m_port.open(QIODevice::ReadWrite))
    {
        m_errorString = QString("Could not open UART driver at '%1': %2").arg(
                   m_port.portName(),
                   m_port.errorString());
        return false;
    }

    for (quint32 i = 0; i < 3; i++)
    {
        this->sendMessage(QByteArray(), stp::ResetConnection);
        if (this->waitForReadyRead(300))
        {
            QByteArray reply = readNextMessage();
            if (reply.length() == 0)
            {
                m_connected = true;
                break;
            }
        }
    }

    if (!m_connected)
    {
        m_errorString = QString("Probe at port '%1' does not respond.")
                      .arg(m_port.portName());
        m_port.close();
        return false;
    }

    connect(&m_port, &QSerialPort::readyRead, this, &StpSocket::onUartReadyRead);
    return true;
}

void StpSocket::disconnectFromTarget()
{
    QObject::disconnect(&m_port, &QSerialPort::readyRead, this, &StpSocket::onUartReadyRead);
    m_connected = false;
    m_port.close();
}

void StpSocket::onUartReadyRead()
{
    while (m_port.bytesAvailable() >= m_rxBytesNeeded)
    {
        const stp::MessageHeader* header = reinterpret_cast<const stp::MessageHeader*>(m_rxBuffer.constData());
        RxState nextState = RxInvalidState;

        switch(m_rxState)
        {
        case RxDelimiter:
            m_rxBuffer.append(m_port.read(m_rxBytesNeeded));
            if (header->delimiterBytes[m_rxBuffer.length() - 1] != stp::MessageHeader::DelimiterByte) {
                m_rxBuffer.clear();
            }
            if (m_rxBuffer.length() == stp::MessageHeader::DelimiterLength) {
                nextState = RxHeader;
                m_rxBytesNeeded = sizeof(stp::MessageHeader) - stp::MessageHeader::DelimiterLength;
            } else {
                nextState = RxDelimiter;
                m_rxBytesNeeded = 1;
            }
            break;
        case RxHeader:
            m_rxBuffer.append(m_port.read(m_rxBytesNeeded));
            if (header->delimiterWord != stp::MessageHeader::DelimiterWord) {
                nextState = RxDelimiter;
                m_rxBytesNeeded = 1;
            } else {
                nextState = RxPayload;
                m_rxBytesNeeded = header->payloadLength;
            }
            break;
        case RxPayload:
            m_rxBuffer.append(m_port.read(m_rxBytesNeeded));
            // Todo: Check data integrity
            m_rxMessageQueue.append(QByteArray(m_rxBuffer.constData() + sizeof(stp::MessageHeader),
                    header->payloadLength));
            m_rxBuffer.clear();
            m_rxBuffer.reserve(sizeof(stp::MessageHeader));
            nextState = RxHeader;
            m_rxBytesNeeded = sizeof(stp::MessageHeader);
            if (m_connected == true)
            {
                emit readyRead();
            }
            break;
        case RxInvalidState:
            qFatal("RxInvalid state");
            break;
        }
        m_rxState = nextState;
    }
}

bool StpSocket::waitForReadyRead(qint32 milliseconds)
{
    if (m_rxMessageQueue.length() > 0)
    {
        return true;
    }

    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);
    timeoutTimer.setTimerType(Qt::PreciseTimer);
    timeoutTimer.start(milliseconds);

    while (timeoutTimer.isActive() && (m_rxMessageQueue.length() == 0))
    {
        if (m_port.waitForReadyRead(100))
        {
            onUartReadyRead();
        }
        QCoreApplication::processEvents();
    }

    return (m_rxMessageQueue.length() > 0);
}


void StpSocket::onUartError(QSerialPort::SerialPortError error)
{
    if ((error != QSerialPort::NoError) && (error != QSerialPort::TimeoutError))
    {
        m_errorString = QString("In SerialInterface at port '%1': %2")
                      .arg(m_port.portName())
                      .arg(m_port.errorString());
    }
}

void StpSocket::sendMessage(QByteArray message, stp::MessageType type)
{
    stp::MessageHeader header;
    header.type = type;
    header.delimiterWord = stp::MessageHeader::DelimiterWord;
    header.payloadLength = message.length();
    header.checksum = 0x47;

    m_port.write(reinterpret_cast<const char*>(&header), sizeof(stp::MessageHeader));
    m_port.write(reinterpret_cast<const char*>(message.constData()), message.length());
    m_port.flush();
}

QByteArray StpSocket::readNextMessage()
{
    return m_rxMessageQueue.takeFirst();
}
