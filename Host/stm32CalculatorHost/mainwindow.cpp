#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), m_ui(new Ui::MainWindow), m_status(new QLabel), m_settings(new SettingsDialog), m_serialPort(new QSerialPort(this))
{
    //通过designer的ui设置布局的初始化
    m_ui->setupUi(this);

    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionQuit->setEnabled(true);
    m_ui->actionConfigure->setEnabled(true);

    m_ui->statusBar->addWidget(m_status);

    // 创建一个新的 QWidget，用作主窗口中央的自定义区域
    m_centralWidget = new QWidget(this);

    // ==== displayWidget ====
    // 创建显示区域，这里假设您已经有一个显示区域的对象
    // 如果没有，您可以根据需要创建一个 QLabel 或 QTextEdit 作为显示区域
    m_displayWidget =new QTextEdit;
    QFont font;
    font.setPointSize(11);
    // 设置显示区域的布局和内容
    //设置只读
    // ==== displayWidget ====

    // ==== keyboardLayout ====
    // 创建键盘区域
    m_keyboardWidget = new QWidget;
    createKeyboardWidget();
    // 在 keyboardLayout 中添加您的键盘按钮
    // 使用前面提到的方式创建按钮并添加到布局中
    // ==== keyboardLayout ====

    // 将键盘区域和显示区域添加到 centralWidget 的 QVBoxLayout 中
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_displayWidget); // 添加显示区域
    mainLayout->addWidget(m_keyboardWidget); // 添加键盘区域
    m_centralWidget->setLayout(mainLayout);

    // 设置 centralWidget 为主窗口的中央部件
    setCentralWidget(m_centralWidget);

    // ==== calcHistory Widget ====
    m_calcHistory = new calcHistory;

    //打开所有Actions，启用slots
    initActionsConnections();
    /*初始化串口发送与接受帧*/
    initSerialFrame();
    connect(m_serialPort, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(m_serialPort, &QSerialPort::readyRead, this, &MainWindow::readData);

    setWindowTitle(tr("STM32 Calculation Demo."));
}

MainWindow::~MainWindow()
{
    delete m_ui;
    delete m_settings;
}

