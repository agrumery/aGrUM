# requires: pandas
import pyagrum as gum
import pyagrum.skbn as skbn
from pyagrum.lib.discreteTypeProcessor import DiscreteTypeProcessor

gum.initRandom(42)
bn = gum.fastBN("A->Y<-B;Y->C")
gum.generateSample(bn, 500, "data.csv")
clf = skbn.BNClassifier(type_processor=DiscreteTypeProcessor())
X, y = clf.XYfromCSV("data.csv", target="Y")
clf.fit(X, y)
clf.predict_proba(X.head(4)).round(3)  # one column per class
# array([[0.269, 0.731],
#        [0.345, 0.655],
#        [0.478, 0.522],
#        [0.219, 0.781]])
