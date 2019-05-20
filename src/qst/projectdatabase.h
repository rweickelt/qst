#ifndef PROJECTDATABASE_H
#define PROJECTDATABASE_H

#include <QtCore/QMultiMap>
#include <QtCore/QPair>
#include <QtCore/QString>
#include <QtCore/QVariantMap>

#include "dependency.h"
#include "directedgraph.h"
#include "job.h"
#include "resource.h"

struct ProjectDatabase {
    QVariantMap profile;
    QVariantMap project;
    DirectedGraph<Job, Dependency> jobGraph;
    QMultiMap<Job, QPair<Dependency, QSet<Resource> > > eligibleResourcesPerJob;
    QMap<Dependency, QSet<Resource> > resourceMapping;
};

#endif // PROJECTDATABASE_H
