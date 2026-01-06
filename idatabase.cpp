#include "idatabase.h"
#include <QUuid>

void IDatabase::ininDatabase()
{
    database=QSqlDatabase::addDatabase("QSQLITE");
    QString aFile = "E:/Qtzy/ALL/all.db";
    database.setDatabaseName(aFile);
    if(!database.open()){
        qDebug()<<"failed to open detabase";
    }else
        qDebug()<<"open detabase is ok"<< database.connectionName();
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
    bookTabModel =new QSqlTableModel(this,database);
    bookTabModel->setTable("book");
    bookTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    bookTabModel->setSort(bookTabModel->fieldIndex("name"),Qt::AscendingOrder);//排序
    if(!(bookTabModel->select()))
        return false;

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
