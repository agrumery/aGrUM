import pyagrum as gum

bn = gum.fastBN("A[3]->B[3]<-C[3]")
bn.cpt("B").fillFromFunction("(A+C)/2")
bn.cpt("B")[{"A": 2, "C": 2}].tolist()
# [0.0, 0.0, 1.0]
bn.cpt("B")[{"A": 0, "C": 2}].tolist()
# [0.0, 1.0, 0.0]
