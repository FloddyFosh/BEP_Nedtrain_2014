#include "arrow.h"
#include <cmath>

Arrow::Arrow(){
	arrowSize=5;
}

Arrow::Arrow(float arrowSize) : arrowSize(arrowSize){

}

Arrow::Arrow(QPointF A, QPointF B, float arrowSize) : arrowSize(arrowSize) {
	setPoints(A,B);
}

void Arrow::setPoints(QPointF A, QPointF B){
	lines.clear();
	float h = arrowSize*sqrt(3.0), w = arrowSize;

	double dx = B.x()-A.x();
	double dy = B.y()-A.y();
	double length = sqrt(dx*dx + dy*dy);

	QPointF U = (B - A)/length;
	QPointF V = QPointF(-U.y(), U.x());
	QPointF v1 = B - h*U + w*V;
	QPointF v2 = B - h*U - w*V;

	QLineF baseLine = QLineF(A,B);
	QLineF l1 = QLineF(v1,B);
	QLineF l2 = QLineF(v2,B);

	lines.append(baseLine);
	lines.append(l1);
	lines.append(l2);
}

const QVector<QLineF> Arrow::getArrowShape() const {
	return lines;
}
