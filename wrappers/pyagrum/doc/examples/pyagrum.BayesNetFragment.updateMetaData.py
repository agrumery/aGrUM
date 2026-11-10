import pyagrum as gum

bn = gum.fastBN("A->B->C<-D;C->E")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("C")  # A, B, C, D
sorted(fragment.properties())
# []
fragment.updateMetaData()
sorted(fragment.properties())
# ['creation', 'lastModification', 'software']
