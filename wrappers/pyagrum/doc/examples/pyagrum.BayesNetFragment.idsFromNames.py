import pyagrum as gum

bn = gum.fastBN("A->B->C<-D")
fragment = gum.BayesNetFragment(bn)
fragment.installNode("B")
fragment.installNode("C")
fragment.idsFromNames(("C", "B"))
# (2, 1)
