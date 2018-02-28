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

#include "roctargetobject.h"
#include "roctargetcontroller.h"

extern void sendToHost(uint32_t id, uint32_t method, const void* data, uint32_t length);

RocTargetObject::RocTargetObject()
{
    // TODO Auto-generated constructor stub

}

RocTargetObject::~RocTargetObject()
{
    // TODO Auto-generated destructor stub
}

void RocTargetObject::sendToHost(uint8_t messageId, roc::BlockingMode blockingMode, const void* data, uint32_t length)
{
    RocTargetController::sendToHost(messageId, blockingMode, this->id(), data, length);
}
