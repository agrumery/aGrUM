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

cn=gum.CredalNet("bn_c_8.bif","den_c_8.bif")
cn.bnToCredal(0.8,False)

ie=gum.MCSampling(cn)
ie.insertModalsFile("modalities.modal")

ie.setRepetitiveInd(True)
ie.setTimeLimit(30)
ie.setIterStop(1000)

ie.makeInference_v3()

print ie.dynamicExpMax("temp")
print ie.dynamicExpMin("temp")
