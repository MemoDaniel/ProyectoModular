# Proyecto: Sistema de Monitoreo Ambiental con ESP32 y LoRa
## Descripción general
Este proyecto consiste en un sistema de monitoreo ambiental distribuido, que utiliza microcontroladores ESP32 para medir parámetros ambientales mediante sensores y transmitir los datos a través de tecnología LoRa para comunicación de largo alcance. Además, cuenta con visualización local de los datos en pantalla OLED.

El sistema está diseñado para monitorear variables como temperatura, humedad, presión, resistencia de gas, CO2, compuestos orgánicos volátiles (TVOC) y calidad del aire (AQI), ideal para aplicaciones en zonas protegidas, agricultura, o control ambiental remoto.

## Componentes del proyecto
El repositorio contiene tres programas principales:

### 1. Transmisor de sensor con LoRa y pantalla SH1106
Este programa recibe datos de sensores (simulados o reales), los empaqueta y los envía vía LoRa a una estación receptora. También puede mostrar información básica en una pantalla OLED SH1106.

Uso de librerías: SPI.h, LoRa.h, Wire.h, Adafruit_SH110X.h

Comunicación LoRa configurada a 433 MHz.

Estructura para almacenar múltiples datos ambientales.

Recepción de datos y actualización en pantalla.

### 2. Receptor LoRa con pantalla OLED SH1106
Programa para recibir paquetes LoRa, decodificar los datos enviados por sensores remotos y mostrar la información en una pantalla OLED SH1106 dividida para dos sensores. Monitorea la conexión y actualiza la visualización periódicamente.

Recepción y parseo de datos en formato delimitado.

Visualización organizada de temperatura, humedad, presión, gas, CO2, TVOC y AQI para dos sensores.

Indicador de estado de conexión para cada sensor.

### 3. Transmisor con sensores BME680 y ENS160
Programa para ESP32 que lee datos de sensores ambientales BME680 y ENS160, aplica compensaciones, y envía los valores a un receptor LoRa.

Lectura de sensores BME680 (temperatura, humedad, presión, gas).

Sensor ENS160 para CO2, TVOC y AQI con compensación basada en BME680.

Reenvío de datos vía LoRa con estructura de paquete definida.

Manejo de reinicios del sensor ENS160 en caso de fallo.

### 4. Interfaz gráfica en Qt para monitoreo remoto
Aplicación de escritorio desarrollada en C++ con Qt que permite visualizar en tiempo real los datos recibidos por el módulo LoRa a través del puerto serial.

Lectura y análisis de datos desde el puerto serial.

Visualización textual clara de los valores ambientales actuales.

Gráficas interactivas utilizando QCustomPlot para cada parámetro (temperatura, humedad, etc.).

Historial de datos almacenado localmente en un archivo para seguimiento posterior.

Interfaz intuitiva con posibilidad de seleccionar un parámetro y mostrar su gráfica detallada.

Envío de alertas automáticas por Telegram si los valores superan umbrales definidos (temperatura elevada, alta concentración de gases, etc.).

Tecnologías y librerías usadas
ESP32 como microcontrolador principal.

LoRa para comunicación de largo alcance (433 MHz).

Sensores ambientales: BME680 y ENS160.

Pantalla OLED SH1106 para visualización local.

Librerías: SPI, LoRa, Wire, Adafruit_GFX, Adafruit_SH110X, Adafruit_BME680, SparkFun_ENS160.

## Cómo usar
Configurar y cargar el programa transmisor con sensores BME680 y ENS160 en un ESP32 para enviar datos ambientales por LoRa.

Configurar y cargar el programa receptor LoRa con pantalla OLED en otro ESP32 para recibir y mostrar datos de dos sensores.

(Opcional) Programa transmisor alternativo que envía datos simulados o provenientes de otro tipo de sensor, mostrando información en pantalla OLED.

