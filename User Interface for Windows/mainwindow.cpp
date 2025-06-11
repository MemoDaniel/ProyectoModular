#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QPixmap>
#include <QDebug>

#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>

#include <QTimer>

#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>


double temp1 = 0, temp2 = 0;
int eco2_1 = 0, eco2_2 = 0;
qint64 lastUpdateId = 0;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    retryTimer = new QTimer(this); //nuevo timer
    connect(retryTimer, &QTimer::timeout, this, &MainWindow::retryOpenPort); // asignar el timer a la funcion retryopen
    telegramCheckTimer = new QTimer(this);
    connect(telegramCheckTimer, &QTimer::timeout, this, &MainWindow::checkTelegramMessages);
    telegramCheckTimer->start(10000); // cada 10 segundos

    QLabel *background = new QLabel(this);// Crear un QLabel para la imagen de fondo


    QPixmap pixmap("C:/example/example.png");// Ruta de la imagen (usando barras diagonales)

    networkManager = new QNetworkAccessManager(this); // configuracion para acceso a internet

    botToken = "1111111111111111";  // Token del bot
    chatId = "1111111111111";  // ID del chat

    if (pixmap.isNull()) {
        qDebug() << "Error: No se pudo cargar la imagen de fondo.";// Verificar si la imagen se cargó correctamente
    } else {
        qDebug() << "La imagen de fondo se cargó correctamente.";// Verificar si la imagen se cargó correctamente
    }

    background->setPixmap(pixmap);
    background->setScaledContents(true);  // Escalar la imagen al tamaño del QLabel
    background->setGeometry(0, 0, this->width(), this->height());  // Ajustar al tamaño de la ventana

    // Mover el QLabel al fondo
    background->lower();

    // hacer los cuadros de texto semi transparentes oscuros
    ui->textEdit_temp1->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->textEdit_hum1->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->textEdit_pres1->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->textEdit_gas1->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->textEdit_eco2_1->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->textEdit_eco2_1->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    //  ui->textEdit_tvoc1->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->textEdit_aqi1->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    //
    ui->textEdit_pres2->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->textEdit_hum2->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->textEdit_pres2->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->textEdit_gas2->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->textEdit_eco2_2->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->textEdit_eco2_2->setStyleSheet("background: rgba(0 , 0, 0, 80);");
   // ui->textEdit_tvoc2->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->textEdit_aqi2->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    // poner encima los cuadros de textos
    //ui->label_1->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->label_2->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->label_3->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->label_4->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->label_5->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->label_6->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->label_7->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->label_8->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->label_9->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    //ui->label_10->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->label_11->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->label_12->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->label_13->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->label_14->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->label_16->setStyleSheet("background: rgba(0 , 0, 0, 80);");
    ui->label_17->setStyleSheet("background: rgba(0 , 0, 0, 80);");
   // ui->label_18->setStyleSheet("background: rgba(0 , 0, 0, 80);");



    ui->textEdit_temp1->raise();
    ui->textEdit_hum1->raise();
    ui->textEdit_pres1->raise();
    ui->textEdit_gas1->raise();
    ui->textEdit_eco2_1->raise();
   // ui->textEdit_tvoc1->raise();
    ui->textEdit_aqi1->raise();
    ui->label_2->raise();
    ui->label_2->raise();
    // Iniciar la lectura del puerto serie
    startReading();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startReading() {
    // Configurar el puerto serie
    serialPort.setPortName(ui->comboBox->currentText());  // nombre del puerto serial obtenido del combo_box


    QString baudRateStr = ui->comboBox_2->currentText();
    int baudRate = baudRateStr.toInt();  // pasar variable string a int para el valor de baudios

    serialPort.setBaudRate(baudRate); //baudios obtenidos de la bariable baudrate
    serialPort.setDataBits(QSerialPort::Data8);  // 8 bits de datos
    serialPort.setParity(QSerialPort::NoParity);  // Sin paridad
    serialPort.setStopBits(QSerialPort::OneStop);  //  bit de parada
    serialPort.setFlowControl(QSerialPort::NoFlowControl);  // Sin control de flujo

    // Intentar abrir el puerto serie en modo lectura
    if (serialPort.open(QIODevice::ReadOnly)) { //si el puerto esta abierto
        qDebug() << "Puerto abierto:" << serialPort.portName(); //mandar por el debugger que el puerto esta abierto y el nombre del puerto
        retryTimer->stop();  // Detener el temporizador si el puerto se abre correctamente
    } else {      //si no lo esta
        qDebug() << "Error al abrir el puerto:" << serialPort.errorString(); //enviar por el debugger la falla
        qDebug() << "Reintentando en 5 segundos...";    //indicar en el debugger que habra reintento
        retryTimer->start(5000);  // Iniciar el temporizador para reintentar en 5 segundos
    }

    // Conectar la señal 'readyRead' para leer los datos cuando estén disponibles
    connect(&serialPort, &QSerialPort::readyRead, this, &MainWindow::readData);
}

