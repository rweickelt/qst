#include "dimension.h"
#include "matrix.h"
#include "project.h"
#include "projectresolver.h"
#include "qst.h"

#include <QtDebug>

Matrix::Matrix(QObject* parent) : QObject(parent)
{

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


void Matrix::handleParserEvent(ParserEventHandler::ParserEvent event)
{
    switch (event)
    {
    case ParserEventHandler::ClassBegin:
        ProjectResolver::instance()->registerMatrixComponent(this);
        break;
    case ParserEventHandler::AfterClassBegin:
        for (const auto& child: m_children)
        {
            if (!child->metaObject()->inherits(&Dimension::staticMetaObject))
            {
                QmlContext context = qst::qmlDefinitionContext(child);
                auto message = QString("Only Dimension items are accepted inside Matrix items at %1:%2")
                        .arg(context.file()).arg(context.line());
                QST_ERROR_AND_EXIT(message);
            }
        }

        if ((this->parent() == nullptr) || (this->parent()->metaObject()->inherits(&Project::staticMetaObject)))
        {
            m_scope = ProjectScope;
        }
        else if (this->parent()->metaObject()->inherits(&Testcase::staticMetaObject))
        {
            m_scope = TestcaseScope;
        }
        else
        {
            QmlContext context = qst::qmlDefinitionContext(this);
            auto message = QString("Matrix can only be defined as child of Project or Testcase. At %1:%2")
                    .arg(context.file()).arg(context.line());
            QST_ERROR_AND_EXIT(message);
        }
        break;
    default:
        break;
    }
}

void Matrix::setTestcases(const QStringList& names)
{
// TODO
//    QRegularExpression patternCheck("[\\w-\\*\\?]");

//    for (const auto& pattern: patterns)
//    {
//        QRegularExpressionMatch match = patternCheck.match(pattern);
//        if (!match.hasMatch())
//        {

//        }
//    }


    m_testcases = names;
    emit testcasesChanged();
}
