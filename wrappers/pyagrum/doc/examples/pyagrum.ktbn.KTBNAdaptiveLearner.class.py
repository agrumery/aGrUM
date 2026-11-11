import pyagrum as gum
import pyagrum.ktbn as ktbn

gum.initRandom(42)
model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1->Y1;Y0;C->X0"))
ktbn.KTBNDatabaseGenerator(model).drawSamples(50, 10, ".", "traj")  # traj1.csv ... traj50.csv
schema = gum.fastBN("X;Y;C")  # the variables and their domains
learner = ktbn.KTBNAdaptiveLearner(".", "traj", 50, 4, schema, {"C"})  # k in [2, 4], C atemporal
learner.useScoreBIC().useGreedyHillClimbing()
learned = learner.learnKTBN()
[k for k, score in learner.scorePerCandidateK()]  # one cross-k score per candidate order
# [2, 3, 4]
learned.k() == learner.bestK()
# True
