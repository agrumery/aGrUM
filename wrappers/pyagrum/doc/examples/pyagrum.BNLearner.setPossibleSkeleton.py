import pyagrum as gum

with open("data.csv", "w") as f:  # C is mostly "A or B"
  f.write("A,B,C\n" + "0,0,0\n0,1,1\n1,0,1\n1,1,1\n" * 40 + "0,0,1\n0,1,0\n1,0,0\n1,1,0\n" * 5)
learner = gum.BNLearner("data.csv")
learner.useGreedyHillClimbing()
skeleton = gum.UndiGraph()
skeleton.addNodes(3)
skeleton.addEdge(0, 2)
skeleton.addEdge(1, 2)
learner.setPossibleSkeleton(skeleton)  # no edge A-B
bn = learner.learnBN()
bn.existsArc("A", "B") or bn.existsArc("B", "A")
# False
