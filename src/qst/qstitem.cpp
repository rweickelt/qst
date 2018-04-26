#include "qstitem.h"
#include "projectresolver.h"
#include "qst.h"

QstItem::QstItem(const QMetaObject* basetypeInfo, QObject* parent)
    : QObject(parent)
{
    m_allowedChildTypes = QList<const QMetaObject*>{ &QObject::staticMetaObject };
    m_allowedParentTypes = QList<const QMetaObject*>{ &QObject::staticMetaObject, nullptr };
    m_canBeDocumentRoot = true;
    m_basetypeInfo = basetypeInfo;
}

void QstItem::afterClassBegin()
{
    verifyParentAllowed();
    verifyChildrenAllowed();
    this->handleParserEvent(AfterClassBegin);
}

void QstItem::afterComponentComplete()
{
    this->handleParserEvent(AfterComponentComplete);
}

void QstItem::classBegin()
{
    ProjectResolver::instance()->currentDocument()->handlers.append(this);

    handleParserEvent(ClassBegin);
}

void QstItem::componentComplete()
{
    handleParserEvent(ComponentComplete);
}

void QstItem::setAllowedNestedTypes(const QList<const QMetaObject*>& types)
{
    m_allowedChildTypes = types;
    Q_ASSERT(!m_allowedChildTypes.contains(nullptr));
}

void QstItem::setAllowedParentTypes(const QList<const QMetaObject*>& types)
{
    m_allowedParentTypes = types;
    if (m_allowedParentTypes.contains(nullptr))
    {
        m_allowedParentTypes.removeAll(nullptr);
        m_canBeDocumentRoot = true;
    }
    else
    {
        m_canBeDocumentRoot = false;
    }
}

void QstItem::verifyChildrenAllowed()
{
    if ((m_allowedChildTypes.isEmpty()) && (!m_children.isEmpty()))
    {
        QmlContext context = qst::qmlDefinitionContext(m_children.first());
        QString message = QString("At %1:%2: %3 must not contain nested items.")
                    .arg(context.file())
                    .arg(context.line())
                    .arg(m_basetypeInfo->className());
        QST_ERROR_AND_EXIT(message);
    }

    for (const auto child: m_children)
    {
        bool ok = false;
        for (const auto type: m_allowedChildTypes)
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
                        .arg(m_basetypeInfo->className());
            QST_ERROR_AND_EXIT(message);
        }
    }
}

void QstItem::verifyParentAllowed()
{
    bool ok = false;
    if (parent() == nullptr)
    {
        if (m_canBeDocumentRoot)
        {
            ok = true;
        }
    }
    else
    {
        for (const auto type: m_allowedParentTypes)
        {
            if (parent()->metaObject()->inherits(type))
            {
                ok = true;
                break;
            }
        }
    }
    if (!ok)
    {
        QmlContext context = qst::qmlDefinitionContext(this);
        QString message;

        if (parent() == nullptr)
        {
            message = QString("At %1:%2. %3 cannot be document root.")
                    .arg(context.file())
                    .arg(context.line())
                    .arg(this->metaObject()->className());
        }
        else
        {
            message = QString("At %1:%2. %3 is not allowed inside %4")
                    .arg(context.file())
                    .arg(context.line())
                    .arg(this->metaObject()->className())
                    .arg(parent()->metaObject()->className());
        }

        QST_ERROR_AND_EXIT(message);
    }
}
