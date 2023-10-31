#include <Arduino.h>
#include <Wire.h>

#define PCBARTISTS_DBM 0x48
 
#define I2C_REG_VERSION      0x00
#define I2C_REG_ID3          0x01
#define I2C_REG_ID2          0x02
#define I2C_REG_ID1          0x03
#define I2C_REG_ID0          0x04
#define I2C_REG_SCRATCH      0x05
#define I2C_REG_CONTROL      0x06
#define I2C_REG_TAVG_HIGH    0x07
#define I2C_REG_TAVG_LOW     0x08
#define I2C_REG_RESET        0x09
#define I2C_REG_DECIBEL      0x0A
#define I2C_REG_MIN          0x0B
#define I2C_REG_MAX          0x0C
#define I2C_REG_THR_MIN      0x0D
#define I2C_REG_THR_MAX      0x0E
#define I2C_REG_HISTORY_0    0x14
#define I2C_REG_HISTORY_99   0x77
 

byte reg_read(byte addr, byte reg) 
{
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(addr, (uint8_t)1);
  byte data = Wire.read();
  return data;
}

void setup() 
{
  Serial.begin(115200);
  Wire.begin();
  
  byte version = reg_read(PCBARTISTS_DBM, I2C_REG_VERSION);
  Serial.print("dbMeter VERSION = 0x");
  Serial.println(version, HEX);
 
  byte id[4];
  id[0] = reg_read(PCBARTISTS_DBM, I2C_REG_ID3);
  id[1] = reg_read(PCBARTISTS_DBM, I2C_REG_ID2);
  id[2] = reg_read(PCBARTISTS_DBM, I2C_REG_ID1);
  id[3] = reg_read(PCBARTISTS_DBM, I2C_REG_ID0);
 
  Serial.print("Unique ID = ");
  Serial.print(id[0], HEX); 
  Serial.print(" ");
  Serial.print(id[1], HEX); 
  Serial.print(" ");
  Serial.print(id[2], HEX); 
  Serial.print(" ");
  Serial.println(id[3], HEX); 
}
 
void loop() 
{
  byte sound_level = reg_read(PCBARTISTS_DBM, I2C_REG_DECIBEL);
  Serial.print("Sound Level (dB SPL) = ");
  Serial.println(sound_level);
  delay(2000);
}