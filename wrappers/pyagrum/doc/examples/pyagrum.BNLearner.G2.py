import pyagrum as gum

with open("data.csv", "w") as f:  # C is mostly "A or B"
  f.write("A,B,C\n" + "0,0,0\n0,1,1\n1,0,1\n1,1,1\n" * 40 + "0,0,1\n0,1,0\n1,0,0\n1,1,0\n" * 5)
learner = gum.BNLearner("data.csv")
stat, pvalue = learner.G2("A", "B")
round(stat, 4), round(pvalue, 4)
# (0.0, 1.0)
stat, pvalue = learner.G2("A", "B", ["C"])  # A and B are dependent given C
round(stat, 4), round(pvalue, 4)
# (27.9536, 0.0)
