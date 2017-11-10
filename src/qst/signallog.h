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
#ifndef SIGNALLOG_H
#define SIGNALLOG_H

#include <QObject>
#include <QString>

/*
Logs any kind of signal events and writes them into a (VCD) file.
Sources are especially probes.
*/
class SignalLog : public QObject
{
    Q_OBJECT
public:
    explicit SignalLog(QObject *parent = 0);

    void setFilename(const QString& filename);

    void addEntry(const QString& name, quint64 time, const QVariant& value);

    bool hasErrors() const;
    QString errorString() const;

private:
    QString format;

};

#endif // SIGNALLOG_H
