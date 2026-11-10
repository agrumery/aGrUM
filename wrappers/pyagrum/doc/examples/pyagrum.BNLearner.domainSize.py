import pyagrum as gum

with open("data.csv", "w") as f:
  f.write("A,B\n0,x\n1,y\n2,z\n")
learner = gum.BNLearner("data.csv")
learner.domainSize("A"), learner.domainSize(1)
# (3, 3)
