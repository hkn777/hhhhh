#include "idatabase.h"
#include <QUuid>

void IDatabase::ininDatabase()
{
    database=QSqlDatabase::addDatabase("QSQLITE");
    QString aFile = "E:/Qtzy/ALL/all.db";
    database.setDatabaseName(aFile);
    if(!database.open()){
        qDebug()<<"failed to open database";
    }else
        qDebug()<<"open database is ok"<< database.connectionName();

    // 检查 Book 表是否有 BORROW_COUNT 字段，如果没有则添加
    QSqlQuery checkQuery;
    checkQuery.exec("PRAGMA table_info(Book)");
    bool hasBorrowCountField = false;
    while (checkQuery.next()) {
        if (checkQuery.value(1).toString() == "BORROW_COUNT") {
            hasBorrowCountField = true;
            break;
        }
    }

    if (!hasBorrowCountField) {
        QSqlQuery addQuery;
        if (addQuery.exec("ALTER TABLE Book ADD COLUMN BORROW_COUNT INTEGER DEFAULT 0")) {
            qDebug() << "Added BORROW_COUNT column to Book table";
        } else {
            qDebug() << "Failed to add BORROW_COUNT column:" << addQuery.lastError().text();
        }
    }
}

bool IDatabase::initPatientModel()
{
    patientTabModel =new QSqlTableModel(this,database);
    patientTabModel->setTable("patient");
    patientTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    patientTabModel->setSort(patientTabModel->fieldIndex("name"),Qt::AscendingOrder);//排序
    if(!(patientTabModel->select()))
        return false;

    thePatientSelection =new QItemSelectionModel(patientTabModel);
    return true;
}

