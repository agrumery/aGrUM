import pyagrum as gum
import pyagrum.ktbn as ktbn

gum.initRandom(42)
model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1->Y1;Y0"))
ktbn.KTBNDatabaseGenerator(model).drawSamples(50, 10, ".", "traj")  # traj1.csv ... traj50.csv
learner = ktbn.KTBNLearner(".", "traj", 50, 2, gum.fastBN("X;Y"))  # k=2, variables X and Y
other = ktbn.KTBNLearner(".", "traj", 50, 2, gum.fastBN("X;Y"))
other.useGreedyHillClimbing()
other.useScoreAIC()
other.addForbiddenArc("X[0]", "X[1]")
learner.copyState(other)
dict(s[:2] for s in learner.state())["Score"]
# 'AIC'
dict(s[:2] for s in learner.state())["Constraint Forbidden Arcs"]
# '{X[0]->X[1]}'
