#include "alles.h"

#include "stjn.h"
#include "tmsp.h"
#include "debug.h"

node_t * cycle_node; // hier gedefinieerd

void chkEST()
{
    REP(i, tmsp->n_trains) if (T(i)) REP(j, N(i)) if (A(i,j) and len(acts[i][j]->group))
    {
        node_t * t (acts[i][j]);
        assert(t->est >= RD(i));
        FOREACH(t->next, it) { node_t * t2 (*it); assert(t->est + t->len <= t2->est); }
    }
}
void chkLST()
{
    bool wrong (0);
    REP(i, tmsp->n_trains) if (T(i)) REP(j, N(i)) if (A(i,j) and len(acts[i][j]->group))
    {
        node_t * t (acts[i][j]);
        assert(t->lst + t->len <= DD(i));
        FOREACH(t->prev, it)
        {
            node_t * t2 (*it);
            if (!(t2->lst + t2->len <= t->lst))
            {
                cout << "wrong: t = " << t << ", prev = " << t2 << endl;
                wrong = 1;
            }
        }
    }
    if (wrong) throw 0;
}

void stjn_calculate_est();
int stjn_calculate_lst();

stjn_t *stjn;
node_t ***acts;
vector<node_t *> ordinv;

void set_precedence(node_t *n, node_t *m) {
    n->next.append(m);
    m->prev.append(n);
}

int stjn_construct() {
    int i, j;

    stjn = new stjn_t;
    acts = (node_t ***) calloc(tmsp->n_trains, sizeof(node_t **));
    ordinv.clear();

    map<activity *, node_t *> activity_to_node;
    for(i = 0; i < tmsp->n_trains; i++) if (T(i)) {
        acts[i] = (node_t **) calloc(N(i), sizeof(node_t *));
        for(j = 0; j < N(i); j++) if (A(i,j)) {
            A(i,j)->requirements.resize(tmsp->n_resources);
            acts[i][j] = new node_t;
            acts[i][j]->len = D(i,j);
            if (A(i,j)->flex) {
                acts[i][j]->canmerge = false;
                acts[i][j]->len += A(i,j)->flex;
                acts[i][j]->flex = A(i,j)->flex;
            }
            else {
                acts[i][j]->canmerge = true;
                acts[i][j]->flex = 0;
            }

            acts[i][j]->i = i;
            acts[i][j]->j = j;
            acts[i][j]->group.append(A(i, j));
            activity_to_node[A(i, j)] = acts[i][j];
        }
    }

    for(i = 0; i < tmsp->n_trains; i++) if (T(i)) {
        for(j = 0; j < N(i); j++) if (A(i,j) and len(A(i,j)->groupchilds)) {
            node_t * parent (acts[i][j]);
            FOREACH(A(i,j)->groupchilds, it)
            {
                node_t * child (acts[(*it)->i][(*it)->j]);
                stjn_merge_init(parent, child);
                activity_to_node[*it] = parent;
            }
        }
    }

    for(i = 0; i < tmsp->n_trains; i++) if (T(i)) {
        for(j = 0; j < N(i); j++) if (A(i,j) and len(acts[i][j]->group)) {
            acts[i][j]->est = A(i,j)->est != -1 ? A(i,j)->est : RD(i);
            acts[i][j]->node_pred = NULL;
            acts[i][j]->lst = A(i,j)->lst != -1 ? A(i,j)->lst : DD(i) - acts[i][j]->len;
            acts[i][j]->node_succ = NULL;
            acts[i][j]->old_est = -1;
        }
    }

    FOREACH(tmsp->precedences, it) {
        set_precedence(activity_to_node[A((*it)->i1, (*it)->j1)],
                       activity_to_node[A((*it)->i2, (*it)->j2)]);
    }

    stjn_calculate_est();
    int re = stjn_calculate_lst();
    chkEST(), chkLST();
    return re;
}

void stjn_calculate_est() {
    int order=0;
    deque<node_t *> q;

    for (int i = 0; i < tmsp->n_trains; i++) if (T(i)) {
        for (int j = 0; j < N(i); j++) if (A(i,j)) {
            if (acts[i][j]->prev.empty())
                q.push_back(acts[i][j]);
            acts[i][j]->flag = len(acts[i][j]->prev);
        }
    }

    while (!q.empty()) {
        node_t * n = q.front(); q.pop_front();

        n->order = order;
        ordinv.append(n);
        order++;

        FOREACH(n->next, it) {
            node_t * m (*it);
            if (m->est < n->est + n->len) {
                m->est = n->est + n->len;
                m->node_pred = n;
            }

            m->flag--; if(!m->flag) q.push_back(m);
            m->old_est = -1;
        }
    }
}

