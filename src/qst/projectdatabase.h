#ifndef PROJECTDATABASE_H
#define PROJECTDATABASE_H

#include <QString>
#include <QVariantMap>


struct Action {
    int id;
    QLatin1String name;
    QString filepath;
    QVariantMap exports;
};

struct ProjectDatabase
{
    QVariantMap profile;
    QVariantMap project;
};

/*
project: {
    blabla: "blub",
    bdisd: "blblsd"
},
tags: [
    { id: 0, animal: "dishd" },
    { id: 1, does: "blabla" },
],
actions: [
    {
        id: 0
        name: "blabla",
        filepath: "/hdosdh/q.qml",
        // We need this as dummy in case multiple testcases
        // are in the same file depending on each other
        exports: {
            property: blabla,
            blub: blsdbsd
        },
    }
]
jobs: [
    {
        id: 0;
        action: 2;
        depends: [ 8, 10, 20 ],
        tags: [ 1, 4, 7 ]
    }
]



*/
#endif // PROJECTDATABASE_H
