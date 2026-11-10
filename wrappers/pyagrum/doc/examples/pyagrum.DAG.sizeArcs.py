import pyagrum as gum

dag = gum.fastDAG("A->B->C;A->C")
dag.sizeArcs()
# 3
