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
#include "pinprobe.h"
#include "qst.h"
#include "testcase.h"
#include "rochostcontroller.h"

#include <protocols/pin.h>

#include "QDebug"

PinProbe::PinProbe(QObject *parent) : Component(&PinProbe::staticMetaObject, parent)
{
    setObjectName("PinProbe");
    m_ioid = 0;
    m_value = Undefined;
    m_type = Read;
    m_active = false;
    m_pullMode = PullDisabled;
}

void PinProbe::cleanupTestCase()
{
    unregisterFromHost();
}

void PinProbe::cleanupTestFunction()
{
    m_active = false;
}

void PinProbe::initTestCase()
{
    Q_ASSERT(!m_port.isEmpty());
    registerWithHost(RocHostController::instance(m_port));

    for (const auto& probe : testCase()->childrenByType<PinProbe>())
    {
        if (((probe->m_ioid == m_ioid) && (probe != this) && (this->m_port == probe->m_port)))
        {
            qst::error(QString("Conflicting ioid between PinProbe '%1' and '%2' in %3.")
                       .arg(this->name()).arg(probe->name()).arg(testCase()->filepath()));
        }
    }

    pin::PinInitMessage config {
        .ioid = static_cast<uint8_t>(m_ioid),
        .pullMode = static_cast<pin::PullMode>(m_pullMode),
        .type = static_cast<pin::Type>(m_type),
        .value = static_cast<pin::Value>(m_value)
    };
    this->sendToTarget(pin::Init, QByteArray(reinterpret_cast<char*>(&config), sizeof(config)));
}


void PinProbe::initTestFunction()
{
    m_active = true;
}

void PinProbe::setIoid(int ioid)
{
    bool changed = (m_ioid != ioid);
    m_ioid = ioid;
    if (m_active)
    {
        sendToTarget(pin::ChangeIoid, QByteArray(reinterpret_cast<const char*>(&m_ioid), 1));
        if (changed)
        {
            emit ioidChanged();
        }
    }
}

void PinProbe::setPullMode(PullMode mode)
{
    bool changed = (m_pullMode != mode);
    m_pullMode = mode;
    if (m_active)
    {
        sendToTarget(pin::ChangePullMode, QByteArray(reinterpret_cast<const char*>(&m_pullMode), sizeof(pin::PullMode)));
        if (changed)
        {
            emit pullModeChanged();
        }
    }
}

void PinProbe::setType(Type type)
{
    bool changed = (m_type != type);
    m_type = type;
    if (m_active)
    {
        sendToTarget(pin::ChangeType, QByteArray(reinterpret_cast<const char*>(&m_type), sizeof(pin::Type)));
        if (changed)
        {
            emit typeChanged();
        }
    }
}

void PinProbe::setValue(int value)
{
    //qDebug() << "set value " <<  value << " on probe " << m_name << ", active: " << m_active << ", type: " << m_type;

    Q_ASSERT((value >= pin::Undefined) && (value <= pin::High));

    if (!m_active)
    {
        m_value = static_cast<Value>(value);
    }
    else if (m_type == Write)
    {
        if (m_value == value)
        {
            return;
        }
        this->sendToTarget(pin::ChangeValue, QByteArray(reinterpret_cast<const char*>(&value), 1));
        m_value = static_cast<Value>(value);
        emit valueChanged();
    }
    else
    {
        qst::warning(QString("Error: Cannot set %1.value to '%2' because it is not an output.").arg(name()).arg(value));
    }
}


void PinProbe::processFromTarget(quint32 id, const QByteArray& data)
{
    switch (id)
    {
        case pin::ChangeValue:
            Q_ASSERT((data.at(0) >= pin::Undefined) && (data.at(0) <= pin::High));
            if (data.at(0) != m_value)
            {
                m_value = static_cast<Value>(data.at(0));
                if (m_active)
                {
                    emit valueChanged();
                }
            }
        break;
    default:
        Q_ASSERT(false);
    }
}

quint32 PinProbe::classId() const
{
    return roc::PinClass;
}
