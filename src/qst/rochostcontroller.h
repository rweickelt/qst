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
#ifndef RPCHOST_H
#define RPCHOST_H

#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QSharedPointer>

#include "rochostobject.h"
#include "stpsocket.h"

class RocHostController : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(RocHostController)

    friend class SingleMpiHost;
    friend class RocHostObject;

    typedef uint8_t TransactionId;

    enum {
        ConnectionTimeoutIntervalMs = 517
    };

public:
    bool sendMessageBlocking(uint8_t messageType,  RocHostObject* object = nullptr,
                              const QByteArray& data = QByteArray(), int timeout = INT32_MAX);

    bool sendMessageBlocking(uint8_t messageType,  uint32_t objectId,
                              const QByteArray& data = QByteArray(), int timeout = INT32_MAX);

    void sendMessageNonBlocking(uint8_t messageType,  uint32_t objectId = 0,
                                const QByteArray& data = QByteArray());

    static RocHostController* instance(const QString& port);

    quint32 registerObject(RocHostObject* object);
    void unregisterObject(const RocHostObject* object);

public slots:
    void onReadyRead();

protected slots:
    void onPingTimerTick();


private:
    enum State {
        Disconnected, // Not connected to any probe
        Standby,      // Connected, but no probes active
        Connected,    // Connected and probes active
    };

    void setNextState(State newState);
    bool waitForState(int milliseconds);

    RocHostController(const QString& port);

    QHash<quint32, RocHostObject*> m_objects;
    QHash<const RocHostObject*, quint32> m_ids;

    StpSocket m_socket;
    RocHostObject* m_currentObject;
    quint32 m_currentObjectId;
    State m_state;
    QString m_port;
    QEventLoop m_loop;
    QTimer m_pingTimer;
    int m_unansweredPings;
};

#endif // RPCHOST_H
