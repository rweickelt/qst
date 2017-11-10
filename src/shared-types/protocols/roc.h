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
#ifndef RPCHEADER_H
#define RPCHEADER_H

#include <stdint.h>

namespace roc {

enum ClassId
{
    PinClass = 0
};

enum MessageType : uint8_t
{
    Reset = 1,
    Error = 2,
    Construct = 3,
    Destruct = 4,
    CallMethod = 5,
    ObjectId = 6,
    SerializedData = 7,
};

struct MessageHeader
{
    uint16_t payloadLength;
    uint8_t type;
    union  {
        uint8_t classId;
        uint8_t methodId;
    };
    uint32_t objectId;
} __attribute__((packed));

}

#endif // RPCHEADER_H
