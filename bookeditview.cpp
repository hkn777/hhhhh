#include "bookeditview.h"
#include "ui_bookeditview.h"
#include <QSqlTableModel>
#include "idatabase.h"


BookEditView::BookEditView(QWidget *parent,int index)
    : QWidget(parent)
    , ui(new Ui::BookEditView)
{
    ui->setupUi(this);

    dataBookMapper=new QDataWidgetMapper();
    QSqlTableModel *tabModel=IDatabase::getInstance().bookTabModel;
    dataBookMapper->setModel(IDatabase::getInstance().bookTabModel);
    dataBookMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

    dataBookMapper->addMapping(ui->dbEditID,tabModel->fieldIndex("ID"));
    dataBookMapper->addMapping(ui->dbEditName,tabModel->fieldIndex("NAME"));

    dataBookMapper->setCurrentIndex(index);

}

BookEditView::~BookEditView()
{
    delete ui;
}

void BookEditView::on_pushButton_clicked()
{
    IDatabase::getInstance().submitBookEdit();

    emit goPreviousView();
}


void BookEditView::on_pushButton_2_clicked()
{
    IDatabase::getInstance().revertBookEdit();

    emit goPreviousView();
}

