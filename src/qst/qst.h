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
#ifndef QST_H
#define QST_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariantList>
#include <QtCore/QVariantMap>

class QQmlEngine;
class Testcase;

struct QmlContext : public QVariantMap
{
    Q_GADGET
    Q_PROPERTY(int column READ column CONSTANT)
    Q_PROPERTY(QString file READ file CONSTANT)
    Q_PROPERTY(int line READ line CONSTANT)

public:
    inline int column() const { return this->value("column").toInt(); }
    inline QString file() const { return this->value("file").toString(); }
    inline int line() const { return this->value("line").toInt(); }
};

namespace qst {


enum ExitCode {
    ExitNormal = 0,
    ExitApplicationError = 1,
    ExitTestCaseFailed = 2
};

void info(const QString& message, const QString& file = "", int line = 0);
void error(const QString& message);
QmlContext qmlDefinitionContext(const QObject* object);
void verify(bool condition, const QString& message = "", const QString& file = "", int line = 0);
void warning(const QString& message, const QString& file = "", int line = 0);

}

#define QST_ERROR_AND_EXIT(message) \
    qst::error(message)

#define QST_INFO(message) \
    qst::info(message, __FILE__, __LINE__)

#define QST_VERIFY(condition, message) \
    qst::verify(condition, message, __FILE__, __LINE__)

#define QST_WARNING(message) \
    qst::warning(message, __FILE__, __LINE__)

class QstService : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString hostOS READ hostOS CONSTANT)
    Q_PROPERTY(Testcase* currentTestcase READ currentTestcase CONSTANT)

public:
    QstService(QObject* parent = 0);
    Testcase* currentTestcase();
    QString hostOS() const;

    Q_INVOKABLE void error(const QString& message, QString file = "", int line = 0);
    Q_INVOKABLE QmlContext qmlCallerContext();
    Q_INVOKABLE QVariantList qmlCallerTrace();
    Q_INVOKABLE QmlContext qmlDefinitionContext(QObject* object);
    Q_INVOKABLE void info(const QVariant& message);

    static QstService* instance(QQmlEngine* engine);
};

Q_DECLARE_METATYPE(QmlContext)
Q_DECLARE_METATYPE(QList<QmlContext>)


#endif // QST_H
