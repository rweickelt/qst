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

#include "arraylist.h"
#include "messagebuffer.h"
#include "pintargetobject.h"
#include "roctargetcontroller.h"
#include "roctargetobject.h"
#include "serialinterface.h"

#include <string.h>
#include <protocols/roc.h>

#include <Board.h>
#include <ti/devices/cc13x0/driverlib/sys_ctrl.h>
#include <ti/drivers/PIN.h>

extern PIN_Handle pins;

enum {
    WatchdogReloadValue = 2
};

namespace {
    ArrayList<RocTargetObject*, 32> probes;
    int watchdogCounter = WatchdogReloadValue;
    bool connected = false;

    void constructObject(roc::ClassId classId);
}

void RocTargetController::init()
{
}

void RocTargetController::checkForConnectionTimeout()
{
    if (!connected)
    {
        return;
    }
    if (watchdogCounter > 0)
    {
        watchdogCounter--;
    }
    else
    {
        SysCtrlSystemReset();
    }
}

void RocTargetController::processMessage(const SharedPointer<MessageBuffer>& message)
{
    roc::MessageHeader rxRocHeader = message->takeAtFront<roc::MessageHeader>();
    // Must always start connection with a reset message.
    if (!connected && (rxRocHeader.type != roc::Reset))
    {
        for (;;);
    }
    switch (rxRocHeader.type)
    {
    case roc::Reset:
        for (int32_t i = 0; i < probes.length(); i++)
        {
            assert(probes[i] != NULL);
            delete probes[i];
        }
        probes.clear();

    {
        SharedPointer<MessageBuffer> txMessage(new MessageBuffer(
                    sizeof(stp::MessageHeader) + sizeof(roc::MessageHeader)));

        roc::MessageHeader* txRocHeader = txMessage->allocateAtFront<roc::MessageHeader>();
        txRocHeader->type = roc::StateMessage;
        txRocHeader->state = roc::Ready;
        txRocHeader->payloadLength = 0;
        SerialInterface::write(txMessage);
    }
        watchdogCounter = WatchdogReloadValue;
        connected = true;
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
    case roc::Ping:
    {
        watchdogCounter = WatchdogReloadValue;
        PIN_setOutputValue(pins, Board_PIN_LED1, !PIN_getOutputValue(Board_PIN_LED1));

        SharedPointer<MessageBuffer> txMessage(new MessageBuffer(
                    sizeof(roc::MessageHeader) + sizeof(stp::MessageHeader)));
        roc::MessageHeader* txRocHeader = txMessage->allocateAtFront<roc::MessageHeader>();
        txRocHeader->type = roc::Pong;
        txRocHeader->classId = rxRocHeader.classId;
        txRocHeader->objectId = rxRocHeader.objectId;
        txRocHeader->payloadLength = 0;
        SerialInterface::write(txMessage);
    }
        break;
    default:
        assert(false);
    }
}

void RocTargetController::sendToHost(uint32_t id, uint32_t method, const void* data, uint32_t length)
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
    SerialInterface::write(txMessage);
}

namespace {

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
        SerialInterface::write(reply);
        break;
    default:
        assert(false);
    }
}

}
