#include "alles.h"

#include "tmsp.h"
#include "output.h"

vector<activity *> nodesInVolgordeVanInput;

tmsp_t* tmsp;

void tmsp_init() {
	if (!tmsp) {
		tmsp = new tmsp_t;

		tmsp->n_resources = 0;
		tmsp->n_trains = 0;
		tmsp->n_activities = 0;
	}
}

void add_resource(int i, int capacity, char* name) {
	tmsp_init();

	resource* r = new resource;
	r->capacity = capacity;
	r->name = strdup(name);

    vector<resource *> & res (tmsp->resources);
    if (i >= len(res)) res.resize(i + 1, NULL);
	list_set(tmsp->resources, i, r);
    tmsp->n_resources = len(res);
}

void add_train(int i, int release_date, int due_date, char* name) {
	tmsp_init();

	train* t = new train;
	t->release_date = release_date;
	t->due_date = due_date;
	t->name = strdup(name);
	t->n_activities = 0;

    vector<train *> & trains (tmsp->trains);
    if (i >= len(trains)) trains.resize(i + 1, NULL);
	list_set(tmsp->trains, i, t);
    tmsp->n_trains = len(trains);
}

void add_activity(int i, int j, int duration, char* name, int est, int lst) {
	tmsp_init();

	activity* a = new activity (i, j, duration, strdup(name), est, lst);
    nodesInVolgordeVanInput.push_back(a);
    vector<activity *> & acts (Tr(i)->activities);
    if (j >= len(acts)) acts.resize(j + 1, NULL);
	list_set(Tr(i)->activities, j, a);
    N(i) = len(acts);
}

void add_requirement(int i, int j, int k, int q) {
	requirement* req = new requirement (i, j, k, q);

    vector<requirement *> & reqs (A(i, j)->requirements);
    if (k >= len(reqs)) reqs.resize(k + 1, NULL);
	list_set(A(i,j)->requirements, k, req); // save indexed by resource id in the activity
	list_append(R(k)->requirements, req);   // save in 'normal' list in the resource
}

void add_precedence(int i1, int j1, int i2, int j2, bool isHard) {
	precedence* p = new precedence;

	p->i1 = i1;
	p->j1 = j1;

	p->i2 = i2;
    p->j2 = j2;
    p->isHard = isHard;
    FOREACH(tmsp->precedences, it){
        precedence* p2 = *it;
        if(p->i1==p2->i1 && p->i2==p2->i2 && p->j1==p2->j1 && p->j2==p2->j2) return;
    }
    list_append(tmsp->precedences, p);
    output("PC: %d %d %d %d\n", i1, j1, i2, j2);
}

void add_flex(int i, int j, int duration) {
    A(i, j)->flex = duration;
}

void add_groupchild(int i1, int j1, int i2, int j2) {
    A(i1, j1)->groupchilds.append(A(i2, j2));
}

void add_train_mutexes() {
	int i,j,r;

	r = tmsp->n_resources;

	for (i = 0; i < tmsp->n_trains; i++, r++) if (Tr(i)) {
		add_resource(r, 1, strdup(Tr(i)->name));
		for (j = 0; j < N(i); j++) if (A(i,j)) {
			add_requirement(i, j, r, 1);
		}
	}
}		

void update_starttimes(int i1, int j1, int i2, int j2){
    activity* a1 = A(i1,j1);
    activity* a2 = A(i2,j2);
    a1->lst = min(a1->lst, a2->lst - a1->duration);
    a2->est = max(a2->est, a1->est + a1->duration);
}
