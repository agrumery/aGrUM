import pyagrum as gum

bn = gum.fastBN("A->B;C->D")
fragment = gum.BayesNetFragment(bn)
for name in "ABC":
  fragment.installNode(name)
ccl = fragment.connectedComponentsList()  # root id -> set of the nodes of the component
sorted(sorted(component) for component in ccl.values())
# [[0, 1], [2]]
