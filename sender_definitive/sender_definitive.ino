#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_BME680.h>
#include "SparkFun_ENS160.h"

// Pines LoRa
#define LORA_SCK   18
#define LORA_MISO  19
#define LORA_MOSI  23
#define LORA_SS    5
#define LORA_RST   14
#define LORA_DIO0  2

// Pines I2C ENS160
#define I2C_SDA_ENS160 32
#define I2C_SCL_ENS160 33

// Direcciones
byte localAddress = 0xAA;
byte destination  = 0x01;

int ens160_fail_count = 0;
// Sensores
Adafruit_BME680 bme;
SparkFun_ENS160 ens160;

bool printedCompensation = false;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // BME680 (Wire = pines 21/22 por defecto en ESP32)
  if (!bme.begin(0x77)) {
    Serial.println("Error con BME680!");
    while (1);
  }
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);

  // ENS160 en Wire1 (pines 32/33)
  Wire1.begin(I2C_SDA_ENS160, I2C_SCL_ENS160);
  if (!ens160.begin(Wire1, 0x53)) {
    Serial.println("Error con ENS160!");
    while (1);
  }

  Serial.println("Sensores inicializados, configurando compensación...");

  if (!bme.performReading()) {
    Serial.println("Error lectura inicial BME680!");
    while (1);
  }

  ens160.setOperatingMode(SFE_ENS160_RESET);
  delay(100);
  ens160.setTempCompensationCelsius(bme.temperature);
  ens160.setRHCompensationFloat(bme.humidity);
  delay(500);
  ens160.setOperatingMode(SFE_ENS160_STANDARD);

  // Inicializar LoRa
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Error al iniciar LoRa!");
    while (1);
  }

  Serial.println("Sensores y LoRa listos.");
}

void loop() {
  if (!bme.performReading()) {
    Serial.println("Error lectura BME680!");
    return;
  }

  float t    = bme.temperature;
  float h    = bme.humidity;
  float p    = bme.pressure / 100.0;
  float gas  = bme.gas_resistance;

  ens160.setTempCompensationCelsius(t);
  ens160.setRHCompensationFloat(h);

  if (ens160.checkDataStatus()) {
    ens160_fail_count = 0;  // Reiniciar el contador de fallos

    if (!printedCompensation) {
      Serial.println("---- Compensación aplicada ----");
      Serial.print("Humedad (%): "); Serial.println(ens160.getRH());
      Serial.print("Temperatura (C): "); Serial.println(ens160.getTempCelsius());
      Serial.println("--------------------------------");
      printedCompensation = true;
    }

    uint16_t co2  = ens160.getECO2();
    uint16_t tvoc = ens160.getTVOC();
    uint8_t aqi   = ens160.getAQI();

    Serial.print("T:"); Serial.print(t); Serial.print("C ");
    Serial.print("H:"); Serial.print(h); Serial.print("% ");
    Serial.print("P:"); Serial.print(p); Serial.print("hPa ");
    Serial.print("Gas:"); Serial.print(gas); Serial.print("Ohm ");
    Serial.print("CO2:"); Serial.print(co2); Serial.print("ppm ");
    Serial.print("TVOC:"); Serial.print(tvoc); Serial.print("ppb ");
    Serial.print("AQI:"); Serial.println(aqi);

    String data = "0" + String(t) + ":" + String(h) + ":" + String(p) + ":" +
                  String(gas) + ":" + String(co2) + ":" + String(tvoc) + ":" + String(aqi);

    LoRa.beginPacket();
    LoRa.write(destination);
    LoRa.write(localAddress);
    LoRa.write(data.length());
    LoRa.print(data);
    LoRa.endPacket();

    Serial.println("→ Paquete enviado por LoRa.");
  } else {
    Serial.println("Esperando datos válidos del ENS160...");
    ens160_fail_count++;

    // Si falla más de 10 veces consecutivas, reinicializa el sensor
    if (ens160_fail_count > 10) {
      Serial.println("⚠ Reiniciando ENS160 por inactividad...");
      ens160.setOperatingMode(SFE_ENS160_RESET);
      delay(100);
      ens160.setTempCompensationCelsius(t);
      ens160.setRHCompensationFloat(h);
      delay(500);
      ens160.setOperatingMode(SFE_ENS160_STANDARD);
      ens160_fail_count = 0;
    }
  }

  delay(2000);
}