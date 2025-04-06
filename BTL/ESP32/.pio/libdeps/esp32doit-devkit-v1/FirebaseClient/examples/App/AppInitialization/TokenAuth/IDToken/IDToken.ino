/**
 * ABOUT:
 *
 * The bare minimum example for using user authentication with ID token.
 *
 * The complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 *
 * SYNTAX:
 *
 * 1.------------------------
 *
 * IDToken::IDToken(<api_key>, <ID_token>, <expire_in_seconds>, <refresh_token>);
 * IDToken::IDToken(<file_config_data>);
 * IDToken::save(<file_config_data>);
 *
 * <api_key> - API key can be obtained from Firebase console > Project Overview > Project settings.
 * <ID_token> - The ID token.
 * <expire_in_seconds> - Expire period in seconds (less than 3600), 3300 is the default value.
 * <refresh_token> - Refresh token.
 * <file_config_data> -  The filesystem data (file_config_data) obtained from FileConfig class object of file that the IDToken credentials will be saved to or read from.
 *
 * By leaving auth_token empty and assign the refresh token, the auth token will be re-authenticated when calling initializeApp.
 *
 * 2.------------------------
 *
 * initializeApp(<AsyncClient>, <FirebaseApp>, <user_auth_data>, <AsyncResultCallback>, <uid>);
 *
 * <AsyncClient> - The async client.
 * <FirebaseApp> - The authentication and access token handler.
 * <user_auth_data> - The user auth data (user_auth_data struct) that holds the user input sign-in credentials and token.
 *
 * The <user_auth_data> can be obtained from the following sign-in credentials, access key, auth token providers classs via getAuth function i.e.
 * CustomAuth, ServiceAuth, UserAuth, NoAuth, CustomToken, AccessToken, IDToken, LegacyToken.
 *
 * <AsyncResultCallback> - The AsyncResultCallback function for debugging.
 * <uid> - Optional The user defined task UID for debugging.
 *
 * 3.------------------------
 *
 * initializeApp(<AsyncClient>, <FirebaseApp>, <user_auth_data>, <timeoutMs>, <AsyncResultCallback>);
 *
 * <AsyncClient> - The async client.
 * <FirebaseApp> - The authentication and access token handler.
 * <user_auth_data> - The user auth data (user_auth_data struct) that holds the user input sign-in credentials and token.
 *
 * The <user_auth_data> can be obtained from the following sign-in credentials, access key, auth token providers classs via getAuth function i.e.
 * CustomAuth, ServiceAuth, UserAuth, NoAuth, CustomToken, AccessToken, IDToken, LegacyToken.
 *
 * <timeoutMs> - Optional. The await timeout in milliseconds.
 * <AsyncResultCallback> - Optional. The async result callback (AsyncResultCallback) for debugging.
 *
 * NOTE:
 *
 * The timeoutMs is the timeout for waiting the authentication process to be done.
 *
 * By providing AsyncResultCallback in the initializeApp function, the debug information will be collected and send to AsyncResultCallback
 * function immediately.
 */

#include <Arduino.h>
#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define API_KEY "Web_API_KEY"

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

IDToken id_token(API_KEY, "ID token", 3000 /* expire period in seconds (<3600) */, "refresh");

FirebaseApp app;

SSL_CLIENT ssl_client;

// This uses built-in core WiFi/Ethernet for network connection.
// See examples/App/NetworkInterfaces for more network examples.
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

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
    initializeApp(aClient, app, getAuth(id_token), auth_debug_print, "authToken");

    // Or intialize the app and wait.
    // initializeApp(aClient, app, getAuth(id_token), 120 * 1000, auth_debug_print);
}

void loop()
{
    // To maintain the authentication and async tasks.
    app.loop();
}