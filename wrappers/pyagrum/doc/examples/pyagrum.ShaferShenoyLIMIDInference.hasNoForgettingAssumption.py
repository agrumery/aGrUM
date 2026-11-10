import pyagrum as gum

diag = gum.fastID("A->*D1->$U<-*D2<-B")
ie = gum.ShaferShenoyLIMIDInference(diag)
ie.hasNoForgettingAssumption()
# False
ie.addNoForgettingAssumption(["D1", "D2"])
ie.hasNoForgettingAssumption()
# True
