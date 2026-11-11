VariableElimination computes the posteriors of its targets by eliminating the other variables one after the other,
summing out each of them from the product of the factors where it appears :cite:`zhang1994simple,dechter1999bucket`.
The elimination order is given by a triangulation of the moral graph, the factors are kept as sets as in
:cite:`madsen1999lazy`, and the barren nodes and the factors irrelevant to the targets are removed with the Bayes-Ball
algorithm :cite:`shachter1998bayes`.
