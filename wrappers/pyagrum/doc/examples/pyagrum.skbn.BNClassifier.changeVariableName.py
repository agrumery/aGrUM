# requires: pandas, sklearn
import pyagrum as gum
import pyagrum.skbn as skbn
from pyagrum.lib.discreteTypeProcessor import DiscreteTypeProcessor

gum.initRandom(42)
bn = gum.fastBN("A->Y<-B;Y->C")
gum.generateSample(bn, 500, "data.csv")
clf = skbn.BNClassifier(type_processor=DiscreteTypeProcessor())
clf.fromTrainedModel(bn, "Y")
clf.changeVariableName("A", "Age")
sorted(clf.model().names())
# ['Age', 'B', 'C', 'Y']
