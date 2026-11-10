import pyagrum as gum

bn = gum.fastBN("A->B->C")
jt = gum.JunctionTreeGenerator().junctionTree(bn)
dot = jt.toDotWithNames(bn)  # cliques labelled with the names of the variables
'label="A B"' in dot
# True
