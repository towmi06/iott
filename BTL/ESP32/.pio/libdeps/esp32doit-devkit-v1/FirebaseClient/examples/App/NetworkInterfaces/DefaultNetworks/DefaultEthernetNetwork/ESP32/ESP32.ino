

/**
 * ABOUT:
 *
 * The bare minimum example for working with the ESP32 Arduino Core Ethernet network using ETH.h.
 *
 * The divices used in this example are LAN8720 Ethernet module and ESP32.
 *
 * In case of external LAN8720 module, the following hardeare connection is recommended.
 * Some EV board e.g. Olimex ESP32-EVB board has built with LAN8720 chip on board, the modification is not needed.
 *
 * The modification and interconnection of external LAN8720 module provided in this example are mostly worked as
 * the 50 MHz clock was created internally in ESP32 which GPIO 17 is set to be output of this clock
 * and feeds to the LAN8720 chip XTAL input.
 *
 * The on-board LAN8720 50 MHz XTAL chip will be disabled by connect its enable pin or pin 1 to GND.
 *
 * Pleae see the images in the folder "modified_LAN8720_board_images" for how to modify the LAN8720 board.
 *
 * The LAN8720 Ethernet modified board and ESP32 board wiring connection.
 *
 * ESP32                        LAN8720
 *
 * GPIO17 - EMAC_CLK_OUT_180    nINT/REFCLK - LAN8720 XTAL1/CLKIN     4k7 Pulldown
 * GPIO22 - EMAC_TXD1           TX1
 * GPIO19 - EMAC_TXD0           TX0
 * GPIO21 - EMAC_TX_EN          TX_EN
 * GPIO26 - EMAC_RXD1           RX1
 * GPIO25 - EMAC_RXD0           RX0
 * GPIO27 - EMAC_RX_DV          CRS
 * GPIO23 - MDC                 MDC
 * GPIO18 - MDIO                MDIO
 * GND                          GND
 * 3V3                          VCC
 *
 *
 * The complete usage guidelines, please read README.md or visit https://github.com/mobizt/FirebaseClient
 *
 */

#include <Arduino.h>
#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.

#define API_KEY "Web_API_KEY"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<= 3600) */);

FirebaseApp app;

WiFiClientSecure ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);

#ifdef ETH_CLK_MODE
#undef ETH_CLK_MODE
#endif

// Type of the Ethernet PHY (LAN8720 or TLK110)
#define ETH_PHY_TYPE ETH_PHY_LAN8720

// I²C-address of Ethernet PHY (0 or 1 for LAN8720, 31 for TLK110)
#define ETH_PHY_ADDR 1

// Pin# of the I²C clock signal for the Ethernet PHY
#define ETH_PHY_MDC 23

// Pin# of the I²C IO signal for the Ethernet PHY
#define ETH_PHY_MDIO 18

// Pin# of the enable signal for the external crystal oscillator (-1 to disable)
#define ETH_PHY_POWER -1

// RMII clock output from GPIO17 (for modified LAN8720 module only)
// For LAN8720 built-in, RMII clock input at GPIO 0 from LAN8720 e.g. Olimex ESP32-EVB board
// #define ETH_CLK_MODE ETH_CLOCK_GPIO0_IN
#define ETH_CLK_MODE ETH_CLOCK_GPIO17_OUT

static bool eth_connected = false;

bool firebaseConfigReady = false;

void WiFiEvent(WiFiEvent_t event)
{
    // Do not run any function here to prevent stack overflow or nested interrupt
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 0)

    switch (event)
    {
    case ARDUINO_EVENT_ETH_START:
        Serial.println("ETH Started");
        // set eth hostname here
        ETH.setHostname("esp32-ethernet");
        break;
    case ARDUINO_EVENT_ETH_CONNECTED:
        Serial.println("ETH Connected");
        break;
    case ARDUINO_EVENT_ETH_GOT_IP:
        Serial.print("ETH MAC: ");
        Serial.print(ETH.macAddress());
        Serial.print(", IPv4: ");
        Serial.print(ETH.localIP());
        if (ETH.fullDuplex())
        {
            Serial.print(", FULL_DUPLEX");
        }
        Serial.print(", ");
        Serial.print(ETH.linkSpeed());
        Serial.println("Mbps");
        eth_connected = true;
        break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
        Serial.println("ETH Disconnected");
        eth_connected = false;
        break;
    case ARDUINO_EVENT_ETH_STOP:
        Serial.println("ETH Stopped");
        eth_connected = false;
        break;
    default:
        break;
    }

#else
    switch (event)
    {
    case SYSTEM_EVENT_ETH_START:
        Serial.println("ETH Started");
        // set eth hostname here
        ETH.setHostname("esp32-ethernet");
        break;
    case SYSTEM_EVENT_ETH_CONNECTED:
        Serial.println("ETH Connected");
        break;
    case SYSTEM_EVENT_ETH_GOT_IP:
        Serial.print("ETH MAC: ");
        Serial.print(ETH.macAddress());
        Serial.print(", IPv4: ");
        Serial.print(ETH.localIP());
        if (ETH.fullDuplex())
        {
            Serial.print(", FULL_DUPLEX");
        }
        Serial.print(", ");
        Serial.print(ETH.linkSpeed());
        Serial.println("Mbps");
        eth_connected = true;
        break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
        Serial.println("ETH Disconnected");
        eth_connected = false;
        break;
    case SYSTEM_EVENT_ETH_STOP:
        Serial.println("ETH Stopped");
        eth_connected = false;
        break;
    default:
        break;
    }
#endif
}

void setup()
{
    Serial.begin(115200);

    // This delay is needed in case ETH_CLK_MODE was set to ETH_CLOCK_GPIO0_IN,
    // to allow the external clock source to be ready before initialize the Ethernet.
    delay(500);

    Serial.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    WiFi.onEvent(WiFiEvent);
    ETH.begin(ETH_PHY_TYPE, ETH_PHY_ADDR, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_PHY_POWER, ETH_CLK_MODE);
}

void setConfig()
{
    if (firebaseConfigReady)
        return;

    firebaseConfigReady = true;

    ssl_client.setInsecure();

    Serial.println("Initializing app...");
    initializeApp(aClient, app, getAuth(user_auth), auth_debug_print, "🔐 authTask");

    // Or intialize the app and wait.
    // initializeApp(aClient, app, getAuth(user_auth), 120 * 1000, auth_debug_print);
}

void loop()
{
    if (eth_connected)
    {
        setConfig();
    }

    // To maintain the authentication and async tasks.
    app.loop();
}