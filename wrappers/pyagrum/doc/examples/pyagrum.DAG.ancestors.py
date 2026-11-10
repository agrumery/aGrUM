import pyagrum as gum

dag = gum.fastDAG("A->B->C;D->C")
dag.ancestors(2)  # the ancestors of C
# {0, 1, 3}
dag.ancestors(0)
# set()
