import pyagrum as gum

diag = gum.InfluenceDiagram.fastPrototype("C{low|high}->*D[3]->$U<-C")
diag.chanceNodeSize(), diag.decisionNodeSize(), diag.utilityNodeSize()
# (1, 1, 1)
print(diag.variable("D"))
# D:Range([0,2])
