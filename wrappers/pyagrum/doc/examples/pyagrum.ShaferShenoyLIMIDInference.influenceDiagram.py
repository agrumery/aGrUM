import pyagrum as gum

diag = gum.fastID("W{dry|rain}->F{sun|cloud}->*D{out|home}->$U<-W")
ie = gum.ShaferShenoyLIMIDInference(diag)
sorted(ie.influenceDiagram().names())
# ['D', 'F', 'U', 'W']
ie.influenceDiagram().isDecisionNode("D")
# True
