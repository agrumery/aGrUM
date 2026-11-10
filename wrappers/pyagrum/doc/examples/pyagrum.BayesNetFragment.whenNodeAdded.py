import pyagrum as gum

bn = gum.fastBN("A->B->C<-D;C->E")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("C")  # A, B, C, D
bn.add("F")  # the fragment is notified, but the new node is not installed
fragment.isInstalledNode(bn.idFromName("F"))
# False