int stjn_calculate_lst() {
    int inconsistent=0;
    deque<node_t *> q;

    for (int i = 0; i < tmsp->n_trains; i++) if (T(i)) {
        for (int j = 0; j < N(i); j++) if (A(i,j)) {
            if(len(acts[i][j]->next) == 0)
                q.push_back(acts[i][j]);
            acts[i][j]->flag = len(acts[i][j]->next);
        }
    }

    while (!q.empty()) {
        node_t * n = q.front(); q.pop_front();
        if (n->lst < n->est) inconsistent = 1, cycle_node = n;

        FOREACH(n->prev, it) {
            node_t * m (*it);
            if (m->lst > n->lst - m->len) {
                m->lst = n->lst - m->len;
                m->node_succ = n;
            }

            m->flag--; if(!m->flag) q.push_back(m);
        }
    }
    return !inconsistent;
}

// Marchetti-Spaccamela et al.: "Maintaining a topological order under edge insertions", Information Processing Letters 59 (1996) 53-58
int update_ordering(node_t *n, node_t *m) {
    int shift, end;
    deque<node_t *> q;

    if(n->order < m->order)
        return 1; // topological order is still correct, do nothing

    // The constraint n --> m has been added, but m is ordered before n
    // The algorithm focuses on the set of unstable vertices: the vertices not after n and not before m
    // First, mark successors of m which have a topol.ordering lower than n: these should be updated
    q.push_back(m);
    while(!q.empty()) {
        node_t * w = q.front(); q.pop_front();
        if(w->flag == 0) {
            if(w->order > n->order) {
                continue; // Vertex is ordered after n, we can safely ignore this part
            } else {
                w->flag = 1; // Vertex is ordered before n but after m: needs update, flag
            }
            FOREACH(w->next, it) q.push_back(*it);
        }
    }

    if(n->flag == 1) {
        return 0; // We started at m, so if n is reached we have a cycle: abort
    }

    shift = 0;
    vector<node_t *> l;
    end = n->order;
    for (int i = m->order; i <= n->order; i++) {
        node_t * w = ordinv[i];
        if(w == NULL) {
            ordinv[i - shift] = NULL;
            continue;
        }
        if(w->flag) {
            // Vertex needs to be reordered after n: we remove it from the list, and re-add it later at the correct place
            // shift records the room made available this way
            w->flag = 0;
            l.push_back(w);
            shift++;
        } else {
            // Shift this vertex to the front
            w->order = i - shift;
            ordinv[w->order] = w;
        }
    }
    // Lastly, add the removed vertices to their correct location
    for (int i = 0; i < len(l); i++) {
        node_t * w = l[i];
        w->order = end - shift + i + 1;
        ordinv[w->order] = w;
    }
    return 1;
}

int stjn_update_est(node_t *n) {
    set<node_t *, orderLT> h;
    int inconsistent = 0;

    h.insert(n);

    while (!h.empty()) {
        node_t * n = *h.begin(); h.erase(h.begin());
        FOREACH(n->next, it) {
            node_t * m (*it);
            if(m->est < n->est + n->len) {
                if(m->old_est < 0) // old_est might already be set due to reconvergence
                    m->old_est = m->est;
                m->est = n->est + n->len;
                m->node_pred = n;
                if(m->est > m->lst) inconsistent = 1, cycle_node = m;
                h.insert(m);
            }
        }
    }
    chkEST();
    return inconsistent;
}

int stjn_update_lst(node_t *n) {
    set<node_t *, orderGT> h;
    int inconsistent = 0;

    h.insert(n);

    while (!h.empty()) {
        node_t * n = *h.begin(); h.erase(h.begin());
        FOREACH(n->prev, it) {
            node_t * m (*it);
            if(m->lst > n->lst - m->len) {
                m->lst = n->lst - m->len;
                m->node_succ = n;
                if(m->est > m->lst) inconsistent = 1, cycle_node = m;
                h.insert(m);
            }
        }
    }
    assert(!inconsistent); // ...
    chkLST();
    return inconsistent;
}

int stjn_add_precedence(node_t *from, node_t *to) {
    chkEST(), chkLST();
    int inconsistent = 0;
    set_precedence(from, to);
    if(!update_ordering(from, to)) {
        printf("Cycle detected.\n");
        return 0;
    }
    if(to->est < from->est + from->len) {
        to->old_est = to->est;
        to->est = from->est + from->len;
        to->node_pred = from;
        inconsistent |= stjn_update_est(to);
    }
    if(from->lst > to->lst - from->len) {
        from->lst = to->lst - from->len;
        from->node_succ = to;
        inconsistent |= stjn_update_lst(from);
    }

    return !inconsistent;
}

vector<node_t *> merge_actlist(vector<node_t *> a, vector<node_t *> b) {
    vector<node_t *> res;
    FOREACH(a, it) res.append(*it);
    FOREACH(b, it) res.append(*it);
    sort(res.begin(), res.end(), orderLT ());
    res.erase(unique(res.begin(), res.end()), res.end());
    return res;
}

inline void remove_requirement(requirement * req) {
    int i (req->i), j (req->j), k (req->k);
    A(i,j)->requirements[k] = NULL;
    vector<requirement *> & v (R(k)->requirements);
    assert(find(v.begin(), v.end(), req) != v.end());
    v.erase(find(v.begin(), v.end(), req));
    assert(find(v.begin(), v.end(), req) == v.end());
    delete req;
}

