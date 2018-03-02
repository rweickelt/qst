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

#include <ti/drivers/PIN.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/Watchdog.h>

#include <Board.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Mailbox.h>

#include <protocols/roc.h>

#include "serialinterface.h"
#include "mailbox.h"
#include "messagebuffer.h"
#include "roctargetcontroller.h"

namespace {

enum ApplicationEvent {
    MessageReceived = 1 << 0,
    KickWatchdog = 1 << 1,
    AllEvents = MessageReceived | KickWatchdog
};

enum {
    WatchdogTimeoutMs = 527  // Reset happens after 2*value. Value has to be a little bit
                             // larger than the kick period to avoid false triggers.
};

PIN_Config pinTable[] =
{
    Board_PIN_GLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    Board_PIN_RLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};
PIN_State pinState;
Watchdog_Handle watchdog;

extern "C" const Event_Handle applicationEvent;
extern "C" const Task_Handle mainTask;
extern "C" const Mailbox_Handle rxMailbox;
extern "C" const Clock_Handle watchdogClock;

}

PIN_Handle pins;

void __assert_func(const char * function, int line , const char * file, const char * expression )
{
    for (;;);
}


extern "C" void watchdogClockCallback(UArg arg)
{
    (void)arg;
    Event_post(applicationEvent, KickWatchdog);
}


extern "C" void mainTaskFunction()
{
    // Need to wait some milliseconds on the launchpad because the TIVA
    // pulls the UART line low at startup.
    usleep(70000);

    pins = PIN_open(&pinState, pinTable);
    assert(pins != nullptr);

    watchdog = Watchdog_open(Board_WATCHDOG0, nullptr);
    assert(watchdog != nullptr);
    Watchdog_setReload(watchdog, Watchdog_convertMsToTicks(watchdog, WatchdogTimeoutMs));
    Clock_start(watchdogClock);

    SerialInterface::init();
    RocTargetController::init();
    SerialInterface::startRx();

    for (;;)
    {
        uint32_t events = Event_pend(applicationEvent, 0, AllEvents, BIOS_WAIT_FOREVER);

        if (events & MessageReceived)
        {
            while (Mailbox_getNumPendingMsgs(rxMailbox) > 0)
            {
                MessageBuffer* data;
                Mailbox_pend(rxMailbox, &data, BIOS_WAIT_FOREVER);
                SharedPointer<MessageBuffer> rxMessageBuffer(data);
                RocTargetController::processMessage(rxMessageBuffer);
                rxMessageBuffer.decrementRefCount();
            }
        }
        else if (events & KickWatchdog)
        {
            Watchdog_clear(watchdog);
        }
    }
}

int main(void)
{
    /* Call driver init functions. */
    Power_init();
    PIN_init(BoardGpioInitTable);
    Watchdog_init();

    BIOS_start();
    return (0);
}

