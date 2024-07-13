#include "WiFiS3.h"
#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>
#include "arduino_secrets.h"
#include <WebSocketClient.h>
#include "functions.h"

char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;
// const uint16_t websockets_server_port = 8000; // Cambiar por tu puerto
// String websocket_path = "/ws/"; // Cambiar por tu ruta, asegúrate de incluir el token
using namespace net;
WebSocketClient client;

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
  printWifiStatusGraphics();

  client.onOpen([](WebSocket &ws) {
    const char message[]{ "Hello from Arduino client!" };
    ws.send(WebSocket::DataType::TEXT, message, strlen(message));
  });
  client.onClose([](WebSocket &ws, const WebSocket::CloseCode code,
                   const char *reason, uint16_t length) {
    // ...
  });
  client.onMessage([](WebSocket &ws, const WebSocket::DataType dataType,
                     const char *message, uint16_t length) {
    // ...
  });

  client.open("ws://0.0.0.0:8000/ws/eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6InVzZXIxIiwiZXhwIjoxNzE5MjgzNTgzLjU4NTIwNjV9.xzLdPKxEHYXbGaEnXtHuUsHEC-fiytF1modDW3-PE38", 80);
}

void loop() {
  client.listen();
}
