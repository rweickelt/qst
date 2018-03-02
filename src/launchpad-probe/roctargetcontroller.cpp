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
#include <ti/sysbios/knl/Clock.h>

extern PIN_Handle pins;
extern const Clock_Handle checkConnectionClock;

namespace {
    ArrayList<RocTargetObject*, 32> probes;
    uint32_t probeCount = 0;
    bool connected = false;
}

extern "C" void checkConnectionCallback(UArg arg)
{
    (void)arg;
    if (connected)
    {
        SysCtrlSystemReset();
    }
}

void RocTargetController::init()
{
}

void RocTargetController::processMessage(const SharedPointer<MessageBuffer>& message)
{
    roc::MessageHeader rxRocHeader = message->takeAtFront<roc::MessageHeader>();
    // Must always start connection with a reset message.
    if (!connected && (rxRocHeader.type != roc::Connect))
    {
        for (;;);
    }

    switch (rxRocHeader.type)
    {
    case roc::Connect:
        for (int32_t i = 0; i < probes.length(); i++)
        {
            if (probes[i] != nullptr)
            {
                delete probes[i];
            }
        }
        probes.clear();
        probeCount = 0;
        sendToHost(roc::StatusOk, roc::Blocking);
        connected = true;
        Clock_start(checkConnectionClock);
        PIN_setOutputValue(pins, Board_PIN_GLED, 1);

        break;
    case roc::Construct:
        constructObject(static_cast<roc::ClassId>(rxRocHeader.objectId));
        break;
    case roc::Destruct:
        for (int32_t i = 0; i < probes.length(); i++)
        {
            if (probes[i] == reinterpret_cast<RocTargetObject*>(rxRocHeader.objectId))
            {
                delete probes[i];
                probes[i] = nullptr;
                probeCount--;
            }
        }
        if (probeCount == 0)
        {
            probes.clear();
            connected = false;
            PIN_setOutputValue(pins, Board_PIN_GLED, 0);
        }
        sendToHost(roc::StatusOk, roc::Blocking);
        break;
    case roc::Ping:
        Clock_stop(checkConnectionClock);
        Clock_start(checkConnectionClock);
        sendToHost(roc::Pong, roc::NonBlocking);
        break;
    default:
        assert(rxRocHeader.type >= roc::UserMessage);
        assert(rxRocHeader.objectId != 0);
        reinterpret_cast<RocTargetObject*>(rxRocHeader.objectId)->parseMessage(rxRocHeader.type, message->data());
        break;
    }

}

void RocTargetController::sendToHost(uint8_t messageId, roc::BlockingMode blockingMode, uint32_t objectId, const void* data, uint32_t length)
{
    uint32_t totalSize = length + sizeof(stp::MessageHeader) + sizeof(roc::MessageHeader);
    SharedPointer<MessageBuffer> txMessage(new MessageBuffer(totalSize, sizeof(stp::MessageHeader) + sizeof(roc::MessageHeader)));
    void* destination = txMessage->allocateAtBack(length);
    memcpy(destination, data, length);
    roc::MessageHeader* header = txMessage->allocateAtFront<roc::MessageHeader>();
    header->type = messageId;
    header->objectId = objectId;
    header->payloadLength = length;
    header->transactionId = blockingMode;
    SerialInterface::write(txMessage);
}

void RocTargetController::constructObject(roc::ClassId classId)
{
    RocTargetObject* object;
    switch (classId)
    {
    case roc::PinClass:
        object = new PinTargetObject();
        assert(object != nullptr);
        sendToHost(roc::ObjectId, roc::Blocking, reinterpret_cast<uint32_t>(object));
        break;
    default:
        assert(false);
    }
    probes.append(object);
    probeCount++;
}
