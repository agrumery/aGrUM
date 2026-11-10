import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyBeliefPropagation(bn)
ie.addEvidence("A", 1)  # hard: not listed
ie.addEvidence("C", [0.3, 0.9])
sorted(ie.softEvidenceNodes())
# [2]
