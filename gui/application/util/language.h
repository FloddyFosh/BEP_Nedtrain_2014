#ifndef TAAL_H
#define TAAL_H

#include<QApplication>

/**
 * This class represents a language file and provides static functions to
 * load and get language objects.
 */
class Language : public QObject
{
    Q_OBJECT
public slots:
    void setDefaultLang(); ///< sets default language in qsettings, called by signal
public:
    /**
     * Constructs a language object and sets attributes.
     * @param id the id of the language
     * @param name the name of the language
     * @param pathToFile path to the language file, without .qm
     */
    Language (QString id, QString name, QString pathToFile);

    /**
     * @return The language currently set as default
     */
    static Language * defaultLang();

    /**
     * @return A list containing all available languages
     */
    static QList<Language *> loadAll();

    /**
     * Verifies if the language file exists.
     * @param t language object with path set
     * @return verified language object, for fluency
     */
    static Language * check(Language * t);

    /**
     * Loads and installs default language in the application object.
     * @param app application object to install the default language
     * @return true on success, false otherwise
     */
    static bool loadDefaultLang(QApplication & app);
    
    const QString id, name, pathToFile;
};
#endif
