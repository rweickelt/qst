#ifndef DEPENDENCYINFO_H
#define DEPENDENCYINFO_H

#include "tag.h"

#include <QtCore/QSharedData>
#include <QtCore/QString>
#include <QtCore/QExplicitlySharedDataPointer>

class Depends;

class Dependency
{
public:
    enum class Type {
        Undefined,
        Precedence,
        Resource
    };

    Dependency();
    Dependency(const Dependency& other);
    Dependency& operator=(const Dependency& other);

    inline QString name() const { return d->name; }
    inline void setName(const QString& name) { d->name = name; }
    inline QString alias() const { return d->alias; }
    inline void setAlias(const QString& alias) { d->alias = alias; }
    inline QList<TagSet> tags() const { return d->tags; }
    inline void setTags(const QList<TagSet>& tags) const { d->tags = tags; }
    inline Type type() const { return d->type; }
    inline void setAlias(Dependency::Type type) { d->type = type; }

private:

    struct DependencyData : public QSharedData {
        Type type;
        QString name;
        QString alias;
        QList<TagSet> tags;
    };

    QExplicitlySharedDataPointer<DependencyData> d;
};

#endif // DEPENDENCYINFO_H
