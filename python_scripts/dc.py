#!/usr/bin/python

import sys, getopt, os
from heapq import *

def usage():
    print "read the source, Luke!"

def read_instance():
    res = []
    trains = []
    acts = []
    keys = []
    for line in sys.stdin:
        parts = line.strip().split(" ")
        if parts[0] == 'R':
            idx = int(parts[1])
            cap = int(parts[2])
            if(idx != len(res)):
                raise Exception("Resource index mismatch")
            res.append(cap)
        elif parts[0] == 'T' or parts[0] == 'J':
            idx = int(parts[1])
            rel = int(parts[2])
            due = int(parts[3])
            if(idx != len(trains)):
                raise Exception("Train index mismatch")
            trains.append((rel, due))
            acts.append([])
        elif parts[0] == 'A':
            i = int(parts[1])
            j = int(parts[2])
            d = int(parts[3])
            if(j != len(acts[i])):
                raise Exception("Activity index mismatch")

            acts[i].append({
                'idx':(i,j), 
                'len':d, 
                'res':[0 for x in range(0, len(res))], 
                'prev':[],
                'next':[],
                'rel':trains[i][0],
                'due':trains[i][1],
                })
            keys.append((i,j))
        elif parts[0] == 'Q':
            i = int(parts[1])
            j = int(parts[2])
            k = int(parts[3])
            use = int(parts[4])
            if(k >= len(res)):
                raise Exception("Resource index exceeded")
            acts[i][j]['res'][k] = use
        elif parts[0] == 'P':
            i1 = int(parts[1])
            j1 = int(parts[2])
            i2 = int(parts[3])
            j2 = int(parts[4])
            acts[i2][j2]['prev'].append((i1,j1))
            acts[i1][j1]['next'].append((i2,j2))

    return (res,acts)

def assign_topo(acts):
    h = []
    topo = []
    idx = 0
    horizon = 0
    for t in acts:
        for a in t:
            a['flag'] = len(a['prev'])
            if len(a['prev']) == 0:
                h.append(a)

    while len(h) > 0:
        a = h.pop(0)
        
        a['order'] = idx
        topo.append(a)
        idx += 1

        a['est'] = 0

        for nxt_tuple in a['next']:
            nxt = acts[nxt_tuple[0]][nxt_tuple[1]]
            nxt['flag'] -= 1
            if nxt['flag'] == 0: h.append(nxt)

    return topo

def calc_est(acts, a):
    while len(a):
        a.sort(key = lambda x : x['order'])
        act = a.pop(0)
        est = act['rel']
        for prevtup in act['prev']:
            prev = acts[prevtup[0]][prevtup[1]]
            est = max(est, prev['est'] + prev['len'])
        act['est'] = est
        for nxttup in act['next']:
            nxt = acts[nxttup[0]][nxttup[1]]
            if not nxt in a: a.append(nxt)

def calc_lst(acts, a):
    while len(a):
        a.sort(key = lambda x : -x['order'])
        act = a.pop(0)
        lst = act['due'] - act['len']
        for nxttup in act['next']:
            nxt = acts[nxttup[0]][nxttup[1]]
            lst = min(lst, nxt['lst'] - act['len'])
        act['lst'] = lst
        if lst < act['est']:
            print "PANIC: HET IS HELEMAAL KAPOTSTUK"
            print act
            os.exit(1)
        for prevtup in act['prev']:
            prev = acts[prevtup[0]][prevtup[1]]
            if not prev in a: a.append(prev)

def printstate(acts):
    sys.stderr.write("STATE:")
    for t in acts:
        for a in t:
            sys.stderr.write(" %d %d %d 1 %d %d" % (a['idx'][0], a['est'], a['lst'], a['idx'][0], a['idx'][1]))
    sys.stderr.write(" -1\n")

def solve():
    (res, acts) = read_instance()

    topo = assign_topo(acts)
    sys.stderr.write("PROGRESS: 0\n")
    startlist = []
    endlist = []
    for a in topo:
        if len(a['prev']) == 0:
            startlist.append(a)
        if len(a['next']) == 0:
            endlist.append(a)
    calc_est(acts, startlist)
    calc_lst(acts, endlist)
    for t in acts:
        for a in t:
            sys.stderr.write("EST: %d %d %d\n" % (a['idx'][0], a['idx'][1], a['est']));
    printstate(acts)
    chains = []
    for r in range(0, len(res)):
        topo = assign_topo(acts)
        startlist = []
        endlist = []
        for a in topo:
            if len(a['prev']) == 0:
                startlist.append(a)
            if len(a['next']) == 0:
                endlist.append(a)
        calc_est(acts, startlist)
        calc_lst(acts, endlist)
        cap = res[r]
        curchain = [[] for x in range(0, cap)]
        chainlens = []
        for i in range(0, cap):
            chainlens.append((i, 0))
        for act in topo:
            act['flag'] = len(act['prev'])
        while True:
            candidates = []
            for act in topo:
                #if act['res'][r] > 0 and act['flag'] == 0:
                if act['flag'] == 0:
                    candidates.append(act)
            if len(candidates) == 0: break
            act = candidates[0]
            act['flag'] = -1
            chainlens.sort(key = lambda x:x[1])
            newlen = 0
            for i in range(0, act['res'][r]):
                c = chainlens[i]
                if len(curchain[c[0]]) > 0:
                    prvtup = curchain[c[0]][len(curchain[c[0]])-1]
                    if not prvtup in act['prev']:
                        act['prev'].append(prvtup)
                        prv = acts[prvtup[0]][prvtup[1]]
                        prv['next'].append(act['idx'])
                        #calc_est(acts, [act])
                        #calc_lst(acts, [prv])
                        startlist = []
                        endlist = []
                        for a in topo:
                            if len(a['prev']) == 0:
                                startlist.append(a)
                            if len(a['next']) == 0:
                                endlist.append(a)
                        calc_est(acts, startlist)
                        calc_lst(acts, endlist)
                        sys.stderr.write("PC: %d %d %d %d\n" % (prvtup[0], prvtup[1], act['idx'][0], act['idx'][1]))
                        printstate(acts)
                curchain[c[0]].append(act['idx'])
                newlen = max(newlen, c[1] + act['len'])
            for i in range(0, act['res'][r]):
                chainlens[i] = (chainlens[i][0], newlen)
            for tup in act["next"]:
                nxt = acts[tup[0]][tup[1]]
                nxt['flag'] -= 1
    for t in acts:
        for a in t:
            sys.stderr.write("EST: %d %d %d\n" % (a['idx'][0], a['idx'][1], a['est']))
    sys.stderr.write("PROGRESS: 100\n")
    sys.stderr.write("Instance solved.\n")



def main(argv=None):
    if argv is None:
        argv = sys.argv
    
    try:
        opts, args = getopt.getopt(argv[1:], "")
    except getopt.GetoptError, err:
        print str(err)
        usage()
        return 1

    solve()
    return 0

if __name__ == "__main__":
    sys.exit(main())
