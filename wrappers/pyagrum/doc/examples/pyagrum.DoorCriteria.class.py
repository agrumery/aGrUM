import pyagrum as gum

bn = gum.fastBN("X->Z->Y")
dag = bn.dag()
x, y, z = bn.idFromName("X"), bn.idFromName("Y"), bn.idFromName("Z")
gum.DoorCriteria.satisfiesBackdoorCriterion(dag, x, y, set())
# True
gum.DoorCriteria.enumerateFrontdoorSets(dag, x, y) == [{z}]
# True
