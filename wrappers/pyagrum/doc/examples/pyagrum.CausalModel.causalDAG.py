import pyagrum as gum

bn = gum.fastBN("A->B->C")
cm = gum.CausalModel(bn, [("U", ["A", "C"])])
dag = cm.causalDAG()
dag.size()  # observed and latent nodes
# 4
sorted(dag.arcs())
# [(0, 1), (1, 2), (3, 0), (3, 2)]
