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
#ifndef SHAREDBUFFER_H
#define SHAREDBUFFER_H

#include "shareddata.h"

#include <stdint.h>
#include <string>

#include <ti/sysbios/knl/Queue.h>

/*
A memory buffer for messages that can be shared across
the whole application.
*/
class MessageBuffer final : public SharedData, public Queue_Elem
{
public:
    explicit MessageBuffer(uint32_t maxSize, uint32_t offset = 0);
    ~MessageBuffer();

    uint32_t size() const;

    char* allocateAtBack(uint32_t bytes);
    template<typename T> T* allocateAtBack();

    char* allocateAtFront(uint32_t bytes);
    template<typename T> T* allocateAtFront();

    char* data();

    template<typename T> T takeAtFront();

    template<typename T>
    void prepend(const T& data);

private:
    MessageBuffer(const MessageBuffer&);
    MessageBuffer& operator=(const MessageBuffer&);

    char* m_memory;
    uint32_t m_size;
    uint32_t m_frontPos;
    uint32_t m_tailPos;
};

template<typename T>
T* MessageBuffer::allocateAtBack()
{
    return reinterpret_cast<T*>(this->allocateAtBack(sizeof(T)));
}

template<typename T>
T* MessageBuffer::allocateAtFront()
{
    return reinterpret_cast<T*>(this->allocateAtFront(sizeof(T)));
}

template<typename T>
T MessageBuffer::takeAtFront()
{
    T result;
    memcpy(&result, m_memory + m_frontPos, sizeof(T));
    m_frontPos += sizeof(T);
    return result;
}

template<typename T>
void MessageBuffer::prepend(const T& data)
{
    memcpy(m_memory + m_frontPos, &data, sizeof(T));
    m_frontPos -= sizeof(T);
}

#endif // SHAREDBUFFER_H
