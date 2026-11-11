MarginalShapValues computes exactly the Shapley values :cite:`shapley1953value` of the features for the posterior of
the target, as in SHAP :cite:`lundberg2017unified`: the features out of a coalition are drawn from a background
sample, independently of the features of the coalition (marginal, or interventional, Shapley values, advocated by
:cite:`janzing2020feature`).
