#ifndef __TMSP_H
#define __TMSP_H

#include "alles.h"

struct tmsp_t; struct resource; struct train; struct activity; struct requirement; struct precedence;

typedef struct tmsp_t {
	vector<resource *> resources;
	vector<train *> trains;
	vector<precedence *> precedences;
	int n_resources, n_trains, n_activities;
} tmsp_t;

typedef struct resource {
	int capacity;
	char* name;
	vector<requirement *> requirements;
} resource;

typedef struct train {
	int release_date;
	int due_date;
	vector<activity *> activities;
	int n_activities;
	char* name;
	int start_vertex;
	int end_vertex;
} train;

typedef struct activity {
    int i, j; // voor debug/state outputten
	int duration, flex;
	char* name;
	int est, lst;
	vector<requirement *> requirements;
	vector<activity *> groupchilds;
	
	activity (int i, int j, int duration, char * name, int est, int lst) : i (i), j (j), duration (duration), flex (0), name (name), est (est), lst (lst) { }
} activity;

typedef struct requirement {
	int i; // train index
	int j; // activity index
	int k; // resource index
	int amount;
	
	requirement (int i, int j, int k, int amount) : i (i), j (j), k (k), amount (amount) { }
} requirement;

typedef struct precedence {
	int i1, j1, i2, j2;
    bool isHard;
} precedence;

void add_resource(int, int, char *);
void add_train(int, int, int, char *);
void add_activity(int, int, int, char *, int est = -1, int lst = -1);
void add_requirement(int, int, int, int);
void add_precedence(int, int, int, int, bool isHard);
void add_train_mutexes();
void add_flex(int, int, int);
void add_groupchild(int, int, int, int);

void update_starttimes(int i1, int j1, int i2, int j2);

extern tmsp_t* tmsp;

// resources
#define R(i)		((resource*)list_get(tmsp->resources, i))
#define C(i)		(R(i)->capacity)

// precedences
#define P(i)		((precedence*)list_get(tmsp->precedences, i))

// treinen
// #define T(i)		((train*)list_get(tmsp->trains, i))
#define RD(i)		(T(i)->release_date)
#define DD(i)		(T(i)->due_date)
#define N(i)		(T(i)->n_activities)

// activiteiten
#define A(i,j)		((activity*)list_get(((train*)list_get(tmsp->trains, i))->activities, j))
#define D(i,j)		(A(i,j)->duration)
#define EST(i,j)	(-W(AS(i,j), Z))
#define EET(i,j)	(-W(AE(i,j), Z))
#define REQ(i,j,k)	(len(A(i,j)->requirements) <= k ? 0 : A(i,j)->requirements[k])
#define Q(i,j,k)	(REQ(i,j,k) == 0 ? 0 : REQ(i,j,k)->amount)

#endif
