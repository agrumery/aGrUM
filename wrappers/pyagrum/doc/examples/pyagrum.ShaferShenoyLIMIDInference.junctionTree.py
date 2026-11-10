import pyagrum as gum

diag = gum.fastID("W{dry|rain}->F{sun|cloud}->*D{out|home}->$U<-W")
ie = gum.ShaferShenoyLIMIDInference(diag)
ie.makeInference()
jt = ie.junctionTree()
sorted(sorted(diag.variable(n).name() for n in jt.clique(c)) for c in jt.nodes())
# [['D', 'F', 'W']]
