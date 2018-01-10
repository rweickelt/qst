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
#include "console.h"
#include "proxylogger.h"
#include "qst.h"
#include "testcase.h"
#include "textfile.h"

#include <QtCore/QDebug>
#include <QtQml/QQmlEngine>

namespace qst {

void error(const QString& message)
{
    Console::printError(QString("Error: %1").arg(message));
    ::exit(qst::ExitApplicationError);
}

void info(const QString& file, int line, const QString& message)
{
    LogInfo info {
        .test = Testcase::instance()->name(),
        .component = "",
        .file = file,
        .line = line,
        .message = message,
        .type = LogInfo::Info
    };
    ProxyLogger::instance()->print(info);
}

void verify(bool condition, const QString& file, int line, const QString& message)
{
    if (!condition) {
        Testcase::instance()->finishAndExit(Testcase::Fail, file, line, message);
    }
}

void warning(const QString& file, int line, const QString& message)
{
    LogInfo info {
        .test = Testcase::instance()->name(),
        .component = "",
        .file = file,
        .line = line,
        .message = message,
        .type = LogInfo::Warning
    };
    ProxyLogger::instance()->print(info);}

}

QstService::QstService(QObject *parent) : QObject(parent)
{
}

QObject* QstService::createObject(const QString& typeName, const QVariantMap& arguments)
{
    if (typeName == "TextFile")
    {
        QString filePath = arguments.value("filePath").toString();
        TextFile::OpenMode openMode =
                qvariant_cast<TextFile::OpenMode>(arguments.value("openMode", TextFile::ReadWrite));
        QString codec = arguments.value("codec", "UTF-8").toString();
        return new TextFile(qmlEngine(this), filePath, openMode, codec);
    }
    else
    {
        Q_ASSERT(false);
    }
}

Testcase* QstService::currentTestcase()
{
    return Testcase::instance();
}


QString QstService::hostOS() const
{
    QString kernel = QSysInfo::kernelType();
    if (kernel == "winnt")
    {
        return "windows";
    }
    else if (kernel == "darwin")
    {
        return QSysInfo::productType();
    }
    else
    {
        return kernel;
    }
}
