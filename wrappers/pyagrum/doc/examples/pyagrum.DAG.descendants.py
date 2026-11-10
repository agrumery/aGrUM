import pyagrum as gum

dag = gum.fastDAG("A->B->C;A->D")
dag.descendants(0)  # the descendants of A
# {1, 2, 3}
dag.descendants(2)
# set()
