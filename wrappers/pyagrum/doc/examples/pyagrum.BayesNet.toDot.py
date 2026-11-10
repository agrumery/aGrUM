import pyagrum as gum

bn = gum.fastBN("A->B")
dot = bn.toDot()  # the graph in the dot language
dot.splitlines()[0]
# 'digraph "anonymousBN" {'
'"A" -> "B"' in dot
# True
