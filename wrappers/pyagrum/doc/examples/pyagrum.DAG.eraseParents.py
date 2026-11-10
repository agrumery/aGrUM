import pyagrum as gum

dag = gum.fastDAG("A->C;B->C;C->D")
dag.eraseParents(dag.idFromName("C"))
sorted((dag.nameFromId(t), dag.nameFromId(h)) for t, h in dag.arcs())
# [('C', 'D')]
