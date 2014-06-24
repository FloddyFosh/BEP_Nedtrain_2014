#include <gtest/gtest.h>

#include "model/chainframe.h"
#include "model/activity.h"
#include "model/chain.h"

class AddProfilesTest : public ::testing::Test {
    protected:
      QVector<Activity*> vec;
      Chain* ch;
      ChainFrame* chfr;
      QList<QPoint*>* lista;
      QList<QPoint*>* listb;

      virtual void SetUp() {
          ch = new Chain(0, 0, vec, NULL);
          chfr = new ChainFrame(ch, new QList<QPoint*>());
          lista = new QList<QPoint*>();
          listb = new QList<QPoint*>();
      }

      virtual void TearDown() {
        delete ch;
      }
};

TEST_F(AddProfilesTest, BothEmpty) {
    QList<QPoint*>* result = chfr->addProfiles(lista, listb);
    EXPECT_TRUE(result->empty());
}

TEST_F(AddProfilesTest, OneEmpty) {
    listb->append(new QPoint(0, 0));
    listb->append(new QPoint(0, 1));
    listb->append(new QPoint(3, 1));
    listb->append(new QPoint(3, 0));
    QList<QPoint*>* result = chfr->addProfiles(lista, listb);
    EXPECT_EQ(listb, result);
}

TEST_F(AddProfilesTest, Easy) {
    int x[6] = {0, 0, 1, 1, 2, 2};
    int y[6] = {0, 2, 2, 1, 1, 0};
    for (int i = 0; i < 6; i++) {
        lista->append(new QPoint(x[i], y[i]));
        listb->append(new QPoint(x[i], y[i]));
    }

    QList<QPoint*>* result = chfr->addProfiles(lista, listb);
    ASSERT_EQ(6, result->size());

    int xa[6] = {0, 0, 1, 1, 2, 2};
    int ya[6] = {0, 4, 4, 2, 2, 0};

    for (int i = 0; i < 6; i++) {
        EXPECT_EQ(xa[i], result->at(i)->x());
        EXPECT_EQ(ya[i], result->at(i)->y());
    }
}

TEST_F(AddProfilesTest, Difficult) {
    int xa[8] = {1, 1, 2, 2, 3, 3, 5, 5};
    int ya[8] = {0, 4, 4, 5, 5, 2, 2, 0};
    for (int i = 0; i < 8; i++) {
        lista->append(new QPoint(xa[i], ya[i]));
    }    

    int xb[6] = {0, 0, 2, 2, 4, 4};
    int yb[6] = {0, 3, 3, 1, 1, 0};
    for (int i = 0; i < 6; i++) {
        listb->append(new QPoint(xb[i], yb[i]));
    }

    QList<QPoint*>* result = chfr->addProfiles(lista, listb);

    ASSERT_EQ(12, result->size());
    int xc[12] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5};
    int yc[12] = {0, 3, 3, 7, 7, 6, 6, 3, 3, 2, 2, 0};
    for (int i = 0; i < 12; i++) {
        EXPECT_EQ(xc[i], result->at(i)->x());
        EXPECT_EQ(yc[i], result->at(i)->y());
    }
}
