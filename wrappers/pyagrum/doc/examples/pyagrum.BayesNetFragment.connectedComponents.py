import pyagrum as gum

bn = gum.fastBN("A->B;C->D")
fragment = gum.BayesNetFragment(bn)
for name in "ABC":
  fragment.installNode(name)
cc = fragment.connectedComponents()  # node id -> id of the root of its component
cc[0] == cc[1], cc[0] == cc[2]
# (True, False)
