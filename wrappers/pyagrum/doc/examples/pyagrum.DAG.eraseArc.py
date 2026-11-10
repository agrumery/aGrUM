import pyagrum as gum

dag = gum.fastDAG("A->B->C")
dag.eraseArc(1, 2)
sorted(dag.arcs())
# [(0, 1)]
