#ifndef DEPENDENCYINFO_H
#define DEPENDENCYINFO_H

#include "tag.h"

#include <QtCore/QSharedData>
#include <QtCore/QString>
#include <QtCore/QExplicitlySharedDataPointer>

class Depends;

/*
Contains information about a job dependency as plain data types.
This is necessary because Depends items are shared between jobs
and their content might change based on the tags that are currently
applied to the testcase.

Dependency objects instead are owned by a single job only.
*/
class Dependency
{
public:
    enum class Type {
        Undefined,
        Job,
        Resource
    };

    enum class ResourceFlag {
        Read,
        Write
    };
    Q_DECLARE_FLAGS(ResourceFlags, ResourceFlag)

    Dependency();
    Dependency(const Dependency& other);
    Dependency& operator=(const Dependency& other);

    inline int count() const { return d->count; }
    inline void setCount(int count) { d->count = count; }
    inline ResourceFlags flags() const { return d->flags; }
    inline void setFlags(ResourceFlags flags) { d->flags = flags; }
    inline QString name() const { return d->name; }
    inline void setName(const QString& name) { d->name = name; }
    inline QString alias() const { return d->alias; }
    inline void setAlias(const QString& alias) { d->alias = alias; }
    inline QList<TagSet> tags() const { return d->tags; }
    inline void setTags(const QList<TagSet>& tags) const { d->tags = tags; }
    inline Type type() const { return d->type; }
    inline void setType(Dependency::Type type) { d->type = type; }

//    bool operator==(const Dependency& other) const;
    bool operator<(const Dependency& other) const;
    friend uint qHash(const Dependency& dependency);

private:

    struct DependencyData : public QSharedData {
        Type type = Type::Undefined;
        QString name;
        QString alias;
        QList<TagSet> tags;
        int count = 0;
        Dependency::ResourceFlags flags;
    };

    QExplicitlySharedDataPointer<DependencyData> d;
};

inline bool Dependency::operator<(const Dependency& other) const { return (d.data() < other.d.data()); }

inline uint qHash(const Dependency& dependency) { return reinterpret_cast<uintptr_t>(dependency.d.data()) & 0xFFFFFFFF; }

#endif // DEPENDENCYINFO_H
