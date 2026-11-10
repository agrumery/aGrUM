import pyagrum as gum

dag = gum.DAG()
dag.addNodes(3)
# {0, 1, 2}
dag.addArc(0, 2)
dag.size(), dag.sizeArcs()
# (3, 1)
