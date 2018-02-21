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
#ifndef RPCHEADER_H
#define RPCHEADER_H

#include <stdint.h>

namespace roc {

enum ClassId
{
    PinClass = 0
};

enum State {
    Ready = 1,
    Occupied = 2,
};

enum MessageType : uint8_t
{
    StateMessage = 0,
    Connect = 1,
    Error = 2,
    Construct = 3,
    ObjectId = 4,
    SerializedData = 5,
    Destruct = 6,
    CallMethod = 7,
    Ping = 8,
    Pong = 9
};

struct MessageHeader
{
    uint16_t payloadLength;
    uint8_t type;
    union  {
        uint8_t classId;
        uint8_t methodId;
        uint8_t state;
    };
    uint32_t objectId;
} __attribute__((packed));

}

#endif // RPCHEADER_H
