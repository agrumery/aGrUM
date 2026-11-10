import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1->Y1;Y0;C->X1"))
gen = ktbn.KTBNDatabaseGenerator(model)
gum.initRandom(42)
loglikelihoods = gen.drawSamples(3, 5, ".", "traj")  # traj1.csv, traj2.csv, traj3.csv
len(loglikelihoods)
# 3
len(open("traj2.csv").readlines())  # a header and 5 time steps
# 6
gen.drawSamples([4, 6], ".", "other")  # one horizon per trajectory
len(open("other2.csv").readlines())
# 7
