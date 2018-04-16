#ifndef PARSERSTATEHANDLER_H
#define PARSERSTATEHANDLER_H

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
};

inline void ParserEventHandler::afterClassBegin() { handleParserEvent(AfterClassBegin); }
inline void ParserEventHandler::afterComponentComplete() { handleParserEvent(AfterComponentComplete); }
inline void ParserEventHandler::classBegin() { handleParserEvent(ClassBegin); }
inline void ParserEventHandler::componentComplete() { handleParserEvent(ComponentComplete); }

#endif // PARSERSTATEHANDLER_H
