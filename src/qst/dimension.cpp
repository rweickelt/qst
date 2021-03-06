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
#include "qst.h"
#include "qstitemvisitor.h"

#include <QtDebug>

#include <QtQml/QJSValue>
#include <QtQml/private/qjsvalue_p.h>

int i = 0;

Dimension::Dimension(QObject* parent) : QstItem(parent)
{
}

void Dimension::callVisitor(QstItemVisitor* visitor)
{
    visitor->visit(this);
}


const QVector<QVariantMap>& Dimension::data() const
{
    return m_data;
}

int Dimension::length() const
{
    return m_data.length();
}

void Dimension::handleParserEvent(QstItem::ParserEvent event)
{
    switch (event)
    {
    case QstItem::AfterComponentComplete:
        if (m_data.length() == 0)
        {
            QmlContext context = qst::qmlDefinitionContext(this);
            auto message = QString("Dimension item defined at %1:%2 must contain at least 1 tag.")
                    .arg(context.file()).arg(context.line());
            QST_ERROR_AND_EXIT(message);
        }
        break;
    default:
        break;
    }
}

void Dimension::setData(const QVector<QVariantMap>& data)
{
    m_data = data;
}

void Dimension::setTestcases(const QStringList& names)
{
    if (m_testcases != names)
    {
        m_testcases = names;
        emit testcasesChanged();
    }
}

void DimensionParser::verifyBindings(const QV4::CompiledData::Unit* qmlUnit, const QList<const QV4::CompiledData::Binding*>& props)
{
    QSet<QString> names;
    for (const auto& binding: props) {
        QString propName = qmlUnit->stringAt(binding->propertyNameIndex);

        if (names.contains(propName))
        {
            error(binding, "A dimension cannot have multiple tags with the same name.");
            return;
        }
        else
        {
            names << propName;
        }

        if (binding->type == QV4::CompiledData::Binding::Type_Script)
        {
            QString script = binding->valueAsScriptString(qmlUnit);
            if (definesEmptyList(script))
            {
                error(binding, "List must not be empty.");
                return;
            }
        }
        else if (binding->type >= QV4::CompiledData::Binding::Type_Object)
        {
            error(binding, "Type is not allowed here. Only plain properties or lists can be used as tags.");
            return;
        }
    }
}

void DimensionParser::applyBindings(QObject* object, QV4::CompiledData::CompilationUnit* compilationUnit, const QList<const QV4::CompiledData::Binding*>& bindings)
{
    Dimension* dimension = qobject_cast<Dimension*>(object);
    QV4::ExecutionEngine* v4 = compilationUnit->engine;
    QV4::Scope scope(v4);

    QMap<QString, QVariantList> values;
    QMap<QString, const QV4::CompiledData::Binding*> bindingsByName;
    QMap<QString, int> arrayLengths;

    for (const auto& binding: bindings) {
        QString propName = compilationUnit->stringAt(binding->propertyNameIndex);
        bindingsByName[propName] = binding;

        QQmlBinding::Identifier id = binding->value.compiledScriptIndex;
        Q_ASSERT(id != QQmlBinding::Invalid);

        if (binding->type == QV4::CompiledData::Binding::Type_Boolean)
        {
            values[propName] << binding->valueAsBoolean();
        }
        else if (binding->type == QV4::CompiledData::Binding::Type_Number)
        {
            values[propName] << binding->valueAsNumber();
        }
        else if (binding->type == QV4::CompiledData::Binding::Type_String)
        {
            values[propName] << binding->valueAsString(compilationUnit->data);
        }
        else if (binding->type == QV4::CompiledData::Binding::Type_Script)
        {
            QString script = binding->valueAsScriptString(compilationUnit->data);
            QQmlExpression expression(qmlContext(dimension), nullptr, script);

            bool undefined = false;
            QVariant result = expression.evaluate(&undefined);

            if (!result.isValid() || undefined)
            {
                QString file = QUrl(compilationUnit->fileName()).path();
                int line = binding->location.line;
                QST_ERROR_AND_EXIT(QString("Unable to parse value at %1:%2. Only basic types and lists of basic types can be used as tags.")
                                   .arg(file).arg(line));
            }

            switch (result.type())
            {
            case QMetaType::Bool:
            case QMetaType::Int:
            case QMetaType::QString:
                values[propName] << result;
                break;
            case QMetaType::User: // qMetaTypeId<QJSValue>() does not work here for unknown reasons
            {
                QJSValue jsValue = result.value<QJSValue>();
                // Fails of this assert are unexpected and should be investigated.
                Q_ASSERT(jsValue.isArray());
                QVariantList array = jsValue.toVariant().toList();
                values[propName] = array;
                arrayLengths[propName] = array.length();
            }
                break;
            default:
                // Fails of this assert are unexpected and should be investigated.
                Q_ASSERT(false);
                break;
            }
        }
    }

    if (arrayLengths.values().toSet().size() > 1)
    {
        QmlContext context = qst::qmlDefinitionContext(dimension);
        QST_ERROR_AND_EXIT(QString("The tag arrays (%1) defined at %2:%3 must have equal length.")
                           .arg(arrayLengths.keys().join(", "))
                           .arg(context.file()).arg(context.line()));
    }

    /* Transform map of arrays into an array of maps.
     * If the array contains only one value, duplicate it.
     */
    QVector<QVariantMap> data(arrayLengths.values().value(0, 1));
    for (int i = 0; i < data.length(); i++)
    {
        for (const auto& name: values.keys())
        {
            const auto& array = values[name];
            data[i][name] = array.value(i, array.first());
        }
    }

    dimension->setData(data);
}

bool DimensionParser::definesEmptyList(const QString& script)
{
    if (script.startsWith(QLatin1Char('[')) && script.endsWith(QLatin1Char(']'))) {
        for (int i = 1; i < script.length() - 1; i++) {
            if (!script[i].isSpace())
                return false;
        }
        return true;
    }
    return false;
}
