#ifndef __ALLES__
#define __ALLES__

using namespace std;
#include<iostream>
#include<cstdio>
#include<algorithm>
#include<vector>
#include<cassert>
#include<cstring>
#include<cmath>

#define ALL(v) (v).begin(),(v).end()
#define REP(i, n) for (int i (0); i < (n); i ++)
#define FORIT(a,b, it) for(__typeof(b)it(a);it!=(b);++it)
#define FOREACH(v, it) FORIT(v.begin(), v.end(), it)
#define len(v) ((int)(v).size())
#define append push_back

struct activity; struct node_t;
ostream & operator<< (ostream & o, activity * act);
ostream & operator<< (ostream & o, node_t * node);

template<class A,class B>ostream&operator<<(ostream&o,pair<A,B>const&p){return o<<'('<<p.first<<','<<p.second<<')';}
template<class T>void pv(T a,T b){FORIT(a,b,it)cout<<*it<<" ";cout<<endl;}

#include<cstdlib>
#include<cmath>

#include <sys/time.h>
#include <string.h>
#include <stdio.h>

#include <stdio.h>
#include <stdarg.h>

#include <limits.h>

#include<deque>
#include<map>
#include<set>
#include<queue>

template<class T> inline T list_get(vector<T> const & v, int i)
{
    return v[i];
}

template<class T> inline void list_append(vector<T> & v, T x)
{
    return v.push_back(x);
}

template<class T> inline void list_set(vector<T> & v, int i, T x)
{
    v[i] = x;
}

#endif
