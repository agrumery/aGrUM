BNClassifier is a classifier whose model is a Bayesian network over the features and the class :math:`Y`. The
continuous features are first discretized by a `pyagrum.lib.discreteTypeProcessor.DiscreteTypeProcessor`
(``createBNClassifier``: quantiles, 5 intervals, for the numerical features with more than 25 distinct values). The
structure is learned by `pyagrum.BNLearner` (MIIC :cite:`verny2017learning` by default, greedy hill climbing, tabu
search) or has a fixed form: naive Bayes (:math:`Y` parent of every feature), the Chow-Liu tree
:cite:`chow1968approximating` over the features and the class, rooted at :math:`Y`, or the tree-augmented naive Bayes
(TAN) :cite:`friedman1997bayesian`. Without prior given, the parameters are estimated with a smoothing prior of
weight 0.01.

**TAN.** The selective TAN of :cite:`blanco2005feature` is learned with the level :math:`\alpha` given by
``TANSignificance`` (default: the key ``skbn.tanSignificance`` of `pyagrum.config`, 0.01). The features
:math:`X_i` whose G-test of independence from :math:`Y` (:math:`G^2 = 2N\,I(X_i; Y)`, :math:`\chi^2` with
:math:`(r_i-1)(r_Y-1)` degrees of freedom) gives a p-value above :math:`\alpha` are dropped (they stay in the
network as isolated nodes; if none is kept, a warning is emitted and the prior of :math:`Y` is predicted). Between the
kept
features, the pairs whose conditional G-test (:math:`G^2 = 2N\,I(X_i; X_j \mid Y)`, :math:`\chi^2` with
:math:`(r_i-1)(r_j-1)r_Y` degrees of freedom) gives a p-value above :math:`\alpha` are dropped, and a maximum weight
spanning tree of each connected component of the remaining pairs, weighted by :math:`I(X_i; X_j \mid Y)`, is built
(Kruskal): the result may be a forest. :cite:`blanco2005feature` test each slice :math:`Y=y` separately, with
:math:`(r_i-1)(r_j-1)` degrees of freedom, and keep the pair if one slice is significant; pyAgrum uses the
conditional test, whose asymptotic law is known. Each tree is directed from the feature that has the largest
:math:`I(X; Y)`, and :math:`Y` is a parent of every kept feature. With :math:`\alpha = 1`, every feature and every
pair is kept: this is the TAN of :cite:`friedman1997bayesian`.

**Prediction.** The posterior of :math:`Y` is computed in the Markov blanket of :math:`Y` only. With more than two
classes, the most probable one is predicted. With two classes, the second label :math:`y_1` is predicted when
:math:`p = P(Y=y_1 \mid x) \geq t`, the threshold :math:`t` being chosen on the training data: the point of the ROC
curve closest to :math:`(0, 1)`, or, with ``usePR``, the point of the precision-recall curve of maximal
:math:`F_\beta`. ``predict_proba`` rescales :math:`p` piecewise linearly (:math:`p/2t` below :math:`t`,
:math:`\frac12 + \frac{p-t}{2(1-t)}` above) so that its largest value always gives the predicted class.
