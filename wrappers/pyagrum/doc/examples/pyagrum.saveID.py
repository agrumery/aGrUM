import pyagrum as gum

infdiag = gum.fastID("A->*D->$U<-A")
gum.saveID(infdiag, "model.bifxml")
gum.loadID("model.bifxml").size()
# 3
