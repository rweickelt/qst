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

#define PIN_GEN             (((uint32_t)1)<<31) ///< Flags that generic options are used

#define PIN_INPUT_EN        (PIN_GEN|(0<<29))   ///< (*) Enable input buffer
#define PIN_INPUT_DIS       (PIN_GEN|(1<<29))   ///< Disable input buffer
#define PIN_HYSTERESIS      (PIN_GEN|(1<<30))   ///< Enable input buffer hysteresis
#define PIN_NOPULL          (PIN_GEN|(0<<13))   ///< (*) No pull-up or pull-down resistor
#define PIN_PULLUP          (PIN_GEN|(1<<13))   ///< Pull-up resistor enabled
#define PIN_PULLDOWN        (PIN_GEN|(2<<13))   ///< Pull-down resistor enabled
#define PIN_BM_INPUT_EN     (1<<29)             ///< Bitmask for input enable option
#define PIN_BM_HYSTERESIS   (1<<30)             ///< Bitmask input hysteresis option
#define PIN_BM_PULLING      (0x3<<13)           ///< Bitmask for pull-up/pull-down options

/// Bitmask for all input mode options
#define PIN_BM_INPUT_MODE   (PIN_BM_INPUT_EN|PIN_BM_HYSTERESIS|PIN_BM_PULLING)

#define PIN_GPIO_OUTPUT_DIS (PIN_GEN|(0<<23))   ///< (*) Disable output buffer when GPIO
#define PIN_GPIO_OUTPUT_EN  (PIN_GEN|(1<<23))   ///< Enable output buffer when GPIO
#define PIN_GPIO_LOW        (PIN_GEN|(0<<22))   ///< Output buffer drives to VSS when GPIO
#define PIN_GPIO_HIGH       (PIN_GEN|(1<<22))   ///< Output buffer drives to VDD when GPIO
#define PIN_PUSHPULL        (PIN_GEN|(0<<25))   ///< (*) Output buffer mode: push/pull
#define PIN_OPENDRAIN       (PIN_GEN|(2<<25))   ///< Output buffer mode: open drain
#define PIN_OPENSOURCE      (PIN_GEN|(3<<25))   ///< Output buffer mode: open source
#define PIN_SLEWCTRL        (PIN_GEN|(1<<12))   ///< Enable output buffer slew control
#define PIN_DRVSTR_MIN      (PIN_GEN|(0x0<<8))  ///< (*) Lowest drive strength
#define PIN_DRVSTR_MED      (PIN_GEN|(0x4<<8))  ///< Medium drive strength
#define PIN_DRVSTR_MAX      (PIN_GEN|(0x8<<8))  ///< Highest drive strength
#define PIN_BM_GPIO_OUTPUT_EN  (1<<23)          ///< Bitmask for output enable option
#define PIN_BM_GPIO_OUTPUT_VAL (1<<22)          ///< Bitmask for output value option
#define PIN_BM_OUTPUT_BUF   (0x3<<25)           ///< Bitmask for output buffer options
#define PIN_BM_SLEWCTRL     (0x1<<12)           ///< Bitmask for slew control options
#define PIN_BM_DRVSTR       (0xF<<8)            ///< Bitmask for drive strength options

/// Bitmask for all output mode options
#define PIN_BM_OUTPUT_MODE  (PIN_BM_GPIO_OUTPUT_VAL|PIN_BM_GPIO_OUTPUT_EN| \
                             PIN_BM_OUTPUT_BUF|PIN_BM_SLEWCTRL|PIN_BM_DRVSTR)

#define PIN_INV_INOUT       (PIN_GEN|(1<<24))   ///< Logically invert input and output
#define PIN_BM_INV_INOUT    (1<<24)             ///< Bitmask for input/output inversion option

#define PIN_IRQ_DIS         (PIN_GEN|(0x0<<16)) ///< (*) Disable IRQ on pin
#define PIN_IRQ_NEGEDGE     (PIN_GEN|(0x5<<16)) ///< Enable IRQ on negative edge
#define PIN_IRQ_POSEDGE     (PIN_GEN|(0x6<<16)) ///< Enable IRQ on positive edge
#define PIN_IRQ_BOTHEDGES   (PIN_GEN|(0x7<<16)) ///< Enable IRQ on both edges
#define PIN_BM_IRQ          (0x7<<16)           ///< Bitmask for pin interrupt option


PinProbe::PinProbe(QObject *parent) : Component(parent)
{
    setObjectName("PinProbe");
    m_ioid = 0;
    m_value = Undefined;
    m_type = Read;
    m_active = false;
    m_pullMode = PullDisabled;
}

void PinProbe::componentComplete()
{
    Component::componentComplete();
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

    quint32 config = m_ioid;
    switch (m_type)
    {
    case Read:
        config |= PIN_INPUT_EN | PIN_IRQ_BOTHEDGES;
        break;
    case Write:
        config |= PIN_GPIO_OUTPUT_EN | PIN_PUSHPULL | PIN_DRVSTR_MAX;
        config |= m_value == 1 ? PIN_GPIO_HIGH : PIN_GPIO_LOW;
        break;
    }

    if (m_pullMode == PullUp)
    {
        config |= PIN_PULLUP;
    }
    else if (m_pullMode == PullDown)
    {
        config |= PIN_PULLDOWN;
    }

    QByteArray serialized(reinterpret_cast<char*>(&config), sizeof(config));
    this->sendToTarget(roc::SerializedData, serialized, roc::SerializedData);
}


void PinProbe::initTestFunction()
{
    m_active = true;
}

void PinProbe::setValue(int value)
{
    //qDebug() << "set value " <<  value << " on probe " << m_name << ", active: " << m_active << ", type: " << m_type;

    if (!m_active) {
        m_value = value;
    } else if (m_type == Write) {
        this->sendToTarget(pin::PinValue, QByteArray(reinterpret_cast<const char*>(&value), 1));
    } else {
        qst::warning(QString("Error: Cannot set %1.value to '%2' because it is not an output.").arg(m_name).arg(value));
    }
}


void PinProbe::processFromTarget(quint32 id, const QByteArray& data)
{
    switch (id)
    {
        case pin::PinValue:
        if (data.at(0) != m_value)
        {
            m_value = data.at(0);
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
