import pyagrum as gum

diag = gum.fastID("A->*D1->$U<-*D2<-B")
ie = gum.ShaferShenoyLIMIDInference(diag)
ie.isSolvable()  # no order between the two decisions
# False
ie.addNoForgettingAssumption(["D1", "D2"])  # D1 is decided before D2
ie.hasNoForgettingAssumption()
# True
ie.isSolvable()
# True
