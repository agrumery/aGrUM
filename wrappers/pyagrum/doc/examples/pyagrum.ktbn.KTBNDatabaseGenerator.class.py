import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1->Y1;Y0;C->X1"))
gen = ktbn.KTBNDatabaseGenerator(model)
gum.initRandom(42)
loglikelihoods = gen.drawSamples(10, 5, ".", "traj")  # 10 trajectories of length 5: traj1.csv ... traj10.csv
len(loglikelihoods)  # one log2-likelihood per trajectory
# 10
len(open("traj10.csv").readlines())  # a header and 5 time steps
# 6
