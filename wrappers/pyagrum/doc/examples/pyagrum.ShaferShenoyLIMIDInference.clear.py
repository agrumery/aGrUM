import pyagrum as gum

diag = gum.fastID("A->*D1->$U<-*D2<-B")
ie = gum.ShaferShenoyLIMIDInference(diag)
ie.addNoForgettingAssumption(["D1", "D2"])
ie.addEvidence("A", 0)
ie.clear()
ie.nbrEvidence()
# 0
ie.hasNoForgettingAssumption()
# False
