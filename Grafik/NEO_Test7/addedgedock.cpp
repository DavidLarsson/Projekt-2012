#include "addedgedock.h"
#include "datadock.h"
#include <QGridLayout>
#include <QtGui>
#include <QDialog>
#include "neopushbutton.h"

AddEdgeDock::AddEdgeDock(QWidget *parent) :
    QDockWidget(parent)
{

    QWidget *wi = new QWidget;

    QVBoxLayout *layout = new QVBoxLayout;

    start = new QLineEdit();
    end = new QLineEdit();

    start->setText("Startnod");
    start->setGeometry(100,80,50,30);
    layout->addWidget(start);

    end->setText("Slutnod");
    end->setGeometry(100,120,50,30);
    layout->addWidget(end);

    add_edge_ = new neoPushButton(this);
    add_edge_->setGeometry(100,160,100,20);
    add_edge_->setText("L�gg till b�ge");
    layout->addWidget(add_edge_);

    remove_edge_ = new neoPushButton(this);
    remove_edge_->setGeometry(100,200,100,20);
    remove_edge_->setText("Ta bort b�ge");
    layout->addWidget(remove_edge_);

    //Nodinfo-ruta
/*
    QLabel *nodePrice1 = new QLabel("Test");

    layout->addWidget(nodePrice1);

    nodeFlow_ = new QGraphicsSimpleTextItem();
    nodeFlow_->setText("Nodfl�de: ");
    nodeFlow_->setPos(180,220);
    layout->addItem(nodeFlow_);
*/

    layout->addWidget(start);
    setAllowedAreas(Qt::LeftDockWidgetArea |
                                Qt::RightDockWidgetArea);

    setWindowTitle("L�gg till b�ge");


    wi->setLayout(layout);
    setWidget(wi);
}

QString AddEdgeDock::start_text()
{
    return start->text();
}

QString AddEdgeDock::end_text()
{
    return end->text();
}
