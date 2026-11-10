import pyagrum as gum

bn = gum.fastBN("A[0:10:5]")  # 5 intervals from 0 to 10
bn.evIn("A", 3, 7).tolist()
# [0.0, 1.0, 1.0, 1.0, 0.0]
