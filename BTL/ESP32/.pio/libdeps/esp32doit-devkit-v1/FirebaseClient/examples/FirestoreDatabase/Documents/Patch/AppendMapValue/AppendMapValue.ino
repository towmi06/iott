/**
 * ABOUT:
 *
 * The example to append the map values to the Firestore document using patch.
 *
 * This example uses the UserAuth class for authentication.
 * See examples/App/AppInitialization for more authentication examples.
 *
 * The complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 *
 * SYNTAX:
 *
 * 1.------------------------
 *
 * Firestore::Documents::patch(<AsyncClient>, <Firestore::Parent>, <documentPath>, <DocumentMask(updateMask)>, <DocumentMask(mask)>, <Document>, <Precondition(currentDocument)>, <AsyncResultCallback>, <uid>);
 *
 * <AsyncClient> - The async client.
 * <Firestore::Parent> - The Firestore::Parent object included project Id and database Id in its constructor.
 * <documentPath> - The relative path of document to patch with the input document.
 * <DocumentMask(updateMask)> - The fields to update. If the document exists on the server and has fields not referenced in the mask, they are left unchanged.
 * <DocumentMask(mask)> - The fields to return. If not set, returns all fields. If the document has a field that is not present in this mask, that field will
 * not be returned in the response. Use comma (,) to separate between the field names.
 * <Document> - The Firestore document.
 * <Precondition(currentDocument)> - An optional precondition on the document. The request will fail if this is set and not met by the target document.
 * <AsyncResultCallback> - The async result callback (AsyncResultCallback).
 * <uid> - The user specified UID of async result (optional).
 *
 * The Firebase project Id should be only the name without the firebaseio.com.
 * The Firestore database id should be (default) or empty "".
 */

#include <Arduino.h>
#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define API_KEY "Web_API_KEY"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"
#define FIREBASE_PROJECT_ID "PROJECT_ID"

void processData(AsyncResult &aResult);
void append_map_value_async(const String &documentPath, PatchDocumentOptions &patchOptions, Document<Values::Value> &doc);
void append_map_value_async2(const String &documentPath, PatchDocumentOptions &patchOptions, Document<Values::Value> &doc);
void append_map_value_await(const String &documentPath, PatchDocumentOptions &patchOptions, Document<Values::Value> &doc);

SSL_CLIENT ssl_client;

// This uses built-in core WiFi/Ethernet for network connection.
// See examples/App/NetworkInterfaces for more network examples.
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<3600) */);
FirebaseApp app;

Firestore::Documents Docs;

AsyncResult firestoreResult;

int cnt = 0;

unsigned long dataMillis = 0;

void setup()
{
    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    set_ssl_client_insecure_and_buffer(ssl_client);

    Serial.println("Initializing app...");
    initializeApp(aClient, app, getAuth(user_auth), auth_debug_print, "🔐 authTask");

    // Or intialize the app and wait.
    // initializeApp(aClient, app, getAuth(user_auth), 120 * 1000, auth_debug_print);

    app.getApp<Firestore::Documents>(Docs);
}

void loop()
{
    // To maintain the authentication and async tasks
    app.loop();

    if (app.ready() && (millis() - dataMillis > 60000 || dataMillis == 0))
    {
        dataMillis = millis();
        cnt++;

        // test_collection is the collection id, test_document is the document id.
        String documentPath = "test_collection/test_document";

        // map
        Values::MapValue mapV("key" + String(cnt), Values::StringValue("value" + String(cnt)));

        Document<Values::Value> doc("myMap", Values::Value(mapV));

        // The value of Values::xxxValue, Values::Value and Document can be printed on Serial.

        PatchDocumentOptions patchOptions(DocumentMask("myMap.key" + String(cnt)) /* updateMask */, DocumentMask() /* mask */, Precondition() /* precondition */);

        // You can set the content of doc object directly with doc.setContent("your content")

        append_map_value_async(documentPath, patchOptions, doc);

        // append_map_value_async2(documentPath, patchOptions, doc);

        // append_map_value_await(documentPath, patchOptions, doc);
    }

    // For async call with AsyncResult.
    processData(firestoreResult);
}

void processData(AsyncResult &aResult)
{
    // Exits when no result available when calling from the loop.
    if (!aResult.isResult())
        return;

    if (aResult.isEvent())
    {
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());
    }

    if (aResult.isDebug())
    {
        Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());
    }

    if (aResult.isError())
    {
        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());
    }

    if (aResult.available())
    {
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
    }
}

void append_map_value_async(const String &documentPath, PatchDocumentOptions &patchOptions, Document<Values::Value> &doc)
{
    Serial.println("Appending the map values to the document... ");

    // Async call with callback function.
    Docs.patch(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, patchOptions, doc, processData, "patchTask");
}

void append_map_value_async2(const String &documentPath, PatchDocumentOptions &patchOptions, Document<Values::Value> &doc)
{
    Serial.println("Appending the map values to the document... ");

    // Async call with AsyncResult for returning result.
    Docs.patch(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, patchOptions, doc, firestoreResult);
}

void append_map_value_await(const String &documentPath, PatchDocumentOptions &patchOptions, Document<Values::Value> &doc)
{
    Serial.println("Appending the map values to the document... ");

    // Sync call which waits until the payload was received.
    String payload = Docs.patch(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, patchOptions, doc);
    if (aClient.lastError().code() == 0)
        Serial.println(payload);
    else
        Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
}
