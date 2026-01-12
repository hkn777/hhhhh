#include "borrowview.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QDebug>
#include <QTableView>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include "idatabase.h"  // 确保包含这个头文件

BorrowView::BorrowView(QWidget *parent)
    : QWidget(parent)
    , borrowModel(nullptr)
{
    setWindowTitle("图书借还管理");
    setupUI();
    setupModel();
}

BorrowView::~BorrowView()
{
    // 注意：这里不删除 borrowModel，因为它由 IDatabase 管理
}

void BorrowView::setupUI()
{
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 搜索区域
    QHBoxLayout *searchLayout = new QHBoxLayout();
    QLabel *searchLabel = new QLabel("搜索图书:", this);
    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("输入图书名称或ID");
    searchEdit->setMaximumWidth(200);

    btnSearch = new QPushButton("搜索", this);
    connect(btnSearch, &QPushButton::clicked, this, &BorrowView::onSearchBook);

    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(searchEdit);
    searchLayout->addWidget(btnSearch);
    searchLayout->addStretch();

    // 按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    btnBorrow = new QPushButton("借书", this);
    btnReturn = new QPushButton("还书", this);

    connect(btnBorrow, &QPushButton::clicked, this, &BorrowView::onBorrowBook);
    connect(btnReturn, &QPushButton::clicked, this, &BorrowView::onReturnBook);

    buttonLayout->addWidget(btnBorrow);
    buttonLayout->addWidget(btnReturn);
    buttonLayout->addStretch();

    // 表格视图
    tableView = new QTableView(this);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->setAlternatingRowColors(true);

    // 添加到主布局
    mainLayout->addLayout(searchLayout);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(tableView);

    // 设置表格列宽
    tableView->horizontalHeader()->setStretchLastSection(true);
}

void BorrowView::setupModel()
{
    // 使用 IDatabase 中的 bookTabModel
    IDatabase &db = IDatabase::getInstance();

    // 确保 book 模型已初始化
    if (!db.initBookModel()) {
        qDebug() << "Failed to initialize book model in BorrowView";
        QMessageBox::warning(this, "错误", "无法初始化图书数据模型");
        return;
    }

    borrowModel = db.bookTabModel;

    if (!borrowModel) {
        qDebug() << "Book model is null";
        return;
    }

    // 检查是否有 BORROW 字段
    int borrowFieldIndex = borrowModel->fieldIndex("BORROW");
    if (borrowFieldIndex == -1) {
        qDebug() << "BORROW field not found in Book table";
        QMessageBox::warning(this, "错误", "数据库中没有找到 BORROW 字段");
        return;
    }

    // 检查是否有 BORROW_COUNT 字段
    int borrowCountFieldIndex = borrowModel->fieldIndex("BORROW_COUNT");
    if (borrowCountFieldIndex == -1) {
        qDebug() << "BORROW_COUNT field not found in Book table";
        QMessageBox::warning(this, "错误", "数据库中没有找到 BORROW_COUNT 字段");
        return;
    }

    // 设置选择模型
    tableView->setModel(borrowModel);
    tableView->setSelectionModel(db.theBookSelection);

    // 刷新数据
    borrowModel->select();

    // 调试信息：检查表结构
    qDebug() << "Book table has" << borrowModel->columnCount() << "columns";
    qDebug() << "BORROW field index:" << borrowFieldIndex;
    qDebug() << "BORROW_COUNT field index:" << borrowCountFieldIndex;
}

void BorrowView::onBorrowBook()
{
    QModelIndex currentIndex = tableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "警告", "请先选择要借阅的图书");
        return;
    }

    int row = currentIndex.row();

    // 使用新的 borrowBook 函数，该函数会自动增加借阅次数
    if (IDatabase::getInstance().borrowBook(row)) {
        QMessageBox::information(this, "成功", "借书成功");
        borrowModel->select(); // 刷新数据

        // 显示借阅次数
        int borrowCount = borrowModel->record(row).value("BORROW_COUNT").toInt();
        QMessageBox::information(this, "借阅信息",
                                 QString("借书成功！\n该图书已被借阅 %1 次").arg(borrowCount));
    } else {
        QMessageBox::warning(this, "错误", "借书失败: 该图书可能已被借出或发生其他错误");
    }
}

void BorrowView::onReturnBook()
{
    QModelIndex currentIndex = tableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "警告", "请先选择要归还的图书");
        return;
    }

    int row = currentIndex.row();

    // 使用新的 returnBook 函数
    if (IDatabase::getInstance().returnBook(row)) {
        QMessageBox::information(this, "成功", "还书成功");
        borrowModel->select(); // 刷新数据

        // 显示借阅次数
        int borrowCount = borrowModel->record(row).value("BORROW_COUNT").toInt();
        QMessageBox::information(this, "借阅信息",
                                 QString("还书成功！\n该图书已被借阅 %1 次").arg(borrowCount));
    } else {
        QMessageBox::warning(this, "错误", "还书失败: 该图书可能未被借出或发生其他错误");
    }
}

void BorrowView::onSearchBook()
{
    QString searchText = searchEdit->text().trimmed();

    if (searchText.isEmpty()) {
        borrowModel->setFilter("");
    } else {
        // 搜索图书名称或ID
        QString filter = QString("NAME LIKE '%%1%' OR ID LIKE '%%1%'").arg(searchText);
        borrowModel->setFilter(filter);
    }

    if (!borrowModel->select()) {
        qDebug() << "Search failed: " << borrowModel->lastError().text();
    }
}
