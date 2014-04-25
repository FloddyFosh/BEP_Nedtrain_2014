#ifndef OVERLAYWIDGET_H
#define OVERLAYWIDGET_H

#include <QPen>

#include "util/arrow.h"
#include "widgets/activitywidget.h"

struct InstanceController;

/**
 * This widget is a transparent overlay widget to paint dependencies on. It also
 * has an option to add an edge from a fixed point to a point defined by the
 * location of the mouse pointer. This edge is repainted when the mouse pointer
 * moves to a new location on the job zoomable.
 */
class OverlayWidget : public QWidget
{
    Q_OBJECT

    QPoint p1;
    Arrow arrow;
    int hZoom;
    QPen penHard, penSoft;
    bool isAddingPrecedence, isDrawingPrecedences;
    ActivityWidget* widget;
    InstanceController* controller;
    QList<Precedence *> framePrecedences;

    /**
     * Paints a precedence arrow or line between specified activity widgets.
     * @param painter painter to be used
     * @param from activity widget where new precedence leaves from
     * @param to activity widget where precedence line has to point to.
     * @param arrow set true if precedence has to be an arrow
     */
    void paintPrecedence(QPainter &painter, ActivityWidget *from, ActivityWidget *to, bool arrow);

    /**
     * Paints precedence adder arrow.
     * @param painter painter to be used
     */
    void paintPrecedenceAdding(QPainter &painter);


    /**
     * Paints all soft and hard precedences.
     * @param painter painter to be used
     */
    void paintPrecedences(QPainter &painter);

    /**
     * Paints incoming precedences.
     * @param painter the painter to be used
     * @param aWidget activity widget
     */
    void paintIncomingPrecedences(QPainter& painter, ActivityWidget *aWidget);

    /**
     * Paints outgoing precedences.
     * @param painter the painter to be used
     * @param aWidget activity widget
     */
    void paintOutgoingPrecedences(QPainter& painter, ActivityWidget *aWidget);

    /**
     * Paints all soft and hard precedences for the activitywidget.
     * @param painter painter to be used
     * @param aw activity widget
     */
    void paintPrecedences(QPainter& painter, ActivityWidget *aw);

    /**
     * Paints all frame precedences.
     * @param painter painter to be used
     */
    void paintFramePrecedences(QPainter &painter);

    /**
     * Sets highlight for incoming and outgoing widgets and draws vertical lines for decreases.
     * @param aw activity widget
     */
    void setHighlightAndVerticalLine(ActivityWidget *aw);

public:
    /**
     * Constructs an overlay widget and sets pen colors for soft and hard constraints.
     * @param controller instance controller
     * @param parent reference to parent widget
     */
    OverlayWidget(InstanceController* controller, QWidget * parent = 0);

    /**
     * Sets first point of the edge when adding a precedence.
     * @param p first point of precedence edge
     */
    void setPrecedencePoint(QPoint p);

    /**
     * Draws all dependencies associated with the specified activity widget.
     * @param aw activity widget for which dependencies have to be painted
     */
    void drawDependencies(ActivityWidget *aw);

    /**
     * Sets framePrecedences variable. (the precedences of the current frame, managed by the frame slider)
     * @param precedences list of precedences
     */
    void startPaintingFramePrecedences(QList<Precedence *> precedences);

	/**
	 * Stops drawing dependencies iff the overlay currently shows the dependencies of the specified activity widget.
	 * @param aw the selected activity widget
	 */
	void stopDrawingDependencies(ActivityWidget *aw);

	ActivityWidget* getActWidget(); ///< returns the activity widget
	void setHZoom(int h); ///< sets horizontal zoom factor
    void unsetPrecedencePoint(); ///< disables precedence adding and repaints the overlay
    void moveLine(int, int); ///< invoked when mouse pointer position changes, updates second point of precedence edge and repaints the widget
    void stopDrawingDependencies(); ///< stops drawing dependencies mode and removes highlights from activitywidgets

protected:
    void paintEvent(QPaintEvent*); ///< paint event to paint precedences, frame precedence and precedence adder edge

};

#endif // OVERLAYWIDGET_H
