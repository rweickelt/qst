#include "dimension.h"
#include "matrix.h"
#include "project.h"
#include "projectresolver.h"
#include "qst.h"

#include <QtDebug>

Matrix::Matrix(QObject* parent) : QstItem(&Matrix::staticMetaObject, parent)
{
    QstItem::setAllowedParentTypes({&Project::staticMetaObject, &Testcase::staticMetaObject, nullptr});
    QstItem::setAllowedNestedTypes({&Dimension::staticMetaObject});
}

QList<Dimension*> Matrix::dimensions() const
{
    QList<Dimension*> dimensions;
    for (QObject* dimension: m_children)
    {
        dimensions << qobject_cast<Dimension*>(dimension);
    }
    return dimensions;
}


void Matrix::handleParserEvent(QstItem::ParserEvent event)
{
    switch (event)
    {
    case QstItem::ClassBegin:
        ProjectResolver::instance()->registerMatrixComponent(this);
        break;
    case QstItem::AfterClassBegin:
        if ((this->parent() == nullptr) || (this->parent()->metaObject()->inherits(&Project::staticMetaObject)))
        {
            m_scope = ProjectScope;
        }
        else if (this->parent()->metaObject()->inherits(&Testcase::staticMetaObject))
        {
            m_scope = TestcaseScope;
        }
        break;
    default:
        break;
    }
}

void Matrix::setTestcases(const QStringList& names)
{
    m_testcases = names;
    emit testcasesChanged();
}