void merge_usage(node_t * g, node_t * a) {
    // Update resource usage
    // The worst-case profile is computed. Since all activities in the group
    // are concurrent this boils down to computing the maximum for each resource.
    // FIXME: the method used here is somewhat crude: requirement amounts are
    // set to zero. Instead, the whole requirement could be removed, but that's
    // slightly more involved.
    for (int i = 0; i < tmsp->n_resources; i++) if (R(i)) {
        if(Q(a->i, a->j, i) > Q(g->i, g->j, i)) {
            // a has higher requirements: move req from a to g, clear req from g
            requirement *ra = REQ(a->i, a->j, i);
            vector<requirement *> & l = A(g->i, g->j)->requirements;
            requirement *rg = l[i];

            if(rg)
                remove_requirement(rg);
            l[i] = ra;
            ra->i = g->i;
            ra->j = g->j;
        } else {
            // g has higher reqs than a, clear req from a
            requirement *ra = REQ(a->i, a->j, i);
            if(ra)
                remove_requirement(ra);
        }
    }
}

node_t *stjn_merge_init(node_t *n1, node_t *n2) {
    int len = n1->len + n2->len;
    node_t * g = n1, * a = n2;
    g->len=len;
    FOREACH(a->group, it) g->group.append(*it);
    g->prev = merge_actlist(g->prev, a->prev);
    FOREACH(a->prev, it) {
        node_t * v (*it);
        replace(v->next.begin(), v->next.end(), a, g);
    }
    g->next = merge_actlist(g->next, a->next);
    FOREACH(a->next, it) {
        node_t * v (*it);
        replace(v->prev.begin(), v->prev.end(), a, g);
    }
    a->group.clear();
    merge_usage(g, a);
    return g;
}

// Merge n and m into grouped activity
node_t *stjn_merge(node_t *n1, node_t *n2) {
    chkEST(), chkLST();
    // Merge is coded as a merge between group g and activity a.
    // If either n or m is a group, assign the group to g, assign the other to a
    // If neither is a group, make a new group with n only, and assign m to a
    // Next, merge a into g

    if(!update_ordering(n1, n2) || !update_ordering(n2, n1)) {
        printf("Cycle detected while merging.\n");
        puts("o.O unexpected"); throw 0;
        return NULL;
    }

    // niet direct op g schrijven want g==n1.
    int            len = n1->len + n2->len;
    int        old_est = min(n1->est, n2->est);
    int            est = max(n1->est, n2->est);
    node_t * node_pred = (n1->est > n2->est) ? n1->node_pred : n2->node_pred;
    int            lst = min(n1->lst + n1->len, n2->lst + n2->len) - (n1->len + n2->len);
    node_t * node_succ = (n1->lst < n2->lst) ? n1->node_succ : n2->node_succ;

    node_t * g = n1, * a = n2;
    g->len=len,g->old_est=old_est,g->est=est,g->node_pred=node_pred,g->lst=lst,g->node_succ=node_succ;
    FOREACH(a->group, it) g->group.append(*it);
    g->prev = merge_actlist(g->prev, a->prev);
    FOREACH(a->prev, it) {
        node_t * v (*it);
        replace(v->next.begin(), v->next.end(), a, g);
        if (v->node_succ == a) v->node_succ = g;
    }
    g->next = merge_actlist(g->next, a->next);
    FOREACH(a->next, it) {
        node_t * v (*it);
        replace(v->prev.begin(), v->prev.end(), a, g);
        if (v->node_pred == a) v->node_pred = g;
    }
    ordinv[a->order] = NULL; // weg
    a->group.clear();
    merge_usage(g, a);
    if (g->lst > DD(g->i) - g->len) {
        g->lst = DD(g->i) - g->len;
        if (g->lst < g->est) {
            cycle_node = g;
            g->node_succ = NULL;
            return NULL;
        }
    }
    if(stjn_update_est(g) || stjn_update_lst(g)) return NULL;
    assert(g->est >= max(n1->est, n2->est));
    assert(g->lst + g->len <= min(n1->lst + n1->len, n2->lst + n2->len));
    chkEST(), chkLST();
    return g;
}

void print_est_schedule() {
    int i, j;

    for(i = 0; i < tmsp->n_trains; i++) if (T(i)) {
        for(j = 0; j < N(i); j++) if (A(i,j)) {
            //fprintf(stderr, "G: %x\n", !!acts[i][j]->group.size());
            fprintf(stderr, "EST: %d %d %d\n", i, j, acts[i][j]->est);
            //fprintf(stderr, "LST: %d %d %d\n", i, j, acts[i][j]->lst);
            //A(i,j)->est = acts[i][j]->est;
        }
    }
}

ostream & operator<< (ostream & o, node_t * node)
{
    o << "node(i=" << node->i << ",j=" << node->j << ",len="<<node->len<<",acts=[";
    FOREACH(node->group, it) o << *it << ", ";
    o << "])"; return o;
}

ostream & operator<< (ostream & o, activity * act)
{
    o << "act(i=" << act->i << ",j=" << act->j << ",dur="<<act->duration<<")"; return o;
}
