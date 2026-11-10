import pyagrum as gum

bn = gum.fastBN("Z->X->Y;Z->Y;W->Y")
dag = bn.dag()
sorted(bn.variable(i).name() for i in gum.DoorCriteria.backdoorReach(dag, bn.idFromName("X")))
# ['Y', 'Z']
