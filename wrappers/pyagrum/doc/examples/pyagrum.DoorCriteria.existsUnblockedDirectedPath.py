import pyagrum as gum

bn = gum.fastBN("X->M->Y")
dag = bn.dag()
x, m, y = (bn.idFromName(n) for n in "XMY")
gum.DoorCriteria.existsUnblockedDirectedPath(dag, x, y, set())
# True
gum.DoorCriteria.existsUnblockedDirectedPath(dag, x, y, {m})
# False
