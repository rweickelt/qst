#ifndef TAG_H
#define TAG_H

#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QVariantMap>

typedef QVariantMap Tag;
typedef uint TagGroupId;
typedef uint TagId;
typedef QMap<TagGroupId, QMap<TagId, Tag> > TagStorage;

enum {
    InvalidId = static_cast<uint>(-1)
};

TagGroupId makeTagGroupId(const Tag& tag);
TagId makeTagId(const Tag& tag);

#endif // TAG_H
