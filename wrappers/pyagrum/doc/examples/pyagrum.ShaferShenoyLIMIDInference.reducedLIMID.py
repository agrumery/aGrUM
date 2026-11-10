import pyagrum as gum

diag = gum.fastID("W->F->*D->$U<-W;N->*D")
ie = gum.ShaferShenoyLIMIDInference(diag)
reduced = ie.reducedLIMID()
diag.existsArc("N", "D"), reduced.existsArc("N", "D")  # N is useless for D
# (True, False)
