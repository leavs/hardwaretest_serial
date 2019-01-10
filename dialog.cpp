#include "dialog.h"
#include "ui_dialog.h"
#include <qextserialport.h>
#include <qextserialenumerator.h>
#include <QDateTime>
#include <QList>


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    // port
    foreach (QextPortInfo info, QextSerialEnumerator::getPorts())
            ui->portComboBox->addItem(info.portName);

    // bandrate
    ui->bandRateComboBox->addItem("1200",BAUD1200);
    ui->bandRateComboBox->addItem("2400",BAUD2400);
    ui->bandRateComboBox->addItem("4800",BAUD4800);
    ui->bandRateComboBox->addItem("9600",BAUD9600);
    ui->bandRateComboBox->addItem("19200",BAUD19200);
    ui->bandRateComboBox->addItem("115200",BAUD115200);
    ui->bandRateComboBox->setCurrentIndex(5);
    // parity
    ui->parityComboBox->addItem("NONE",PAR_NONE);
    ui->parityComboBox->addItem("ODD",PAR_ODD);
    ui->parityComboBox->addItem("EVEN",PAR_EVEN);
    // databits
    ui->dataBitsComboBox->addItem("6",DATA_6);
    ui->dataBitsComboBox->addItem("7",DATA_7);
    ui->dataBitsComboBox->addItem("8",DATA_8);
    ui->dataBitsComboBox->setCurrentIndex(2);

    // stopbits
    ui->stopBitsComboBox->addItem("1",STOP_1);
    ui->stopBitsComboBox->addItem("2",STOP_2);
    // flowcontrol
    ui->flowControlComboBox->addItem("OFF",FLOW_OFF);
    ui->flowControlComboBox->addItem("HARDWARE",FLOW_HARDWARE);
    ui->flowControlComboBox->addItem("XONXOFF",FLOW_XONXOFF);
    // querymode
    ui->queryModeComboBox->addItem("Polling",QextSerialPort::Polling);
    ui->queryModeComboBox->addItem("EventDriven",QextSerialPort::EventDriven);

    // led
    ui->led->turnOff();

    // open/close
    ui->openCloseButton->setEnabled(true);

    // send
    ui->sendButton->setEnabled(false);

    // clear
    ui->clearButton->setEnabled(true);

    // timer
    timer = new QTimer(this);
    timer->setInterval(40);

    PortSettings settings = {BAUD115200, DATA_8, PAR_NONE, STOP_1, FLOW_OFF, 10};
    port = new QextSerialPort(ui->portComboBox->currentText(), settings, QextSerialPort::Polling);

    // connect
    connect(ui->bandRateComboBox, SIGNAL(currentIndexChanged(int)), SLOT(onBaudRateChanged(int)));
    connect(ui->parityComboBox, SIGNAL(currentIndexChanged(int)), SLOT(onParityChanged(int)));
    connect(ui->dataBitsComboBox, SIGNAL(currentIndexChanged(int)), SLOT(onDataBitsChanged(int)));
    connect(ui->stopBitsComboBox, SIGNAL(currentIndexChanged(int)), SLOT(onStopBitsChanged(int)));
    connect(ui->flowControlComboBox, SIGNAL(currentIndexChanged(int)), SLOT(onFlowControlChanged(int)));
    connect(ui->queryModeComboBox, SIGNAL(currentIndexChanged(int)), SLOT(onQueryModeChanged(int)));
    connect(ui->timeOutBox, SIGNAL(valueChanged(int)), SLOT(onTimeoutChanged(int)));
    connect(ui->portComboBox, SIGNAL(editTextChanged(QString)), SLOT(onPortNameChanged(QString)));
    connect(ui->openCloseButton, SIGNAL(clicked()), SLOT(onOpenCloseButtonClicked()));
    connect(ui->sendButton, SIGNAL(clicked()), SLOT(onSendButtonClicked()));
    connect(ui->clearButton, SIGNAL(clicked()), SLOT(onClearButtonClicked()));
    connect(timer, SIGNAL(timeout()), SLOT(onReadyRead()));
    connect(port, SIGNAL(readyRead()), SLOT(onReadyRead()));

    setWindowTitle(tr("hardwaretest_serial"));  //set dialog title
}

