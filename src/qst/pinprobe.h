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
#ifndef PINPROBE_H
#define PINPROBE_H

#include <QtCore/QString>
#include <protocols/pin.h>
#include "component.h"
#include "rochostobject.h"

class PinProbe : public Component, public RocHostObject
{
    Q_OBJECT

protected:
    //void setActive(bool active);
    void cleanupTestCase();
    void cleanupTestFunction();
    void initTestCase();
    void initTestFunction();

signals:
    void ioidChanged();
    void pullModeChanged();
    void typeChanged();
    void valueChanged();

public:
    enum Type: uint8_t
    {
        Read = 0,
        Write = 1,
    };
    Q_ENUMS(Type)

    enum Value: int8_t
    {
        Undefined = -1,
        Low = 0,
        High = 1
    };
    Q_ENUMS(Value)

    enum PullMode: uint8_t
    {
        PullDisabled = 0,
        PullUp = 1,
        PullDown = 2,
    };
    Q_ENUMS(PullMode)

    Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(int ioid READ ioid WRITE setIoid NOTIFY ioidChanged)
    Q_PROPERTY(PullMode pullMode READ pullMode WRITE setPullMode NOTIFY pullModeChanged)
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QString port MEMBER m_port)

    PinProbe(QObject *parent = 0);

    int ioid() const;
    QString name() const;
    PullMode pullMode() const;
    Type type() const;
    int value() const;

    void setIoid(int ioid);
    void setPullMode(PullMode mode);
    void setType(Type type);
    void setValue(int value);

protected:
    void onDriverValueChanged(int value);
    void componentComplete();

    quint32 classId() const override;
    void processFromTarget(quint32 id, const QByteArray& data) override;

private:
    Type m_type;
    Value m_value;
    int m_ioid;
    PullMode m_pullMode;
    QString m_port;
    bool m_active;
};

inline int PinProbe::ioid() const { return m_ioid; }
inline PinProbe::PullMode PinProbe::pullMode() const { return m_pullMode; }
inline int PinProbe::value() const { return m_value; }
inline QString PinProbe::name() const { return m_name; }
inline PinProbe::Type PinProbe::type() const { return m_type; }



#endif // PINPROBE_H
