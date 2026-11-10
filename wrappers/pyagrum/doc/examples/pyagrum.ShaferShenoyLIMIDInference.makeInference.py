import pyagrum as gum

diag = gum.fastID("W{dry|rain}->F{sun|cloud}->*D{out|home}->$U<-W")
diag.cpt("W").fillWith([0.7, 0.3])
diag.cpt("F").fillWith([0.8, 0.2, 0.3, 0.7])
diag.utility("U").fillWith([100, 50, 0, 50])
ie = gum.ShaferShenoyLIMIDInference(diag)
ie.makeInference()
print(ie.optimalDecision("D"))
#
#       ║  D                │
# F     ║out      │home     │
# ──────║─────────│─────────│
# sun   ║ 1.0000  │ 0.0000  │
# cloud ║ 0.0000  │ 1.0000  │
round(ie.MEU()["mean"], 4)
# 73.5
