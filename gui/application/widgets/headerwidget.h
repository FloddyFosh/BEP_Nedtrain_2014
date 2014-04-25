#ifndef HEADERWIDGET_H_
#define HEADERWIDGET_H_

#include <QWidget>

/**
 * HeaderWidget is an abstract class that is inherited by
 * ResourceHeaderWidget and JobHeaderWidget. It draws a
 * line at the bottom of those widgets.
 */
class HeaderWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor, must be called by inherited classes.
     * @param parent reference to parent widget
     */
    HeaderWidget(QWidget *parent = 0);

    /**
     * Return the recommended size if it is a valid size.
     * @return recommended size
     */
    virtual QSize sizeHint() const  = 0;

protected:
    /**
     * Makes sure that a horizontal line is drawn at the bottom of a header widget.
     * @param e A QPaintEvent
     */
    void paintEvent(QPaintEvent *e);
};

#endif /* HEADERWIDGET_H_ */
