import pyagrum as gum

with open("data.csv", "w") as f:  # "?" is a missing value
  f.write("A,B\n" + "0,0\n0,1\n1,1\n1,1\n?,0\n1,?\n" * 20)
learner = gum.BNLearner("data.csv")
learner.useEM(1e-4, 0.0)  # no random noise at initialization
learner.EMsetVerbosity(True)  # needed to record the history
bn = learner.learnParameters(gum.fastDAG("A->B"))
[round(x, 4) for x in learner.EMHistory()]
# [121.3685, 121.3685]
