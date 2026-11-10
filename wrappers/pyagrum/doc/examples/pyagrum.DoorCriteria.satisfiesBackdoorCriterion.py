import pyagrum as gum

bn = gum.fastBN("Z->X->M->Y;Z->Y")
dag = bn.dag()
x, y, z, m = (bn.idFromName(n) for n in "XYZM")
gum.DoorCriteria.satisfiesBackdoorCriterion(dag, x, y, {z})
# True
gum.DoorCriteria.satisfiesBackdoorCriterion(dag, x, y, {z, m})  # M is a descendant of X
# False
