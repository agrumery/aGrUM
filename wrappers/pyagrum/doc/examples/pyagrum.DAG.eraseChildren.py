import pyagrum as gum

dag = gum.fastDAG("A->B;A->C;D->A")
dag.eraseChildren(0)
sorted(dag.arcs())
# [(3, 0)]
