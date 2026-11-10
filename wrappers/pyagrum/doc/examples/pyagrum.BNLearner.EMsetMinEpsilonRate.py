import pyagrum as gum

with open("data.csv", "w") as f:  # "?" is a missing value
  f.write("A,B\n" + "0,0\n0,1\n1,1\n1,1\n?,0\n1,?\n" * 20)
learner = gum.BNLearner("data.csv")
learner.useEMWithDiffCriterion(1e-3)
learner.EMsetMinEpsilonRate(1e-5)
learner.EMMinEpsilonRate(), learner.EMisEnabledMinEpsilonRate(), learner.EMisEnabledEpsilon()
# (1e-05, True, False)
