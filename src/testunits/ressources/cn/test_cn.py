import pyAgrum as gum

cn=gum.CredalNet("2Umin.bif","2Umax.bif")
cn.intervalToCredal(0)

ie=gum.MCSampling(cn)
ie.insertFileOfEvidence("L2U.evi")

mcSampling.setRepetitiveInd(False)
mcSampling.setTimeLimit(1)
mcSampling.setIterStop(1000)