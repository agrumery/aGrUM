import pyagrum as gum

dag = gum.fastDAG("A->B->C;A->C")
sorted(dag.arcs())
# [(0, 1), (0, 2), (1, 2)]
