import pyagrum as gum

diag = gum.fastID("A->C<-B;C->*D->$U")
g = diag.moralGraph()
sorted(g.edges())
# [(0, 1), (0, 2), (1, 2), (1, 3), (3, 4)]
