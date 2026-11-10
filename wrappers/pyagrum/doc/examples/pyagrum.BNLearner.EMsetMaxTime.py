import pyagrum as gum

with open("data.csv", "w") as f:  # "?" is a missing value
  f.write("A,B\n" + "0,0\n0,1\n1,1\n1,1\n?,0\n1,?\n" * 20)
learner = gum.BNLearner("data.csv")
learner.useEM(1e-4)
learner.EMsetMaxTime(500.0)
learner.EMMaxTime(), learner.EMisEnabledMaxTime()
# (500.0, True)
