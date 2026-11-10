import pyagrum as gum

diag = gum.fastID("W{dry|rain}->F{sun|cloud}->*D{out|home}->$U<-W")
ie = gum.ShaferShenoyLIMIDInference(diag)
ie.setEvidence({"W": "dry", "F": "sun"})
ie.nbrEvidence()
# 2
ie.eraseAllEvidence()
ie.nbrEvidence()
# 0
