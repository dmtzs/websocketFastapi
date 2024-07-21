#include "WiFiS3.h"
#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>
#include "arduino_secrets.h"
#include "functions.h"

char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

const char* host = "192.168.50.219";         // Reemplaza con la IP de tu servidor
const int port = 8000;
const char* token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6InVzZXIxIiwiZXhwIjoxNzIxMzQ5NDQwLjQ3NjYyNzh9.tLMFVodnRan5lv1C5q1qvheuQEQxNK2kX9HA0c1udXc";         // Reemplaza con el token obtenido

WiFiClient client;

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
  //printWifiStatusGraphics();

  // Conexión al servidor WebSocket
  if (!client.connect(host, port)) {
    Serial.println("Conexión fallida");
    return;
  }

  // Enviar solicitud WebSocket
  String handshake = "GET /ws/" + String(token) + " HTTP/1.1\r\n";
  handshake += "Host: " + String(host) + ":" + String(port) + "\r\n";
  handshake += "Upgrade: websocket\r\n";
  handshake += "Connection: Upgrade\r\n";
  handshake += "Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==\r\n";
  handshake += "Sec-WebSocket-Version: 13\r\n\r\n";

  client.print(handshake);

  // Leer la respuesta del servidor
  while (client.connected() && !client.available()) {
    delay(100);
  }

  while (client.available()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);
  }
}

void loop() {
  // Ejemplo de cómo enviar un mensaje al servidor WebSocket
  if (client.connected()) {
    String message = "{\"topic\": \"topic1\", \"content\": \"Hola desde Arduino\"}";
    client.print(message);
  }

  delay(5000); // Espera 5 segundos antes de enviar el siguiente mensaje

  // Leer mensajes del servid
  while (client.available()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);
  }
}
