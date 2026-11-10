import pyagrum as gum

diag = gum.fastID("W{dry|rain}->F{sun|cloud}->*D{out|home}->$U<-W")
diag.cpt("W").fillWith([0.7, 0.3])
diag.cpt("F").fillWith([0.8, 0.2, 0.3, 0.7])
diag.utility("U").fillWith([100, 50, 0, 50])
ie = gum.ShaferShenoyLIMIDInference(diag)
ie.makeInference()
print(ie.posteriorUtility("U"))  # the utility of U given its parents D and W
#
#              ║  U      │
# D     │W     ║0        │
# ──────│──────║─────────│
# out   │dry   ║ 100.0000│
# home  │dry   ║ 50.0000 │
# out   │rain  ║ 0.0000  │
# home  │rain  ║ 50.0000 │
