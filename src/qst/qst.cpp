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
#include "qst.h"
#include "testcase.h"
#include "textfile.h"

#include <QtQml/QJSEngine>
#include <QtQml/QQmlEngine>

namespace qst
{


void verify(bool condition, const QString& file, int line, const QString& message)
{
    if (!condition) {
        TestCase::instance()->finishAndExit(TestCase::Fail, file, line, message);
    }
}



}

QJSEngine* Qst::jsEngine;

QObject* Qst::createObject(const QString& typeName, const QVariantMap& arguments)
{
    if (typeName == "TextFile")
    {
        QString filePath = arguments.value("filePath").toString();
        TextFile::OpenMode openMode =
                qvariant_cast<TextFile::OpenMode>(arguments.value("openMode", TextFile::ReadWrite));
        QString codec = arguments.value("codec", "UTF-8").toString();
        return new TextFile(jsEngine, filePath, openMode, codec);
    }
    else
    {
        Q_ASSERT(false);
    }
}

QString Qst::hostOS() const
{
    return QSysInfo::kernelType();
}


QObject* Qst::createSingleInstance(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine);
    jsEngine = scriptEngine;

    return new Qst();
}
