/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "widgets/instancemdiwidget.h"

#include<QActionGroup>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QCloseEvent;
class QScrollArea;
class QSplitter;
class QLabel;
QT_END_NAMESPACE

class Controller;


/**
 * This is the main window of the application. In the main window the menu and the tool bar
 * are initialized. The central widget (QMdiArea) is set up in this class as well.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT



public:
    /**
     * Sets up the UI with the QMDIArea, creates menus and toolbar and sets up all actions
     * @param c The controller of the application
     */
    MainWindow(Controller *c);

    /**
     * Enables some toolbar and menu buttons when an instance is active, otherwise these widgets are disabled
     */
    void setButtonState();

    /**
     * Indicate that the action must be disabled when the instance doesn't exist
     * @param a The action which should be disabled
     */
    void disableIfInstanceEmpty(QAction* a);

    InstanceMdiWidget* getCurrentInstanceMdiWidget(); ///< returns the MDI widget belonging to the active instance
    InstanceWidget* getCurrentInstanceWidget(); ///< returns the Instance widget belonging to the active instance
    void uncheckPrecedenceAdding(); ///< uncheck the push button for adding precedences
    void uncheckPrecedenceRemoving(); ///< uncheck the push button for
    void setLastSolver(QVariant); ///< Change function of the solve button on the tool bar. (the solve button must always solve with the last used solver)
    void synchronizeMenuWithInstance(); ///< Synchronizes menu options with properties of current instance. (Currently only whether time should be displayed in hours)
    void invokeTestMethod(QString methodName, QString param = ""); ///< method to let the testsuite call private methods in the mainwindow, for testing purposes only.
    bool eventFilter(QObject *o, QEvent *e); ///< used to define actions for pressing CTRL+W

    QAction *frameLeftAct;
    QAction *frameRightAct;
    QLabel *frameLabel;
    
    void rememberRecentFile(const QString &fileName); ///< save the recently opened files in QSettings
    void loadLastInstance(); ///< for developing purpose
protected:
    void readSettings();                 ///< load window settings (maximized state and geometry are saved)
    void closeEvent(QCloseEvent *event); ///< save window settings

private slots:
    void newInstance(); ///< handles File -- New Instance action
    void open(); ///< handles File -- Open Instance action
    void openRecent(); ///< handles opening an instance from the recent instances list in the File menu
    
    void newResource(); ///< handles the edit -- new resource action
    void newJob(); ///< handles the edit -- new job action
    void newActivity(); ///< handles the edit -- new activity action
    void newPrecedence(); ///< handles the edit -- new precedence action
    void removePrecedence(); ///< handles the edit -- remove precedence action
    void clearPrecedences(); ///< clears all soft precedences

    void configureSolvers(); ///< opens the SolverConfigDialog

    void importInstance(); ///< opens dialog to import instance

    void compareInstances(); ///< opens dialog to choose instances to be compared

    void manageTemplates(); ///< opens the ManageTemplatesDialog
    void hoursTimelineChanged(); ///< enables or disables hours on timeline

    void updateWindowTitle(const QString &fileName); ///< updates the window title, not sure if this is still used

    void langUpdated(); ///< called when the language was updated. Shows an info dialog
    
    void disablePrecedenceAdding(QKeyEvent *); ///< called when a key was pressed. Disables adding precedences if ESC key was pressed.
signals:
    void keyPressEvent(QKeyEvent *); ///< signal for key press event
private:
    Controller *controller;

    // ui init
    void createActions(); ///< setup the actions for the menu and the toolbar
    void createMenus(); ///< setup menu
    void createToolbar(); ///< setup toolbar
    void setupCentralWidget(); ///< setup MDI widget

    // action processing
    void loadFile(const QString &fileName); ///< load a saved instance
private:
    void updateRecentFileActions(); ///< update the array of recently opened files
    void updateSolverActions(); ///< load solvers from QSettings

    /* menus */
    QMenu *fileMenu, *editMenu, *solveMenu, *solveButtonMenu, *langMenu, *helpMenu;

    // file menu
    QAction *newAct, *openAct, *importAct, *saveAct, *saveAsAct, *closeAct, *exitAct, *separatorAct;

    enum { MaxRecentFiles = 5 };
    QAction *recentFileActs[MaxRecentFiles];

    // edit menu
    QAction *newResourceAct, *newJobAct, *newActivityAct, *newPrecedenceAct, *removePrecedenceAct,
    *clearPrecedencesAct, *autoClearPrecedencesAct, *useHoursAct, *setHoursAct, *manageTemplatesAct,
    *paintFeasibleIntervalsAct, *paintFlexibilityIntervalsAct;

    // solve menu
    QAction *configureSolversAction;
    QList<QAction *> solverActions;
    QAction *solveLastAction;
    QAction *solveWithParamsAction;
    
    // zoom
    QAction *zoomInAct;
    QAction *zoomOutAct;

    // compare
    QAction *compareAct, *compareAct2;

    QActionGroup languageActions;
    QAction *aboutAct;

    InstanceMdiWidget *tabwidget;
    QList<QAction*> disabledActions; ///< actions which are disabled if instance==0
    
    QSlider * framesSlider;
};

#endif
