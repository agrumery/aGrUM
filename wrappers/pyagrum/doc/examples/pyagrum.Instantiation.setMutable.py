import pyagrum as gum

bn = gum.fastBN("A->B")
i = gum.Instantiation(bn.cpt("B"))  # a slave of the CPT
i.setMutable()  # now independent of the CPT
i.isMutable()
# False
