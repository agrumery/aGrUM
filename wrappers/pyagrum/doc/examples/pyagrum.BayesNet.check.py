import pyagrum as gum

bn = gum.fastBN("A->B")
bn.check()
# ()
bn.cpt("A").fillWith([0.3, 0.3])
bn.check()
# ('Variable A : the CPT sum to less than 1.',)
