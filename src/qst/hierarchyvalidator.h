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
#ifndef HIERARCHYVALIDATOR_H
#define HIERARCHYVALIDATOR_H

#include "qstitemvisitor.h"

class QMetaObject;
class QstItem;
class ProjectResolver;

template<typename T> class QList;
typedef QList<const QMetaObject*> TypeList;

class HierarchyValidator : public QstItemVisitor
{
public:
    HierarchyValidator(ProjectResolver& resolver);
    void validate(QstItem* rootItem);

    enum DocumentRootMode
    {
        DocumentRootAllowed,
        DocumentRootDisallowed
    };

protected:
    virtual void visit(Component* item) final;
    virtual void visit(Depends* item) final;
    virtual void visit(Dimension* item) final;
    virtual void visit(Exports* item) final;
    virtual void visit(Matrix* item) final;
    virtual void visit(Project* item) final;
    virtual void visit(Testcase* item) final;

private:
    void validateChildrenAllowed(QstItem* item, const TypeList& allowedChildren);
    void validateParentAllowed(QstItem* item, const TypeList& allowedParents, DocumentRootMode rootMode);

    ProjectResolver& m_resolver;
};

#endif // HIERARCHYVALIDATOR_H
