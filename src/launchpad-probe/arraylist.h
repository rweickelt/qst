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
#ifndef ARRAYLIST_
#define ARRAYLIST_

#include <stdint.h>
#include <assert.h>

template<typename T, int32_t maxLength>
class ArrayList
{
public:
    typedef int32_t Index;
    typedef int32_t LengthT;

    enum {
        InvalidIndex = -1
    };

    void append(const T& item);
    void clear();
    const T& at(Index index) const;
    Index indexOf(const T& item) const;
    LengthT length() const;

    T& operator[](Index index);
    const T& operator[](Index index) const;

private:
    T m_data[maxLength];
    Index m_length;
};


template<typename T, int32_t maxLength>
void ArrayList<T, maxLength>::append(const T& item)
{
    assert(m_length < maxLength);
    m_data[m_length] = item;
    m_length++;
}

template<typename T, int32_t maxLength>
const T& ArrayList<T, maxLength>::at(ArrayList<T,maxLength>::Index index) const
{
    assert(index < m_length);
    return m_data[index];
}

template<typename T, int32_t maxLength>
void ArrayList<T, maxLength>::clear()
{
    for (int32_t i = 0; i < m_length; i++)
    {
        m_data[i].~T();
    }
    m_length = 0;
}

template<typename T, int32_t maxLength>
typename ArrayList<T,maxLength>::Index ArrayList<T, maxLength>::indexOf(const T& item) const
{
    for (Index i = 0; i < m_length; i++)
    {
        if (m_data[i] == item)
        {
            return i;
        }
    }
    return InvalidIndex;
}

template<typename T, int32_t maxLength>
typename ArrayList<T,maxLength>::LengthT ArrayList<T, maxLength>::length() const
{
    return m_length;
}

template<typename T, int32_t maxLength>
T& ArrayList<T, maxLength>::operator[](Index index)
{
    assert(index < m_length);
    return m_data[index];
}

template<typename T, int32_t maxLength>
const T& ArrayList<T, maxLength>::operator[](Index index) const
{
    assert(index < m_length);
    return m_data[index];
}



#endif /* ARRAYLIST_ */
