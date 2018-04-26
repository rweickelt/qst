#ifndef PARSERSTATEHANDLER_H
#define PARSERSTATEHANDLER_H

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtQml/QQmlListProperty>
#include <QtQml/QQmlParserStatus>

class QstItem : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_DISABLE_COPY(QstItem)

    Q_PROPERTY(QQmlListProperty<QObject> children READ children CONSTANT)
    Q_CLASSINFO("DefaultProperty", "children")

    friend class ProjectResolver;

public:
    QstItem(const QMetaObject* basetypeInfo, QObject* parent = nullptr);

protected:
    enum ParserEvent {
        AfterClassBegin,
        AfterComponentComplete,
        ClassBegin,
        ComponentComplete
    };

    const QList<const QMetaObject*>& allowedChildTypes() const;
    const QList<const QMetaObject*>& allowedParentTypes() const;
    QQmlListProperty<QObject> children();

    virtual void handleParserEvent(ParserEvent event) = 0;

    void setAllowedNestedTypes(const QList<const QMetaObject*>& types);
    void setAllowedParentTypes(const QList<const QMetaObject*>& typenames);

    QList<QObject*> m_children;

private:
    void afterClassBegin();
    void afterComponentComplete();
    virtual void classBegin() final;
    virtual void componentComplete() final;
    void verifyChildrenAllowed();
    void verifyParentAllowed();

    QList<const QMetaObject*> m_allowedChildTypes;
    QList<const QMetaObject*> m_allowedParentTypes;
    bool m_canBeDocumentRoot;
    const QMetaObject* m_basetypeInfo;
};

inline const QList<const QMetaObject*>& QstItem::allowedChildTypes() const { return m_allowedChildTypes; }
inline const QList<const QMetaObject*>& QstItem::allowedParentTypes() const { return m_allowedParentTypes; }
inline QQmlListProperty<QObject> QstItem::children() { return QQmlListProperty<QObject>(this, m_children); }


#endif // PARSERSTATEHANDLER_H
