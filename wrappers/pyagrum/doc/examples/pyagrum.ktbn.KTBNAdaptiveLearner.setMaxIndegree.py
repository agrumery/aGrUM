import pyagrum as gum
import pyagrum.ktbn as ktbn

gum.initRandom(42)
model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1->Y1;Y0"))
ktbn.KTBNDatabaseGenerator(model).drawSamples(50, 10, ".", "traj")  # traj1.csv ... traj50.csv
learner = ktbn.KTBNAdaptiveLearner(".", "traj", 50, 3, gum.fastBN("X;Y"))  # k in [2, 3]
learner.setMaxIndegree(1)
dict(s[:2] for s in learner.state())["Max in-degree"]
# '1'
learned = learner.learnKTBN()
max(len(learned.parents(*node)) for node in learned.nodes())
# 1
