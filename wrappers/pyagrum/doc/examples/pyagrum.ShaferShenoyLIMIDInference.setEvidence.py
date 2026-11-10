import pyagrum as gum

diag = gum.fastID("W{dry|rain}->F{sun|cloud}->*D{out|home}->$U<-W")
diag.cpt("W").fillWith([0.7, 0.3])
diag.cpt("F").fillWith([0.8, 0.2, 0.3, 0.7])
diag.utility("U").fillWith([100, 50, 0, 50])
ie = gum.ShaferShenoyLIMIDInference(diag)
ie.setEvidence({"F": "cloud"})
ie.makeInference()
print(ie.posterior("W"))
#
#   W                │
# dry      │rain     │
# ─────────│─────────│
#  0.4000  │ 0.6000  │
ie.setEvidence({"W": "rain"})  # replaces all the previous evidence
sorted(ie.hardEvidenceNodes())
# [0]
