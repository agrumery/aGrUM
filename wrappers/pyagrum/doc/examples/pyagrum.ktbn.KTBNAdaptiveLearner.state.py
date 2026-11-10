import pyagrum as gum
import pyagrum.ktbn as ktbn

gum.initRandom(42)
model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1->Y1;Y0"))
ktbn.KTBNDatabaseGenerator(model).drawSamples(50, 10, ".", "traj")  # traj1.csv ... traj50.csv
learner = ktbn.KTBNAdaptiveLearner(".", "traj", 50, 3, gum.fastBN("X;Y"))  # k in [2, 3]
learner.useGreedyHillClimbing()
print("\n".join(f"{key}: {value}" for key, value, comment in learner.state()))
# Candidate orders: 2..3
# Selected k: not learned yet
# Base variables: 2
# Atemporal variables: {}
# Algorithm: Greedy Hill Climbing
# Structure score: BDeu
# Order selection score: BIC
# Prior: no prior is set
