import pyagrum as gum

bn = gum.fastBN("A->B")
i = gum.Instantiation(bn.cpt("B"))  # a slave of the CPT
i.isMutable()  # True for a slave
# True
i.setMutable()  # detached from the CPT
i.isMutable()
# False
