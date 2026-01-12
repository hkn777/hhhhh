#ifndef BORROWVIEW_H
#define BORROWVIEW_H

#include <QWidget>
#include <QSqlTableModel>

class QTableView;
class QPushButton;
class QLineEdit;

class BorrowView : public QWidget
{
    Q_OBJECT

public:
    explicit BorrowView(QWidget *parent = nullptr);
    ~BorrowView();

private slots:
    void onBorrowBook();
    void onReturnBook();
    void onSearchBook();

private:
    void setupUI();
    void setupModel();

    QSqlTableModel *borrowModel;
    QTableView *tableView;
    QPushButton *btnBorrow;
    QPushButton *btnReturn;
    QPushButton *btnSearch;
    QLineEdit *searchEdit;
};

#endif // BORROWVIEW_H
