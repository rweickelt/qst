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

#include "serialinterface.h"

#include <Board.h>
#include <assert.h>

#include <ti/drivers/PIN.h>
#include <ti/drivers/UART.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/gates/GateSwi.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Queue.h>

extern "C" Mailbox_Handle rxMailbox;
extern PIN_Handle pins;

// Message handling
namespace {
    enum RxState
    {
        RxDelimiter,
        RxHeader,
        RxPayload,
        RxInvalidState
    };

    RxState rxState;
    uint32_t delimitersReceived;
    UART_Handle uart;
    volatile bool txActive;
    Queue_Struct txQueue;
    GateSwi_Struct txGate;
    SharedPointer<MessageBuffer> rxBuffer;
    SharedPointer<MessageBuffer> txBuffer;
    stp::MessageHeader rxHeader;

    void setActivityLedEnabled(bool enabled);

}

void SerialInterface::init()
{
    uart = NULL;
    txActive = false;

    GateSwi_construct(&txGate, NULL);
    Queue_construct(&txQueue, NULL);

    UART_init();
    UART_Params uartParams;
    UART_Params_init(&uartParams);
    uartParams.baudRate = 115200;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readMode = UART_MODE_CALLBACK;
    uartParams.readCallback = &::onUartBytesReceived;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.stopBits = UART_STOP_ONE;
    uartParams.writeMode = UART_MODE_CALLBACK;
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.writeCallback = &::onUartBytesWritten;

    uart = UART_open(Board_UART0, &uartParams);
    assert(uart != NULL);
}


void SerialInterface::startRx()
{
    rxState = RxDelimiter;
    delimitersReceived = 0;

    UART_read(uart, &rxHeader, 1);
}


extern "C" void onUartBytesReceived(UART_Handle handle, void* rxBuf, size_t size)
{
    (void)handle;
    (void)rxBuf;
    assert(size > 0);

    setActivityLedEnabled(true);

    RxState nextState = RxInvalidState;
    uint32_t bytesNeeded = 0;
    void* destination = 0;

    switch(rxState) {
    case RxDelimiter:
        if (rxHeader.delimiterBytes[delimitersReceived] != stp::MessageHeader::DelimiterByte) {
            delimitersReceived = 0;
        } else {
            delimitersReceived++;
        }
        if (delimitersReceived == stp::MessageHeader::DelimiterLength) {
            delimitersReceived = 0;
            nextState = RxHeader;
            destination = &rxHeader.payloadLength;
            bytesNeeded = sizeof(stp::MessageHeader) - stp::MessageHeader::DelimiterLength;
        } else {
            nextState = RxDelimiter;
            destination = &rxHeader.delimiterBytes[delimitersReceived];
            bytesNeeded = 1;
        }
        break;
    // Validate header
    case RxHeader:
        if (rxHeader.delimiterWord != stp::MessageHeader::DelimiterWord) {
            nextState = RxDelimiter;
            destination = &rxHeader;
            bytesNeeded = 1;
        } else if (rxHeader.type == stp::RocMessage) {
            assert(rxHeader.payloadLength <= 32);
            nextState = RxPayload;
            rxBuffer = SharedPointer<MessageBuffer>(new MessageBuffer(rxHeader.payloadLength, 0));
            destination = rxBuffer->data();
            bytesNeeded = rxHeader.payloadLength;
        } else if (rxHeader.type == stp::ResetConnection) {
            nextState = RxHeader;
            bytesNeeded = sizeof(stp::MessageHeader);
            destination = &rxHeader;
            SharedPointer<MessageBuffer> resetMessage(new MessageBuffer(sizeof(stp::MessageHeader), sizeof(stp::MessageHeader)));
            SerialInterface::write(resetMessage, stp::ResetConnection);
        }
        break;
    case RxPayload:
        // Todo: Don't fake data integrity check
        if (rxHeader.checksum != 0x47)
        {
            rxBuffer.reset();
            nextState = RxDelimiter;
            bytesNeeded = 1;
        }
        else
        {
            assert(Mailbox_getNumFreeMsgs(rxMailbox) > 0);
            rxBuffer.incrementRefCount();
            MessageBuffer* data = rxBuffer.data();
            Mailbox_post(rxMailbox, &data, BIOS_NO_WAIT);
            rxBuffer.reset();
            nextState = RxHeader;
            bytesNeeded = sizeof(stp::MessageHeader);
        }
        destination = &rxHeader;
        break;
    case RxInvalidState:
        assert(false);
        break;
    }

    rxState = nextState;
    if (!txActive)
    {
        setActivityLedEnabled(false);
    }
    UART_read(uart, destination, bytesNeeded);}

extern "C" void onUartBytesWritten(UART_Handle handle, void* txBuf, size_t size)
{
    (void)handle;
    (void)txBuf;

    if (Queue_empty(Queue_handle(&txQueue)))
    {
        txBuffer = SharedPointer<MessageBuffer>();
        txActive = false;
        setActivityLedEnabled(false);
        return;
    }

    MessageBuffer* data = static_cast<MessageBuffer*>(
                static_cast<Queue_Elem*>(Queue_dequeue(Queue_handle(&txQueue))));
    txBuffer = SharedPointer<MessageBuffer>(data);
    txBuffer.decrementRefCount();

    UART_write(uart, txBuffer->data(), txBuffer->size());
}

void SerialInterface::write(const SharedPointer<MessageBuffer>& buffer)
{
    write(buffer, stp::RocMessage);
}

void SerialInterface::write(const SharedPointer<MessageBuffer>&buffer, stp::MessageType type)
{
    SharedPointer<MessageBuffer> txBuffer(buffer);
    uint32_t payloadLength = txBuffer->size();
    stp::MessageHeader* stpHeader = txBuffer->allocateAtFront<stp::MessageHeader>();

    stpHeader->checksum = 0x47;
    stpHeader->delimiterWord = stp::MessageHeader::DelimiterWord;
    stpHeader->payloadLength = payloadLength;
    stpHeader->type = type;

    uint32_t key = GateSwi_enter(GateSwi_handle(&txGate));

    txBuffer.incrementRefCount();
    Queue_enqueue(Queue_handle(&txQueue), txBuffer.data());

    if (!txActive)
    {
        txActive = true;
        setActivityLedEnabled(true);
        onUartBytesWritten(uart, nullptr, 0);
    }
    GateSwi_leave(GateSwi_handle(&txGate), key);
}

namespace {
void setActivityLedEnabled(bool enabled)
{
    if (enabled)
    {
        PIN_setOutputValue(pins, Board_PIN_RLED, 1);
    }
    else
    {
        PIN_setOutputValue(pins, Board_PIN_RLED, 0);
    }
}
}

