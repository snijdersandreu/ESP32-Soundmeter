#include <Arduino.h>
#include <WiFi.h>

//Llibreria amb metodes per controlar comunicacions I2C:
#include <Wire.h> 

//Constants per conectar amb el sensor:
#define PCBARTISTS_DBM       0x48
#define I2C_REG_DECIBEL      0x0A

//Constants de conexió al Acces Point
const char* ssid = "Galaxy S22 Ultra 64F7";
const char* password = "hola1234";

//----------------    PARAMS Connexio API    --------------------
const char* host = "147.83.83.21";
const char* token = "847a9b815a70f7b6d9176bb9e746471ccdb9bf22689505752dba31fe0ef567cf";
const char* provider = "grup_4-101@provider_sonometre";
const char* sensor = "Sensor_sonometre";
//---------------------------------------------------------------

WiFiClient client;
const int httpPort = 8081;

//Funció per llegir un únic byte del dispositiu I2C (sensor_sonometre)
// memory-register read
byte reg_read(byte addr, byte reg)
{
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(addr, (uint8_t)1);
  byte data = Wire.read();
  return data;
}

void reconnect_wifi() 
{
  // Connect to WiFi
  WiFi.begin(ssid, password);

  // Wait for the connection to be established
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000); // Wait for 1 second
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect_client(WiFiClient &client, const char* host, const int httpPort) 
{
  while (!client.connected())
  {
    if (client.connect(host, httpPort)){} 
    else{
      delay(2000);
    }
  }
}

void PUT_request(WiFiClient &client, byte SPL_dBA)
{
  client.print("PUT /data/");
  client.print(String(provider));
  client.print('/');
  client.print(String(sensor));
  client.print('/');
  client.print(String(SPL_dBA));
  client.print(" HTTP/1.1\r\nIDENTITY_KEY: ");
  client.print(String(token));
  client.print("\r\n\r\n");
}

void setup() 
{
  Serial.begin(115200); 
  Wire.begin(); 

  delay(10);

  WiFi.mode(WIFI_STA); 
}

  // --------------------------------------------------------------

void loop()
{ 
  reconnect_wifi(); // Reconnect to WiFi
  reconnect_client(client, host, httpPort);

  byte SPL_dBA = reg_read(PCBARTISTS_DBM, I2C_REG_DECIBEL);

  PUT_request(client, SPL_dBA);

    // si ha respondido esperamos un poco para cerrar la conexion con el servidor
  unsigned long timeout;
  timeout = millis();
  while (millis() - timeout < 200);
  // Cerramos la conexion
  client.stop();

  delay(300000); //Llegeix mesura i envia cada 5mins.
}

