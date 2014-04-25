#include "data/dbc.h"
#include <QSqlDatabase>
#include <QApplication>
#include <QDebug>

dbc::dbc() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QApplication::applicationDirPath()+"/db.sqlite");
    connectionName = db.connectionName();
}

dbc::~dbc() {
    close();
    QSqlDatabase::removeDatabase(connectionName);
}

void dbc::open() {
    QSqlDatabase db = QSqlDatabase::database();

    if(!db.isOpen()){
        bool succes = db.open();
        if(!succes)
        	qDebug() << "Database connection failed.";
    }
}

void dbc::close() {
    QSqlDatabase db = QSqlDatabase::database();

    if(db.isOpen()){
        db.close();
    }

}
