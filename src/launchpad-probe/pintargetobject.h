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

#ifndef PROBES_PIN_H_
#define PROBES_PIN_H_

#include <ti/drivers/PIN.h>
#include <ti/devices/cc13x0/driverlib/ioc.h>

#include "roctargetobject.h"

extern "C" void onPinEvent(PIN_Handle, PIN_Id);

class PinTargetObject : public RocTargetObject
{
public:
    PinTargetObject();
    ~PinTargetObject();

    void parseMessage(uint8_t methodId, const char* data) override;

    void deserialize(const char* data);

    uint32_t config() const;
    uint32_t ioid() const;
    void setValue(uint32_t value);

    void onValueChanged();

private:
    PIN_Handle m_handle;
    PIN_State m_state;
    PIN_Config m_config[2];

    static PinTargetObject* instances[32];

    friend void onPinEvent(PIN_Handle, PIN_Id);
};

inline uint32_t PinTargetObject::ioid() const
{
    return m_config[0] & IOC_IOID_MASK;
}

inline uint32_t PinTargetObject::config() const
{
    return m_config[0];
}

#endif /* PROBES_PIN_H_ */
