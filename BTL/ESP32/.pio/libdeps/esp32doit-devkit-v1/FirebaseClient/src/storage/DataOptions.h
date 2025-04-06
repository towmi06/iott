/**
 * 2025-02-13
 *
 * The MIT License (MIT)
 * Copyright (c) 2025 K. Suwatchai (Mobizt)
 *
 *
 * Permission is hereby granted, free of charge, to any person returning a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef STORAGE_DATA_OPTIONS_H
#define STORAGE_DATA_OPTIONS_H

#include <Arduino.h>
#include "./FirebaseConfig.h"
#include "./core/Utils/JSON.h"
#include "./core/Utils/ObjectWriter.h"
#include "./core/AsyncClient/AsyncClient.h"

#if defined(ENABLE_STORAGE)
namespace FirebaseStorage
{
    enum firebase_storage_request_type
    {
        fs_undefined,
        fs_upload,
        fs_download,
        fs_get_meta,
        fs_delete,
        fs_list,
        fs_download_ota
    };

    class Parent
    {
        friend class Storage;

    private:
        String bucketId, object, accessToken;

    public:
        Parent() {}
        explicit Parent(const String &bucketId, const String &object = "", const String &accessToken = "")
        {
            URLUtil uut;
            this->bucketId = uut.getHost(bucketId);
            this->object = object.length() && object[0] == '/' ? object.substring(1, object.length() - 1) : object;
            this->accessToken = accessToken;
        }
        String getObject() const { return object; }
        String getBucketId() const { return bucketId; }
        const char *getAccessToken() const { return accessToken.c_str(); }
    };

    class DataOptions
    {
    public:
        String payload, extras;
        FirebaseStorage::Parent parent;
        firebase_storage_request_type requestType = fs_undefined;
        unsigned long requestTime = 0;

        void copy(const DataOptions &rhs)
        {
            this->extras = rhs.extras;
            this->payload = rhs.payload;
        }

    private:
    };

    struct req_data
    {
    public:
        AsyncClientClass *aClient = nullptr;
        String path, uid, mime;
        reqns::http_request_method method = reqns::http_undefined;
        slot_options_t opt;
        DataOptions *options = nullptr;
        file_config_data *file = nullptr;
        AsyncResult *aResult = nullptr;
        AsyncResultCallback cb = NULL;
        req_data() {}
        explicit req_data(AsyncClientClass *aClient, reqns::http_request_method method, slot_options_t opt, DataOptions *options, file_config_data *file, AsyncResult *aResult, AsyncResultCallback cb, const String &uid = "")
        {
            this->aClient = aClient;
            this->method = method;
            this->opt = opt;
            this->options = options;
            this->file = file;
            this->aResult = aResult;
            this->cb = cb;
            this->uid = uid;
        }
    };
}
#endif
#endif