Dialog::~Dialog()
{
    delete ui;
    delete port;
}

void Dialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Dialog::onPortNameChanged(const QString &)
{
    if (port->isOpen()) {
        port->close();
        ui->led->turnOff();
        ui->sendButton->setEnabled(false);
    }
}

void Dialog::onBaudRateChanged(int idx)
{
    port->setBaudRate((BaudRateType)ui->bandRateComboBox->itemData(idx).toInt());
}

void Dialog::onParityChanged(int idx)
{
    port->setParity((ParityType)ui->parityComboBox->itemData(idx).toInt());
}

void Dialog::onDataBitsChanged(int idx)
{
    port->setDataBits((DataBitsType)ui->dataBitsComboBox->itemData(idx).toInt());
}

void Dialog::onStopBitsChanged(int idx)
{
    port->setStopBits((StopBitsType)ui->stopBitsComboBox->itemData(idx).toInt());
}

void Dialog::onQueryModeChanged(int idx)
{
    port->setQueryMode((QextSerialPort::QueryMode)ui->queryModeComboBox->itemData(idx).toInt());
}

void Dialog::onFlowControlChanged(int idx)
{
    port->setFlowControl((FlowType)ui->flowControlComboBox->itemData(idx).toInt());
}

void Dialog::onTimeoutChanged(int val)
{
    port->setTimeout(val);
}

void Dialog::onOpenCloseButtonClicked()
{
    QString currenttime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    if(!port->isOpen()){
        port->setPortName(ui->portComboBox->currentText());
        port->open(QIODevice::ReadWrite);
        ui->textBrowser->setText(currenttime+"\n"+port->portName()+" is opended!!");
        onDisableUi();
    }
    else {
        port->close();
        ui->textBrowser->setText(currenttime+"\n"+port->portName()+" is closed!!");
        onEnableUi();
    }

    // if using polling mode, we need a QTimer
    if (port->isOpen() && port->queryMode() == QextSerialPort::Polling)
        timer->start();
    else
        timer->stop();

    // update led's status
    ui->led->turnOn(port->isOpen());
}

void Dialog::onSendButtonClicked()
{
    QString currenttime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QByteArray sendDate = "Message from serial by manual!!!\n";
    if (port->isOpen()) {
        port->write(sendDate);
        ui->textBrowser->setText(currenttime+"\n"+port->portName()+" Sended Message by manual!!");
    }
    else
        ui->textBrowser->setText(currenttime+"\n"+port->portName()+" is not opened!!");
}

void Dialog::onClearButtonClicked()
{
    ui->textBrowser->clear();
}

void Dialog::onReadyRead()
{
    QString currenttime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    if (port->bytesAvailable())
        ui->textBrowser->setText(currenttime+"\n"+port->portName()+" received: \n"+QString::fromLatin1(port->readAll()));
}

void Dialog::onDisableUi()
{
    ui->portComboBox->setEnabled(false);
    ui->bandRateComboBox->setEnabled(false);
    ui->parityComboBox->setEnabled(false);
    ui->dataBitsComboBox->setEnabled(false);
    ui->stopBitsComboBox->setEnabled(false);
    ui->flowControlComboBox->setEnabled(false);
    ui->queryModeComboBox->setEnabled(false);
    ui->timeOutBox->setEnabled(false);
    ui->sendButton->setEnabled(true);
}

void Dialog::onEnableUi()
{
    ui->portComboBox->setEnabled(true);
    ui->bandRateComboBox->setEnabled(true);
    ui->parityComboBox->setEnabled(true);
    ui->dataBitsComboBox->setEnabled(true);
    ui->stopBitsComboBox->setEnabled(true);
    ui->flowControlComboBox->setEnabled(true);
    ui->queryModeComboBox->setEnabled(true);
    ui->timeOutBox->setEnabled(true);
    ui->sendButton->setEnabled(false);
}
