import pyagrum as gum

bn = gum.fastBN("A->B;C->D")
fragment = gum.BayesNetFragment(bn)
for name in "ABC":
  fragment.installNode(name)
fragment.connectedComponentsCount()
# 2
