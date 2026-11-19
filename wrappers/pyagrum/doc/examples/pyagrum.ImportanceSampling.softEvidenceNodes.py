import pyagrum as gum

bn = gum.fastBN("A->B<-C", uniform=True)
ie = gum.ImportanceSampling(bn)
ie.addEvidence("A", 1)  # hard: not listed
ie.addEvidence("C", 0)
sorted(ie.softEvidenceNodes())  # sampling accepts hard evidence only
# []
