#ifndef DATABASE_H
#define DATABASE_H

#include <QDate>
#include <QDebug>
#include <QFile>
#include <QObject>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#define DATABASE_HOSTNAME "ExampleDataBase"
#define DATABASE_NAME "mydb.db"

#define TABLE_COMP "components"
#define COMP_ID "component_id"
#define COMP_NAME "name"
#define COMP_PRICE "price"
#define COMP_SHOP "shop_id"

#define TABLE_SHOP "shops"
#define SHOP_ID "shop_id"
#define SHOP_NAME "name"

class DataBase : public QObject
{
    Q_OBJECT
  public:
    explicit DataBase(QObject *parent = nullptr);
    ~DataBase();
    void connectToDataBase();
    bool inserIntoMainTable(const QVariantList &data);
    bool inserIntoDeviceTable(const QVariantList &data);

  private:
    QSqlDatabase db;

  private:
    bool openDataBase();
    bool restoreDataBase();
    void closeDataBase();
    bool createMainTable();
    bool createDeviceTable();
};

#endif   // DATABASE_H
