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

#ifndef MAILBOX_H
#define MAILBOX_H

#include <stdint.h>
#include <assert.h>

#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/BIOS.h>

template<typename T, uint32_t elements = 0>
class Mailbox;

template<typename T>
class Mailbox<T, 0>
{
public:
    int32_t freeMessages() const;
    int32_t pendingMessages() const;
    bool post(const T& item, uint32_t timeout = BIOS_WAIT_FOREVER);
    bool pend(T& item, uint32_t timeout = BIOS_WAIT_FOREVER);

protected:
    Mailbox(uint32_t elements);
    Mailbox_Struct m_mailbox;
};

template<typename T, uint32_t elements>
class Mailbox : public Mailbox<T,0>
{
public:
    Mailbox(const Event_Handle event = NULL, uint32_t readerEventId = 0);

private:
    char m_data[elements][sizeof(T)];
};


template<typename T>
Mailbox<T,0>::Mailbox(uint32_t elements)
{
}

template<typename T, uint32_t elements>
Mailbox<T, elements>::Mailbox(const Event_Handle readerEvent, uint32_t readerEventId) : Mailbox<T,0>(elements)
{
    Mailbox_Params params;
    Mailbox_Params_init(&params);
    params.buf = &m_data[0];
    params.bufSize = sizeof(m_data);
    params.readerEvent = readerEvent;
    params.readerEventId = readerEventId;
    Mailbox_construct(&this->m_mailbox, sizeof(T), elements, &params, NULL);
}

template<typename T>
int32_t Mailbox<T,0>::freeMessages() const
{
    return Mailbox_getNumFreeMsgs(Mailbox_handle(const_cast<Mailbox_Struct*>(&m_mailbox)));
}

template<typename T>
int32_t Mailbox<T,0>::pendingMessages() const
{
    return Mailbox_getNumPendingMsgs(Mailbox_handle(const_cast<Mailbox_Struct*>(&m_mailbox)));
}

template<typename T>
bool Mailbox<T,0>::post(const T& item, uint32_t timeout)
{
    return Mailbox_post(Mailbox_handle(&m_mailbox), const_cast<T*>(&item), timeout);
}

template<typename T>
bool Mailbox<T,0>::pend(T& item, uint32_t timeout)
{
    return Mailbox_pend(Mailbox_handle(&m_mailbox), &item, timeout);
}

#endif // MAILBOX_H
