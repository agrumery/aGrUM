import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyWeightedSampling(bn)
ie.setVerbosity(True)  # history is only kept in verbose mode
ie.setEpsilon(1e-10)
ie.setMaxIter(1000)
ie.setPeriodSize(200)
ie.makeInference()
len(ie.history())  # one epsilon per period
# 5
