import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyBeliefPropagation(bn)
ie.addEvidence("A", 1)
ie.addEvidence("C", [0.3, 0.9])  # soft: not listed
sorted(ie.hardEvidenceNodes())
# [0]
