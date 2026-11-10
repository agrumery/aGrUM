import pyagrum as gum

dag = gum.DAG()
dag.addNodes(2)
# {0, 1}
dag.emptyArcs()
# True
dag.addArc(0, 1)
dag.emptyArcs()
# False
