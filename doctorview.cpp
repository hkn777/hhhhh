#include "doctorview.h"
#include "ui_doctorview.h"
#include <QSqlTableModel>
#include "idatabase.h"

DoctorView::DoctorView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DoctorView)
{
    ui->setupUi(this);
    IDatabase &iDatabase =IDatabase::getInstance();
    if(iDatabase.initBookModel()){
        ui->tableView->setModel(iDatabase.bookTabModel);
        ui->tableView->setSelectionModel(iDatabase.theBookSelection);
    }
}

DoctorView::~DoctorView()
{
    delete ui;
}

void DoctorView::on_btAdd_clicked()
{
    emit goBookEditView();
}


void DoctorView::on_btSearch_clicked()
{

}


void DoctorView::on_btEdit_clicked()
{

}


void DoctorView::on_btDelete_clicked()
{

}

