#include "dimension.h"
#include "qst.h"

#include <QtDebug>

int i = 0;

Dimension::Dimension(QObject* parent) : QObject(parent)
{
    if (i == 0) {
        m_data << QVariantMap { {"attribute", "black"} };
        m_data << QVariantMap { {"attribute", "brown"} };
    }
    else if (i == 1)
    {
        m_data << QVariantMap { {"animal", "dog"} };
        m_data << QVariantMap { {"animal", "cat"} };
    }
    else
    {
        m_data << QVariantMap { {"op", "sleeps"} };
        m_data << QVariantMap { {"op", "eats"} };
        m_data << QVariantMap { {"op", "studies animal rights"} };
    }
    i = (i+1) % 3;
}

const QVector<QVariantMap>& Dimension::data() const
{
    return m_data;
}

int Dimension::length() const
{
    return m_data.length();
}

void Dimension::handleParserEvent(ParserEventHandler::ParserEvent event)
{
    switch (event)
    {
    case ParserEventHandler::AfterClassBegin:
        if (!m_children.isEmpty())
        {
            QmlContext ourContext = qst::qmlDefinitionContext(this);
            QmlContext childContext = qst::qmlDefinitionContext(m_children.first());
            auto message = QString("At %1:%2: Dimension item defined at %3:%4 cannot contain other items.")
                    .arg(childContext.file()).arg(childContext.line())
                    .arg(ourContext.file()).arg(ourContext.line());
            QST_ERROR_AND_EXIT(message);
        }
        break;
    case ParserEventHandler::AfterComponentComplete:
        break;
    default:
        break;
    }
}

void Dimension::setTestcases(const QStringList& names)
{
    m_testcases = names;
    emit testcasesChanged();
}
