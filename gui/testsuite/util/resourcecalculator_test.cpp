#include  <gtest/gtest.h>
#include "model/resource.h"
#include "model/instance.h"
#include "model/resourcedecrease.h"
#include "resourcecalculator_fixture.cpp"

/*
 *        _                  ___   .   .
 *     __| |_               |   |  .   .
 *  __|      |______     ___|   |  |   |__
 * |                |___|       |__|      |
 * 0  3  5 7 8      15  17  20  27 29  32 33
 */
TEST_F(ResourceCalculatorTest, TestDemandProfile) {
	calc->calculate();
	QList<QPoint> demandProfile = calc->getDemandProfile();

    ASSERT_TRUE(demandProfile.contains(QPoint(0,1)));
    ASSERT_TRUE(demandProfile.contains(QPoint(3,2)));
    ASSERT_TRUE(demandProfile.contains(QPoint(5,3)));
    ASSERT_TRUE(demandProfile.contains(QPoint(7,2)));
    ASSERT_TRUE(demandProfile.contains(QPoint(8,1)));
    ASSERT_TRUE(demandProfile.contains(QPoint(15,0)));
    ASSERT_TRUE(demandProfile.contains(QPoint(17,1)));
    ASSERT_TRUE(demandProfile.contains(QPoint(20,3)));
    ASSERT_TRUE(demandProfile.contains(QPoint(27,0)));
    ASSERT_TRUE(demandProfile.contains(QPoint(29,18)));
    ASSERT_TRUE(demandProfile.contains(QPoint(32,1)));
    ASSERT_TRUE(demandProfile.contains(QPoint(33,0)));
}

TEST_F(ResourceCalculatorTest, TestJobProfile) {
	calc->setJob(j);
	calc->calculate();
	QList<QPoint> jobProfile = calc->getJobProfile();

    ASSERT_TRUE(jobProfile.contains(QPoint(0,1)));
    ASSERT_TRUE(jobProfile.contains(QPoint(3,2)));
    ASSERT_TRUE(jobProfile.contains(QPoint(5,3)));
    ASSERT_TRUE(jobProfile.contains(QPoint(7,2)));
    ASSERT_TRUE(jobProfile.contains(QPoint(8,1)));
    ASSERT_TRUE(jobProfile.contains(QPoint(15,0)));
    ASSERT_TRUE(jobProfile.contains(QPoint(17,1)));
    ASSERT_TRUE(jobProfile.contains(QPoint(20,3)));
    ASSERT_TRUE(jobProfile.contains(QPoint(27,0)));
    ASSERT_TRUE(jobProfile.contains(QPoint(29,18)));
    ASSERT_TRUE(jobProfile.contains(QPoint(32,1)));
    ASSERT_TRUE(jobProfile.contains(QPoint(33,0)));
}

TEST_F(ResourceCalculatorTest,TestResourceProfile) {
	calc->calculate();
	QList<QPoint> resProfile = calc->getResourceProfile();

	ASSERT_TRUE(resProfile.contains(QPoint(0,r->capacity())));

	ResourceDecrease* decrease1 = new ResourceDecrease(2,28,1,0);
	ResourceDecrease* decrease2 = new ResourceDecrease(16,21,1,1);
	r->addDecrease(decrease1);
	r->addDecrease(decrease2);

	calc->calculate();
	resProfile = calc->getResourceProfile();

	ASSERT_TRUE(resProfile.contains(QPoint(2,2)));
	ASSERT_TRUE(resProfile.contains(QPoint(16,1)));
	ASSERT_TRUE(resProfile.contains(QPoint(21,2)));
	ASSERT_TRUE(resProfile.contains(QPoint(28,3)));
}

TEST_F(ResourceCalculatorTest,TestExceed) {
	ResourceDecrease* decrease1 = new ResourceDecrease(2,28,1,0);
	ResourceDecrease* decrease2 = new ResourceDecrease(16,21,1,1);
	r->addDecrease(decrease1);
	r->addDecrease(decrease2);

	r->setCapacity(50);
	calc->calculate();
	QPolygon exceedPolygon = calc->getExceedPolygon(33);

	ASSERT_TRUE(exceedPolygon.isEmpty());

	r->setCapacity(3);
	calc->calculate();
	exceedPolygon = calc->getExceedPolygon(33);
	ASSERT_TRUE(exceedPolygon.contains(QPoint(5,3)));
	ASSERT_TRUE(exceedPolygon.contains(QPoint(7,3)));
	ASSERT_TRUE(exceedPolygon.contains(QPoint(5,2)));
	ASSERT_TRUE(exceedPolygon.contains(QPoint(7,2)));

	ASSERT_TRUE(exceedPolygon.contains(QPoint(20,3)));
	ASSERT_TRUE(exceedPolygon.contains(QPoint(27,3)));
	ASSERT_TRUE(exceedPolygon.contains(QPoint(20,1)));
	ASSERT_TRUE(exceedPolygon.contains(QPoint(27,2)));
	ASSERT_TRUE(exceedPolygon.contains(QPoint(21,2)));
}

TEST_F(ResourceCalculatorTest,TestSelect) {
	calc->calculate();
	QRect area;

	area = calc->selectedResourceRegion(QPoint(-1,-3));
	ASSERT_EQ(-1,area.x());
	ASSERT_EQ(-1,area.y());
	ASSERT_EQ(-1,area.width());

	area = calc->selectedResourceRegion(QPoint(6,3));
	ASSERT_EQ(5,area.x());
	ASSERT_EQ(3,area.y());
	ASSERT_EQ(2,area.width());

	area = calc->selectedResourceRegion(QPoint(21,1));
	ASSERT_EQ(17,area.x());
	ASSERT_EQ(1,area.y());
	ASSERT_EQ(10,area.width());

	area = calc->selectedResourceRegion(QPoint(21,2));
	ASSERT_EQ(20,area.x());
	ASSERT_EQ(2,area.y());
	ASSERT_EQ(7,area.width());
}
