#ifndef RESOURCE_H
#define RESOURCE_H

#include "resourceitem.h"
#include "tag.h"

#include <QtCore/QList>
#include <QtCore/QMultiMap>
#include <QtCore/QVariantMap>

class Resource;

using ResourceTable = QMultiMap<QString, Resource>;

class Resource {
public:
    using Id = int;
    enum { InvalidId = -1 };

    QVariantMap data() const;
    Id id() const;
    bool isValid() const;
    QString name() const;
    void setData(const QVariantMap& data);
    TagSet tags() const;

    bool operator==(const Resource& other) const;
    bool operator<(const Resource& other) const;

    static Resource create(ResourceItem* item, const TagSet& tags = TagSet());

private:
    Id m_id = InvalidId;
};

Q_DECLARE_METATYPE(Resource)

uint qHash(const Resource& Resource);

inline bool Resource::isValid() const { return m_id != InvalidId; }
inline Resource::Id Resource::id() const { return m_id; }
inline bool Resource::operator==(const Resource& other) const { return (m_id == other.m_id); }
inline bool Resource::operator<(const Resource& other) const { return (m_id < other.m_id); }

inline uint qHash(const Resource& resource) { return resource.id(); }

#endif // RESOURCE_H
