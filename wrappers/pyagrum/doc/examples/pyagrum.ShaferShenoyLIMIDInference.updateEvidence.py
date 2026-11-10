import pyagrum as gum

diag = gum.fastID("W{dry|rain}->F{sun|cloud}->*D{out|home}->$U<-W")
ie = gum.ShaferShenoyLIMIDInference(diag)
ie.setEvidence({"W": "dry"})
ie.updateEvidence({"W": "rain", "F": [0.9, 0.4]})  # W is changed, F is added
ie.nbrEvidence()
# 2
sorted(ie.softEvidenceNodes())
# [1]
