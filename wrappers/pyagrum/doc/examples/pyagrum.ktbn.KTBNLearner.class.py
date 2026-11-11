import pyagrum as gum
import pyagrum.ktbn as ktbn

gum.initRandom(42)
model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1->Y1;Y0;C->X0"))
ktbn.KTBNDatabaseGenerator(model).drawSamples(50, 10, ".", "traj")  # traj1.csv ... traj50.csv
schema = gum.fastBN("X;Y;C")  # the variables and their domains
learner = ktbn.KTBNLearner(".", "traj", 50, 2, schema, {"C"})  # k=2, C atemporal (an actual set)
learner.useScoreBIC().useGreedyHillClimbing()
learned = learner.learnKTBN()
learned.k()
# 2
sorted(learned.temporalVarNames()), sorted(learned.atemporalVarNames())
# (['X', 'Y'], ['C'])
