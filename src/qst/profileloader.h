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
#ifndef PROFILELOADER_H
#define PROFILELOADER_H

#include <QtCore/QObject>
#include <QtCore/QVariant>

/*
Loads a profile from the file system and returns an object representation
that can be attached to the QML context.
 */
class ProfileLoader : public QObject
{
    Q_OBJECT
public:
    ProfileLoader(const QStringList& profilePaths = QStringList());
    QString errorString() const;
    bool hasError() const;
    QVariant loadProfile(const QString& name);
    void setProfileSearchPaths(const QStringList& paths);

private:
    QString m_error;
    QStringList m_profileSearchPaths;
};

inline QString ProfileLoader::errorString() const { return m_error; }
inline bool ProfileLoader::hasError() const { return m_error.length() > 0; }
inline void ProfileLoader::setProfileSearchPaths(const QStringList &paths) { m_profileSearchPaths = paths; }


#endif // PROFILELOADER_H
