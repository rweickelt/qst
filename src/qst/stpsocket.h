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
#ifndef SERIALINTERFACE_H
#define SERIALINTERFACE_H

#include <QtCore/QByteArray>
#include <QtCore/QHash>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QSharedPointer>
#include <QtCore/QTimer>
#include <QtSerialPort/QtSerialPort>

#include <protocols/stp.h>

class PinTargetObject;
class MpiHostMessage;

class StpSocket : public QObject
{
    Q_OBJECT

public:
    StpSocket();
    ~StpSocket();

    bool connectToTarget(const QString& port);
    void disconnectFromTarget();

    bool messagesAvailable();
    QByteArray readNextMessage();
    void sendMessage(QByteArray message, stp::MessageType type = stp::RocMessage);
    bool waitForReadyRead(qint32 milliseconds);

    QString errorString() const;
    bool hasError() const;

signals:
    void readyRead();

private slots:
    void onUartReadyRead();
    void onUartError(QSerialPort::SerialPortError error);

private:
    void onReadyReadWhenUnconnected();

    enum RxState
    {
        RxDelimiter,
        RxHeader,
        RxPayload,
        RxInvalidState
    };

    QSerialPort m_port;

    RxState m_rxState;
    QByteArray m_rxBuffer;
    quint32 m_rxBytesNeeded;
    QList<QByteArray> m_rxMessageQueue;
    QString m_errorString;
    bool m_connected;
};

inline bool StpSocket::messagesAvailable()
{
    return m_rxMessageQueue.size() > 0;
}

inline bool StpSocket::hasError() const
{
    return !m_errorString.isEmpty();
}

inline QString StpSocket::errorString() const
{
    return m_errorString;
}


#endif // SERIALINTERFACE_H
