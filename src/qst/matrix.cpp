/****************************************************************************
 **
 ** Copyright (C) 2018 The Qst project.
 **
 ** Contact: https://github.com/rweickelt/qst
 **
 ** $BEGIN_LICENSE$
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.

 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.

 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **
 ** $END_LICENSE$
****************************************************************************/

#include "dimension.h"
#include "matrix.h"
#include "project.h"
#include "qst.h"
#include "qstitemvisitor.h"
#include "testcase.h"

#include <QtDebug>

Matrix::Matrix(QObject* parent) : QstItem(parent)
{
}

void Matrix::callVisitor(QstItemVisitor* visitor)
{
    visitor->visit(this);
}

QList<Dimension*> Matrix::dimensions() const
{
    return findChildren<Dimension*>(QString(), Qt::FindDirectChildrenOnly);
}


void Matrix::handleParserEvent(QstItem::ParserEvent event)
{
    switch (event)
    {
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
            m_names = QStringList{ test->name() };
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
    m_names = names;
    emit testcasesChanged();
}
