import pyagrum as gum
import pyagrum.ktbn as ktbn

gum.initRandom(42)
model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1->Y1;Y0"))
ktbn.KTBNDatabaseGenerator(model).drawSamples(50, 10, ".", "traj")  # traj1.csv ... traj50.csv
learner = ktbn.KTBNLearner(".", "traj", 50, 2, gum.fastBN("X;Y"))  # k=2, variables X and Y
learned = learner.learnParameters(model)  # the arcs of model, new CPTs
learned.arcs() == model.arcs()
# True
learned.cpt("Y", 1).sumOut("Y[1]").tolist()  # normalized
# [1.0, 1.0]
