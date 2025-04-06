#ifndef CORE_ASYNC_RESULT_RTDB_RESULT_IMPL_H
#define CORE_ASYNC_RESULT_RTDB_RESULT_IMPL_H

#include <Arduino.h>
#include "./core/AsyncResult/AsyncResult.h"

namespace firebase_ns
{
    class RTDBResultImpl : public RealtimeDatabaseResult
    {
        friend class AsyncResult;

    public:
        RTDBResultImpl() {}

    protected:
        void setNullETagOption(RealtimeDatabaseResult *rtdbResult, bool val) { rtdbResult->null_etag = val; }
        bool getNullETagOption(const RealtimeDatabaseResult *rtdbResult) { return rtdbResult->null_etag; }
        void setRefPayload(RealtimeDatabaseResult *rtdbResult, String *payload) { rtdbResult->ref_payload = payload; }
    };
}
#endif