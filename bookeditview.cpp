#include "bookeditview.h"
#include "ui_bookeditview.h"

BookEditView::BookEditView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BookEditView)
{
    ui->setupUi(this);
}

BookEditView::~BookEditView()
{
    delete ui;
}
