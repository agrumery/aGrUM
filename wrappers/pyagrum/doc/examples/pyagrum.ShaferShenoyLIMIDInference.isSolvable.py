import pyagrum as gum

diag = gum.fastID("A->*D1->$U<-*D2<-B")
ie = gum.ShaferShenoyLIMIDInference(diag)
ie.isSolvable()  # D1 and D2 are not ordered
# False
ie.addNoForgettingAssumption(["D1", "D2"])
ie.isSolvable()
# True
