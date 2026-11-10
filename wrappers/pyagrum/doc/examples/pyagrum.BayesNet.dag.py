import pyagrum as gum

bn = gum.fastBN("A->B->C;A->C")
dag = bn.dag()
sorted(dag.arcs())
# [(0, 1), (0, 2), (1, 2)]
dag.size()
# 3
