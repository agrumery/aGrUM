import pyagrum as gum
import pyagrum.ktbn as ktbn

gum.initRandom(42)
model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1->Y1;Y0"))
ktbn.KTBNDatabaseGenerator(model).drawSamples(50, 10, ".", "traj")  # traj1.csv ... traj50.csv
learner = ktbn.KTBNAdaptiveLearner(".", "traj", 50, 3, gum.fastBN("X;Y"))  # k in [2, 3]
learner.useGreedyHillClimbing()
learner.useScoreBD()
learner.checkScorePriorCompatibility()
# 'The BD score requires an prior'
learner.useSmoothingPrior()
learner.checkScorePriorCompatibility()
# 'The prior is currently compatible with the BD score but if you change the weight, it may become biased'
