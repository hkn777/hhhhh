#ifndef IDATABASE_H
#define IDATABASE_H

#include <QObject>
#include <QtSql>
#include <QSqlDatabase>
#include <QDataWidgetMapper>

class IDatabase : public QObject
{
    Q_OBJECT
public:

    static IDatabase& getInstance()
    {
        static IDatabase instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    QString userLogin(QString userName,QString password);

private:
    explicit IDatabase(QObject *parent = nullptr);

    IDatabase(IDatabase const&)               = delete;
    void operator=(IDatabase const&)  = delete;

    QSqlDatabase database;

    void ininDatabase();

signals:

public:
    bool initPatientModel();
    int addNewPatient();
    bool searchPatient(QString filter);
    void deleteCurrentPatient();
    bool submitPatientEdit();
    void revertPatientEdit();

    QSqlTableModel *patientTabModel;
    QItemSelectionModel *thePatientSelection;


    bool initBookModel();
    int addNewBook();
    bool searchBook(QString filter);
    void deleteCurrentBook();
    bool submitBookEdit();
    void revertBookEdit();

    // 新增：借还书相关函数
    bool borrowBook(int row);
    bool returnBook(int row);

    QSqlTableModel *bookTabModel;
    QItemSelectionModel *theBookSelection;

    QSqlTableModel* getBookModel() { return bookTabModel; }
    QItemSelectionModel* getBookSelectionModel() { return theBookSelection; }

};

#endif // IDATABASE_H
