#ifndef BOOKEDITVIEW_H
#define BOOKEDITVIEW_H

#include <QWidget>

namespace Ui {
class BookEditView;
}

class BookEditView : public QWidget
{
    Q_OBJECT

public:
    explicit BookEditView(QWidget *parent = nullptr);
    ~BookEditView();

private:
    Ui::BookEditView *ui;
};

#endif // BOOKEDITVIEW_H
