import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.VariableElimination(bn)
jt = ie.junctionTree(2)  # the junction tree used to compute the posterior of node 2 (C)
sorted(sorted(jt.clique(c)) for c in jt.nodes())
# [[0, 1], [1, 2]]
