#ifndef TAG_H
#define TAG_H

#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QVariantMap>

typedef QVariantMap Tag;
typedef uint TagGroupId; // Hash of tag names (property names)
typedef uint TagId;      // Hash of tag values (property values)
typedef QMap<TagGroupId, QMap<TagId, Tag> > TagLookupTable;

enum {
    InvalidId = static_cast<uint>(-1)
};

TagGroupId makeTagGroupId(const Tag& tag);
TagId makeTagId(const Tag& tag);

#endif // TAG_H
