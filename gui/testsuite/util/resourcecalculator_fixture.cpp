#include  <gtest/gtest.h>
#include "util/resourcecalculator.h"
#include "model/resource.h"

class ResourceCalculatorTest: public ::testing::Test {

protected:
	Instance *instance;
	Resource *r;
	Job *j;
	ResourceCalculator* calc;
	Activity *a0, *a1, *a2, *a3, *a4, *a5, *a6;
	/*
	we setten hier steeds lst==est.
	*/
    virtual void SetUp() {
    	instance = new Instance();
    	r = instance->addResource(0, 3, "r1");
    	j = instance->addJob(0,  100, "job0");
    	calc = new ResourceCalculator(r);

    	a0 = new Activity(j, 0, "A0", 15);
		a0->setRequiredAmount(r,1);
		Group *g0 = new Group();
		g0->addActivity(a0);
		g0->setEST(0);
        g0->setST(a0->est());

		a1 = new Activity(j, 1, "A1", 5);
		a1->setRequiredAmount(r,1);
        Group *g1 = new Group();
        g1->addActivity(a1);
		g1->setEST(3);
        g1->setST(a1->est());

		a2 = new Activity(j, 2, "A2", 2);
		a2->setRequiredAmount(r,1);
        Group *g2 = new Group();
        g2->addActivity(a2);
		g2->setEST(5);
        g2->setST(a2->est());

		a3 = new Activity(j, 3, "A3", 10);
		a3->setRequiredAmount(r,1);
        Group *g3 = new Group();
        g3->addActivity(a3);
		g3->setEST(17);
        g3->setST(a3->est());

		a4 = new Activity(j, 4, "A4", 7);
		a4->setRequiredAmount(r,2);
        Group *g4 = new Group();
        g4->addActivity(a4);
		g4->setEST(20);
        g4->setST(a4->est());

		a5 = new Activity(j, 5, "A5", 3);
		a5->setRequiredAmount(r,18);
        Group *g5 = new Group();
        g5->addActivity(a5);
		g5->setEST(29);
        g5->setST(a5->est());

		a6 = new Activity(j, 6, "A5", 1);
		a6->setRequiredAmount(r,1);
        Group *g6= new Group();
        g6->addActivity(a6);
		g6->setEST(32);
        g6->setST(a6->est());

		j->addActivity(a0);
		j->addActivity(a1);
		j->addActivity(a2);
		j->addActivity(a3);
		j->addActivity(a4);
		j->addActivity(a5);
		j->addActivity(a6);

    }
    virtual void TearDown() {
    	delete instance;
    }
};
