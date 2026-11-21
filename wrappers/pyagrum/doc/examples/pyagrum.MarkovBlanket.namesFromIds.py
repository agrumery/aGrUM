import pyagrum as gum

mb = gum.MarkovBlanket(gum.fastBN("A->B->C<-D;C->E"), "B")
sorted(mb.namesFromIds(mb.nodes()))  # B, its parents, its children and the other parents of its children
# ['A', 'B', 'C', 'D']
