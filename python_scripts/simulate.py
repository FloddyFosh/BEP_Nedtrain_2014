#!/usr/bin/python

import sys, getopt, random, copy, math, os
from heapq import *
from gurobipy import *
from multiprocessing import Pool

def usage():
    print """Usage: simulate.py [OPTIONS] instance(s)

Options:
  -m        Only calculate flexibility potential
  -i N      Number of iterations
  -o TYPE   Type of open schedule optimization. Possible values are:
            maximal           maximal open schedule (default)
            quadequal         optimize squared flex loss
            quadequal_fixflex squared flex loss with maximal flex
            wpre              with w based on direct predecessors,
            wallpre           on all predecessors,
            wdiscount=x       discounted using x,
            wnext             with w based on direct successors,
            wallnext          on all successors
  -g        Maximize minimal interval size, to guarantee flexibility
  -d N      Delay N% randomly chosen tasks (default: 5)
  -t P      Delay percentage (default: 25)
  -q        Omit header
  -r        Randomize start times in simulation"""

def read_instance(file):
    trains = []
    acts = []
    f = open(file, 'r')
    for line in f:
        parts = line.strip().split(" ")
        if parts[0] == 'R':
            continue
        elif parts[0] == 'T':
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
                'prev':[],
                'next':[],
                })
        elif parts[0] == 'Q':
            continue
        elif parts[0] == 'P':
            i1 = int(parts[1])
            j1 = int(parts[2])
            i2 = int(parts[3])
            j2 = int(parts[4])
            acts[i1][j1]['next'].append((i2,j2))
            acts[i2][j2]['prev'].append((i1,j1))
    f.close()

    return acts

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
        a['allprev'] = set(a['prev'])
        for prv_tuple in a['prev']:
            prv = acts[prv_tuple[0]][prv_tuple[1]]
            a['est'] = max(a['est'], prv['est'] + prv['len'])
            a['allprev'] |= prv['allprev']
        horizon = max(horizon, a['est']+a['len'])

        for nxt_tuple in a['next']:
            nxt = acts[nxt_tuple[0]][nxt_tuple[1]]
            nxt['flag'] -= 1
            if nxt['flag'] == 0: h.append(nxt)

    due = horizon * 1.10
    for idx in reversed(range(0, len(topo))):
        a = topo[idx]
        a['lst'] = due - a['len']
        a['allnext'] = set(a['next'])
        for nxt_tuple in a['next']:
            nxt = acts[nxt_tuple[0]][nxt_tuple[1]]
            a['lst'] = min(a['lst'], nxt['lst'] - a['len'])
            a['allnext'] |= nxt['allnext']
    return topo

def generate_delays(rnd, acts, pdelay, tardiness):
    keys = []
    delays = {}

    for t in acts:
        for a in t:
            keys.append(a['idx'])

    rnd.shuffle(keys)
    ndelay = int((pdelay/100.0) * len(keys))
    tardiness = tardiness / 100.0 + 1
    for key in keys[0:ndelay]:
        delays[key] = tardiness
    return delays

def return_gurobi_open(acts, topo, m):
    m.optimize()
    total = 0
    for a in topo:
        a['ests'] = a['gurobi_ests'].x
        a['lsts'] = a['gurobi_lsts'].x
        if a['ests'] > a['lsts']: # sometimes happens due to rounding errors!
            a['ests'] = a['lsts'] = min(a['ests'], a['lsts'])
        total += a['lsts'] - a['ests']
    return total

def make_open_schedule_naive(acts, topo):
    slack = 1
    for a in topo:
        a['ests'] = 0
        for prv_tup in a['prev']:
            prv = acts[prv_tup[0]][prv_tup[1]]
            if prv['lsts'] + prv['len'] > a['ests']:
                a['ests'] = prv['lsts'] + prv['len']
        a['lsts'] = a['ests'] + slack

def min_max_flex_open_schedule(acts, topo):
    m = Model("openschedule")
    m.setParam("outputFlag", 0)
    minflex = m.addVar(vtype=GRB.CONTINUOUS, name="minflex")
    #obj = LinExpr()
    for a in topo:
        a['gurobi_ests'] = m.addVar(vtype=GRB.CONTINUOUS, name="a%dests" % a['order'])
        a['gurobi_lsts'] = m.addVar(vtype=GRB.CONTINUOUS, name="a%dlsts" % a['order'])
        m.update()
        for prv_tup in a['prev']:
            prv = acts[prv_tup[0]][prv_tup[1]]
            m.addConstr(prv['gurobi_lsts'] + prv['len'] <= a['gurobi_ests'])
        m.addConstr(a['gurobi_ests'] <= a['gurobi_lsts'])
        m.addConstr(a['gurobi_lsts'] <= a['lst'])
        m.addConstr(a['gurobi_ests'] >= a['est'])
        m.addConstr(a['gurobi_lsts'] - a['gurobi_ests'] >= minflex)
        #obj += (a['gurobi_lsts'] - a['gurobi_ests'])
    m.setObjective(minflex, GRB.MAXIMIZE)
    m.optimize()
    return minflex.x

