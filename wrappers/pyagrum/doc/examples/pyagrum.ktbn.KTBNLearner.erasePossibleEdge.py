import pyagrum as gum
import pyagrum.ktbn as ktbn

gum.initRandom(42)
model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1->Y1;Y0"))
ktbn.KTBNDatabaseGenerator(model).drawSamples(50, 10, ".", "traj")  # traj1.csv ... traj50.csv
learner = ktbn.KTBNLearner(".", "traj", 50, 2, gum.fastBN("X;Y"))  # k=2, variables X and Y
learner.addPossibleEdge("X[0]", "X[1]")
learner.erasePossibleEdge("X[0]", "X[1]")
"Constraint Possible Edges" in dict(s[:2] for s in learner.state())
# False
