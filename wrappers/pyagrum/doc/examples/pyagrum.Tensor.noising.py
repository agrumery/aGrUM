import pyagrum as gum

gum.initRandom(42)
bn = gum.fastBN("A->B")
bn.cpt("B").fillWith([1, 0, 0, 1])
bn.cpt("B").noising(0.1)  # 90% of the CPT + 10% of a random CPT
bn.cpt("B").min() > 0
# True
[round(x, 4) for x in bn.cpt("B").sumOut(["B"]).tolist()]  # still a CPT
# [1.0, 1.0]
