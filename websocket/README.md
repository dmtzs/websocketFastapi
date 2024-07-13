# Bibliotecas
* ArduinoHttpClient
* ArduinoJson
* ArduinoWebsockets

# Código con solo wifi sirviendo
```ino
#include "WiFiS3.h"
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>
#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

const char* websockets_server_host = "0.0.0.0"; // Cambiar por tu host
const uint16_t websockets_server_port = 8000; // Cambiar por tu puerto
String websocket_path = "/ws/"; // Cambiar por tu ruta, asegúrate de incluir el token

WiFiClient client;

void setup() {
  /* -------------------------------------------------------------------------- */  
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
  
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
  
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
     
    // wait 10 seconds for connection:
    delay(10000);
  }
  
  printWifiStatus();

  // Obtener Token (Este paso es simplificado, necesitarás adaptarlo para realizar una petición HTTP POST)
  String token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6InVzZXIxIiwiZXhwIjoxNzE5MjgzNTgzLjU4NTIwNjV9.xzLdPKxEHYXbGaEnXtHuUsHEC-fiytF1modDW3-PE38"; // Aquí deberías tener el token obtenido de tu servidor
  websocket_path += token; // Añadir el token al path del WebSocket

  // Conectar al WebSocket
  client.connect(websockets_server_host, websockets_server_port, websocket_path);

  // Enviar mensaje para suscribirse (ajustar el mensaje según tu protocolo)
  client.send("{\"topic\": \"topic1\", \"content\": \"Subscribing to topic1\"}");

  // Escuchar mensajes
  client.onMessage(handleMessage);
}


void loop() {
  client.poll();
}

void handleMessage(WebsocketsMessage message) {
  Serial.print("Received message: ");
  Serial.println(message.data());
}

void printWifiStatus() {
/* -------------------------------------------------------------------------- */  
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
```

version 2 old:
```ino
#include "WiFiS3.h"
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>
#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

const char* websockets_server_host = "0.0.0.0:8000/ws/eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6InVzZXIxIiwiZXhwIjoxNzE5MjgzNTgzLjU4NTIwNjV9.xzLdPKxEHYXbGaEnXtHuUsHEC-fiytF1modDW3-PE38"; // Cambiar por tu host
// const uint16_t websockets_server_port = 8000; // Cambiar por tu puerto
// String websocket_path = "/ws/"; // Cambiar por tu ruta, asegúrate de incluir el token

using namespace websockets;
WebsocketsClient client;

void onMessageCallback(WebsocketsMessage message) {
    Serial.print("Got Message: ");
    Serial.println(message.data());
}

void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connnection Opened");
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connnection Closed");
    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }
  
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
  
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  
  printWifiStatus();

  // String token = "your_token_here"; // Aquí deberías tener el token obtenido de tu servidor
  // websocket_path += token; // Añadir el token al path del WebSocket

  // Conectar al WebSocket
  client.connect(websockets_server_host);

  // Enviar mensaje para suscribirse
  client.send("{\"topic\": \"topic1\", \"content\": \"Subscribing to topic1\"}");

  // Escuchar mensajes
  client.onMessage(handleMessage);
}

void loop() {
  client.poll();
}

void handleMessage(WebsocketsMessage message) {
  Serial.print("Received message: ");
  Serial.println(message.data());
}

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
```