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

#include "messagebuffer.h"
#include "interruptlock.h"

#include <assert.h>

MessageBuffer::MessageBuffer(uint32_t maxSize, uint32_t offset)
{
    m_memory = new char [maxSize];
    m_size = maxSize;
    m_frontPos = offset;
    m_tailPos = offset;
    prev = 0;
    next = 0;
}

MessageBuffer::~MessageBuffer()
{
    delete[] m_memory;
}

char* MessageBuffer::allocateAtBack(uint32_t bytes)
{
    assert(m_size - this->size() >= bytes);

    char* address = m_memory + m_tailPos;
    m_tailPos += bytes;
    return address;
}

char* MessageBuffer::allocateAtFront(uint32_t bytes)
{
    assert(m_frontPos >= bytes);

    m_frontPos -= bytes;
    return m_memory + m_frontPos;
}

char* MessageBuffer::data()
{
    return m_memory + m_frontPos;
}

uint32_t MessageBuffer::size() const
{
    return m_tailPos - m_frontPos;
}

