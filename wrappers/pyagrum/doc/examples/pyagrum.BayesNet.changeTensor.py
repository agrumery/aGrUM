import pyagrum as gum

bn = gum.fastBN("A->B")
pot = gum.Tensor(bn.cpt("B")).fillWith([0.1, 0.9, 0.8, 0.2])
bn.changeTensor("B", pot)

# the BN keeps its own copy: modifying pot does not change the CPT
pot.fillWith(0.5)
bn.cpt("B").tolist()
# [[0.1, 0.9], [0.8, 0.2]]
