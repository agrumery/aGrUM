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
clf.preparedData(X.head(3), y.head(3))  # the data as seen by the BN
#    C  B  A  Y
# 0  1  1  0  1
# 1  0  1  0  1
# 2  0  0  0  1
