import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyImportanceSampling(bn)
ie.addEvidence("B", 0)
ie.setEvidence({"A": 1, "C": 0})  # previous evidence is erased
sorted(ie.hardEvidenceNodes()), sorted(ie.softEvidenceNodes())  # sampling accepts hard evidence only
# ([0, 2], [])
