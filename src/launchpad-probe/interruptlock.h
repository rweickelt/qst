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
#ifndef INTERRUPTLOCK_H
#define INTERRUPTLOCK_H

#include <ti/devices/cc13x0/driverlib/interrupt.h>

/**
Locks HWIs and implements the RAII pattern.
 */
class InterruptLock
{
public:
    InterruptLock();
    ~InterruptLock();

private:
    InterruptLock(const InterruptLock&);
    InterruptLock& operator=(const InterruptLock&);

    bool m_alreadyDisabled;
};

inline InterruptLock::InterruptLock()
{
    m_alreadyDisabled = IntMasterDisable();
}

inline InterruptLock::~InterruptLock()
{
    if (!m_alreadyDisabled)
    {
        IntMasterEnable();
    }
}

#endif // INTERRUPTLOCK_H
