import pyagrum as gum

bn = gum.fastBN("A->B->C;D")
fragment = gum.BayesNetFragment(bn)
fragment.installNode("A")
fragment.installNode("B")
sorted(v.name() for v in fragment.variables())
# ['A', 'B']
sorted(v.name() for v in fragment.variables(["B"]))
# ['B']
