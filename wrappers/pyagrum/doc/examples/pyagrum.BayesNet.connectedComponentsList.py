import pyagrum as gum

bn = gum.fastBN("A->B;C->D;E")
ccl = bn.connectedComponentsList()  # root id -> set of the nodes of the component
sorted(sorted(component) for component in ccl.values())
# [[0, 1], [2, 3], [4]]
