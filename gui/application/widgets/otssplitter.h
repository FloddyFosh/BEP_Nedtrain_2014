#ifndef OTSSPLITTER_H
#define OTSSPLITTER_H

#include <QSplitter>

/**
 * A QSplitter with public method to change the splitter position.
 */
class OtsSplitter : public QSplitter
{
    Q_OBJECT

public:
    /**
     * Constructs OtsSplitter.
     * @param parent reference to parent widget
     */
    explicit OtsSplitter(QWidget *parent = 0);

    /**
     * Constructs OtsSplitter with orientation
     * @param o orientation
     * @param parent reference to parent widget
     */
    explicit OtsSplitter(Qt::Orientation o, QWidget *parent = 0);

public slots:
    /**
     * Moves the splitter at index index to position pos.
     * @param pos position where the splitter moves to
     * @param index index of the splitter
     */
    void moveSplitter(int pos, int index);
};

#endif // OTSSPLITTER_H
