import pyagrum as gum

bn = gum.fastBN("X_0{0|1}->X_1{0|1}")  # X at time steps 0 and 1
bn.cpt("X_0").fillWith([0.5, 0.5])
bn.cpt("X_1")[:] = [[0.8, 0.2], [0.4, 0.6]]
cn = gum.CredalNet(bn, bn)
cn.intervalToCredal()
cn.computeBinaryCPTMinMax()
ie = gum.CNLoopyPropagation(cn)
with open("modals.txt", "w") as f:
  f.write("X 0 10\n")  # the values of the 2 modalities of X
ie.insertModalsFile("modals.txt")
ie.makeInference()
tuple(round(e, 4) for e in ie.dynamicExpMax("X"))  # one expectation for each time step
# (5.0, 4.0)
tuple(round(e, 4) for e in ie.dynamicExpMin("X"))
# (5.0, 4.0)
