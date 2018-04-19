#ifndef PARSERSTATEHANDLER_H
#define PARSERSTATEHANDLER_H

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtQml/QQmlParserStatus>

class ParserEventHandler : public QQmlParserStatus
{
    Q_INTERFACES(QQmlParserStatus)

public:
    void afterClassBegin();
    void afterComponentComplete();
    void classBegin() final;
    void componentComplete() final;

protected:
    enum ParserEvent {
        AfterClassBegin,
        AfterComponentComplete,
        ClassBegin,
        ComponentComplete
    };

    virtual void handleParserEvent(ParserEvent event) = 0;

private:
    QList<QMetaObject*> m_allowedChildrenTypes;
    QList<QMetaObject*> m_allowedParentTypes;
};

#endif // PARSERSTATEHANDLER_H
