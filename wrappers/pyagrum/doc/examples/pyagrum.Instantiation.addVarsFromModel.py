import pyagrum as gum

bn = gum.fastBN("A->B->C")
i = gum.Instantiation().addVarsFromModel(bn, ["A", "C"])
print(i)
# <A:0|C:0>
