# requires: pandas, sklearn
import pyagrum as gum
import pyagrum.skbn as skbn
from pyagrum.lib.discreteTypeProcessor import DiscreteTypeProcessor

gum.initRandom(42)
bn = gum.fastBN("A->Y<-B;Y->C")
gum.generateSample(bn, 500, "data.csv")
clf = skbn.BNClassifier(type_processor=DiscreteTypeProcessor())
X, y = clf.XYfromCSV("data.csv", target="Y")
list(X.columns)
# ['A', 'B', 'C']
y.name, len(y)
# ('Y', 500)
