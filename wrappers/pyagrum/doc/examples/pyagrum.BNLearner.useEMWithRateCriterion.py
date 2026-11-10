import pyagrum as gum

with open("data.csv", "w") as f:  # "?" is a missing value
  f.write("A,B\n" + "0,0\n0,1\n1,1\n1,1\n?,0\n1,?\n" * 20)
learner = gum.BNLearner("data.csv")
learner.useEMWithRateCriterion(1e-4, 0.0)  # no random noise at initialization
bn = learner.learnParameters(gum.fastDAG("A->B"))
learner.EMisEnabledMinEpsilonRate(), learner.EMMinEpsilonRate()
# (True, 0.0001)
