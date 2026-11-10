import pyagrum as gum

with open("data.csv", "w") as f:  # C is mostly "A or B"
  f.write("A,B,C\n" + "0,0,0\n0,1,1\n1,0,1\n1,1,1\n" * 40 + "0,0,1\n0,1,0\n1,0,0\n1,1,0\n" * 5)
learner = gum.BNLearner("data.csv")
learner.useK2([0, 1, 2])  # the order A, B, C given by the ids of the columns
bn = learner.learnBN()
sorted(bn.parents("C"))
# [0, 1]
