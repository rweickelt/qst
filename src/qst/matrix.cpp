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
    case QstItem::AfterComponentComplete:
        if (m_scope == TestcaseScope)
        {
            Testcase* test = qobject_cast<Testcase*>(parent());
            m_testcases = QStringList{ test->name() };
        }
    default:
        break;
    }
}

void Matrix::setTestcases(const QStringList& names)
{
    if (m_scope == TestcaseScope)
    {
        QmlContext context = qst::qmlDefinitionContext(this);
        QString message = QString("At %1:%2: 'testcases' property cannot be used when Matrix resides inside Testcase.")
                .arg(context.file()).arg(context.line());
        QST_ERROR_AND_EXIT(message);
    }
    m_testcases = names;
    emit testcasesChanged();
}
