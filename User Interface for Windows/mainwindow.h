#include <QMainWindow>
#include <QSerialPort>
#include <QTextEdit>
#include <QTimer>
#include <QNetworkAccessManager>  // Para enviar solicitudes HTTP
#include <QNetworkReply>          // Para manejar las respuestas HTTP

//#include <QTimer>
#include <QNetworkRequest>
//#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QUrlQuery>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startReading();  // Declarar el slot startReading
    void readData();      // Declarar el slot readData
    void retryOpenPort(); //declarar el slot para volver a intentar conectar el puerto

    void on_pushButton_clicked(); //slot para presionar el boton

    void sendTelegramMessage(const QString &message); // slot para enviar mensaje telegram

    void checkForFireAlert(int sensorId);
private:
    Ui::MainWindow *ui;
    QSerialPort serialPort;  // Objeto de puerto serie para manejar la comunicación
    QTimer *retryTimer;
    QTimer * checkTelegramTimer();
    QTimer *telegramCheckTimer;
   // double temp1;  // Variable para almacenar el valor de temp 1
    //double temp2; //variable para almacenar el valor de temp2

    //QString serialBuffer;
    // Sensor 1
    double temp1, humidity1, pressure1, gas_resistance1, eco2_1, tvoc1, aqi1;
    double temp2, humidity2, pressure2, gas_resistance2, eco2_2, tvoc2, aqi2;
    QString serialBuffer;

    QNetworkAccessManager *networkManager;  // Para manejar solicitudes HTTP
    QString botToken;  // variable string para Token del bot de Telegram
    QString chatId;    // variable string ID del chat de Telegram



    void checkTelegramMessages();
};
