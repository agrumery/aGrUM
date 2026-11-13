import pyagrum as gum

# an imprecise transition of X between the time steps 0 and 1: lower and upper probabilities
bnmin = gum.fastBN("X_0{0|1}->X_1{0|1}")
bnmin.cpt("X_0").fillWith([0.4, 0.4])
bnmin.cpt("X_1")[:] = [[0.7, 0.1], [0.3, 0.5]]
bnmax = gum.BayesNet(bnmin)
bnmax.cpt("X_0").fillWith([0.6, 0.6])
bnmax.cpt("X_1")[:] = [[0.9, 0.3], [0.5, 0.7]]
cn = gum.CredalNet(bnmin, bnmax)
cn.intervalToCredal()
ie = gum.CNMonteCarloSampling(cn)
with open("modals.txt", "w") as f:
  f.write("X 0 10\n")  # the values of the 2 modalities of X
ie.insertModalsFile("modals.txt")
ie.makeInference()
tuple(round(e, 4) for e in ie.dynamicExpMin("X"))  # the lower expectation of X at each time step
# (4.0, 2.6)
tuple(round(e, 4) for e in ie.dynamicExpMax("X"))  # the upper expectation of X at each time step
# (6.0, 5.4)
