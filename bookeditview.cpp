#include "bookeditview.h"
#include "ui_bookeditview.h"
#include <QSqlTableModel>
#include "idatabase.h"


BookEditView::BookEditView(QWidget *parent,int index)
    : QWidget(parent)
    , ui(new Ui::BookEditView)
{
    ui->setupUi(this);

    dataBookMapper = new QDataWidgetMapper();
    QSqlTableModel *tabModel = IDatabase::getInstance().bookTabModel;
    dataBookMapper->setModel(IDatabase::getInstance().bookTabModel);
    dataBookMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

    dataBookMapper->addMapping(ui->dbEditID, tabModel->fieldIndex("ID"));
    dataBookMapper->addMapping(ui->dbEditName, tabModel->fieldIndex("NAME"));

    // 添加 BORROW_COUNT 字段映射（如果需要编辑借阅次数）
    int borrowCountIndex = tabModel->fieldIndex("BORROW_COUNT");
    if (borrowCountIndex != -1) {
        // 如果有对应的 UI 组件，可以添加映射
        // 例如：dataBookMapper->addMapping(ui->dbSpinBorrowCount, borrowCountIndex);

        // 注意：借阅次数通常不应该直接编辑，而是由系统自动更新
        // 所以这里我们可能不添加映射，或者将其设置为只读
    }

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
