import pyagrum as gum

dag = gum.fastDAG("A->B")
dag.existsArc(0, 1)
# True
dag.existsArc(1, 0)
# False
