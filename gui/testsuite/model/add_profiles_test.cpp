#include <gtest/gtest.h>

#include "model/chainframe.h"
#include "model/activity.h"
#include "model/chain.h"

TEST(AddProfilesTest, BothEmpty) {
    QVector<Activity*> vec;
    Chain* ch = new Chain(0,0,vec,NULL);
    ChainFrame* chfr = new ChainFrame(ch,new QList<QPoint*>());
    QList<QPoint*>* lista = new QList<QPoint*>();
    QList<QPoint*>* listb = new QList<QPoint*>();
    QList<QPoint*>* result = chfr->addProfiles(lista,listb);
    ASSERT_TRUE(result->empty());
}

TEST(AddProfilesTest, OneEmpty) {
    QVector<Activity*> vec;
    Chain* ch = new Chain(0,0,vec,NULL);
    ChainFrame* chfr = new ChainFrame(ch,new QList<QPoint*>());
    QList<QPoint*>* lista = new QList<QPoint*>();
    QList<QPoint*>* listb = new QList<QPoint*>();
    listb->append(new QPoint(0,0));
    listb->append(new QPoint(0,1));
    listb->append(new QPoint(3,1));
    listb->append(new QPoint(3,0));
    QList<QPoint*>* result = chfr->addProfiles(lista,listb);
    ASSERT_EQ(listb,result);
}

TEST(AddProfilesTest, Easy) {
    QVector<Activity*> vec;
    Chain* ch = new Chain(0,0,vec,NULL);
    ChainFrame* chfr = new ChainFrame(ch,new QList<QPoint*>());
    QList<QPoint*>* lista = new QList<QPoint*>();
    QList<QPoint*>* listb = new QList<QPoint*>();
    listb->append(new QPoint(0,0));
    listb->append(new QPoint(0,2));
    listb->append(new QPoint(1,2));
    listb->append(new QPoint(1,1));
    listb->append(new QPoint(2,1));
    listb->append(new QPoint(2,0));

    lista->append(new QPoint(0,0));
    lista->append(new QPoint(0,2));
    lista->append(new QPoint(1,2));
    lista->append(new QPoint(1,1));
    lista->append(new QPoint(2,1));
    lista->append(new QPoint(2,0));
    QList<QPoint*>* result = chfr->addProfiles(lista,listb);
    ASSERT_EQ(6,result->size());
    ASSERT_TRUE(result->at(0)->x()==0);ASSERT_TRUE(result->at(0)->y()==0);
    ASSERT_TRUE(result->at(1)->x()==0);ASSERT_TRUE(result->at(1)->y()==4);
    ASSERT_TRUE(result->at(2)->x()==1);ASSERT_TRUE(result->at(2)->y()==4);
    ASSERT_TRUE(result->at(3)->x()==1);ASSERT_TRUE(result->at(3)->y()==2);
    ASSERT_TRUE(result->at(4)->x()==2);ASSERT_TRUE(result->at(4)->y()==2);
    ASSERT_TRUE(result->at(5)->x()==2);ASSERT_TRUE(result->at(5)->y()==0);
}

TEST(AddProfilesTest, Difficult) {
    QVector<Activity*> vec;
    Chain* ch = new Chain(0,0,vec,NULL);
    ChainFrame* chfr = new ChainFrame(ch,new QList<QPoint*>());
    QList<QPoint*>* lista = new QList<QPoint*>();
    QList<QPoint*>* listb = new QList<QPoint*>();
    listb->append(new QPoint(0,0));
    listb->append(new QPoint(0,3));
    listb->append(new QPoint(2,3));
    listb->append(new QPoint(2,1));
    listb->append(new QPoint(4,1));
    listb->append(new QPoint(4,0));

    lista->append(new QPoint(1,0));
    lista->append(new QPoint(1,4));
    lista->append(new QPoint(2,4));
    lista->append(new QPoint(2,5));
    lista->append(new QPoint(3,5));
    lista->append(new QPoint(3,2));
    lista->append(new QPoint(5,2));
    lista->append(new QPoint(5,0));
    QList<QPoint*>* result = chfr->addProfiles(lista,listb);

    EXPECT_EQ(12,result->size());
    ASSERT_TRUE(result->at(0)->x()==0);ASSERT_TRUE(result->at(0)->y()==0);
    ASSERT_TRUE(result->at(1)->x()==0);ASSERT_TRUE(result->at(1)->y()==3);
    ASSERT_TRUE(result->at(2)->x()==1);ASSERT_TRUE(result->at(2)->y()==3);
    ASSERT_TRUE(result->at(3)->x()==1);ASSERT_TRUE(result->at(3)->y()==7);
    ASSERT_TRUE(result->at(4)->x()==2);ASSERT_TRUE(result->at(4)->y()==7);
    ASSERT_TRUE(result->at(5)->x()==2);ASSERT_TRUE(result->at(5)->y()==6);
    ASSERT_TRUE(result->at(6)->x()==3);ASSERT_TRUE(result->at(6)->y()==6);
    ASSERT_TRUE(result->at(7)->x()==3);ASSERT_TRUE(result->at(7)->y()==3);
    ASSERT_TRUE(result->at(8)->x()==4);ASSERT_TRUE(result->at(8)->y()==3);
    ASSERT_TRUE(result->at(9)->x()==4);ASSERT_TRUE(result->at(9)->y()==2);
    ASSERT_TRUE(result->at(10)->x()==5);ASSERT_TRUE(result->at(10)->y()==2);
    ASSERT_TRUE(result->at(11)->x()==5);ASSERT_TRUE(result->at(11)->y()==0);
}
