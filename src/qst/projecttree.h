#ifndef PROJECTTREE_H
#define PROJECTTREE_H

#include <QtCore/QList>

class Document;
class Matrix;
class Project;
class QstItem;
class Testcase;

struct ProjectTree
{
    QList<Document*> documents;
    Project* project;
    QList<Matrix*> matrices;
    QList<Testcase*> testcases;
};

#endif // PROJECTTREE_H
