import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1->Y1;Y0;C->X1"))
gen = ktbn.KTBNDatabaseGenerator(model)
gen.drawSamples(1, 5, ".", "traj", ktbn.KTBNDatabaseGenerator.VarOrderMode_TOPOLOGICAL)
open("traj1.csv").readline()  # the column order
# 'C,X,Y\n'
