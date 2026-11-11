import pyagrum as gum

bn = gum.fastBN("X->Y;X->Z;Y->Z")
cm = gum.CausalModel(bn)
cm.hasObservationalBN()
# True

# a latent confounder U between X and Y (the arc X->Y is removed)
cm = gum.CausalModel(bn, [("U", ["X", "Y"])])
cm.latentVariablesNames()
# {'U'}
cm.existsArc("X", "Y")
# False

# a causal model from a DAG only (no CPTs needed for structural queries)
dag = gum.fastDAG("X->Y->Z")
cm = gum.CausalModel(dag)
cm.hasObservationalBN()
# False
cm.backDoor("X", "Z")
# set()
