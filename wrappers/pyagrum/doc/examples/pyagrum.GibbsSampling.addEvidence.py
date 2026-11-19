import pyagrum as gum

bn = gum.fastBN("A->B<-C", uniform=True)
ie = gum.GibbsSampling(bn)
ie.addEvidence("A", 1)  # hard evidence by index
ie.addEvidence("B", "0")  # hard evidence by label
ie.addEvidence("C", 0)
sorted(ie.hardEvidenceNodes()), sorted(ie.softEvidenceNodes())  # sampling accepts hard evidence only
# ([0, 1, 2], [])
