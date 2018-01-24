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

#include "pintargetobject.h"

#include <assert.h>
#include <string.h>
#include <ti/drivers/PIN.h>

#include <protocols/pin.h>
#include <protocols/roc.h>

PinTargetObject* PinTargetObject::instances[32];

extern "C" void onPinEvent(PIN_Handle handle, PIN_Id pin)
{
    (void)handle;
    PinTargetObject* instance = PinTargetObject::instances[pin];
    assert(instance != NULL);
    instance->onValueChanged();
}

PinTargetObject::PinTargetObject()
{
    m_config[0] = 0;
    m_config[1] = PIN_TERMINATE;
    m_handle = NULL;
}

PinTargetObject::~PinTargetObject()
{
    PIN_close(m_handle);
    instances[ioid()] = NULL;
}

void PinTargetObject::parseMessage(uint8_t methodId, const char* data)
{
    switch(methodId)
    {
    case pin::PinValue:
        this->setValue(data[0]);
        break;
    default:
        assert(false);
    }
}

void PinTargetObject::deserialize(const char* data)
{
    memcpy(&m_config[0], data, 4);
    uint32_t dio = ioid();
    assert(instances[dio] == NULL);
    instances[dio] = this;

    m_handle = PIN_open(&m_state, m_config);
    assert(m_handle != NULL);

    if (((config() & (PIN_BM_INPUT_EN | PIN_GEN)) == PIN_INPUT_EN)
            && (config() & PIN_BM_IRQ))
    {
        PIN_Status status = PIN_registerIntCb(m_handle, &onPinEvent);
        assert(status == PIN_SUCCESS);

        uint32_t value = PIN_getInputValue(dio);
        sendToHost(pin::PinValue, &value, 1);
    }
}

void PinTargetObject::onValueChanged()
{
    uint32_t value = PIN_getInputValue(this->ioid());
    sendToHost(pin::PinValue, &value, 1);
}

void PinTargetObject::setValue(uint32_t value)
{
    uint32_t config = m_config[0];
    assert(config & PIN_BM_GPIO_OUTPUT_EN);
    PIN_setOutputValue(m_handle, this->ioid(), value);
}

