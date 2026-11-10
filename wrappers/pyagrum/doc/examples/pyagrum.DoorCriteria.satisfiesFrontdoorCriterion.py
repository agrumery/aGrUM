import pyagrum as gum

bn = gum.fastBN("U->X->M->Y;U->Y")
dag = bn.dag()
x, y, m, u = (bn.idFromName(n) for n in "XYMU")
gum.DoorCriteria.satisfiesFrontdoorCriterion(dag, x, y, {m})
# True
gum.DoorCriteria.satisfiesFrontdoorCriterion(dag, x, y, {u})
# False
