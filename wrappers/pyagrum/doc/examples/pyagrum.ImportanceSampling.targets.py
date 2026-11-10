import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.ImportanceSampling(bn)
sorted(ie.targets())  # all nodes by default
# [0, 1, 2]
ie.setTargets({"B"})
ie.targets()
# {1}
