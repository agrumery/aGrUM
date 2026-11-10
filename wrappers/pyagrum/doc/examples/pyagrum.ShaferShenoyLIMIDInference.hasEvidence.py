import pyagrum as gum

diag = gum.fastID("W{dry|rain}->F{sun|cloud}->*D{out|home}->$U<-W")
ie = gum.ShaferShenoyLIMIDInference(diag)
ie.hasEvidence("F")
# False
ie.setEvidence({"F": "sun"})
ie.hasEvidence("F")
# True
