import pyagrum as gum

dag = gum.fastDAG("A->C;B->C;C->D")
mg = dag.moralGraph()
mg.existsEdge(dag.idFromName("A"), dag.idFromName("B"))  # A and B, parents of C, are married
# True
mg.sizeEdges()
# 4
