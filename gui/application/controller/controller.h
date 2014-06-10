#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "widgets/mainwindow.h"
#include "util/randomcolorgenerator.h"
#include "solve/solver.h"

class InstanceController;

class Controller : public QObject {
    Q_OBJECT

public:
    static bool develop;

private:
    MainWindow *mainWindow;
    RandomColorGenerator colorGenerator;
    bool clearSoftPrecedences;
    QSlider * framesSlider;
    bool paintFeasibleIntervals;
    bool paintFlexibilityIntervals;
    int untitledCounter;
    QString workingDirectory;
    
public:
    Controller(); ///< Constructor of controller. Opens DB connection
    ~Controller(); ///< Destructor of controller. Removes DB connection
    void showMainWindow(); ///< Creates and displays a main window
    MainWindow* getMainWindow(); ///< @return the main window belonging to this controller
    Instance* getCurrentInstance(); ///< @return the current active instance managed by the main window (the instance of the active tab).
    QList<Instance*> getAllInstances(); ///< @return a list of all instances opened in the main window (the instance of each opened tab).
    InstanceWidget* getCurrentInstanceWidget();///< @return the instance widget of the instance which is currently visible in the main window.
    InstanceController* getCurrentInstanceController();///< @return the controller of the currently visible instance in the main window.
    QColor getColorFor(QString); ///< @return a color which depends of the given string. Used for generating colors of activitywidgets.
    void startPaintingFramePrecedences(QList<Precedence *> precedences); ///< sets a list of precedences of the current frame in OverlayWidget.
    void setAutoClearPrecedences(bool); ///< sets a flag which indicates whether added soft precedences should be cleared before solving.
    void setFramesSlider(QSlider *); ///< sets and connects the frame slider
    QSlider * getFramesSlider(); ///< @return the currently active frame slider
    //void shadeActivities(int, int, Resource*); //
    //void shadeResources(Job*);
    
    bool isPaintingFeasibleIntervals(); ///< @return true iff feasible intervals are being painted for activities and groups
    void setPaintingFeasibleIntervals(bool val); ///< @return indicates whether feasible intervals should be painted
    bool isPaintingFlexibilityIntervals(); ///< @return true iff flexibility intervals are being painted for activities and group
    void setPaintingFlexibilityIntervals(bool val); ///< @return indicates whether flexibility intervals should be painted
    void setFlexTimes();

    void solve(Solver *); ///< solves the currently visible instance with the given solver
    void duplicateInstance(); ///< clone currently visible instance

    void setStatusMessage(QString, int timeout = 0); ///< shows message in statusbar
    
    QString getWorkingDirectory();///< @return working directory of current instance (location of file).
    void setWorkingDirectory(QString);///< sets working directory of current instance.
    
    int getUntitledNumber();///< @return how many untitled (unsaved) instances have been created.
public slots:
    void disableToolbarButtons(); ///< disables toolbarbuttons iff no instance is active.
    void autoClearPrecedences(); ///< determines whether soft precedences should be cleared before solving.
    void solve(); ///< determines which solver must be used and subsequently solves the instance.
    void solveWithOptions(); ///< solve the instance with execution flags
    void showAbout(); ///< shows the 'About' window
    void zoomIn(); ///< Zooms in
    void zoomOut(); ///< Zooms out
    void disableHistory(int range); ///<disables slider and sets range of the slider from 'range' to 'range'.
    void rewind(); ///< step forward in frames
    void forward(); ///< step backward in frames
    
    void toFrame(int frameNr); ///< go to frame frameNr.
    
    void doPaintFeasibleIntervals(); ///< determines whether feasible intervals should be painted
    void doFlexibilityIntervals(); ///< determines whether flexibility intervals should be painted.
};

#endif /* CONTROLLER_H_ */
