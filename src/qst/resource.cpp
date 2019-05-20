#include "resource.h"

#include <QtCore/QVector>

struct ResourceData {
    QString name;
    TagSet tags;
    QVariantMap data;
};

namespace {
QVector<ResourceData> resources;
}

QVariantMap Resource::data() const
{
    return resources[m_id].data;
}

QString Resource::name() const
{
    return resources[m_id].name;
}

void Resource::setData(const QVariantMap& data)
{
    resources[m_id].data = data;
}

TagSet Resource::tags() const
{
    return resources[m_id].tags;
}

Resource Resource::create(ResourceItem* item, const TagSet& tags)
{
    ResourceData data;
    data.name = item->name();
    data.tags = tags;
    data.data = item->data();

    resources.append(data);

    Resource resource;
    resource.m_id = resources.length() - 1;
    return resource;
}
