#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "database.h"

#include <QMainWindow>
#include <QSqlRelation>
#include <QSqlRelationalDelegate>
#include <QSqlRelationalTableModel>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private slots:
    void on_checkBox_stateChanged(int state);

    void on_btnexit_clicked();

    void on_btnabout_clicked();

  private:
    Ui::MainWindow *ui;
    DataBase *db;
    QSqlRelationalTableModel *model;

  private:
    void setupModel(const QString &tableName, const QStringList &headers);
    void createUI();
};

#endif   // MAINWINDOW_H
