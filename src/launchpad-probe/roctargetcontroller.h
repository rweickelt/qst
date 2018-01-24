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

#ifndef ROCTARGETCONTROLLER_H
#define ROCTARGETCONTROLLER_H

#include "messagebuffer.h"
#include "sharedpointer.h"

class RocTargetController
{
public:
    static void init();
    static void checkForConnectionTimeout();
    static void processMessage(const SharedPointer<MessageBuffer>& message);
    static void sendToHost(uint32_t id, uint32_t method, const void* data, uint32_t length);

private:
    RocTargetController();
};

#endif // ROCTARGETCONTROLLER_H
