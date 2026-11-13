import pyagrum as gum

diag = gum.InfluenceDiagram.fastPrototype("C{low|high}->*D[3]->$U<-C")
diag.chanceNodeSize(), diag.decisionNodeSize(), diag.utilityNodeSize()
# (1, 1, 1)
print(diag.variable("D"))
# D:Range([0,2])

# the second argument is the default domain size of the variables without specification
diag = gum.InfluenceDiagram.fastPrototype("A->B[1,3]<-*C{yes|No}->$D<-E[1,2.5,3.9]", 6)
print(diag.variable("A"))
# A:Range([0,5])
print(diag.variable("E"))
# E:Discretized(<[1;2.5[,[2.5;3.9]>)

# a name between single quotes, after the prefix of its type
infdiag = gum.InfluenceDiagram.fastPrototype("'my state'->*'my decision'->$'my utility'")
infdiag.isUtilityNode("my utility")
# True
