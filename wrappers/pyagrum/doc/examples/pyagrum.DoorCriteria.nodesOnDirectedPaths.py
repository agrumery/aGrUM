import pyagrum as gum

bn = gum.fastBN("X->A->Y;X->B->Y;C->Y")
dag = bn.dag()
nodes = gum.DoorCriteria.nodesOnDirectedPaths(dag, bn.idFromName("X"), bn.idFromName("Y"))
sorted(bn.variable(i).name() for i in nodes)
# ['A', 'B']