//通过*keyboardWidget,创建keyboardWidget组件,处理键盘布局
void MainWindow::createKeyboardWidget(){
    for (int i = 0; i < NumDigitButtons; ++i)
        digitButtons[i] = createButton(QString::number(i), SLOT(digitClicked()));

    //"+"
    Button *plusButton = createButton(tr("+"), SLOT(operatorClicked()));
    //"-"
    Button *minusButton = createButton(tr("-"), SLOT(operatorClicked()));
    //"*"
    Button *timesButton = createButton(tr("\303\227"), SLOT(operatorClicked()));
    //"/"
    Button *divisionButton = createButton(tr("\303\267"), SLOT(operatorClicked()));
    // .
    Button *pointButton = createButton(tr("."), SLOT(operatorClicked()));
    // sqrt
    Button *squareRootButton = createButton(tr("Sqrt"), SLOT(operatorClicked()));
    // cbrt
    Button *cubeRootButton = createButton(tr("cbrt"), SLOT(operatorClicked()));
    // 平方squre
    Button *squreButton = createButton(tr("x^2"), SLOT(operatorClicked()));
    // 平方squre
    Button *cubeButton = createButton(tr("x^3"), SLOT(operatorClicked()));
    // =
    Button *equalButton = createButton(tr("="), SLOT(operatorClicked()));
    // delete (backspace)
    Button *backspaceButton = createButton(tr("Backspace"), SLOT(operatorClicked()));
    // clear
    Button *clearButton = createButton(tr("Clear"), SLOT(operatorClicked()));
    //符号变量x,y
    Button *variableXButton = createButton(tr("X"), SLOT(operatorClicked()));
    Button *variableYButton = createButton(tr("Y"), SLOT(operatorClicked()));
    //解方程 新起一行
    Button *newLineButton = createButton(tr("\\n"), SLOT(operatorClicked()));
    //解方程按键SOLVE
    Button *solveButton = createButton(tr("sov"), SLOT(operatorClicked()));

    //Button *changeSignButton = createButton(tr("\302\261"), SLOT(changeSignClicked()));
    Button *clearAllButton = createButton(tr("Clear All"), SLOT(clearAll()));
    addToHistoryButton = createButton(tr("+H"), SLOT(addToHistoryClicked()));
    openHistoryButton = createButton(tr("His"), SLOT(openHistoryClicked()));


    //! [4]

    //! [5]
    QGridLayout *mainLayout = new QGridLayout;
    //! [5] //! [6]
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->addWidget(backspaceButton, 1, 0, 1, 1);
    mainLayout->addWidget(clearButton, 1, 1, 1, 1);
    mainLayout->addWidget(addToHistoryButton, 1, 2, 1, 1);
    mainLayout->addWidget(openHistoryButton, 1, 3, 1, 1);

    mainLayout->addWidget(clearAllButton, 1, 4, 1, 2);

    //mainLayout->addWidget(clearMemoryButton, 2, 0);
    //mainLayout->addWidget(readMemoryButton, 3, 0);
    //mainLayout->addWidget(setMemoryButton, 4, 0);
    //mainLayout->addWidget(addToMemoryButton, 5, 0);
    mainLayout->addWidget(squareRootButton, 2, 0);
    mainLayout->addWidget(cubeRootButton, 3, 0);
    mainLayout->addWidget(squreButton, 4, 0);
    mainLayout->addWidget(cubeButton, 5, 0);

    for (int i = 1; i < NumDigitButtons; ++i) {
        int row = ((9 - i) / 3) + 2;
        int column = ((i - 1) % 3) + 1;
        mainLayout->addWidget(digitButtons[i], row, column);
    }

    mainLayout->addWidget(digitButtons[0], 5, 1);
    mainLayout->addWidget(pointButton, 5, 2);
    mainLayout->addWidget(equalButton, 5, 3);


    mainLayout->addWidget(divisionButton, 2, 4);
    mainLayout->addWidget(timesButton, 3, 4);
    mainLayout->addWidget(minusButton, 4, 4);

    mainLayout->addWidget(plusButton, 5, 4);
    mainLayout->addWidget(newLineButton, 2, 5);
    mainLayout->addWidget(variableXButton, 3, 5);
    mainLayout->addWidget(variableYButton, 4, 5);
    mainLayout->addWidget(solveButton, 5, 5);
    m_keyboardWidget->setLayout(mainLayout);
}



void MainWindow::openSerialPort()
{
    const SettingsDialog::Settings p = m_settings->settings();
    m_serialPort->setPortName(p.name);
    m_serialPort->setBaudRate(p.baudRate);
    m_serialPort->setDataBits(p.dataBits);
    m_serialPort->setParity(p.parity);
    m_serialPort->setStopBits(p.stopBits);
    m_serialPort->setFlowControl(p.flowControl);
    if (m_serialPort->open(QIODevice::ReadWrite)) {
        //m_console->setEnabled(true);
        //m_console->setLocalEchoEnabled(p.localEchoEnabled);
        m_ui->actionConnect->setEnabled(false);
        m_ui->actionDisconnect->setEnabled(true);
        m_ui->actionConfigure->setEnabled(false);
        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                              .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                              .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), m_serialPort->errorString());

        showStatusMessage(tr("Open error"));
    }
}

void MainWindow::closeSerialPort()
{
    if (m_serialPort->isOpen())
        m_serialPort->close();
    //m_console->setEnabled(false);
    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionConfigure->setEnabled(true);
    showStatusMessage(tr("Disconnected"));
}

//! [6]
void MainWindow::writeData(const QByteArray &data)
{
    m_serialPort->write(data);
}
//! [6]

