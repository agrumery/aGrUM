import pyagrum as gum

diag = gum.fastID("W->F->*D->$U<-W;N->*D")
ie = gum.ShaferShenoyLIMIDInference(diag)
sorted(diag.arcs())
# [(0, 1), (0, 3), (1, 2), (2, 3), (4, 2)]
sorted(ie.reducedGraph().arcs())  # N (id 4) is useless for D
# [(0, 1), (0, 3), (1, 2), (2, 3)]
