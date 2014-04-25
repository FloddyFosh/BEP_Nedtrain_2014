#ifndef MYSPINBOX_H_
#define MYSPINBOX_H_

#include<QSpinBox>

/**
 * Spinbox which does not changes its value while scrolling on mouse over.
 */
class FilteredSpinBox : public QSpinBox {
public:
    /**
     * Constructs a filtered spinbox.
     * @param parent parent widget
     */
    FilteredSpinBox(QWidget *parent);

    /**
     * Invoked when spinbox gets focus.
     * @param e focus event
     */
    void focusInEvent(QFocusEvent *e);

    /**
     * Invoked when spinbox has lost focus.
     * @param e focus event
     */
    void focusOutEvent(QFocusEvent *e);
};

#endif
