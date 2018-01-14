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

#ifndef FILEEXTENSION_H
#define FILEEXTENSION_H

#include <QtCore/QFile>
#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <QtCore/QTextStream>

class QJSEngine;
class QQmlEngine;

class TextFile : public QObject
{
    Q_OBJECT
    Q_ENUMS(OpenMode)
public:
    enum OpenMode { ReadOnly, WriteOnly, ReadWrite };
    TextFile(QObject* parent = nullptr, const QString& filePath = "",
             OpenMode mode = ReadOnly, const QString& codec = QLatin1String("UTF8"));
    ~TextFile();

    Q_INVOKABLE void close();
    Q_INVOKABLE void truncate();
    Q_INVOKABLE void write(const QString &text);

    static void registerJSType(QJSEngine* engine);

private:
    QScopedPointer<QFile> m_file;
    QScopedPointer<QTextStream> m_stream;
};

class TextFileCreator : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE TextFile* createObject(const QVariantMap& arguments);
};

Q_DECLARE_METATYPE(TextFile::OpenMode)

#endif // TEXTFILE_H
