import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1->Y1;Y0;C->X1"))
gen = ktbn.KTBNDatabaseGenerator(model)
gen.nbVars()  # one column per base variable
# 3
