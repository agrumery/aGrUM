import pyagrum as gum

mr = gum.MeekRules()
dag = mr.propagateToDAG(gum.fastMixedGraph("A->B-C-D"))
type(dag).__name__, sorted(dag.arcs())
# ('DAG', [(0, 1), (1, 2), (2, 3)])
