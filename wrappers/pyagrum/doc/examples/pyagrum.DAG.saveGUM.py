import pyagrum as gum

dag = gum.fastDAG("A->B->C")
dag.saveGUM("dag.jgum")
dag2 = gum.loadGraph("dag.jgum")
type(dag2).__name__, sorted(dag2.arcs()), dag2.nameFromId(2)
# ('DAG', [(0, 1), (1, 2)], 'C')
