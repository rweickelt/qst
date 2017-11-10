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

#ifndef SERIALINTERFACE_H_
#define SERIALINTERFACE_H_

#include "mailbox.h"
#include "sharedpointer.h"

#include <protocols/stp.h>

#include <ti/drivers/UART.h>
#include <ti/sysbios/gates/GateSwi.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Queue.h>

extern "C" {
    void onUartBytesReceived(UART_Handle, void*, size_t);
    void onUartBytesWritten(UART_Handle, void*, size_t);
}

class MessageBuffer;

class SerialInterface
{
public:
    SerialInterface(Mailbox<SharedPointer<MessageBuffer> >* rxMailbox);
    ~SerialInterface();

    bool open();
    void startRx();
    void write(const SharedPointer<MessageBuffer>& message);

protected:
    void onUartBytesReceived(size_t bytes);
    void onUartBytesWritten(size_t bytes);
    void write(const SharedPointer<MessageBuffer>& message, stp::MessageType type);

private:
    enum RxState
    {
        RxDelimiter,
        RxHeader,
        RxPayload,
        RxInvalidState
    };

    RxState m_rxState;
    uint32_t m_rxDelimitersReceived;
    UART_Handle m_uart;
    bool m_txActive;
    Mailbox<SharedPointer<MessageBuffer> >* m_rxMailbox;
    Queue_Struct m_txQueue;
    GateSwi_Struct m_txGate;
    SharedPointer<MessageBuffer> m_rxBuffer;
    SharedPointer<MessageBuffer> m_txBuffer;

    stp::MessageHeader m_rxHeader;

    friend void ::onUartBytesReceived(UART_Handle, void*, size_t);
    friend void ::onUartBytesWritten(UART_Handle, void*, size_t);
};

#endif /* SERIALINTERFACE_H_ */