//! [7]
void MainWindow::readData()
{
    QByteArray data = m_serialPort->readAll();
    //m_console->putData(data);
    //数据帧解析函数，解析来自stm32的frame，同时更新相应的值
    //需要补充，如果data.size()过大，应该舍弃多少接受的数据.
    for(int i=0; i<data.size(); ++i) {
        hostGetOneByte((uint8_t)data[i]);
    }
    m_displayWidget->clear();
    m_displayWidget->insertPlainText(QString(m_frameData.equationStr));
    m_displayWidget->insertPlainText("\n");
    m_displayWidget->insertPlainText(QString(m_frameData.answerStr));
}
//! [7]

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Simple Terminal"),
                       tr("The <b>Simple Terminal</b> example demonstrates how to "
                          "use the Qt Serial Port module in modern GUI applications "
                          "using Qt, with a menu bar, toolbars, and a status bar."));
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), m_serialPort->errorString());
        closeSerialPort();
    }
}
//! [8]

void MainWindow::initActionsConnections()
{
    connect(m_ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(m_ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(m_ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(m_ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::show);
    //connect(m_ui->actionClear, &QAction::triggered, m_console, &Console::clear);
    connect(m_ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(m_ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
}

Button *MainWindow::createButton(const QString &text, const char *member)
{
    Button *button = new Button(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}

/*
 * 下面是数字按键功能函数，按下后通过串口发送
 */
void MainWindow::digitClicked(){
    Button *clickedButton = qobject_cast<Button *>(sender());
    int digitValue = clickedButton->text().toInt();
    //digitValue = (int)0~9

    m_serialHost2StmFrame.serialButtonType=digitValue+1;
    m_serialHost2StmFrame.serialHostPressCnt++;
    QByteArray data((char*)&m_serialHost2StmFrame, sizeof(m_serialHost2StmFrame));
    //测试用
    //char c = '0' + digitValue; // 转换为'0'-'9'的ASCII字符
    //QByteArray data((char*)&c,sizeof(c));
    writeData(data);
}

/*
 * 操作符按键SLOT,按下后通过串口发送
*/
void MainWindow::operatorClicked(){
    Button *clickedButton = qobject_cast<Button *>(sender());
    QString clickedOperator = clickedButton->text();
    uint8_t pressedButtonValue = BUT_NO_PRESS;
    if (clickedOperator == tr("+")){
        pressedButtonValue=BUT_ADD;
    }
    else if(clickedOperator==tr("-")){
        pressedButtonValue=BUT_MINUS;
    }
    else if(clickedOperator==tr("\303\227")){
        //"*"
        pressedButtonValue=BUT_MUL;
    }
    else if(clickedOperator==tr("\303\267")){
        // "/"
        pressedButtonValue=BUT_DIV;
    }
    else if(clickedOperator==tr(".")){
        // dot
        pressedButtonValue=BUT_DOT;
    }
    else if(clickedOperator==tr("Sqrt")){
        pressedButtonValue=BUT_SQRT;
    }
    else if(clickedOperator==tr("cbrt")){
        pressedButtonValue=BUT_CBRT;
    }
    else if(clickedOperator==tr("x^2")){
        pressedButtonValue=BUT_SQU;
    }
    else if(clickedOperator==tr("x^3")){
        pressedButtonValue=BUT_CUBE;
    }
    else if(clickedOperator==tr("=")){
        pressedButtonValue=BUT_EQU;
    }
    else if(clickedOperator==tr("Backspace")){
        pressedButtonValue=BUT_DEL;
    }
    else if(clickedOperator==tr("Clear")){
        pressedButtonValue=BUT_CLEAR;
    }
    else if(clickedOperator==tr("X")){
        pressedButtonValue=BUT_VARIABLE_X;
    }
    else if(clickedOperator==tr("Y")){
        pressedButtonValue=BUT_VARIABLE_Y;
    }
    else if(clickedOperator==tr("\\n")){
        pressedButtonValue=BUT_EQ_NL;
    }
    else if(clickedOperator==tr("sov")){
        pressedButtonValue=BUT_SOLVE;
    }
    m_serialHost2StmFrame.serialButtonType=pressedButtonValue;
    m_serialHost2StmFrame.serialHostPressCnt++;
    QByteArray data((char*)&m_serialHost2StmFrame, sizeof(m_serialHost2StmFrame));
    writeData(data);
}

void MainWindow::addToHistoryClicked(){
    m_calcHistory->addContact(QString(m_frameData.equationStr),QString(m_frameData.answerStr));
}
void MainWindow::openHistoryClicked(){
    m_calcHistory->showContact();
    m_calcHistory->show();
}
