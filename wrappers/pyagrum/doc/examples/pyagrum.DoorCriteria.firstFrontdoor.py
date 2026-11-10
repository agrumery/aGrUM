import pyagrum as gum

bn = gum.fastBN("U->X->M->Y;U->Y")
dag = bn.dag()
z = gum.DoorCriteria.firstFrontdoor(dag, bn.idFromName("X"), bn.idFromName("Y"), {bn.idFromName("U")})
{bn.variable(i).name() for i in z}  # U is excluded: it is unobserved
# {'M'}
