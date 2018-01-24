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

#ifndef SERIALINTERFACE_H_
#define SERIALINTERFACE_H_

#include "sharedpointer.h"
#include "messagebuffer.h"

#include <protocols/stp.h>
#include <ti/drivers/UART.h>

extern "C" {
    void onUartBytesReceived(UART_Handle, void*, size_t);
    void onUartBytesWritten(UART_Handle, void*, size_t);
}

class SerialInterface
{
public:
    static void init();
    static void startRx();
    static void write(const SharedPointer<MessageBuffer>& message);

protected:
    static void write(const SharedPointer<MessageBuffer>& message, stp::MessageType type);

private:
    SerialInterface();

    friend void ::onUartBytesReceived(UART_Handle, void*, size_t);
    friend void ::onUartBytesWritten(UART_Handle, void*, size_t);
};

#endif /* SERIALINTERFACE_H_ */
