import pyagrum as gum

gum.fastID("*D1->C->*D2->$U<-D1").decisionOrderExists()
# True
gum.fastID("*D1->$U<-*D2").decisionOrderExists()  # D1 and D2 are not ordered
# False
