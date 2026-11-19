import pyagrum as gum

bn = gum.fastBN("A->B<-C", uniform=True)
ie = gum.ImportanceSampling(bn)
ie.setEvidence({"A": 1, "B": "0", "C": 0})
ie.nbrSoftEvidence()  # sampling accepts hard evidence only
# 0
