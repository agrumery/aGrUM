import pyagrum as gum

dag = gum.fastDAG("A->B;A->C;C->D")
dag.children(0)
# {1, 2}
