import pyagrum as gum

infdiag = gum.fastID("Weather->*Umbrella->$Comfort<-Weather")
infdiag.isDecisionNode("Umbrella")
# True
infdiag.isUtilityNode("Comfort")
# True
infdiag.isChanceNode("Weather")
# True
