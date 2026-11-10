import pyagrum as gum

with open("data.csv", "w") as f:  # C is mostly "A or B"
  f.write("A,B,C\n" + "0,0,0\n0,1,1\n1,0,1\n1,1,1\n" * 40 + "0,0,1\n0,1,0\n1,0,0\n1,1,0\n" * 5)
learner = gum.BNLearner("data.csv")
learner.setDatabaseWeight(18.0)  # 0.1 for each of the 180 rows
round(learner.databaseWeight(), 4), round(learner.recordWeight(0), 4)
# (18.0, 0.1)
