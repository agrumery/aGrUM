import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyBeliefPropagation(bn)
ie.addEvidence("B", 0)
ie.setEvidence({"A": 1, "C": [0.3, 0.9]})  # previous evidence is erased
sorted(ie.hardEvidenceNodes()), sorted(ie.softEvidenceNodes())
# ([0], [2])
