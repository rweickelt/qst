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

#include "textfile.h"
#include "qst.h"

#include <QtQml/QQmlEngine>

TextFile::TextFile(QObject* parent, const QString& filePath,
                   OpenMode mode, const QString& codec)
    : QObject(parent)
{
    m_file.reset(new QFile(filePath));
    m_stream.reset(new QTextStream(m_file.data()));

    QIODevice::OpenMode m = static_cast<QIODevice::OpenMode>(mode);
    if (!m_file->open(m))
    {
        qst::error(QString("Unable to open file '%1': %2").arg(filePath, m_file->errorString()));
    }
    m_stream->setCodec(qPrintable(codec));
}

bool TextFile::atEndOfFile() const
{
    if (isClosed())
    {
        return true;
    }
    return m_stream->atEnd();
}

void TextFile::close()
{
    if (isClosed())
    {
        return;
    }
    m_stream->flush();
    m_file->close();
    m_stream.reset(0);
    m_file.reset(0);
}

bool TextFile::isClosed() const
{
    if (!m_file)
    {
        QstService::instance(qmlEngine(this))->error(QString("File is already closed"));
        return true;
    }
    return false;
}

QString TextFile::readAll()
{
    if (isClosed())
    {
        return QString();
    }
    return m_stream->readAll();
}

QString TextFile::readLine()
{
    if (isClosed())
    {
        return QString();
    }
    return m_stream->readLine();
}

void TextFile::truncate()
{
    if (isClosed())
    {
        return;
    }
    m_file->resize(0);
    m_stream->reset();
}

void TextFile::write(const QString& text)
{
    if (isClosed())
    {
        return;
    }
    (*m_stream) << text;
}

void TextFile::writeLine(const QString& text)
{
    if (isClosed())
    {
        return;
    }
    (*m_stream) << text << endl;
}

void TextFile::registerJSType(QJSEngine* engine)
{
    QJSValue creator = engine->newQObject(new TextFileCreator());
    engine->globalObject().setProperty("_TextFileCreator", creator);
    engine->evaluate("function TextFile(path, mode) { return _TextFileCreator.createObject({ filePath: path, openMode : mode}); }");
}

TextFile* TextFileCreator::createObject(const QVariantMap& arguments)
{
    QString filePath = arguments.value("filePath").toString();
    TextFile::OpenMode openMode =
            qvariant_cast<TextFile::OpenMode>(arguments.value("openMode", TextFile::ReadWrite));
    QString codec = arguments.value("codec", "UTF-8").toString();

    return new TextFile(qmlEngine(this), filePath, openMode, codec);
}
