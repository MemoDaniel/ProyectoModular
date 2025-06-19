// Inclusión de librerías necesarias
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// Configuración de pines para el módulo LoRa
const int csPin = 5;
const int resetPin = 14;
const int irqPin = 2;

// Direcciones para comunicación LoRa
byte localAddress = 0x01;
byte sensorAddress1 = 0xAA;
byte sensorAddress2 = 0xBB;

// Configuración de la pantalla OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Estructura para almacenar datos del sensor
struct SensorData {
  char temperature[8];
  char humidity[8];
  char pressure[8];
  char gas_resistance[8];
  char eco2[8];
  char tvoc[8];
  char aqi[8];
  unsigned long lastUpdate;
};

// Variables para almacenar datos de dos sensores
SensorData sensor1 = {"0", "0", "0", "0", "0", "0", "0", 0};
SensorData sensor2 = {"0", "0", "0", "0", "0", "0", "0", 0};

const long checkInterval = 12500;
#define MAX_PACKET_SIZE 128
volatile bool newData = false;
char incomingBuffer[MAX_PACKET_SIZE];
byte incomingSender = 0;
byte incomingRecipient = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Inicializar pantalla OLED
  if (!display.begin(SCREEN_ADDRESS, true)) {
    Serial.println(F("Error OLED"));
    for (;;);
  }
  
  display.clearDisplay();
  display.display();
  display.setTextColor(SH110X_WHITE);
  
  // Mostrar pantalla de inicio
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Sistema de Monitoreo");
  display.println("Ambiental LoRa");
  display.display();
  delay(2000);

  // Inicializar módulo LoRa
  LoRa.setPins(csPin, resetPin, irqPin);
  if (!LoRa.begin(433E6)) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Error LoRa!");
    display.display();
    while (1);
  }

  LoRa.onReceive(onReceive);
  LoRa.receive();
  Serial.println("Receptor listo");
}

void loop() {
  bool dataAvailable = false;
  
  // Sección crítica para evitar condiciones de carrera
  noInterrupts();
  if (newData) {
    dataAvailable = true;
    newData = false;
  }
  interrupts();

  if (dataAvailable) {
    if (incomingRecipient == localAddress) {
      byte sensorNumber = (incomingSender == sensorAddress1) ? 1 : 2;
      if (sensorNumber == 1) {
        parseSensorData(incomingBuffer, sensor1, sensorNumber);
      } else {
        parseSensorData(incomingBuffer, sensor2, sensorNumber);
      }
    }
  }
  
  updateDisplay();
  delay(3000);
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;

  incomingRecipient = LoRa.read();
  incomingSender = LoRa.read();
  byte incomingMsgId = LoRa.read();
  byte incomingLength = LoRa.read();

  int i = 0;
  while (LoRa.available()) {
    if (i < MAX_PACKET_SIZE - 1) {
      incomingBuffer[i++] = LoRa.read();
    } else {
      LoRa.read(); // Descarta bytes excedentes
    }
  }
  incomingBuffer[i] = '\0';
  newData = true;
  LoRa.receive(); // Reactivar recepción
}

void parseSensorData(char *data, SensorData &sensor, byte sensorNumber) {
  char *token;
  char *rest = data;

  #define COPY_FIELD(field) \
    if ((token = strtok_r(rest, ":", &rest))) { \
      strncpy(sensor.field, token, sizeof(sensor.field)-1); \
      sensor.field[sizeof(sensor.field)-1] = '\0'; \
      Serial.print(sensorNumber); Serial.print(#field ": "); Serial.println(sensor.field); \
    }

  COPY_FIELD(temperature);
  COPY_FIELD(humidity);
  COPY_FIELD(pressure);
  COPY_FIELD(gas_resistance);
  COPY_FIELD(eco2);
  COPY_FIELD(tvoc);
  COPY_FIELD(aqi);

  sensor.lastUpdate = millis();
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);

  // Función para mostrar datos de un sensor
  auto displaySensorData = [&](const SensorData &sensor, int xOffset) {
    display.setCursor(xOffset, 0);
    display.print("T:"); display.print(sensor.temperature);
    display.setCursor(xOffset, 8);
    display.print("H:"); display.print(sensor.humidity);
    display.setCursor(xOffset, 16);
    display.print("P:"); display.print(sensor.pressure);
    display.setCursor(xOffset, 24);
    display.print("G:"); display.print(sensor.gas_resistance);
    display.setCursor(xOffset, 32);
    display.print("CO2:"); display.print(sensor.eco2);
    display.setCursor(xOffset, 40);
    display.print("TVOC:"); display.print(sensor.tvoc);
    display.setCursor(xOffset, 48);
    display.print("AQI:"); display.print(sensor.aqi);
  };

  // Mostrar datos de ambos sensores
  displaySensorData(sensor1, 0);
  displaySensorData(sensor2, 64);

  // Mostrar estado de conexión
  display.setCursor(0, 56);
  display.print(millis() - sensor1.lastUpdate > checkInterval ? "S1:OFF" : "S1:ON");
  display.setCursor(64, 56);
  display.print(millis() - sensor2.lastUpdate > checkInterval ? "S2:OFF" : "S2:ON");
  
  display.display();
}