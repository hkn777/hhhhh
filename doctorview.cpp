#include "doctorview.h"
#include "ui_doctorview.h"
#include <QSqlTableModel>
#include "idatabase.h"

DoctorView::DoctorView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DoctorView)
{
    ui->setupUi(this);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setAlternatingRowColors(true);

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
    int currow=IDatabase::getInstance().addNewBook();
    emit goBookEditView(currow);
}

void DoctorView::on_btEdit_clicked()
{
    QModelIndex  curIndex=IDatabase::getInstance().theBookSelection->currentIndex();
    emit goBookEditView(curIndex.row());
}

void DoctorView::on_btSearch_clicked()
{
    QString filter =QString("name like '%%1%'").arg(ui->txSearch->text());
    IDatabase::getInstance().searchBook(filter);
}

void DoctorView::on_btDelete_clicked()
{
    IDatabase::getInstance().deleteCurrentBook();
}

