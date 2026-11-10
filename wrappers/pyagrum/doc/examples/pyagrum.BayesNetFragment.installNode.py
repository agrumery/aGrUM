import pyagrum as gum

bn = gum.fastBN("A->B->C")
fragment = gum.BayesNetFragment(bn)
fragment.installNode("B")
fragment.installNode("C")
sorted(fragment.names())
# ['B', 'C']
fragment.arcs()  # the arcs between installed nodes
# {(1, 2)}
