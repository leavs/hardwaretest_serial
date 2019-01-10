#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTimer>
#include <QDateTime>

namespace Ui {
class Dialog;
}
class QTimer;
class QextSerialPort;
class QextSerialEnumerator;

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

protected:
    void changeEvent(QEvent *);

private Q_SLOTS:
    void onPortNameChanged(const QString &name);
    void onBaudRateChanged(int idx);
    void onParityChanged(int idx);
    void onDataBitsChanged(int idx);
    void onStopBitsChanged(int idx);
    void onQueryModeChanged(int idx);
    void onFlowControlChanged(int idx);
    void onTimeoutChanged(int val);
    void onOpenCloseButtonClicked();
    void onSendButtonClicked();
    void onClearButtonClicked();
    void onReadyRead();
    void onDisableUi();
    void onEnableUi();

private:
    Ui::Dialog *ui;
    QTimer * timer;
    QextSerialPort *port;
};

#endif // DIALOG_H
