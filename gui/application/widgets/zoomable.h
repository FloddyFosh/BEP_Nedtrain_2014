#ifndef ZOOMABLE_H
#define ZOOMABLE_H

#include <QWidget>

class InstanceController;

/**
 * Reusable QWidget subclass that draws a vertical line at the position of the mouse pointer.
 * Also hZoom and vZoom are editable in this class. Also paints vertical lines on the specified
 * positions. This is used when drawing dependencies to indicate effect of resource decrease.
 */
class Zoomable : public QWidget
{
    Q_OBJECT

    InstanceController * controller;
    int _hZoom;
    int _vZoom;
    int mouseX;
    QList<int> lineList;


public:
    explicit Zoomable(InstanceController *c, QWidget *parent = 0); ///< enables mouse tracking

    int hZoom() const; ///< the current hZoom factor, any pixel coordinate's x value should be multiplied by this value
    int vZoom() const; ///< the current vZoom factor, any pixel coordinate's y value should be multiplied by this value
    void addVerticalLine(int x); ///< adds vertical line, which will be painted on a paint event
    void clearVerticalLines(); ///< clears all vertical lines

protected:
    void paintEvent(QPaintEvent *); ///< paints the mouse following line and vertical lines
    void resizeEvent(QResizeEvent *); ///< emits dimensionChanged signal
    void mouseMoveEvent(QMouseEvent *); ///< emits mouseMoved signal

signals:
    void hZoomChanged(int); ///< emitted when calling setHZoom(int) has led to a different value for _hZoom
    void mouseXChanged(int); ///< emitted when calling setMouseX(int) has led to a different value for mouseX
    void dimensionChanged(const QSize); ///< emitted when resize event causes size change
    void mouseMoved(int,int); ///< emitted when mouse move event is occurs

public slots:
    void setHZoom(int); ///< updates hZoom, emits signal if new value is different from old value
    void setMouseX(int); ///< updates mouseX, emits signal if new value is different from old value

};

#endif // ZOOMABLE_H
