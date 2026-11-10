import pyagrum as gum

bn = gum.fastBN("A->B->C")
cm = gum.CausalModel(bn, [("U", ["A", "C"])])
# deprecated: use cm.causalDAG() instead
dag = cm.causalBN()
dag.size()
# 4
