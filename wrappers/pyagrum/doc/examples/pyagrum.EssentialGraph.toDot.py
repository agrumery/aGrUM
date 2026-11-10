import pyagrum as gum

eg = gum.EssentialGraph(gum.fastBN("A->B<-C;B->D"))
dot = eg.toDot()
dot.startswith("digraph")
# True
'label="A"' in dot  # the nodes are labelled with the names of the variables
# True
