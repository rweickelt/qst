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

#ifndef PROBES_PIN_H_
#define PROBES_PIN_H_

#include <ti/drivers/PIN.h>
#include <protocols/pin.h>

#include "roctargetobject.h"

extern "C" void onPinEvent(PIN_Handle, PIN_Id);

class PinTargetObject : public RocTargetObject
{
public:
    PinTargetObject() {}
    ~PinTargetObject();

    void parseMessage(uint8_t methodId, const char* data) override;

    uint32_t ioid() const;
    bool isEnabled() const;
    void setEnabled(bool enabled);
    void setIoid(uint8_t id);
    void setPullMode(pin::PullMode mode);
    void setType(pin::Type type);
    void setValue(int8_t value);

    void onValueChanged();

private:
    PinTargetObject(const PinTargetObject&);
    PinTargetObject& operator=(const PinTargetObject&);

    void init(const pin::PinInitMessage config);

    PIN_Handle m_handle = nullptr;
    PIN_State m_state;
    int8_t m_value = 0;
    uint8_t m_ioid = 0;

    static PinTargetObject* instances[];

    friend void ::onPinEvent(PIN_Handle, PIN_Id);
};


inline uint32_t PinTargetObject::ioid() const
{
    return m_ioid;
}

inline bool PinTargetObject::isEnabled() const
{
    return instances[ioid()] != nullptr;
}

#endif /* PROBES_PIN_H_ */
