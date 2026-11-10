import pyagrum as gum

bn = gum.fastBN("A[3]->B{yes|no}")
i = gum.Instantiation(bn.cpt("B"))
i.fromdict({"A": 2, "B": "no"})
print(i)
# <B:no|A:2>
