import pyagrum as gum

bn = gum.fastBN("Z->X->Y;Z->Y")
dag = bn.dag()
z = gum.DoorCriteria.firstBackdoor(dag, bn.idFromName("X"), bn.idFromName("Y"))
{bn.variable(i).name() for i in z}
# {'Z'}
