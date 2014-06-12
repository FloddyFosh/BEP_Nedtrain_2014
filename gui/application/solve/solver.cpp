#include <QApplication>
#include <QSettings>
#include <QFile>
#include <QDebug>

#include "solver.h"
#include "controller/exceptions.h"
#include "model/frame.h"
#include "model/instance.h"
#include "model/chain.h"
#include "model/chainframe.h"

Solver::Solver(QString name, QString binary, QString arguments, QObject *parent) :
    QObject (parent), name (name), binary (binary), arguments (arguments), instance(0), solved(false), peakResource(-1), mutexJob(-1) {}

QList<Solver *> Solver::loadAll() {
    QList<Solver *> solvers;

    QSettings settings;

    settings.beginGroup("solvers");

    QStringList groups = settings.childGroups();
    for (int i = 0; i < groups.size(); ++i) {
        settings.beginGroup(groups[i]);

        QString binary = settings.value("binary").toString();
        QString arguments = settings.value("arguments").toString();

        solvers.append(new Solver(groups[i], binary, arguments));

        settings.endGroup();
    }

    return solvers;
}

Solver *Solver::load(QString name) {
    QSettings settings;

    settings.beginGroup("solvers");
    settings.beginGroup(name);

    if (!settings.contains("binary"))
        throw SolverNotFoundException();

    QString binary = settings.value("binary").toString();
    QString arguments = settings.value("arguments").toString();

    QFile f(binary);
    if(!f.exists())
        throw SolverNotFoundException();

    return new Solver(name, binary, arguments);
}

void Solver::remove() {
    QSettings settings;
    settings.beginGroup("solvers");
    settings.remove(name);
}

QString Solver::getName() const { return name; }
QString Solver::getBinary() const { return binary; }
QString Solver::getArguments() const { return arguments; }

void Solver::setName(QString newname) {
    remove();
    name = newname;
    save();
}

void Solver::setBinary(QString newbinary) {
    binary = newbinary;
    save();
}

void Solver::setArguments(QString newarguments) {
    arguments = newarguments;
    save();
}

void Solver::save() {
    QSettings settings;
    settings.beginGroup("solvers");
    settings.beginGroup(name);
    settings.setValue("binary", binary);
    settings.setValue("arguments", arguments);
}

bool Solver::start(Instance *p) {
    instance = p;
    replayFrames.clear();
    wasLocked.clear();
    // Save first frame.
    Frame * first_frame (new Frame);
    foreach(Group * g, instance->getGroups()) {
        first_frame->addGroup(g);
        foreach(Activity * a, g->getActivities()) {
            wasLocked[a] = g->isLocked();
        }
        if (not g->isLocked())
            g->setST(g->getEST());
    }
    replayFrames.append(first_frame);
    
    QByteArray ba;
    QString s = instance->toString();
    ba.append(s);

    connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(solverReadOutput()));
    connect(&process, SIGNAL(readyReadStandardError()), this, SLOT(solverReadDebug()));
    connect(&process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(solverFinished(int, QProcess::ExitStatus)));

    process.start(binary, arguments.split(" "));

    process.write(ba);
    process.closeWriteChannel();

    return true;
}

void Solver::solverFinished(int x, QProcess::ExitStatus state) {
    (void) x;
    if(state == QProcess::NormalExit) {
        solverReadOutput();

        // copy last frame
        Frame * lastFrame (new Frame);
        foreach(Group * g, replayFrames.back()->getGroups()) {
            Group * g_ (new Group (g->getJob(), g->getEST(), g->getLST(), g->getST()));
            g_->setLocked(g->isLocked());
            foreach(Activity * a, g->getActivities()) g_->addActivity(a);
            lastFrame->addGroup(g_);
        }
        setFlexGroups(lastFrame->getGroups());
        replayFrames.append(lastFrame);
        instance->setFrames(replayFrames);
    }
    emit finished(state);
}

