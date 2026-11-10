import pyagrum as gum

diag = gum.fastID("*D1->X->*D2->$U<-X")
ie = gum.ShaferShenoyLIMIDInference(diag)
ie.reversePartialOrder()  # D2 (id 2) is solved before D1 (id 0)
# [{2}, {0}]
