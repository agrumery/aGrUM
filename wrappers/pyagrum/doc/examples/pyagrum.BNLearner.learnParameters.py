import pyagrum as gum

with open("data.csv", "w") as f:  # C is mostly "A or B"
  f.write("A,B,C\n" + "0,0,0\n0,1,1\n1,0,1\n1,1,1\n" * 40 + "0,0,1\n0,1,0\n1,0,0\n1,1,0\n" * 5)
learner = gum.BNLearner("data.csv")
dag = gum.fastDAG("A;B;C;A->C<-B")  # the ids of the nodes are the ids of the columns
bn = learner.learnParameters(dag)
round(bn.cpt("C")[{"A": 0, "B": 0, "C": 0}], 4)
# 0.8889
