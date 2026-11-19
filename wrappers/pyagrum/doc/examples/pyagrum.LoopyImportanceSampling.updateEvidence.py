import pyagrum as gum

bn = gum.fastBN("A->B<-C", uniform=True)
ie = gum.LoopyImportanceSampling(bn)
ie.setEvidence({"A": 0})
ie.updateEvidence({"A": 1, "C": 0})  # A changed, C added
ie.makeInference()
ie.posterior("A").tolist()
# [0.0, 1.0]
sorted(ie.softEvidenceNodes())  # sampling accepts hard evidence only
# []
