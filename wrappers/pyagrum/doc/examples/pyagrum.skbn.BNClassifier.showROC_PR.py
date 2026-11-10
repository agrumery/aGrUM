# requires: pandas, sklearn, matplotlib
import pyagrum as gum
import pyagrum.skbn as skbn
from pyagrum.lib.discreteTypeProcessor import DiscreteTypeProcessor

gum.initRandom(42)
bn = gum.fastBN("A->Y<-B;Y->C")
gum.generateSample(bn, 100, "data.csv")
clf = skbn.BNClassifier(type_processor=DiscreteTypeProcessor())
clf.fitFromTabular("data.csv", "Y")
clf.showROC_PR("data.csv")  # ROC and precision-recall curves on data.csv
