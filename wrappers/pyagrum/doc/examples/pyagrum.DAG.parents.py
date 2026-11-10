import pyagrum as gum

dag = gum.fastDAG("A->C;B->C;C->D")
c = dag.idFromName("C")
sorted(dag.nameFromId(i) for i in dag.parents(c))
# ['A', 'B']
