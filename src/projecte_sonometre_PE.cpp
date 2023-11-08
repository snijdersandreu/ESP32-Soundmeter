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

//Funció per entrar en mode deep sleep
void DeepSleep(uint64_t timeInMicroseconds) {
  // Configurem timer per aturi el mode Deep Sleep
  esp_sleep_enable_timer_wakeup(timeInMicroseconds);

  Serial.println("Mode Deep Sleep");
  esp_deep_sleep_start();
}

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

void reconnect_wifi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
    }
    Serial.println("Connected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void reconnect_client() {
  if (!client.connected()) {
    Serial.println("Connecting to the server...");
    while (!client.connect(host, httpPort)) {
      delay(2000);
    }
  }
}

String make_put_request(byte SPL_dBA) {
  String request = "PUT /data/";
  request += String(provider);
  request += '/';
  request += String(sensor);
  request += '/';
  request += String(SPL_dBA);
  request += " HTTP/1.1\r\nIDENTITY_KEY: ";
  request += String(token);
  request += "\r\n\r\n";
  
  return request;
}

void send_PUT_request(WiFiClient &client, byte SPL_dBA)
{
  String request = make_put_request(SPL_dBA);
  client.print(request);
}

void setup() 
{
  Serial.begin(115200); 
  Wire.begin(); 

  delay(10);

  WiFi.mode(WIFI_STA); 
  reconnect_wifi();
}

  // --------------------------------------------------------------

void loop()
{ 
  reconnect_wifi(); // Reconnect to WiFi
  reconnect_client();

  //byte SPL_dBA = reg_read(PCBARTISTS_DBM, I2C_REG_DECIBEL);
  byte SPL_dBA = 0x14;
  send_PUT_request(client, SPL_dBA);

  // si ha respondido esperamos un poco para cerrar la conexion con el servidor
  unsigned long timeout = millis();
  while (millis() - timeout < 200){}
  // Cerramos la conexion
  client.stop();

  DeepSleep(300000000); //Llegeix mesura i envia cada 5mins. En el interval de 5mins el microcontrolador
                        //entra en mode Deep Sleep per estalviar energia (ja que s'alimenta d'una bateria)
}

