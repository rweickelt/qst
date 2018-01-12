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

#include "messagebuffer.h"
#include "serialinterface.h"

#include <Board.h>
#include <assert.h>
#include <string.h>

#include <ti/drivers/uart/UARTCC26XX.h>
#include <ti/sysbios/BIOS.h>

extern "C" Mailbox_Handle rxMailbox;

// Message handling
namespace {
    SerialInterface* instance = NULL;
}

SerialInterface::SerialInterface()
{
    m_uart = NULL;
    m_txActive = false;
    instance = this;

    GateSwi_construct(&m_txGate, NULL);
    Queue_construct(&m_txQueue, NULL);
}

SerialInterface::~SerialInterface()
{
    UART_close(m_uart);
}

bool SerialInterface::open()
{
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

    m_uart = UART_open(Board_UART0, &uartParams);
    return m_uart != NULL;
}


void SerialInterface::startRx()
{
    m_rxState = RxDelimiter;
    m_rxDelimitersReceived = 0;

    UART_read(m_uart, &m_rxHeader, 1);
}


void SerialInterface::onUartBytesReceived(size_t bytes)
{
    RxState nextState = RxInvalidState;
    uint32_t bytesNeeded = 0;
    void* destination = 0;

    switch(m_rxState) {
    case RxDelimiter:
        if (m_rxHeader.delimiterBytes[m_rxDelimitersReceived] != stp::MessageHeader::DelimiterByte) {
            m_rxDelimitersReceived = 0;
        } else {
            m_rxDelimitersReceived++;
        }
        if (m_rxDelimitersReceived == stp::MessageHeader::DelimiterLength) {
            m_rxDelimitersReceived = 0;
            nextState = RxHeader;
            destination = &m_rxHeader.payloadLength;
            bytesNeeded = sizeof(stp::MessageHeader) - stp::MessageHeader::DelimiterLength;
        } else {
            nextState = RxDelimiter;
            destination = &m_rxHeader.delimiterBytes[m_rxDelimitersReceived];
            bytesNeeded = 1;
        }
        break;
    // Validate header
    case RxHeader:
        if (m_rxHeader.delimiterWord != stp::MessageHeader::DelimiterWord) {
            nextState = RxDelimiter;
            destination = &m_rxHeader;
            bytesNeeded = 1;
        } else if (m_rxHeader.type == stp::RocMessage) {
            assert(m_rxHeader.payloadLength <= 32);
            nextState = RxPayload;
            m_rxBuffer = SharedPointer<MessageBuffer>(new MessageBuffer(m_rxHeader.payloadLength, 0));
            destination = m_rxBuffer->data();
            bytesNeeded = m_rxHeader.payloadLength;
        } else if (m_rxHeader.type == stp::ResetConnection) {
            nextState = RxHeader;
            bytesNeeded = sizeof(stp::MessageHeader);
            destination = &m_rxHeader;
            SharedPointer<MessageBuffer> resetMessage(new MessageBuffer(sizeof(stp::MessageHeader), sizeof(stp::MessageHeader)));
            this->write(resetMessage, stp::ResetConnection);
        }
        break;
    case RxPayload:
        // Todo: Don't fake data integrity check
        if (m_rxHeader.checksum != 0x47)
        {
            m_rxBuffer.reset();
            nextState = RxDelimiter;
            bytesNeeded = 1;
        }
        else
        {
            assert(Mailbox_getNumFreeMsgs(rxMailbox) > 0);
            m_rxBuffer.incrementRefCount();
            MessageBuffer* data = m_rxBuffer.data();
            Mailbox_post(rxMailbox, &data, BIOS_NO_WAIT);
            m_rxBuffer.reset();
            nextState = RxHeader;
            bytesNeeded = sizeof(stp::MessageHeader);
        }
        destination = &m_rxHeader;
        break;
    case RxInvalidState:
        assert(false);
        break;
    }

    m_rxState = nextState;
    UART_read(m_uart, destination, bytesNeeded);
}

void SerialInterface::onUartBytesWritten(size_t bytes)
{
    if (Queue_empty(Queue_handle(&m_txQueue)))
    {
        m_txBuffer = SharedPointer<MessageBuffer>();
        m_txActive = false;
        return;
    }

    MessageBuffer* data = static_cast<MessageBuffer*>(
                static_cast<Queue_Elem*>(Queue_dequeue(Queue_handle(&m_txQueue))));
    m_txBuffer = SharedPointer<MessageBuffer>(data);
    m_txBuffer.decrementRefCount();

    UART_write(m_uart, m_txBuffer->data(), m_txBuffer->size());
}


extern "C" void onUartBytesReceived(UART_Handle handle, void* rxBuf, size_t size)
{
    (void)handle;
    (void)rxBuf;
    assert(size > 0);

    instance->onUartBytesReceived(size);
}

extern "C" void onUartBytesWritten(UART_Handle handle, void* txBuf, size_t size)
{
    (void)handle;
    (void)txBuf;
    assert(size > 0);

    instance->onUartBytesWritten(size);
}

void SerialInterface::write(const SharedPointer<MessageBuffer>& buffer)
{
    this->write(buffer, stp::RocMessage);
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

    uint32_t key = GateSwi_enter(GateSwi_handle(&m_txGate));

    txBuffer.incrementRefCount();
    Queue_enqueue(Queue_handle(&m_txQueue), txBuffer.data());

    if (!m_txActive)
    {
        m_txActive = true;
        this->onUartBytesWritten(0);
    }
    GateSwi_leave(GateSwi_handle(&m_txGate), key);
}


