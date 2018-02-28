/****************************************************************************
 **
 ** Copyright (C) 2017-2018 The Qst project.
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
#ifndef PINDATA_H
#define PINDATA_H

#include <stdint.h>
#include "roc.h"

namespace pin {

enum MessageType
{
    Init = roc::UserMessage,
    ChangeIoid,
    ChangeValue,
    ChangePullMode,
    ChangeType,
};

enum Type: uint8_t
{
    Read = 0,
    Write = 1,
};

enum Value: int8_t
{
    Undefined = -1,
    Low = 0,
    High = 1
};

enum PullMode: uint8_t
{
    PullDisabled = 0,
    PullUp = 1,
    PullDown = 2,
};

struct PinInitMessage {
    uint8_t ioid;
    PullMode pullMode;
    Type type;
    Value value;
} __attribute__((packed));

struct PinChangeMessage {
    union {
        Type type;
        PullMode pullMode;
        Value value;
        uint8_t ioid;
    };
} __attribute__((packed));


}

#endif // PINDATA_H
