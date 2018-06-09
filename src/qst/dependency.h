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
    inline QString target() const { return d->name; }
    inline void setTarget(const QString& target) { d->target = target; }
    inline Type type() const { return d->type; }
    inline void setTarget(Dependency::Type type) { d->type = type; }

private:

    struct DependencyData : public QSharedData {
        Type type;
        QString name;
        QString target;
        TagList tags;
    };

    QExplicitlySharedDataPointer<DependencyData> d;
};

#endif // DEPENDENCYINFO_H
