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
#include "profileloader.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>

ProfileLoader::ProfileLoader(const QStringList& paths)
{
    setProfileSearchPaths(paths);
}

QVariant ProfileLoader::loadProfile(QString name)
{
    m_error.clear();

    if (name.isEmpty())
    {
        return QVariant();
    }

    if (name.endsWith(".json"))
    {
        name.chop(5);
    }
    QString fileName = name + ".json";

    QString profilePath;
    for (const QString& dir: m_profileSearchPaths)
    {
        QString path = QDir(dir).absoluteFilePath(fileName);
        if (QFile::exists(path))
        {
            profilePath = path;
            break;
        }
    }

    if (profilePath.isEmpty())
    {
        m_error = QString("Could not find profile document '%1'").arg(fileName);
        return QVariant();
    }

    QFile file(profilePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        m_error = QString("Could not open the profile document '%1': %2").arg(fileName).arg(file.errorString());
        return QVariant();
    }

    QByteArray content = file.readAll();
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(content, &error);
    if (document.isNull())
    {
        m_error = QString("Could not parse the profile document '%1': %2").arg(fileName).arg(error.errorString());
    }

    QVariantMap profile = document.toVariant().toMap();
    profile.insert("name", name);

    return profile;
}