int IDatabase::addNewPatient()
{
    patientTabModel->insertRow(patientTabModel->rowCount(),QModelIndex());
    QModelIndex curIndex =patientTabModel->index(patientTabModel->rowCount()-1,1);//创建最后一行的ModelIndex

    int curRecNo =curIndex.row();
    QSqlRecord curRec =patientTabModel->record(curRecNo);
    curRec.setValue("CREATEDTIMESTAMP",QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    curRec.setValue("ID",QUuid::createUuid().toString(QUuid::WithoutBraces));

    patientTabModel->setRecord(curRecNo,curRec);

    return curIndex.row();

}
int IDatabase::addNewBook()
{
    bookTabModel->insertRow(bookTabModel->rowCount(),QModelIndex());
    QModelIndex curIndex =bookTabModel->index(bookTabModel->rowCount()-1,1);//创建最后一行的ModelIndex

    int curRecNo =curIndex.row();
    QSqlRecord curRec =bookTabModel->record(curRecNo);
    curRec.setValue("CREATEDTIMESTAMP",QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    curRec.setValue("ID",QUuid::createUuid().toString(QUuid::WithoutBraces));

    // 添加默认的 BORROW 和 BORROW_COUNT 字段值
    curRec.setValue("BORROW", "no");  // 新增图书默认为未借出
    curRec.setValue("BORROW_COUNT", 0); // 新增图书借阅次数为0

    bookTabModel->setRecord(curRecNo,curRec);

    return curIndex.row();
}
bool IDatabase::searchPatient(QString filter)
{
    patientTabModel->setFilter(filter);
    return patientTabModel->select();
}

void IDatabase::deleteCurrentPatient()
{
    QModelIndex curIndex = thePatientSelection->currentIndex();
    patientTabModel->removeRow(curIndex.row());
    patientTabModel->submitAll();
    patientTabModel->select();
}

bool IDatabase::submitPatientEdit()
{
    return patientTabModel->submitAll();
}

void IDatabase::revertPatientEdit()
{
    patientTabModel->revertAll();
}

bool IDatabase::initBookModel()
{
    // 如果模型已经存在，直接返回 true
    if (bookTabModel) {
        return true;
    }

    bookTabModel =new QSqlTableModel(this,database);

    // 注意：表名是 "Book"（大写B），不是 "book"
    bookTabModel->setTable("Book");
    bookTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    bookTabModel->setSort(bookTabModel->fieldIndex("name"),Qt::AscendingOrder);//排序

    // 设置列标题
    bookTabModel->setHeaderData(bookTabModel->fieldIndex("ID"), Qt::Horizontal, "ID");
    bookTabModel->setHeaderData(bookTabModel->fieldIndex("NAME"), Qt::Horizontal, "名称");

    // 检查是否有 BORROW 字段
    int borrowIndex = bookTabModel->fieldIndex("BORROW");
    if (borrowIndex != -1) {
        bookTabModel->setHeaderData(borrowIndex, Qt::Horizontal, "借出状态");
    }

    // 检查是否有 BORROW_COUNT 字段
    int borrowCountIndex = bookTabModel->fieldIndex("BORROW_COUNT");
    if (borrowCountIndex != -1) {
        bookTabModel->setHeaderData(borrowCountIndex, Qt::Horizontal, "借阅次数");
    }

    if(!(bookTabModel->select())) {
        qDebug() << "Failed to select from Book table:" << bookTabModel->lastError().text();
        return false;
    }

    theBookSelection =new QItemSelectionModel(bookTabModel);
    return true;
}



bool IDatabase::searchBook(QString filter)
{
    bookTabModel->setFilter(filter);
    return bookTabModel->select();
}

void IDatabase::deleteCurrentBook()
{
    QModelIndex curIndex = theBookSelection->currentIndex();
    bookTabModel->removeRow(curIndex.row());
    bookTabModel->submitAll();
    bookTabModel->select();
}

bool IDatabase::submitBookEdit()
{
    return bookTabModel->submitAll();
}

void IDatabase::revertBookEdit()
{
    bookTabModel->revertAll();
}

// 新增：借出图书时增加借阅次数
bool IDatabase::borrowBook(int row)
{
    if (!bookTabModel) {
        qDebug() << "Book model is not initialized";
        return false;
    }

    // 检查是否有 BORROW 字段
    int borrowFieldIndex = bookTabModel->fieldIndex("BORROW");
    if (borrowFieldIndex == -1) {
        qDebug() << "BORROW field not found";
        return false;
    }

    // 检查是否有 BORROW_COUNT 字段
    int borrowCountFieldIndex = bookTabModel->fieldIndex("BORROW_COUNT");
    if (borrowCountFieldIndex == -1) {
        qDebug() << "BORROW_COUNT field not found";
        return false;
    }

    // 检查当前图书是否已被借出
    QString borrowStatus = bookTabModel->record(row).value("BORROW").toString();
    if (borrowStatus == "yes") {
        qDebug() << "Book is already borrowed";
        return false;
    }

    // 获取当前借阅次数
    int currentBorrowCount = bookTabModel->record(row).value("BORROW_COUNT").toInt();

    // 更新借出状态
    bookTabModel->setData(bookTabModel->index(row, borrowFieldIndex), "yes");

    // 增加借阅次数
    bookTabModel->setData(bookTabModel->index(row, borrowCountFieldIndex), currentBorrowCount + 1);

    // 提交更改
    if (bookTabModel->submitAll()) {
        qDebug() << "Book borrowed successfully, borrow count:" << currentBorrowCount + 1;
        return true;
    } else {
        qDebug() << "Failed to borrow book:" << bookTabModel->lastError().text();
        bookTabModel->revertAll();
        return false;
    }
}

// 新增：归还图书
bool IDatabase::returnBook(int row)
{
    if (!bookTabModel) {
        qDebug() << "Book model is not initialized";
        return false;
    }

    // 检查是否有 BORROW 字段
    int borrowFieldIndex = bookTabModel->fieldIndex("BORROW");
    if (borrowFieldIndex == -1) {
        qDebug() << "BORROW field not found";
        return false;
    }

    // 检查当前图书是否已被借出
    QString borrowStatus = bookTabModel->record(row).value("BORROW").toString();
    if (borrowStatus == "no") {
        qDebug() << "Book is not borrowed";
        return false;
    }

    // 更新归还状态
    bookTabModel->setData(bookTabModel->index(row, borrowFieldIndex), "no");

    // 提交更改
    if (bookTabModel->submitAll()) {
        qDebug() << "Book returned successfully";
        return true;
    } else {
        qDebug() << "Failed to return book:" << bookTabModel->lastError().text();
        bookTabModel->revertAll();
        return false;
    }
}

QString IDatabase::userLogin(QString userName, QString password)
{
    QSqlQuery query;
    query.prepare("select username, password from user where username = :USER");
    query.bindValue(":USER",userName);
    query.exec();


    if(query.first() && query.value("username").isValid()){
        QString passwd = query.value("password").toString();
        if(passwd == password){
            return "LoginOK";
        }else{
            return"wrongPassword";
        }
    }else{
        qDebug()<<"no such user"<< userName;
        return "wrongUsername";
    }

}

IDatabase::IDatabase(QObject *parent)
    : QObject{parent}
{
    ininDatabase();
}
