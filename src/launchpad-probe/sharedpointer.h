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

#ifndef SHAREDPOINTER_H
#define SHAREDPOINTER_H

#include "interruptlock.h"
#include "shareddata.h"

#include <assert.h>

template<typename T>
class SharedPointer
{
public:
    SharedPointer();
    SharedPointer(T* data);
    SharedPointer(const SharedPointer<T>& other);
    SharedPointer& operator=(const SharedPointer& other);
    ~SharedPointer();

    T* data();
    T* data() const;

    T* operator->();
    T* operator->() const;

    void reset(T* data = NULL);

    void decrementRefCount();
    void incrementRefCount();

protected:
    SharedData* intData();

private:
    T* m_data;
};

template<typename T>
SharedPointer<T>::SharedPointer()
{
    m_data = 0;
}

template<typename T>
SharedPointer<T>::SharedPointer(T* data)
{
    m_data = data;
    if (m_data)
    {
        this->incrementRefCount();
    }
}

template<typename T>
SharedPointer<T>::SharedPointer(const SharedPointer<T>& other)
{
    m_data = other.m_data;
    if (m_data)
    {
        this->incrementRefCount();
    }
}

template<typename T>
SharedPointer<T>& SharedPointer<T>::operator=(const SharedPointer<T>& other)
{
    reset(other.data());
    return *this;
}

template<typename T>
SharedPointer<T>::~SharedPointer()
{
    InterruptLock lock;

    if (m_data)
    {
        this->decrementRefCount();
    }
}

template<typename T>
T* SharedPointer<T>::data()
{
    return m_data;
}

template<typename T>
T* SharedPointer<T>::data() const
{
    return m_data;
}

template<typename T>
SharedData* SharedPointer<T>::intData()
{
    return static_cast<SharedData*>(m_data);
}

template<typename T>
void SharedPointer<T>::reset(T* data)
{
    if (m_data != NULL)
    {
        decrementRefCount();
    }
    m_data = data;
    if (m_data)
    {
        incrementRefCount();
    }
}

template<typename T>
void SharedPointer<T>::decrementRefCount()
{
    assert(m_data != NULL);
    assert(intData()->m_refCount > 0);

    intData()->m_refCount--;
    if (intData()->m_refCount == 0)
    {
        delete m_data;
        m_data = NULL;
    }
}

template<typename T>
void SharedPointer<T>::incrementRefCount()
{
    assert(m_data != NULL);
    m_data->m_refCount++;
}

template<typename T>
T* SharedPointer<T>::operator->()
{
    return m_data;
}

template<typename T>
T* SharedPointer<T>::operator->() const
{
    return m_data;
}


#endif // SHAREDPOINTER_H
