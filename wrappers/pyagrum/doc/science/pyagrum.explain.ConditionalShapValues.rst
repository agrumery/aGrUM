ConditionalShapValues computes exactly the Shapley values :cite:`shapley1953value` of the features for the posterior
of the target, as in SHAP :cite:`lundberg2017unified`, the value of a coalition being the posterior of the target
conditioned on the features of the coalition: the conditional Shapley values, which take the dependence between the
features into account :cite:`aas2021explaining`, are computed by inference in the Bayesian network.
