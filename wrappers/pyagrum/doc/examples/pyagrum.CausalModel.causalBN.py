import pyagrum as gum

bn = gum.fastBN("A->B->C")
cm = gum.CausalModel(bn, [("U", ["A", "C"])])
# deprecated: the causal DAG (with the latent variable U) is given by cm.causalDAG()
dag = cm.causalBN()
dag.size()
# 4
cm.causalDAG().size()
# 4
