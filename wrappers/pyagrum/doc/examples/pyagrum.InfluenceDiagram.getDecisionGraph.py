import pyagrum as gum

diag = gum.fastID("*D1->C->*D2->$U<-D1")
g = diag.getDecisionGraph()
sorted(g.nodes())
# [0, 2]
sorted(g.arcs())
# [(0, 2)]
