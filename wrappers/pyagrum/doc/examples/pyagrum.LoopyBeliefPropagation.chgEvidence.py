import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyBeliefPropagation(bn)
ie.addEvidence("A", 0)
ie.chgEvidence("A", 1)
ie.makeInference()
ie.posterior("A").tolist()
# [0.0, 1.0]
ie.chgEvidence("A", [0.2, 0.6])  # hard evidence changed into a soft one
ie.hasSoftEvidence("A")
# True
