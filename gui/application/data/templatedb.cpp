#include <QDebug>
#include <QSqlError>
#include <iostream>
#include "data/templatedb.h"

TemplateDB::TemplateDB() {
    db = new dbc();
    db->open();
    if(db->isOpenDB()) {
        init();
    }
}

TemplateDB::~TemplateDB() {
    delete db;
}

void TemplateDB::save(ActivityTemplate *activity) {
    QSqlQuery query;
    query.prepare("INSERT INTO activities (name, duration) VALUES (?, ?)");
    query.addBindValue(activity->getName());
    query.addBindValue(activity->getDuration());
    query.exec();
    activity->setID(query.lastInsertId().toInt());
    showQueryError(query);

    saveRequirements(activity->getRequirements(), query.lastInsertId());
}

void TemplateDB::saveRequirements(QMap<QString, int> map, QVariant lastId) {
    QSqlQuery query;
    QMapIterator<QString, int> i(map);
    while (i.hasNext()) {
        i.next();
        query.prepare("INSERT INTO requirements (activity, resource, demand) VALUES (?, ?, ?)");
        query.addBindValue(lastId);
        query.addBindValue(i.key());
        query.addBindValue(i.value());
        query.exec();
        showQueryError(query);
    }
}

ActivityTemplate* TemplateDB::get(int id) {
    QSqlQuery query;
    query.prepare("SELECT name, duration FROM activities WHERE id=?");
    query.addBindValue(id);
    query.exec();
    showQueryError(query);

    ActivityTemplate *a = NULL;

    if(query.next()) {
        // activity exists, add resource requirements
        a = new ActivityTemplate(id, query.value(0).toString(), query.value(1).toInt());
        a->setRequirements(getRequirements(id));
    }

    return a;
}

QMap<QString, int> TemplateDB::getRequirements(int id) {
    QMap<QString, int> map;

    QSqlQuery query;
    query.prepare("SELECT resource, demand FROM requirements WHERE activity=?");
    query.addBindValue(id);
    query.exec();
    showQueryError(query);

    while(query.next()) {
        map.insert(query.value(0).toString(), query.value(1).toInt());
    }

    return map;
}

QList<ActivityTemplate *> TemplateDB::getAll() {
    QList<ActivityTemplate *> list;

    QSqlQuery query("SELECT id, name, duration FROM activities");
    showQueryError(query);

    while(query.next()) {
        ActivityTemplate *a = new ActivityTemplate(query.value(0).toInt(), query.value(1).toString(), query.value(2).toInt());
        a->setRequirements(getRequirements(query.value(0).toInt()));
        list.append(a);
    }

	return list;
}

void TemplateDB::remove(int id) {
    QSqlQuery query;

    query.prepare("DELETE FROM activities WHERE id=?");
    query.addBindValue(id);
    query.exec();
    showQueryError(query);

    removeRequirements(id);
}

void TemplateDB::removeRequirements(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM requirements WHERE activity=?");
    query.addBindValue(id);
    query.exec();
    showQueryError(query);
}

void TemplateDB::remove(ActivityTemplate *activity){
    remove(activity->getID());
}

void TemplateDB::update(ActivityTemplate *activity){
    QSqlQuery query;
    query.prepare("UPDATE activities SET name=?, duration=? WHERE id=?");
    query.addBindValue(activity->getName());
    query.addBindValue(activity->getDuration());
    query.addBindValue(activity->getID());
    query.exec();
    showQueryError(query);

    removeRequirements(activity->getID());
    saveRequirements(activity->getRequirements(), activity->getID());
}

void TemplateDB::init() {
    QSqlQuery query;
    query.prepare("CREATE TABLE activities(id INTEGER PRIMARY KEY, name TEXT, duration INTEGER)");
    query.exec();
    showQueryError(query);
    query.prepare("CREATE TABLE requirements(activity INTEGER, resource TEXT, demand INTEGER)");
    query.exec();
    showQueryError(query);
}

void TemplateDB::showQueryError(QSqlQuery query) {
    if(query.lastError().type() != QSqlError::NoError) {
        qDebug() << query.lastError().databaseText();
    }
}
