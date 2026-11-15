# requires: pandas
import pyagrum as gum
import pyagrum.skbn as skbn
from pyagrum.lib.discreteTypeProcessor import DiscreteTypeProcessor

gum.initRandom(42)
bn = gum.fastBN("A->Y<-B;Y->C")
gum.generateSample(bn, 500, "data.csv")
clf = skbn.BNClassifier(type_processor=DiscreteTypeProcessor())
clf.fromTrainedModel(bn, "Y")  # no learning: bn is used as is
X, y = clf.XYfromCSV("data.csv", target="Y")
clf.predict(X.head(4)).tolist()  # the labels of Y
# ['1', '1', '1', '1']