void MainWindow::readData() {
    QByteArray data = serialPort.readAll();
    serialBuffer += QString::fromUtf8(data);  // Acumular datos

    qDebug().noquote() << "Buffer acumulado:\n" << serialBuffer; // sacar al puerto seria el acumulado del buffer

    int newLineIndex;
    while ((newLineIndex = serialBuffer.indexOf('\n')) != -1) {
        QString line = serialBuffer.left(newLineIndex).trimmed();
        serialBuffer = serialBuffer.mid(newLineIndex + 1);

        qDebug() << "Línea completa procesada:" << line;

        if (line.startsWith("1temperature: ")) {
            temp1 = line.section(": ", 1).toDouble();
            ui->textEdit_temp1->append(QString::number(temp1));
            checkForFireAlert(1);
        } else if (line.startsWith("1humidity: ")) {
            humidity1 = line.section(": ", 1).toDouble();
            ui->textEdit_hum1->append(QString::number(humidity1));
        } else if (line.startsWith("1pressure: ")) {
            pressure1 = line.section(": ", 1).toDouble();
            ui->textEdit_pres1->append(QString::number(pressure1));
        } else if (line.startsWith("1gas_resistance: ")) {
            gas_resistance1 = line.section(": ", 1).toDouble();
            ui->textEdit_gas1->append(QString::number(gas_resistance1));
        } else if (line.startsWith("1eco2: ")) {
            eco2_1 = line.section(": ", 1).toDouble();
            ui->textEdit_eco2_1->append(QString::number(eco2_1));
            checkForFireAlert(1);
        } else if (line.startsWith("1tvoc: ")) {
            tvoc1 = line.section(": ", 1).toDouble();
            //ui->textEdit_tvoc1->append(QString::number(tvoc1));
        } else if (line.startsWith("1aqi: ")) {
            aqi1 = line.section(": ", 1).toDouble();
            ui->textEdit_aqi1->append(QString::number(aqi1));
        } else if (line.startsWith("2temperature: ")) {
            temp2 = line.section(": ", 1).toDouble();
            ui->textEdit_temp2->append(QString::number(temp2));
            checkForFireAlert(2);
        } else if (line.startsWith("2humidity: ")) {
            humidity2 = line.section(": ", 1).toDouble();
            ui->textEdit_hum2->append(QString::number(humidity2));
        } else if (line.startsWith("2pressure: ")) {
            pressure2 = line.section(": ", 1).toDouble();
            ui->textEdit_pres2->append(QString::number(pressure2));
        } else if (line.startsWith("2gas_resistance: ")) {
            gas_resistance2 = line.section(": ", 1).toDouble();
            ui->textEdit_gas2->append(QString::number(gas_resistance2));
        } else if (line.startsWith("2eco2: ")) {
            eco2_2 = line.section(": ", 1).toDouble();
            ui->textEdit_eco2_1->append(QString::number(eco2_2));
            checkForFireAlert(2);
        } else if (line.startsWith("2tvoc: ")) {
            tvoc2 = line.section(": ", 1).toDouble();
            //}ui->textEdit_tvoc2->append(QString::number(tvoc2));
        } else if (line.startsWith("2aqi: ")) {
            aqi2 = line.section(": ", 1).toDouble();
            ui->textEdit_aqi2->append(QString::number(aqi2));
        }
    }
}




void MainWindow::retryOpenPort() {
    qDebug() << "Reintentando abrir el puerto..."; //mandar en el debugger que se intentara otra vez
    startReading();   //mandar a llamar funcion startreading
}

void MainWindow::on_pushButton_clicked()
{
    retryOpenPort();    //mandar a llamar la funcion de reintentar abrir el puerto
}

