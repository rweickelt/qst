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

#include "component.h"
#include "depends.h"
#include "dimension.h"
#include "exports.h"
#include "hierarchyvalidator.h"
#include "matrix.h"
#include "project.h"
#include "projectresolver.h"
#include "qst.h"
#include "testcase.h"

#include <QtCore/QMetaObject>

#define INFO(type) \
    &type::staticMetaObject

namespace {
    const TypeList componentChildren = TypeList { INFO(Component), INFO(QObject) };
    const TypeList componentParents = TypeList { INFO(Component), INFO(Testcase) };
    const HierarchyValidator::DocumentRootMode componentRootMode = HierarchyValidator::DocumentRootDisallowed;

    const TypeList dependsChildren;
    const TypeList dependsParents = TypeList { INFO(Testcase) };
    const HierarchyValidator::DocumentRootMode dependsRootMode = HierarchyValidator::DocumentRootDisallowed;

    const TypeList dimensionChildren;
    const TypeList dimensionParents = TypeList { INFO(Matrix) };
    const HierarchyValidator::DocumentRootMode dimensionRootMode = HierarchyValidator::DocumentRootDisallowed;

    const TypeList exportsChildren;
    const TypeList exportsParents = TypeList { INFO(Testcase) };
    const HierarchyValidator::DocumentRootMode exportsRootMode = HierarchyValidator::DocumentRootDisallowed;

    const TypeList matrixChildren = TypeList { INFO(Dimension) };
    const TypeList matrixParents = TypeList { INFO(Project), INFO(Testcase) };
    const HierarchyValidator::DocumentRootMode matrixRootMode = HierarchyValidator::DocumentRootAllowed;

    const TypeList projectChildren = TypeList { INFO(Matrix), INFO(Testcase) };
    const TypeList projectParents = TypeList {};
    const HierarchyValidator::DocumentRootMode projectRootMode = HierarchyValidator::DocumentRootAllowed;

    const TypeList testcaseChildren = TypeList {
            INFO(Component), INFO(Depends), INFO(Exports), INFO(Matrix), INFO(QObject)
    };
    const TypeList testcaseParents = TypeList { INFO(Project) };
    const HierarchyValidator::DocumentRootMode testcaseRootMode = HierarchyValidator::DocumentRootAllowed;
}

HierarchyValidator::HierarchyValidator(ProjectResolver& resolver)
    : m_resolver(resolver)
{
}

void HierarchyValidator::validate(QstItem* rootItem)
{
    rootItem->accept(this);
}

void HierarchyValidator::validateChildrenAllowed(QstItem* item, const TypeList& allowedChildren)
{
    if (allowedChildren.isEmpty() && (!item->QObject::children().isEmpty()))
    {
        QmlContext context = qst::qmlDefinitionContext(item->QObject::children().first());
        QString message = QString("At %1:%2: %3 must not contain nested items.")
                    .arg(context.file())
                    .arg(context.line())
                    .arg(item->baseTypeInfo()->className());
        m_resolver.appendError(message);
        return;
    }

    for (const auto child: item->QObject::children())
    {
        bool ok = false;
        for (const auto type: allowedChildren)
        {
            if (child->metaObject()->inherits(type))
            {
                ok = true;
                break;
            }
        }
        if (!ok)
        {
            QmlContext context = qst::qmlDefinitionContext(child);
            QString message = QString("At %1:%2: %3 item is not allowed inside %4 item.")
                        .arg(context.file())
                        .arg(context.line())
                        .arg(child->metaObject()->className())
                        .arg(item->baseTypeInfo()->className());
            m_resolver.appendError(message);
        }
    }
}

void HierarchyValidator::validateParentAllowed(QstItem* item, const TypeList& allowedParents, DocumentRootMode rootMode)
{
    bool ok = false;
    if (item->parent() == nullptr)
    {
        if (rootMode == DocumentRootAllowed)
        {
            ok = true;
        }
    }
    else
    {
        for (const auto type: allowedParents)
        {
            if (item->parent()->metaObject()->inherits(type))
            {
                ok = true;
                break;
            }
        }
    }
    if (!ok)
    {
        QmlContext context = qst::qmlDefinitionContext(item);
        QString message;

        if (item->parent() == nullptr)
        {
            message = QString("At %1:%2. %3 cannot be document root.")
                    .arg(context.file())
                    .arg(context.line())
                    .arg(item->metaObject()->className());
        }
        else
        {
            message = QString("At %1:%2. %3 item is not allowed inside %4 item.")
                    .arg(context.file())
                    .arg(context.line())
                    .arg(item->metaObject()->className())
                    .arg(item->parent()->metaObject()->className());
        }

        m_resolver.appendError(message);
    }
}

void HierarchyValidator::visit(Component* item)
{
    validateChildrenAllowed(item, componentChildren);
    validateParentAllowed(item, componentParents, componentRootMode);
}

void HierarchyValidator::visit(Depends* item)
{
    validateChildrenAllowed(item, dependsChildren);
    validateParentAllowed(item, dependsParents, dependsRootMode);
}

void HierarchyValidator::visit(Dimension* item)
{
    validateChildrenAllowed(item, dimensionChildren);
    validateParentAllowed(item, dimensionParents, dimensionRootMode);
}

void HierarchyValidator::visit(Exports* item)
{
    validateChildrenAllowed(item, exportsChildren);
    validateParentAllowed(item, exportsParents, exportsRootMode);
}

void HierarchyValidator::visit(Matrix* item)
{
    validateChildrenAllowed(item, matrixChildren);
    validateParentAllowed(item, matrixParents, matrixRootMode);
}

void HierarchyValidator::visit(Project* item)
{
    validateChildrenAllowed(item, projectChildren);
    validateParentAllowed(item, projectParents, projectRootMode);
}

void HierarchyValidator::visit(Testcase* item)
{
    validateChildrenAllowed(item, testcaseChildren);
    validateParentAllowed(item, testcaseParents, testcaseRootMode);
}
