import pyagrum as gum

bn = gum.fastBN("A[0:10:5]->B")  # 5 intervals from 0 to 10
fragment = gum.BayesNetFragment(bn)
fragment.installNode("A")
fragment.evEq("A", 3).tolist()
# [0.0, 1.0, 0.0, 0.0, 0.0]
