import pyagrum as gum

diag = gum.fastID("W{dry|rain}->F{sun|cloud}->*D{out|home}->$U<-W")
ie = gum.ShaferShenoyLIMIDInference(diag)
ie.setEvidence({"W": "dry", "F": "sun"})
ie.eraseEvidence("W")
ie.hasEvidence("W")
# False
ie.hasEvidence("F")
# True
