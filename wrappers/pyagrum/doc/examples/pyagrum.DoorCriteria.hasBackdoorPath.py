import pyagrum as gum

bn = gum.fastBN("Z->X->Y;Z->Y")
dag = bn.dag()
x, y, z = (bn.idFromName(n) for n in "XYZ")
gum.DoorCriteria.hasBackdoorPath(dag, x, y, set())
# True
gum.DoorCriteria.hasBackdoorPath(dag, x, y, {z})  # conditioning on Z blocks X<-Z->Y
# False