void MainWindow::sendTelegramMessage(const QString &message) {
    // Crear la URL para enviar el mensaje
    QUrl url("https://api.telegram.org/bot" + botToken + "/sendMessage"); //request a pagina de telegram bot + token+ mensaje


    QUrlQuery query;
    query.addQueryItem("chat_id", chatId); //añadir el id destinatario
    query.addQueryItem("text", message);    //añadir message que antes se habia hecho
    url.setQuery(query);

    // Enviar la solicitud HTTP
    QNetworkRequest request(url);
    networkManager->get(request);
}

void MainWindow::checkForFireAlert(int sensorId) {
    if (sensorId == 1) {
        if (temp1 > 40.0 && eco2_1 > 400) {
            QString message = "Posible incendio detectado en sensor 1.\nTemperatura: " + QString::number(temp1) + "°C\nCO₂: " + QString::number(eco2_1) + " ppm";
            sendTelegramMessage(message);
        }
    } else if (sensorId == 2) {
        if (temp2 > 40.0 && eco2_2 > 400) {
            QString message = "Posible incendio detectado en sensor 2.\nTemperatura: " + QString::number(temp2) + "°C\nCO₂: " + QString::number(eco2_2) + " ppm";
            sendTelegramMessage(message);
        }
    }
}

void MainWindow::checkTelegramMessages() {
    // Crear la URL base para obtener los mensajes nuevos del bot
    QUrl url("https://api.telegram.org/bot" + botToken + "/getUpdates");

    // Crear un objeto para manejar los parámetros de la URL
    QUrlQuery query;

    // Si ya hay un update_id anterior, pedir sólo los nuevos (para evitar mensajes repetidos)
    if (lastUpdateId != 0)
        query.addQueryItem("offset", QString::number(lastUpdateId + 1));

    // Asignar los parámetros a la URL
    url.setQuery(query);

    // Crear la solicitud HTTP GET a la URL completa
    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager->get(request); // Enviar la solicitud y obtener una respuesta

    // Conectar la señal de finalización de la solicitud (cuando lleguen los datos de respuesta)
    connect(reply, &QNetworkReply::finished, [=]() {
        // Verificar que no hubo error en la solicitud
        if (reply->error() == QNetworkReply::NoError) {
            // Leer todos los datos recibidos
            QByteArray response = reply->readAll();

            // Convertir la respuesta JSON en un documento legible
            QJsonDocument jsonDoc = QJsonDocument::fromJson(response);

            // Verificar que la respuesta es un objeto JSON
            if (jsonDoc.isObject()) {
                QJsonObject root = jsonDoc.object();           // Obtener el objeto raíz
                QJsonArray results = root["result"].toArray(); // Extraer el arreglo de mensajes

                // Recorrer todos los mensajes recibidos
                for (const QJsonValue &value : results) {
                    QJsonObject messageObj = value.toObject();        // Obtener el objeto de cada mensaje
                    lastUpdateId = messageObj["update_id"].toInt();   // Guardar el último ID para evitar repeticiones

                    QJsonObject message = messageObj["message"].toObject(); // Extraer el contenido del mensaje
                    QString text = message["text"].toString();             // Obtener el texto del mensaje

                    // Si el mensaje recibido es "actualizacion"
                    if (text.toLower() == "actualizacion") {
                        // Armar un mensaje con todos los datos actuales
                        QString response =
                            "📋*Actualización de sensores:*\n\n"
                            "🌡 Temp 1: " + QString::number(temp1) + " °C\n"
                                                       "Humedad 1: " + QString::number(humidity1) + " %\n"
                                                           "Presión 1: " + QString::number(pressure1) + " hPa\n"
                                                           "Gas 1: " + QString::number(gas_resistance1) + " Ω\n"
                                                      "CO₂ 1: " + QString::number(eco2_1) + " ppm\n"
                                                        "TVOC 1: " + QString::number(tvoc1) + "\n"
                                                       "AQI 1: " + QString::number(aqi1) + "\n\n"
                                                      "Temp 2: " + QString::number(temp2) + " °C\n"
                                                       "Humedad 2: " + QString::number(humidity2) + " %\n"
                                                           "Presión 2: " + QString::number(pressure2) + " hPa\n"
                                                           "Gas 2: " + QString::number(gas_resistance2) + " Ω\n"
                                                      "CO₂ 2: " + QString::number(eco2_2) + " ppm\n"
                                                        "TVOC 2: " + QString::number(tvoc2) + "\n"
                                                       "AQI 2: " + QString::number(aqi2);

                        // Enviar el mensaje por Telegram usando la función ya existente
                        sendTelegramMessage(response);
                    }
                }
            }
        }
        // Liberar memoria de la respuesta
        reply->deleteLater();
    });
}


