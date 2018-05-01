#include "testjob.h"

TestJob TestJob::fromTestcase(Testcase* testcase, TagGroupId tagGroupId, TagId tagId)
{
    return TestJob {
        .testcase = testcase,
        .tagGroupId = tagGroupId,
        .tagId = tagId
    };
}
