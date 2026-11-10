import pyagrum as gum

gum.saveID(gum.fastID("A->*D->$U<-A"), "model.bifxml")
infdiag = gum.loadID("model.bifxml")
infdiag.isDecisionNode("D")
# True
infdiag.isUtilityNode("U")
# True
