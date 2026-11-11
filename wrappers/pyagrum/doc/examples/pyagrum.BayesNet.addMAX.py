import pyagrum as gum

bn = gum.fastBN("A[0,3];B[0,3]")
bn.addMAX(gum.RangeVariable("M", "max of the parents", 0, 3))
# 2
bn.addArcs([("A", "M"), ("B", "M")])
bn.cpt("M")[{"A": 1, "B": 2}].tolist()
# [0.0, 0.0, 1.0, 0.0]

# labelized variables: the order of the labels is used (low < medium < high)
bn = gum.fastBN("X{low|medium|high};Y{low|medium|high}")
bn.addMAX(gum.LabelizedVariable("L", "", ["low", "medium", "high"]))
# 2
bn.addArcs([("X", "L"), ("Y", "L")])
bn.cpt("L")[{"X": "medium", "Y": "low"}].tolist()
# [0.0, 1.0, 0.0]
