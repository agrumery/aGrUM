import pyagrum as gum

bn = gum.fastBN("A[0:10:5]")  # 5 intervals from 0 to 10
bn.evLt("A", 5).tolist()
# [1.0, 1.0, 0.0, 0.0, 0.0]
