/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include"calcHistory.h"

calcHistory::calcHistory(QWidget *parent)
    : QWidget(parent)
{
    QLabel *equationLabel = new QLabel(tr("Equation:"));
    equationLine = new QTextEdit;
    equationLine->setReadOnly(true);

    QLabel *answerLabel = new QLabel(tr("Answer:"));
    answerLine = new QTextEdit;
    answerLine->setReadOnly(true);


    nextButton = new QPushButton(tr("&Next"));
    nextButton->setEnabled(false);
    previousButton = new QPushButton(tr("&Previous"));
    previousButton->setEnabled(false);

    //dialog = new FindDialog(this);

    connect(nextButton, &QPushButton::clicked,
            this, &calcHistory::next);
    connect(previousButton, &QPushButton::clicked,
            this, &calcHistory::previous);

    QHBoxLayout *buttonLayout2 = new QHBoxLayout;
    buttonLayout2->addWidget(previousButton);
    buttonLayout2->addWidget(nextButton);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(equationLabel, 0, 0);
    mainLayout->addWidget(equationLine, 0, 1);
    mainLayout->addWidget(answerLabel, 1, 0, Qt::AlignTop);
    mainLayout->addWidget(answerLine, 1, 1);
    mainLayout->addLayout(buttonLayout2, 2, 1);

    setLayout(mainLayout);
    setWindowTitle(tr("Calculation History"));
}

void calcHistory::addContact(QString eqStr,QString answStr)
{
    if (eqStr.isEmpty() || answStr.isEmpty()) {
        QMessageBox::information(this, tr("Empty Field"),
                                 tr("Please enter = to calculate results."));
        return;
    }

        if (!contacts.contains(eqStr)) {
            contacts.insert(eqStr,answStr);
            QMessageBox::information(this, tr("Add Successful"),
                                     tr("This calculation has been added to your calculation history."));
        } else {
            QMessageBox::information(this, tr("Add Unsuccessful"),
                                     tr("Sorry, This calculation is already in your address book."));
        }
}

void calcHistory::updateInterface()
{
        if (contacts.isEmpty()) {
            equationLine->clear();
            answerLine->clear();
        }

        equationLine->setReadOnly(true);
        answerLine->setReadOnly(true);

        int number = contacts.size();
        nextButton->setEnabled(number > 1);
        previousButton->setEnabled(number > 1);
}

void calcHistory::next()
{
    QString name = equationLine->toPlainText();
    QMap<QString, QString>::iterator i = contacts.find(name);

    if (i != contacts.end())
        i++;

    if (i == contacts.end())
        i = contacts.begin();

    equationLine->setText(i.key());
    answerLine->setText(i.value());
    updateInterface();
}

void calcHistory::previous()
{
    QString name = equationLine->toPlainText();
    QMap<QString, QString>::iterator i = contacts.find(name);

    if (i == contacts.end()) {
        equationLine->clear();
        answerLine->clear();
        return;
    }

    if (i == contacts.begin())
        i = contacts.end();

    i--;
    equationLine->setText(i.key());
    answerLine->setText(i.value());
    updateInterface();
}
void calcHistory::showContact(){
    if(!contacts.isEmpty()){
        equationLine->setText(contacts.begin().key());
        answerLine->setText(contacts.begin().value());
    }
    else{
        equationLine->clear();
        answerLine->clear();
    }
    updateInterface();
}

