#ifndef ARROW_H_
#define ARROW_H_

#include <QVector>
#include <QLineF>

class Arrow {
public:
	/**
	 * Constructs an arrow shape with an arrow with a header size of 5.
	 */
	Arrow();

	/**
	 * Constructs an arrow shape
	 * @param size Size of the arrow header
	 */
	Arrow(float size);

	/**
	 * Constructs an arrow shape from point A to point B
	 * @param size Size of the arrow header
	 */
	Arrow(QPointF A, QPointF B, float size);

	/**
	 * @return a list of lines which represent an arrow shape.
	 * Can be painted in qt using the drawLines() function
	 */
	const QVector<QLineF> getArrowShape() const;

	/**
	 * Sets the coordinates of this line
	 * @param A from point of the line
	 * @param B to point of the line
	 */
	void setPoints(QPointF A, QPointF B);

private:
	QVector<QLineF> lines;
	float arrowSize;
};

#endif /* ARROW_H_ */
