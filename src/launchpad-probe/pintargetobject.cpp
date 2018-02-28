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
#include "roctargetcontroller.h"

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
    if (instance != nullptr)
    {
        instance->onValueChanged();
    }
}

PinTargetObject::~PinTargetObject()
{
    setEnabled(false);
    PIN_close(m_handle);
}

void PinTargetObject::parseMessage(uint8_t methodId, const char* data)
{
    switch(methodId)
    {
    case pin::ChangeIoid:
        this->setIoid(data[0]);
        break;
    case pin::ChangePullMode:
        this->setPullMode(static_cast<pin::PullMode>(data[0]));
        break;
    case pin::ChangeType:
        this->setType(static_cast<pin::Type>(data[0]));
        break;
    case pin::ChangeValue:
        this->setValue(data[0]);
        break;
    case pin::Init:
        init(*reinterpret_cast<const pin::PinInitMessage*>(data));
        break;
    default:
        assert(false);
    }
}

void PinTargetObject::init(pin::PinInitMessage config)
{
    m_ioid = config.ioid;

    PIN_Config configTable[2] = {
        m_ioid | PIN_INPUT_DIS | PIN_GPIO_OUTPUT_DIS | PIN_IRQ_BOTHEDGES | PIN_DRVSTR_MAX | PIN_NOPULL,
        PIN_TERMINATE
    };
    m_handle = PIN_open(&m_state, configTable);
    assert(m_handle != nullptr);

    setPullMode(config.pullMode);
    setType(config.type);
    setValue(config.value);
    setEnabled(true);

    PIN_Status status = PIN_registerIntCb(m_handle, &onPinEvent);
    assert(status == PIN_SUCCESS);

    if ((PIN_getConfig(ioid()) & (PIN_BM_INPUT_EN | PIN_GEN)) == PIN_INPUT_EN)
    {
        m_value = PIN_getInputValue(ioid());
    }

    sendToHost(roc::StatusOk, roc::Blocking, nullptr, 0);
    sendToHost(pin::ChangeValue, roc::NonBlocking, &m_value, 1);
}

void PinTargetObject::onValueChanged()
{
    if (PIN_getConfig(ioid()) & PIN_BM_GPIO_OUTPUT_EN)
    {
        return;
    }

    m_value = (PIN_getInputValue(ioid()) == 1) ? pin::High : pin::Low;
    sendToHost(pin::ChangeValue, roc::NonBlocking, &m_value, 1);
}

void PinTargetObject::setEnabled(bool enabled)
{
    instances[ioid()] = (enabled) ? this : nullptr;
}

void PinTargetObject::setIoid(uint8_t id)
{
    bool wasEnabled = isEnabled();
    if (wasEnabled)
    {
        setEnabled(false);
    }

    PIN_Config config = PIN_getConfig(ioid()) & 0xFFFFFF00;
    PIN_remove(m_handle, ioid());
    m_ioid = id;
    PIN_Status status = PIN_add(m_handle, config | ioid());
    assert(status == PIN_SUCCESS);

    if (wasEnabled)
    {
        setEnabled(true);
    }
}

void PinTargetObject::setPullMode(pin::PullMode mode)
{
    PIN_Config config;
    switch (mode)
    {
    case pin::PullDisabled:
        config = PIN_NOPULL;
        break;
    case pin::PullDown:
        config = PIN_PULLDOWN;
        break;
    case pin::PullUp:
        config = PIN_PULLUP;
        break;
    }
    PIN_Config mask = PIN_BM_PULLING;
    PIN_Status status = PIN_setConfig(m_handle, mask, config | ioid());
    assert(status == PIN_SUCCESS);

    if (isEnabled())
    {
        sendToHost(roc::StatusOk, roc::Blocking);
    }

}

void PinTargetObject::setType(pin::Type type)
{
    PIN_Config config;
    if (type == pin::Read)
    {
        config = PIN_INPUT_EN | PIN_GPIO_OUTPUT_DIS;
    }
    else if (type == pin::Write)
    {
        config = PIN_INPUT_DIS | PIN_GPIO_OUTPUT_EN;
    }
    PIN_Config mask = PIN_BM_INPUT_EN | PIN_BM_GPIO_OUTPUT_EN;
    PIN_Status status = PIN_setConfig(m_handle, mask, config | ioid());
    assert(status == PIN_SUCCESS);

    if (isEnabled())
    {
        sendToHost(roc::StatusOk, roc::Blocking);
    }
}

void PinTargetObject::setValue(int8_t value)
{
    m_value = value;
    if (value != pin::Undefined)
    {
        PIN_Config config = ((value == pin::High) ? PIN_GPIO_HIGH : PIN_GPIO_LOW);
        PIN_Config mask = PIN_BM_GPIO_OUTPUT_VAL;
        PIN_Status status = PIN_setConfig(m_handle, mask, config | ioid());
        assert(status == PIN_SUCCESS);
    }
    if (isEnabled())
    {
        sendToHost(roc::StatusOk, roc::Blocking);
    }
}

