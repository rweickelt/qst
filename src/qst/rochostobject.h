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

#ifndef RPCOBJECT_H
#define RPCOBJECT_H

#include <QtCore/QPointer>
#include <QtCore/QtGlobal>

#include <protocols/roc.h>

class RocHostController;

/*
This class allows objects on a local host and a remote target to be losely connected.
An instance of RocHostObject can register itself with a RocHost
the remote target unit whereas remote objects may call slots on the host unit.

After registering at RpcHost, an RpcHostObject gets assigned a 32 bit identifier
which is similar to the address on the target for simplicity.
*/
class RocHostObject
{
    Q_DISABLE_COPY(RocHostObject)
    friend class RocHostController;

public:
    RocHostObject();
    virtual ~RocHostObject();

    quint32 targetId() const;
    RocHostController* host();
    void sendToTarget(uint8_t messageType, const QByteArray& data = QByteArray());

protected:
    void registerWithHost(RocHostController* host);
    void unregisterFromHost();

    virtual void processFromTarget(quint32 messageId, const QByteArray& data) = 0;
    virtual quint32 classId() const = 0;

private:
    quint32 m_targetId = 0;
    QPointer<RocHostController> m_host;
};

inline quint32 RocHostObject::targetId() const { return m_targetId; }

#endif // RPCOBJECT_H
