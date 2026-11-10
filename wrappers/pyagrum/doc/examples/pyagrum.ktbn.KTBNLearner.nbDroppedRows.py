import pyagrum as gum
import pyagrum.ktbn as ktbn

with open("traj1.csv", "w") as f:
  f.write("X\n0\n1\n?\n1\n0\n1\n")  # a missing value at t=2
learner = ktbn.KTBNLearner(".", "traj", 1, 2, gum.fastBN("X"), set(), ["?"], True)
learner.nbDroppedRows()
# 2
