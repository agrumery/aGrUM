import pyagrum as gum

dag = gum.fastDAG("A->B->C;D->B;C->E")
# the sources needed to make C independent of the other sources
dag.minimalCondSet(2, (0, 1, 3, 4))
# {1, 4}
dag.minimalCondSet(2, (0, 3))
# {0, 3}
