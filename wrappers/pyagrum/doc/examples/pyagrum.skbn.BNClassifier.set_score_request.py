# requires: pandas, sklearn
import sklearn

import pyagrum.skbn as skbn
from pyagrum.lib.discreteTypeProcessor import DiscreteTypeProcessor

clf = skbn.BNClassifier(type_processor=DiscreteTypeProcessor())
with sklearn.config_context(enable_metadata_routing=True):
  clf.set_score_request(sample_weight=True)  # score() will receive sample_weight in a meta-estimator
  requests = clf.get_metadata_routing().score.requests
requests
# {'sample_weight': True}