def make_open_schedule_maximal(acts, topo):
    m = Model("openschedule")
    m.setParam("outputFlag", 0)
    obj = LinExpr()
    for a in topo:
        a['gurobi_ests'] = m.addVar(vtype=GRB.CONTINUOUS, name="a%dests" % a['order'])
        a['gurobi_lsts'] = m.addVar(vtype=GRB.CONTINUOUS, name="a%dlsts" % a['order'])
        m.update()
        for prv_tup in a['prev']:
            prv = acts[prv_tup[0]][prv_tup[1]]
            m.addConstr(prv['gurobi_lsts'] + prv['len'] <= a['gurobi_ests'])
        m.addConstr(a['gurobi_ests'] <= a['gurobi_lsts'])
        m.addConstr(a['gurobi_lsts'] <= a['lst'])
        m.addConstr(a['gurobi_ests'] >= a['est'])
        obj += (a['gurobi_lsts'] - a['gurobi_ests'])
    m.setObjective(obj, GRB.MAXIMIZE)
    return return_gurobi_open(acts, topo, m)

def make_open_schedule_quadequal_fixflex(acts, topo, flex):
    m = Model("openschedule")
    m.setParam("outputFlag", 0)
    obj = QuadExpr()
    fix = LinExpr()
    for a in topo:
        a['gurobi_ests'] = m.addVar(vtype=GRB.CONTINUOUS, name="a%dests" % a['order'])
        a['gurobi_lsts'] = m.addVar(vtype=GRB.CONTINUOUS, name="a%dlsts" % a['order'])
        m.update()
        for prv_tup in a['prev']:
            prv = acts[prv_tup[0]][prv_tup[1]]
            m.addConstr(prv['gurobi_lsts'] + prv['len'] <= a['gurobi_ests'])
        m.addConstr(a['gurobi_ests'] <= a['gurobi_lsts'])
        m.addConstr(a['gurobi_lsts'] <= a['lst'])
        m.addConstr(a['gurobi_ests'] >= a['est'])
        expr = ((a['gurobi_ests'] - a['est']) + (a['lst'] - a['gurobi_lsts']))
        obj += (expr*expr)
        fix += (a['gurobi_lsts'] - a['gurobi_ests'])
    m.addConstr(fix == flex)
    m.setObjective(obj, GRB.MINIMIZE)
    return return_gurobi_open(acts, topo, m)

def make_open_schedule_quadequal_w(acts, topo, w):
    global guarantee
    if guarantee:
        minflex = min_max_flex_open_schedule(acts, topo)
        
    m = Model("openschedule")
    m.setParam("outputFlag", 0)
    obj = QuadExpr()
    for a in topo:
        a['gurobi_ests'] = m.addVar(vtype=GRB.CONTINUOUS, name="a%dests" % a['order'])
        a['gurobi_lsts'] = m.addVar(vtype=GRB.CONTINUOUS, name="a%dlsts" % a['order'])
        m.update()
        for prv_tup in a['prev']:
            prv = acts[prv_tup[0]][prv_tup[1]]
            m.addConstr(prv['gurobi_lsts'] + prv['len'] <= a['gurobi_ests'])
        m.addConstr(a['gurobi_ests'] <= a['gurobi_lsts'])
        m.addConstr(a['gurobi_lsts'] <= a['lst'])
        m.addConstr(a['gurobi_ests'] >= a['est'])
        if guarantee:
            m.addConstr(a['gurobi_lsts'] - a['gurobi_ests'] >= minflex)
        expr = ((a['gurobi_ests'] - a['est']) + (a['lst'] - a['gurobi_lsts']))
        obj += (expr*expr) * w(a)
    m.setObjective(obj, GRB.MINIMIZE)
    return return_gurobi_open(acts, topo, m)

def make_open_schedule_wpre(acts, topo):
    return make_open_schedule_quadequal_w(acts, topo, lambda x: len(x['prev']))

def make_open_schedule_wallpre(acts, topo):
    return make_open_schedule_quadequal_w(acts, topo, lambda x: len(x['allprev']))

def make_open_schedule_quadequal(acts, topo):
    return make_open_schedule_quadequal_w(acts, topo, lambda x: 1)