void Solver::solverReadOutput() {
    process.setReadChannel(QProcess::StandardOutput);
    while (process.canReadLine()) {
        QByteArray line = process.readLine();
        if (line.startsWith("PROGRESS: ")) {
            processProgressLine(line);
        } else if (line.startsWith("PC: ")) {
            processPrecedenceLine(line);
        } else if (line.startsWith("STATE: ")) {
            processStateLine(line);
        } else if (line.startsWith("PEAK: ")) {
            processPeakLine(line);
        } else if (line.startsWith("FLEX: ")) {
            processFlexLine(line);
        } else if (line.startsWith("MUTEX: ")) {
            processMutexLine(line);
        } else if (line.startsWith("STATUS: ")) {
            emit statusReceived(QString(line.trimmed()));
            emit messageReceived(QString(line.trimmed()));
        } else if (line.startsWith("CLEARSOFTPREC")) {
           instance->clearSoftPrecedences();
        } else if (line.startsWith("CHAIN: ")) {
            processChainLine(line);
        }
        else if(line.startsWith("FLEX: ")) {
            //processFlexLines(line);
        }
        else if(line.startsWith("Instance solved")) {
                setSolved(true);
        } else if(line.startsWith("Instance not solved")) {
                setSolved(false);
        } else if (line.startsWith("MRG: ") or line.startsWith("EST: ")) {
            /* ignored for now */
        } else if (line == "") {
            continue;
        } else {
            qDebug() << "Unknown Solver Output type: " << line;
        }

    }
}

void Solver::solverReadDebug() {
    process.setReadChannel(QProcess::StandardError);
    while (process.canReadLine()) {
        QByteArray line = process.readLine();
        emit messageReceived(QString(line.trimmed()));
    }
}

void Solver::cancel() {
    process.kill();
}

void Solver::setSolved(bool s) {
    solved = s;
}

bool Solver::isSolved() {
    return solved;
}

int Solver::getPeakResource() {
    return peakResource;
}

int Solver::getMutexJob() {
    return mutexJob;
}

void Solver::processProgressLine(QByteArray &line) {
    QList<QByteArray> tokens = line.trimmed().split(' ');
    if (tokens.size() == 2) {
        int progress = tokens[1].toInt();
        emit progressMade(progress);
    }
}

void Solver::processPrecedenceLine(QByteArray &line) {
    QList<QByteArray> fields = line.trimmed().split(' ');

    if(fields.size() == 5) {
        fields.takeFirst();
        int j1 = fields.takeFirst().toInt();
        int a1 = fields.takeFirst().toInt();
        int j2 = fields.takeFirst().toInt();
        int a2 = fields.takeFirst().toInt();

        try {
            instance->addPrecedence(j1, a1, j2, a2, false, replayFrames.size());
        } catch(InstanceManipulationException const& e) { e.printWarning(); }
    }
}

void Solver::processStateLine(QByteArray &line) {
    QList<QByteArray> fields = line.trimmed().split(' ');
    fields.takeFirst();
    Frame * nextFrame (new Frame);
    processStateGroups(fields, nextFrame);
    replayFrames.append(nextFrame);
    instance->setFrames(replayFrames);
}

void Solver::processStateGroups(QList<QByteArray> fields, Frame *frame) {
    while (1) {
        int jobId = fields.takeFirst().toInt();
        if (jobId == -1) break;
        int est   = fields.takeFirst().toInt();
        int lst   = fields.takeFirst().toInt();
        int nActs = fields.takeFirst().toInt();
        Group * g (new Group(jobId, est, lst, est));
        bool isResourceDecrease = (instance->J(jobId) == NULL);
        if (isResourceDecrease) {
            while (nActs --) fields.takeFirst(), fields.takeFirst();
        }
        else {
            while (nActs --) {
                int jobId1 = fields.takeFirst().toInt();
                int actId1 = fields.takeFirst().toInt();
                
                g->addActivity(instance->A(jobId1, actId1));
                if (wasLocked[instance->A(jobId1, actId1)]) g->setLocked(true);
            }
            frame->addGroup(g);
        }
    }
}

