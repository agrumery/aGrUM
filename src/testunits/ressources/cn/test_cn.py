import pyAgrum as gum

#cn=gum.CredalNet("2Umin.bif","2Umax.bif")
#cn.intervalToCredal(0)

#ie=gum.MCSampling(cn)
#ie.insertEvidenceFile("L2U.evi")

#ie.setRepetitiveInd(False)
#ie.setTimeLimit(1)
#ie.setIterStop(1000)

#ie.makeInference()

#print ie.marginalMin(0)

cn=gum.CredalNet("bn_c_3.bif","den_c_3.bif")
cn.bnToCredal(0.5,False)

ie=gum.MCSampling(cn)

ie.setRepetitiveInd(True)
ie.setTimeLimit(1)
ie.setIterStop(1000)

ie.makeInference_v3()

print ie.marginalMin("temp_0")
