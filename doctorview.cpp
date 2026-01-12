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

    IDatabase &iDatabase = IDatabase::getInstance();
    if(iDatabase.initBookModel()){
        ui->tableView->setModel(iDatabase.bookTabModel);
        ui->tableView->setSelectionModel(iDatabase.theBookSelection);

        // 可以根据需要显示或隐藏 BORROW_COUNT 列
        // 这里我们显示它，因为借阅次数是图书的重要信息
        int borrowCountIndex = iDatabase.bookTabModel->fieldIndex("BORROW_COUNT");
        if (borrowCountIndex != -1) {
            // 显示借阅次数列
            ui->tableView->showColumn(borrowCountIndex);

            // 调整列宽，使借阅次数列更易读
            ui->tableView->horizontalHeader()->setSectionResizeMode(borrowCountIndex, QHeaderView::ResizeToContents);
        }
    }
}

DoctorView::~DoctorView()
{
    delete ui;
}

void DoctorView::on_btAdd_clicked()
{
    int currow = IDatabase::getInstance().addNewBook();
    emit goBookEditView(currow);
}

void DoctorView::on_btEdit_clicked()
{
    QModelIndex curIndex = IDatabase::getInstance().theBookSelection->currentIndex();
    emit goBookEditView(curIndex.row());
}

void DoctorView::on_btSearch_clicked()
{
    QString filter = QString("name like '%%1%'").arg(ui->txSearch->text());
    IDatabase::getInstance().searchBook(filter);
}

void DoctorView::on_btDelete_clicked()
{
    IDatabase::getInstance().deleteCurrentBook();
}
