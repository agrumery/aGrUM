import pyagrum as gum

bn = gum.fastBN("A[3]->B[2]")
i = gum.Instantiation(bn.cpt("B"))  # over the variables of the CPT of B
print(i)
# <B:0|A:0>
i["A"] = 2  # an Instantiation is subscriptable by the names of its variables
i["B"] = "1"  # a label can be given instead of an index
i["A"], i["B"]
# (2, 1)
i.todict()
# {'B': 1, 'A': 2}
bn.cpt("B").get(i) == bn.cpt("B")[{"A": 2, "B": 1}]
# True