def make_open_schedule_wdiscount(acts, topo, factor):
    for n in topo:
        n['flag'] = -1
    def w(n):
        q = []
        visited = []
        weight = 0.0
        for tup in n['prev']:
            q.append((tup, 1))

        while len(q) > 0:
            (node_tup, depth) = q.pop(0)
            node = acts[node_tup[0]][node_tup[1]]
            this_w = max(1 - (depth - 1)/float(factor), 0)
            if this_w > 0 and node['flag'] != n['order']:
                visited.append(node_tup)
                weight += this_w
                for tup in node['prev']:
                    q.append((tup, depth+1))
            node['flag'] = n['order'] # visited flag
        return weight

    return make_open_schedule_quadequal_w(acts, topo, w)

def make_open_schedule_wsucc(acts, topo):
    return make_open_schedule_quadequal_w(acts, topo, lambda x: len(x['next']))

def make_open_schedule_wallsucc(acts, topo):
    return make_open_schedule_quadequal_w(acts, topo, lambda x: len(x['allnext']))

def simulate(rnd, rnd2, acts, topo, delays):
    global randomize_start
    n_violations = 0
    n_strict_violations = 0
    len_violations = 0
    len_strict_violations = 0

    makespan = 0
    for a in topo:
        start = a['ests']
        for prv_tup in a['prev']:
            prv = acts[prv_tup[0]][prv_tup[1]]
            start = max(start, prv['finished'])
        if start > a['ests']:
            n_strict_violations += 1
            len_strict_violations += start - a['ests']
        if start > a['lsts']:
            n_violations += 1
            len_violations += start - a['lsts']
        if a['ests'] > a['lsts']: 
            print "OOPS: ests larger than lsts, impossible"
            print a
            exit(1)

        if randomize_start and start < a['lsts']:
            start = rnd2.uniform(start, a['lsts'])
        else:
            unused = rnd2.uniform(0,1) # pull rnd number anyway, to keep experiments identical
        if a['idx'] in delays:
            a['finished'] = start + delays[a['idx']] * a['len']
        else:
            a['finished'] = start + a['len']
        makespan = max(makespan, a['finished'])
    return (n_violations, n_strict_violations, len_violations, len_strict_violations, makespan)

def process_instance(instance):
    global pdelay, tardiness, iterations, opentype, only_measure, discount
    rnd = random.Random()
    rnd2 = random.Random() # for randomized start times
    rnd.seed(os.path.basename(instance))
    rnd2.seed(os.path.basename(instance))
    acts = read_instance(instance)
    topo =  assign_topo(acts)
    maxflex = make_open_schedule_maximal(acts, topo)
    acts = read_instance(instance)
    topo = assign_topo(acts)
    if opentype == "maximal": flex = make_open_schedule_maximal(acts, topo)
    elif opentype == "quadequal": flex = make_open_schedule_quadequal(acts, topo)
    elif opentype == "quadequal_fixflex": 
        flex = make_open_schedule_maximal(acts, topo)
        acts = read_instance(instance)
        topo = assign_topo(acts)
        flex = make_open_schedule_quadequal_fixflex(acts,topo,flex)
    elif opentype == "wpre": flex = make_open_schedule_wpre(acts, topo)
    elif opentype == "wallpre": flex = make_open_schedule_wallpre(acts, topo)
    elif opentype == "wdiscount": flex = make_open_schedule_wdiscount(acts, topo, discount)
    elif opentype == "wsucc": flex = make_open_schedule_wsucc(acts, topo)
    elif opentype == "wallsucc": flex = make_open_schedule_wallsucc(acts, topo)
    else:
        print "Unknown open schedule type %s" % opentype
        return 0

    if only_measure:
        return { 
                "instance" : instance,
                "flex" : flex,
                "maxflex" : maxflex,
                "flexloss" : maxflex-flex}

    (_, _, _, _, baseline_makespan) = simulate(rnd, rnd2, acts, topo, [])

    mean_violations = m2_violations = max_violations = 0
    mean_violations_strict = m2_violations_strict = max_violations_strict = 0
    mean_viol_length = m2_viol_length = max_viol_length = 0
    mean_viol_length_strict = m2_viol_length_strict = max_viol_length_strict = 0
    mean_tardiness = m2_tardiness = max_tardiness = 0
    min_violations = min_violations_strict = min_viol_length = min_viol_length_strict = min_tardiness = sys.maxint

    def update(mean, m2, low, high, n, val):
        if val > high: high = val
        if val < low: low = val
        # incremental calculation of mean/variance due to Knuth
        delta = val - mean
        mean = mean + delta / float(n)
        m2 = m2 + delta * (val - mean)
        return (mean, m2, low, high)

    for i in range(0, iterations):
        delays = generate_delays(rnd, acts, pdelay, tardiness)

        (n_violations, n_violations_strict, viol_length, viol_length_strict, makespan) = simulate(rnd, rnd2, acts, topo, delays)

        (mean_violations, m2_violations, min_violations, max_violations) = update(mean_violations, m2_violations, min_violations, max_violations, i+1, n_violations)
        (mean_violations_strict, m2_violations_strict, min_violations_strict, max_violations_strict) = update(mean_violations_strict, m2_violations_strict, min_violations_strict, max_violations_strict, i+1, n_violations_strict)
        (mean_viol_length, m2_viol_length, min_viol_length, max_viol_length) = update(mean_viol_length, m2_viol_length, min_viol_length, max_viol_length, i+1, viol_length)
        (mean_viol_length_strict, m2_viol_length_strict, min_viol_length_strict, max_viol_length_strict) = update(mean_viol_length_strict, m2_viol_length_strict, min_viol_length_strict, max_viol_length_strict, i+1, viol_length_strict)
        (mean_tardiness, m2_tardiness, min_tardiness, max_tardiness) = update(mean_tardiness, m2_tardiness, min_tardiness, max_tardiness, i+1, (makespan - baseline_makespan))

    data = {
            "instance" : instance,
            "violations" : (mean_violations, math.sqrt(m2_violations)/iterations, min_violations, max_violations),
            "violations_strict" : (mean_violations_strict, math.sqrt(m2_violations_strict)/iterations, min_violations_strict, max_violations_strict),
            "viol_length" : (mean_viol_length, math.sqrt(m2_viol_length)/iterations, min_viol_length, max_viol_length),
            "viol_length_strict" : (mean_viol_length_strict, math.sqrt(m2_viol_length_strict)/iterations, min_viol_length_strict, max_viol_length_strict),
            "tardiness" : (mean_tardiness, math.sqrt(m2_tardiness)/iterations, min_tardiness, max_tardiness),
            "makespan" : baseline_makespan,
            "flex" : flex,
            "maxflex" : maxflex,
            "flexloss" : maxflex-flex}
    return data

