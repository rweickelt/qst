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

#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#include <ti/drivers/rf/RF.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/Power.h>
#include <ti/devices/cc13x0/driverlib/rf_data_entry.h>
#include <ti/devices/cc13x0/driverlib/rf_prop_cmd.h>
#include <ti/devices/cc13x0/driverlib/rf_prop_mailbox.h>
#include <ti/devices/cc13x0/driverlib/rf_mailbox.h>

#include <Board.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Mailbox.h>

#include <protocols/roc.h>

#include "roctargetobject.h"
#include "arraylist.h"
#include "pintargetobject.h"
#include "serialinterface.h"
#include "mailbox.h"
#include "messagebuffer.h"

PIN_State pinState;
RF_Object rfObject;
RF_Handle rfHandle;
UART_Handle uart;

PIN_Config pinTable[] =
{
    Board_PIN_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    Board_BUTTON0  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    Board_BUTTON1  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    PIN_TERMINATE
};

enum {
    MessageReceived = 1 << 0,
};


ArrayList<RocTargetObject*, 32> probes;

extern "C" const Event_Handle applicationEvent;
extern "C" const Mailbox_Handle rxMailbox;
extern "C" const Task_Handle mainTask;

SerialInterface serial;

void constructObject(roc::ClassId classId);
void processMessage(SharedPointer<MessageBuffer> message);

void __assert_func(const char *, int, const char *, const char *)
{
    for (;;);
}

void breakpoint()
{
    PIN_Handle pinHandle = PIN_open(&pinState, pinTable);
    assert(pinHandle != NULL);
    PIN_setOutputValue(pinHandle, Board_LED1, 1);
    for(;;);
}

extern "C" void mainTaskFunction()
{
    usleep(500000);

    bool success = serial.open();
    assert(success == true);
    serial.startRx();

    for (;;)
    {
        uint32_t events = Event_pend(applicationEvent, 0, MessageReceived, BIOS_WAIT_FOREVER);

        if (events & MessageReceived)
        {
            while (Mailbox_getNumPendingMsgs(rxMailbox) > 0)
            {
                MessageBuffer* data;
                Mailbox_pend(rxMailbox, &data, BIOS_WAIT_FOREVER);
                SharedPointer<MessageBuffer> rxMessageBuffer(data);
                processMessage(rxMessageBuffer);
                rxMessageBuffer.decrementRefCount();
            }
        }
    }
}


void processMessage(SharedPointer<MessageBuffer> message)
{
    roc::MessageHeader rxRocHeader = message->takeAtFront<roc::MessageHeader>();
    switch (rxRocHeader.type)
    {
    case roc::Reset:
    {
        for (int32_t i = 0; i < probes.length(); i++)
        {
            assert(probes[i] != NULL);
            delete probes[i];
        }
        probes.clear();

        SharedPointer<MessageBuffer> txMessage(new MessageBuffer(
                    sizeof(roc::MessageHeader) + sizeof(stp::MessageHeader),
                    sizeof(roc::MessageHeader) + sizeof(stp::MessageHeader)));

        roc::MessageHeader* txRocHeader = txMessage->allocateAtFront<roc::MessageHeader>();
        txRocHeader->type = roc::Reset;
        txRocHeader->objectId = 47;
        txRocHeader->payloadLength = 0;
        serial.write(txMessage);
    }
        break;
    case roc::Construct:
        constructObject(static_cast<roc::ClassId>(rxRocHeader.classId));
        break;
    case roc::SerializedData:
        // Todo: Validate object after connection reset
        reinterpret_cast<RocTargetObject*>(rxRocHeader.objectId)->deserialize(message->data());
        break;
    case roc::CallMethod:
        // Todo: Validate object after connection reset
        reinterpret_cast<RocTargetObject*>(rxRocHeader.objectId)->parseMessage(rxRocHeader.methodId, message->data());
        break;
    default:
        assert(false);
    }
}

int count = 0;

void constructObject(roc::ClassId classId)
{
    SharedPointer<MessageBuffer> reply =
            SharedPointer<MessageBuffer>(new MessageBuffer(
                                             sizeof(roc::MessageHeader) + sizeof(stp::MessageHeader),
                                             sizeof(roc::MessageHeader) + sizeof(stp::MessageHeader))
                                         );
    roc::MessageHeader* txRocHeader = reply->allocateAtFront<roc::MessageHeader>();
    RocTargetObject* object;
    switch (classId)
    {
    case roc::PinClass:
        object = new PinTargetObject();
        assert(object != NULL);
        probes.append(object);

        // Send message back with object id
        txRocHeader->type = roc::ObjectId;
        txRocHeader->objectId = reinterpret_cast<uint32_t>(object);
        txRocHeader->payloadLength = 0;
        serial.write(reply);
        break;
    default:
        assert(false);
    }
}


void sendToHost(uint32_t id, uint32_t method, const void* data, uint32_t length)
{
    uint32_t totalSize = length + sizeof(stp::MessageHeader) + sizeof(roc::MessageHeader);
    SharedPointer<MessageBuffer> txMessage(new MessageBuffer(totalSize, sizeof(stp::MessageHeader) + sizeof(roc::MessageHeader)));
    void* destination = txMessage->allocateAtBack(length);
    memcpy(destination, data, length);
    roc::MessageHeader* header = txMessage->allocateAtFront<roc::MessageHeader>();
    header->type = roc::CallMethod;
    header->objectId = id;
    header->methodId = method;
    header->payloadLength = length;


    serial.write(txMessage);
}


int main(void)
{
    /* Call driver init functions. */
    Board_initGeneral();
    UART_init();

    BIOS_start();
    return (0);
}

