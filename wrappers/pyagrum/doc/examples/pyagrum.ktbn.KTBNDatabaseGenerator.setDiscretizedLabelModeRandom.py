import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN(2)
model.addTemporal(gum.DiscretizedVariable("T", "T", [0, 1, 2, 4]))
model.addArc("T", 0, "T", 1)
model.generateCPTs()
gen = ktbn.KTBNDatabaseGenerator(model)
gen.setDiscretizedLabelModeRandom()  # the default
gen.drawSamples(1, 5, ".", "traj")
values = [float(x) for x in open("traj1.csv").read().split()[1:]]
all(0 <= x <= 4 for x in values)
# True
