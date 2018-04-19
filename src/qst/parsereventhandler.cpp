#include "parsereventhandler.h"
#include "projectresolver.h"

void ParserEventHandler::afterClassBegin()
{
    handleParserEvent(AfterClassBegin);
}

void ParserEventHandler::afterComponentComplete()
{
    handleParserEvent(AfterComponentComplete);
}

void ParserEventHandler::classBegin()
{
    ProjectResolver::instance()->currentDocument()->handlers.append(this);

    handleParserEvent(ClassBegin);
}

void ParserEventHandler::componentComplete()
{
    handleParserEvent(ComponentComplete);
}

