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
#ifndef SERIALHEADER_H
#define SERIALHEADER_H

#include <stdint.h>

namespace stp {

enum MessageType : uint8_t
{
    ResetConnection = 1,
    RocMessage = 2,
};

struct MessageHeader
{
    enum
    {
        DelimiterByte = 0x7d,
        DelimiterWord = 0x7d7d7d7d,
        DelimiterLength = 4
    };

    union {
        uint8_t delimiterBytes[4];
        uint32_t delimiterWord;
    };

    uint16_t payloadLength;
    MessageType type;
    uint8_t checksum;
} __attribute__((packed));

typedef MessageHeader StpHeader;

}

#endif // SERIALHEADER_H
