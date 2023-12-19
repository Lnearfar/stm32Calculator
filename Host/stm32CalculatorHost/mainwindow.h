#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include "settingsdialog.h"
#include <QLabel>
#include <QMessageBox>
#include <QtWidgets>
#include "button.h"
#include "serialFrame.h"

QT_BEGIN_NAMESPACE

class QLabel;

namespace Ui {
class MainWindow;
//dialog.h
class QAction;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QMenuBar;
class QPushButton;
class QTextEdit;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openSerialPort();
    void closeSerialPort();
    void about();
    void writeData(const QByteArray &data);
    void readData();
    void handleError(QSerialPort::SerialPortError error);

    //按键功能 在mainwindow.cpp里面实现为，通过串口发送对应的操作码
    void digitClicked();
    void operatorClicked();

    //void additiveOperatorClicked();
    //void multiplicativeOperatorClicked();
    //void equalClicked();
    //void pointClicked();

    //void backspaceClicked();
    //void clear();
    /*void changeSignClicked();
    void unaryOperatorClicked();
    void clearAll();
    void clearMemory();
    void readMemory();
    void setMemory();
    void addToMemory();*/

    void createKeyboardWidget();

private:
    void initActionsConnections();
private:
    void showStatusMessage(const QString &message);
    Ui::MainWindow *m_ui=nullptr;
    QLabel *m_status = nullptr;
    SettingsDialog *m_settings = nullptr;
    QSerialPort *m_serialPort=nullptr;

    QGroupBox *m_gridGroupBox=nullptr;

    //QTextEdit *bigEditor;

    QWidget *m_centralWidget;
    QWidget *m_keyboardWidget;
    QTextEdit *m_displayWidget;

    //Button相关
    Button *createButton(const QString &text, const char *member);
    // 其他私有变量和方法
    enum { NumDigitButtons = 10 };
    Button *digitButtons[NumDigitButtons];

    void setupSerialPort(); // 设置串口通信参数
    void sendDataToSTM32(const QString &data); // 向 STM32 发送数据
    // 其他私有方法
};
#endif // MAINWINDOW_H
