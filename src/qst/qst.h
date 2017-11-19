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
#ifndef QST_H
#define QST_H

#include <QtCore/QObject>
#include <QtCore/QString>

class QJSEngine;
class QQmlEngine;

namespace qst {


enum ExitCode {
    ExitNormal = 0,
    ExitApplicationError = 1,
    ExitTestCaseFailed = 2
};

void info(const QString& file, int line, const QString& message);
void error(const QString& message);
void verify(bool condition, const QString& file, int line, const QString& message);
void warning(const QString& file, int line, const QString& message);

}

#define QST_ERROR_AND_EXIT(message) \
    qst::error(message)

#define QST_INFO(message) \
    qst::info(__FILE__, __LINE__, message)

#define QST_VERIFY(condition, message) \
    qst::verify(condition, __FILE__, __LINE__, message)

#define QST_WARNING(message) \
    qst::warning(__FILE__, __LINE__, message)

class Qst : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString hostOS READ hostOS)

public:
    Q_INVOKABLE QObject* createObject(const QString& typeName, const QVariantMap& arguments);
    QString hostOS() const;

    static QObject* createSingleInstance(QQmlEngine* engine, QJSEngine* scriptEngine);

private:
    static QJSEngine* jsEngine;
};


#endif // QST_H
