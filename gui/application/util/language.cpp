#include "language.h"

#include <QSettings>
#include <QFile>
#include <QTranslator>
#include <QDebug>

// Path should be _without_ ".qm" extension.
//Linux
Language lang_nedtrain ("nedtrain", "NedTrain (Dutch)", "/../../gui/application/translations/lang_nedtrain"),
         lang_english  ("english",  "English",          "/../../gui/application/translations/lang_english");

//Windows
//Language lang_nedtrain ("nedtrain", "NedTrain (Dutch)", "/../../../gui/application/translations/lang_nedtrain"),
//         lang_english  ("english",  "English",          "/../../../gui/application/translations/lang_english");

Language::Language (QString id, QString name, QString pathToFile) : id (id), name (name), pathToFile (pathToFile) { }

void Language::setDefaultLang() {
    QSettings settings;
    settings.setValue("language", id);
}

Language * Language::check(Language * t) {
    return QFile::exists(QApplication::applicationDirPath() + t->pathToFile + ".qm") ? t : 0;
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
    if (!translator->load(QApplication::applicationDirPath() + t->pathToFile)){
        qDebug() << "Error while loading language " << endl;
        return false;
    }

    app.installTranslator(translator);
    return true;
}
