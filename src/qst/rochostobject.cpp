/****************************************************************************
 **
 ** Copyright (C) 2017 The Qst project.
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

#include "rochostobject.h"
#include "rochostcontroller.h"

RocHostObject::RocHostObject()
{
    m_targetId = 0;
}

RocHostObject::~RocHostObject()
{
    if (targetId() != 0)
    {
        unregisterFromHost();
    }
}

void RocHostObject::registerWithHost(RocHostController* host)
{
    Q_ASSERT(m_targetId == 0);
    m_host = host;
    m_host->registerObject(this);
}

void RocHostObject::unregisterFromHost()
{
    Q_ASSERT(m_targetId != 0);
    Q_ASSERT(!m_host.isNull());
    m_host->unregisterObject(this);
}

void RocHostObject::sendToTarget(quint32 methodId, const QByteArray data, roc::MessageType type)
{
    Q_ASSERT(m_targetId != 0);
    Q_ASSERT(!m_host.isNull());
    m_host->sendMessage(this, methodId, data, type);
}

RocHostController* RocHostObject::host()
{
    return m_host;
}
