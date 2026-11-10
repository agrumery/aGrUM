import pyagrum as gum

bn = gum.fastBN("A->B->C<-D;C->E")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("C")  # A, B, C, D
dot = fragment.toDot()  # the graph in the dot language
dot.splitlines()[0]
# 'digraph "Fragment of anonymousBN" {'
'"C" -> "E"' in dot  # the nodes and arcs not installed are drawn in light grey
# True
