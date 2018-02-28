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

#ifndef PROBES_ABSTRACTPROBE_H_
#define PROBES_ABSTRACTPROBE_H_

#include <stdint.h>
#include <protocols/roc.h>

class RocTargetObject
{
public:
    RocTargetObject();
    virtual ~RocTargetObject();
    virtual void parseMessage(uint8_t messageId, const char* rawData) = 0;
    uint32_t id() const;
    void sendToHost(uint8_t messageId, roc::BlockingMode blockingMode,
                    const void* data = nullptr, uint32_t length = 0);
};

inline uint32_t RocTargetObject::id() const
{
    return reinterpret_cast<uint32_t>(this);
}

#endif /* PROBES_ABSTRACTPROBE_H_ */
