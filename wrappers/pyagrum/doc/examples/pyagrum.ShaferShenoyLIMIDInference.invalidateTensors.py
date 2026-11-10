import pyagrum as gum

diag = gum.fastID("A->*D->$U<-A")
diag.cpt("A").fillWith([0.2, 0.8])
ie = gum.ShaferShenoyLIMIDInference(diag)
ie.makeInference()
[round(p, 4) for p in ie.posterior("A").tolist()]
# [0.2, 0.8]
diag.cpt("A").fillWith([0.7, 0.3])  # changed in place: the inference is not notified
ie.invalidateTensors()
ie.makeInference()
[round(p, 4) for p in ie.posterior("A").tolist()]
# [0.7, 0.3]
