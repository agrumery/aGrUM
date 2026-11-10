import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B->C;D->C"))
dag = cpdag.consistentExtension()
cpdag.contains(dag), dag.sizeArcs()
# (True, 3)