void Solver::processPeakLine(QByteArray &line) {
    QList<QByteArray> fields = line.trimmed().split(' ');
    fields.takeFirst();

    int time = fields.takeFirst().toInt();
    peakResource = fields.takeFirst().toInt();
    fields.takeFirst().toInt();
    emit peak(time, peakResource);

    eatRemainingOutput(fields);
}

QMap<QPair<int, int>, int> estMap;
QMap<QPair<int, int>, int> lstMap;
void Solver::processFlexLine(QByteArray &line) {
    QList<QByteArray> fields = line.trimmed().split(' ');
    fields.takeFirst();

    int minflex = fields.takeFirst().toInt();
    int flextotaal = fields.takeFirst().toInt();
    qDebug() << minflex;

    int act_i = fields.takeFirst().toInt();
    while(act_i != -1){
        int act_j = fields.takeFirst().toInt();
        bool lst = fields.takeFirst().at(0) == '+';
        int time = fields.takeFirst().toInt();

        QPair<int, int> temp(act_i, act_j);
        if(lst) {
            lstMap[temp] = time;
        } else {
            estMap[temp] = time;
        }

        act_i = fields.takeFirst().toInt();
    }
}

void Solver::setFlexGroups(QVector<Group *> groups) {
    // now loop over all activities in the current instance and set est and lst
    if(estMap.size() == 0 || lstMap.size() == 0) return;

    foreach(Group* g, groups) {
        QList<Activity *> activities = g->getActivities();
        foreach(Activity* a, activities) {
            int act_i = a->job()->id();
            int act_j = a->id();
            QPair<int, int> temp(act_i, act_j);
            g->setESTFlex(estMap[temp]);
            g->setLFTFlex(lstMap[temp] + g->getDuration());
        }
    }
    estMap.clear();
    lstMap.clear();
}

void Solver::processMutexLine(QByteArray &line) {
    QList<QByteArray> fields = line.trimmed().split(' ');
    fields.takeFirst();
    fields.takeFirst();
    mutexJob = fields.takeFirst().toInt();
    fields.takeFirst();

    eatRemainingOutput(fields);
}

void Solver::processChainLine(QByteArray &line) {
    QList<QByteArray> fields = line.trimmed().split(' ');
    fields.takeFirst();

    int res, prevRes, chain, numActs, ai, aj;
    prevRes = -1;
    if(Chain* lastChain = replayFrames.last()->getChain()){
        prevRes = lastChain->resourceId();
    }
    QMap<int,Resource*> resources = instance->getResources();
    res = fields.takeFirst().toInt();
    while(res != -1){
        Resource* curResource = resources[res];
        chain = fields.takeFirst().toInt();
        numActs = fields.takeFirst().toInt();
        for(int i=0;i<numActs;i++){
            ai = fields.takeFirst().toInt();
            aj = fields.takeFirst().toInt();
            Activity* act = instance->getJobs()[ai]->getActivities().value(aj);
            curResource->addActToChain(act,chain);
        }

        Chain* c = curResource->getChains()->value(chain);
        QList<QPoint*>* usedProfile = new QList<QPoint*>;
        if(res==prevRes){
            Frame* lastFrame = replayFrames.last();
            usedProfile = lastFrame->getSelectedProfile();
        }

        ChainFrame* nextFrame = new ChainFrame(c,usedProfile);
        foreach(Group* g, replayFrames.last()->getGroups()){
            nextFrame->addGroup(g);
        }
        replayFrames.append(nextFrame);
        instance->setFrames(replayFrames);

        prevRes = res;
        res = fields.takeFirst().toInt();
    }
}

void Solver::eatRemainingOutput(QList<QByteArray> &fields) {
    while (1) {
        int jobId = fields.takeFirst().toInt();
        if (jobId == -1) break;
        fields.takeFirst();
    }
}

QProcess* Solver::getProcess() {
    return &process;
}
