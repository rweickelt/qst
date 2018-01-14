/****************************************************************************
 **
 ** Copyright (C) 2018 The Qst project.
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

#include "xds.h"
#include "qst.h"

#include <QtCore/QList>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlInfo>
#include <QtQml/QQmlEngine>
#include <QtSerialPort/QSerialPortInfo>
#include <QtDebug>

namespace {

enum Vendors {
    TexasInstruments = 0x0451
};

enum Products {
    Xds110 = 0xbef3
};

}

Xds::Xds(QObject *parent) : QObject(parent)
{
}

QStringList Xds::availableSerials() const
{
    QStringList serials;
    for (const auto& info : QSerialPortInfo::availablePorts())
    {
        if ((info.vendorIdentifier() == TexasInstruments)
                && (info.productIdentifier() == Xds110))
        {
            if (!serials.contains(info.serialNumber()))
            {
                serials << info.serialNumber();
            }
        }
    }
    return serials;
}

QString Xds::portFromSerial(const QString& serial)
{
    for (const auto& info : QSerialPortInfo::availablePorts())
    {
        // We will usually get 2 ports in ascending order. The first
        // match should be the correct port for communication.
        // Only tested on Linux.
        if (info.serialNumber() == serial)
        {
            return info.portName();
        }
    }
    qst::error(QString("No launchpad with serial '%1' found in %2:%3.")
               .arg(serial)
               .arg(QstService::instance(m_engine)->qmlCallerFile())
               .arg(QstService::instance(m_engine)->qmlCallerLine()));
    return "";
}

QObject* Xds::createSingleInstance(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(scriptEngine)
    Xds* instance = new Xds();
    instance->m_engine = engine;
    return instance;
}