def main(argv=None):
    global pdelay, tardiness, iterations, opentype, only_measure, discount, randomize_start, guarantee
    if argv is None:
        argv = sys.argv
    
    try:
        opts, args = getopt.getopt(argv[1:], "d:t:i:o:mqrg")
    except getopt.GetoptError, err:
        print str(err)
        usage()
        return 1

    if len(args) == 0:
        usage()
        return 1

    pdelay = 5
    tardiness = 25
    iterations = 1
    discount = 5
    opentype = "maximal"
    only_measure = False
    header = True
    randomize_start = False
    guarantee = False

    for o, a in opts:
        if o == "-d":
            pdelay = int(a)
        elif o == "-t":
            tardiness = int(a)
        elif o == "-i":
            iterations = int(a)
        elif o == "-o":
            if "=" in a:
                a = a.split("=")
                if a[0] == "wdiscount":
                    opentype = a[0]
                    discount = int(a[1])
                else:
                    print "`%s' does not have any options" % a[0]
                    usage()
                    return 1
            else:
                opentype = a
        elif o == "-m":
            only_measure = True
        elif o == "-q":
            header = False
        elif o == "-r":
            randomize_start = True
        elif o == "-g":
            guarantee = True
        else:
            assert False, "unhandled option"

    sum_n_violations = sum_n_strict_violations = 0
    sum_len_violations = sum_len_strict_violations = 0
    total_extra_makespan = 0
    #for instance in args:
    #    (total_delayed, extra_makespan, flex) = process_instance(instance)
    #    total_total_delayed += total_delayed
    #    total_extra_makespan += extra_makespan
    #print "violations:strict violations:violation length:strict violation length:extra makespan"

    pool = Pool()
    #pool = Pool(processes=16)
    result = pool.map(process_instance, args)
    #result = map(process_instance, args)

    if only_measure:
        if header:
            sys.stdout.write("instance,flex,maxflex,flexloss\n")
        for data in result:
            sys.stdout.write("%s,%d,%d,%d\n" % (data["instance"], data["flex"], data["maxflex"], data["flexloss"]))

    else:
        if header:
            sys.stdout.write("instance,% of tasks delayed,% delay,")
            for name in "violations", "violations_strict", "viol_length", "viol_length_strict", "tardiness":
                sys.stdout.write("%(n)s mean,%(n)s stddev,%(n)s min,%(n)s max," % {'n':name})
            sys.stdout.write("makespan,flex,maxflex,flexloss\n")

        for data in result:
            sys.stdout.write("%s,%d,%s," % (data["instance"], pdelay, tardiness))
            for name in "violations", "violations_strict", "viol_length", "viol_length_strict", "tardiness":
                sys.stdout.write("%0.4f,%0.4f,%d,%d," % data[name])
            sys.stdout.write("%d,%d,%d,%d\n" % (data["makespan"], data["flex"], data["maxflex"], data["flexloss"]))

    return 0

if __name__ == "__main__":
    sys.exit(main())
