#include "language.h"

#include <QSettings>
#include <QFile>
#include <QTranslator>
#include <QDebug>

Language lang_nedtrain ("nedtrain", "NedTrain (Dutch)", ":/translations/translations/lang_nedtrain.qm"),
         lang_english  ("english",  "English",          ":/translations/translations/lang_english.qm");


Language::Language (QString id, QString name, QString pathToFile) : id (id), name (name), pathToFile (pathToFile) { }

void Language::setDefaultLang() {
    QSettings settings;
    settings.setValue("language", id);
}

Language * Language::check(Language * t) {
    return QFile::exists(t->pathToFile) ? t : 0;
}

Language * Language::defaultLang() {
    QSettings settings;
    if (settings.contains("language")) {
        QString name (settings.value("language").toString());
        if (name == lang_nedtrain.id) return check(&lang_nedtrain);
        if (name == lang_english .id) return check(&lang_english);
    }

    // default
    return check(&lang_english);
}

QList<Language *> Language::loadAll() {
    QList<Language *> talen;
    if (check(&lang_english))  talen << &lang_english;
    if (check(&lang_nedtrain)) talen << &lang_nedtrain;
    return talen;
}

bool Language::loadDefaultLang(QApplication & app) {
    QTranslator * translator (new QTranslator);
    Language * t (defaultLang());
    if (!t) {
        return false;
    }
    if (!translator->load(t->pathToFile)){
        qDebug() << "Error while loading language " << endl;
        return false;
    }

    app.installTranslator(translator);
    return true;
}
