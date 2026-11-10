import pyagrum as gum

diag = gum.fastID("A->C<-B;C->*D->$U")
g = diag.moralizedAncestralGraph("C")
sorted(g.nodes())
# [0, 1, 2]
sorted(g.edges())
# [(0, 1), (0, 2), (1, 2)]
