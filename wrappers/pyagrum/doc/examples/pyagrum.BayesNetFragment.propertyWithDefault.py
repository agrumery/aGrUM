import pyagrum as gum

bn = gum.fastBN("A->B->C<-D;C->E")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("C")  # A, B, C, D
fragment.setProperty("author", "me")
fragment.propertyWithDefault("author", "unknown")
# 'me'
fragment.propertyWithDefault("license", "unknown")
# 'unknown'
