#include "tag.h"

#include <QtCore/QHash>

TagGroupId makeTagGroupId(const Tag& tag)
{
    QString text;
    for (const auto& key: tag.keys())
    {
        text += key;
    }
    return qHash(text);
}

TagId makeTagId(const Tag& tag)
{
    QString text;
    for (const auto& value: tag.values())
    {
        text += value.toString();
    }
    return qHash(text);
}
