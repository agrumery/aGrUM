import pyagrum as gum

with open("data.csv", "w") as f:  # C is mostly "A or B"
  f.write("A,B,C\n" + "0,0,0\n0,1,1\n1,0,1\n1,1,1\n" * 40 + "0,0,1\n0,1,0\n1,0,0\n1,1,0\n" * 5)
learner = gum.BNLearner("data.csv")
learner.useFCI().setMaxIndegree(1)  # FCI keeps the v-structure A->C<-B anyway
learner.setErrorOnExceededMaxIndegree(False)  # a warning instead of an error
dag = learner.learnDAG()
learner.maxIndegreeExceeded()
# True
learner.setErrorOnExceededMaxIndegree(True)  # learnDAG() now raises pyagrum.OperationNotAllowed
learner.errorOnExceededMaxIndegree()
# True
