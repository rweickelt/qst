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

#ifndef QSTITEMVISITOR_H
#define QSTITEMVISITOR_H

#include <QtCore/QtGlobal>

class Component;
class Depends;
class Dimension;
class Exports;
class Matrix;
class Project;
class QstItem;
class ResourceItem;
class Testcase;

class QstItemVisitor
{
    friend class Component;
    friend class Depends;
    friend class Exports;
    friend class Dimension;
    friend class Matrix;
    friend class Project;
    friend class QstItem;
    friend class ResourceItem;
    friend class Testcase;

public:
    virtual ~QstItemVisitor();

protected:
    virtual void visit(Component* item) { Q_UNUSED(item) }
    virtual void visit(Depends* item) { Q_UNUSED(item) }
    virtual void visit(Dimension* item) { Q_UNUSED(item) }
    virtual void visit(Exports* item) { Q_UNUSED(item) }
    virtual void visit(Matrix* item) { Q_UNUSED(item) }
    virtual void visit(Project* item) { Q_UNUSED(item) }
    virtual void visit(ResourceItem* item) { Q_UNUSED(item) }
    virtual void visit(Testcase* item) { Q_UNUSED(item) }
};

#endif // QSTITEMVISITOR_H
