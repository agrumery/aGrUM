import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyBeliefPropagation(bn)
ie.addEvidence("A", 1)  # hard evidence by index
ie.addEvidence("B", "0")  # hard evidence by label
ie.addEvidence("C", [0.3, 0.9])  # soft evidence (likelihood)
sorted(ie.hardEvidenceNodes()), sorted(ie.softEvidenceNodes())
# ([0, 1], [2])
