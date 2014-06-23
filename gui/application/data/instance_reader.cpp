#include "data/instance_reader.h"

#include "model/instance.h"
#include "util/token.h"

#include <QFile>
#include <QTextStream>

Instance* InstanceReader::load(const QString &fileName) {
    Instance *instance = new Instance;
    instance->setFileName(fileName);

    if(readIntoInstance(fileName, instance))
        return instance;
    else
        return 0;
}

bool InstanceReader::import(Instance *instance, const QString &fileName) {
    return readIntoInstance(fileName, instance);
}

bool InstanceReader::readIntoInstance(const QString &fileName, Instance *instance) {
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
        return false;
    file.close();

    extern Instance *yyinstance;
    yyinstance = instance;
    lexin = new ifstream(fileName.toStdString().c_str());
    int error = yyparse();
    delete lexin;

    return !error;
}

bool InstanceReader::save(Instance *instance, const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Truncate ))
        return false;

    QTextStream out(&file);
    out << instance->toString();
    file.close();
    instance->setFileName(fileName);
    instance->emitSaved();

    return true;
}

Instance* InstanceReader::copy(Instance *instance) {
    QString tempFileName = "copy.tmp";
    QFile file(tempFileName);
    if (!file.open(QFile::WriteOnly | QFile::Truncate ))
        return 0;
    QTextStream out(&file);
    out << instance->toString();
    file.close();

    Instance *copy = load(tempFileName);
    copy->setFileName("");
    file.remove();

    return copy;
}
