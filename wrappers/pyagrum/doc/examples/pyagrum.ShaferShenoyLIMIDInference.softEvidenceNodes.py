import pyagrum as gum

diag = gum.fastID("W{dry|rain}->F{sun|cloud}->*D{out|home}->$U<-W")
ie = gum.ShaferShenoyLIMIDInference(diag)
ie.setEvidence({"W": "dry", "F": [0.9, 0.4]})
sorted(ie.softEvidenceNodes())
# [1]
