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

#include "qstitem.h"
#include "projectresolver.h"
#include "qst.h"

#include <QtDebug>

QstItem::QstItem(QObject* parent) : QObject(parent)
{
}

void QstItem::accept(QstItemVisitor* visitor)
{
    this->callVisitor(visitor);
    for (QstItem* child: this->findChildren<QstItem*>(QString(), Qt::FindDirectChildrenOnly))
    {
        child->accept(visitor);
    }
}

void QstItem::afterClassBegin()
{
    this->handleParserEvent(AfterClassBegin);
}

void QstItem::afterComponentComplete()
{
    this->handleParserEvent(AfterComponentComplete);
}

void QstItem::classBegin()
{
//    ProjectResolver::instance()->currentDocument()->handlers.append(this);

    handleParserEvent(ClassBegin);
}

void QstItem::componentComplete()
{
    handleParserEvent(ComponentComplete);
}
