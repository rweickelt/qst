#ifndef TESTJOB_H
#define TESTJOB_H

#include "tag.h"

#include <QtCore/QSharedPointer>
#include <QtCore/QVariantMap>

class Testcase;

/* TestJobs are testcase-data tuples created by MatrixExpander
   and executed by TestRunner.
 */
struct TestJob {
    Testcase* testcase = nullptr;
    TagGroupId tagGroupId = InvalidId;
    TagId tagId = InvalidId;

    static TestJob fromTestcase(Testcase* testcase, TagGroupId tagGroupId = InvalidId, TagId tagId = InvalidId);
};

#endif // TESTJOB_H
