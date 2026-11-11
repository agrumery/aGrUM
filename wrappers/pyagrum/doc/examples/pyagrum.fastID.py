import pyagrum as gum

infdiag = gum.fastID("Weather->*Umbrella->$Comfort<-Weather")
infdiag.isDecisionNode("Umbrella")
# True
infdiag.isUtilityNode("Comfort")
# True
infdiag.isChanceNode("Weather")
# True

# the second argument is the default domain size of the variables without specification
infdiag = gum.fastID("A->B[1,3]<-*C{yes|No}->$D<-E[1,2.5,3.9]", 6)
print(infdiag.variable("A"))
# A:Range([0,5])
print(infdiag.variable("C"))
# C:Labelized({yes|No})
print(infdiag.variable("D"))  # a utility node has a single value
# D:Range([0,0])
