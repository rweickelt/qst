#ifndef TESTJOB_H
#define TESTJOB_H

#include <QtCore/QSharedPointer>
#include <QtCore/QVariantMap>

class Testcase;

/* TestJobs are testcase-data tuples created by MatrixExpander
   and executed by TestRunner.
 */
struct TestJob {
    Testcase* testcase = nullptr;
    int tagEntry = -1;
};

#endif // TESTJOB_H
