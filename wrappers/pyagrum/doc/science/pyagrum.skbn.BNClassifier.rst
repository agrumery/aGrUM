BNClassifier is a classifier whose model is a Bayesian network over the features and the class :math:`Y`. The
continuous features are first discretized by a `pyagrum.lib.discreteTypeProcessor.DiscreteTypeProcessor`
(``createBNClassifier``: quantiles, 5 intervals, for the numerical features with more than 25 distinct values). The
structure is learned by `pyagrum.BNLearner` (MIIC :cite:`verny2017learning` by default, greedy hill climbing, tabu
search) or has a fixed form: naive Bayes (:math:`Y` parent of every feature), the Chow-Liu tree
:cite:`chow1968approximating` over the features and the class, rooted at :math:`Y`, or the tree-augmented naive Bayes
(TAN) :cite:`friedman1997bayesian`. Without prior given, the parameters are estimated with a smoothing prior of
weight 0.01.

**TAN.** The conditional mutual informations :math:`I(X_i; X_j \mid Y)` of the :math:`d` features are computed from the
counts, and a maximum weight spanning tree is built over them (Kruskal), as in :cite:`friedman1997bayesian`, except
that the pairs whose information is below

.. math::

   \tau = \frac{1}{d(d-1)} \sum_{i<j} I(X_i; X_j \mid Y),

half of the mean information of the pairs, are not used: the result may be a forest. Each tree is directed from the
feature that has the largest :math:`I(X; Y)`, and :math:`Y` is a parent of every feature.

**Prediction.** The posterior of :math:`Y` is computed in the Markov blanket of :math:`Y` only. With more than two
classes, the most probable one is predicted. With two classes, the second label :math:`y_1` is predicted when
:math:`p = P(Y=y_1 \mid x) \geq t`, the threshold :math:`t` being chosen on the training data: the point of the ROC
curve closest to :math:`(0, 1)`, or, with ``usePR``, the point of the precision-recall curve of maximal
:math:`F_\beta`. ``predict_proba`` rescales :math:`p` piecewise linearly (:math:`p/2t` below :math:`t`,
:math:`\frac12 + \frac{p-t}{2(1-t)}` above) so that its largest value always gives the predicted class.
