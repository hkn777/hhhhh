#ifndef BOOKEDITVIEW_H
#define BOOKEDITVIEW_H

#include <QWidget>
#include <QDataWidgetMapper>

namespace Ui {
class BookEditView;
}

class BookEditView : public QWidget
{
    Q_OBJECT

public:
    explicit BookEditView(QWidget *parent = nullptr,int index=0);
    ~BookEditView();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::BookEditView *ui;
    QDataWidgetMapper *dataBookMapper;

signals:
    void goPreviousView();
};

#endif // BOOKEDITVIEW_H
