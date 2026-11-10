import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN(2)
model.addTemporal(gum.DiscretizedVariable("T", "T", [0, 1, 2, 4]))
model.addArc("T", 0, "T", 1)
model.generateCPTs()
gen = ktbn.KTBNDatabaseGenerator(model)
gen.setDiscretizedLabelModeInterval()
gen.drawSamples(1, 5, ".", "traj")
labels = open("traj1.csv").read().split()[1:]
set(labels) <= {"[0;1[", "[1;2[", "[2;4]"}
# True
