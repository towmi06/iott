#ifndef CORE_ASYNC_CLIENT_ASYNC_TCP_CONFIG_H
#define CORE_ASYNC_CLIENT_ASYNC_TCP_CONFIG_H

#include <Arduino.h>
#include "./FirebaseConfig.h"

#if defined(ENABLE_ASYNC_TCP_CLIENT)
/**
 * Async TCP Client Connection Request Callback.
 * @param host The host to connect.
 * @param port The port to connect.
 */
typedef void (*AsyncTCPConnect)(const char *host, uint16_t port);

/**
 * Async TCP Client Connection Status Callback.
 * @param status The server connection status. (set by TCP client)
 *
 * The TCP client should set the server connection status in status.
 */
typedef void (*AsyncTCPStatus)(bool &status);

/**
 * Async TCP Client Send Request Callback.
 * @param data The data to send.
 * @param size The size of data to send.
 * @param sent The data sent amount (0 for failure). (set by TCP client)
 *
 * The TCP client should send the data out by the size provided, and set the amount of sent data in sent.
 *
 * If connection or any sending failure, sets sent with 0.
 *
 */
typedef void (*AsyncTCPSend)(uint8_t *data, size_t size, uint32_t &sent);

/**
 * Async TCP Client Receive Request Callback.
 * @param buff The buffer to return (with copy) the received data.
 * @param buffSize The buffer size available.
 * @param filledSize The amount of data that return (0 for no data, -1 for failure) (set by TCP client).
 * @param available The remaining data that is available to read (set by TCP client).
 *
 * If data is available from TCP client, fills (copies) the data to buffer within the buffSize.
 * And set the amount of filled data in filledSize, and the remaining data amount in available.
 *
 * If no data is available, TCP client should set 0 to available.
 * If connection or any receiving failure, sets filledSize with -1.
 */
typedef void (*AsyncTCPReceive)(uint8_t *buff, size_t buffSize, int32_t &filledSize, uint32_t &available);

/**
 * Async TCP Client Connection Stop Request Callback.
 *
 * The TCP client should stop the server connection.
 */
typedef void (*AsyncTCPStop)();

class AsyncTCPConfig
{
    friend class AsyncClientClass;
    friend class async_request_handler_t;
    friend class async_response_handler_t;

private:
    // Async TCP Client Connection Request Callback.
    AsyncTCPConnect tcpConnect = NULL;
    // Async TCP Client Connection Status Callback.
    AsyncTCPStatus tcpStatus = NULL;
    // Async TCP Client Send Request Callback.
    AsyncTCPSend tcpSend = NULL;
    // Async TCP Client Receive Request Callback.
    AsyncTCPReceive tcpReceive = NULL;
    // Async TCP Client Connection Stop Request Callback.
    AsyncTCPStop tcpStop = NULL;

    int buffPos = 0;
    uint8_t buff[100];
    int32_t filledSize = 0;
    uint32_t available = 0;

public:
    /**
     * @param tcpConnect Async TCP Client Connection Request Callback.
     * @param tcpStatus Async TCP Client Connection Status Callback.
     * @param tcpSend Async TCP Client Send Request Callback.
     * @param tcpReceive Async TCP Client Receive Request Callback.
     * @param tcpStop Async TCP Client Connection Stop Request Callback.
     *
     */
    explicit AsyncTCPConfig(AsyncTCPConnect tcpConnect, AsyncTCPStatus tcpStatus, AsyncTCPSend tcpSend, AsyncTCPReceive tcpReceive, AsyncTCPStop tcpStop)
    {
        this->tcpConnect = tcpConnect;
        this->tcpStatus = tcpStatus;
        this->tcpSend = tcpSend;
        this->tcpReceive = tcpReceive;
        this->tcpStop = tcpStop;
    };
    ~AsyncTCPConfig() {}
};

#endif
#endif