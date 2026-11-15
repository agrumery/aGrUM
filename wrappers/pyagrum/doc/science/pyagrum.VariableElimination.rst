VariableElimination computes each posterior separately, by summing out the other variables one after the other from
the product of the factors where they appear :cite:`zhang1994simple,dechter1999bucket`.

For each query (one target, or one joint target), pyAgrum builds the structure of `pyagrum.LazyPropagation` for this
target alone: the barren nodes and the nodes irrelevant to the target given the evidence (Bayes-Ball
:cite:`shachter1998bayes`) are removed, the remaining moral graph is triangulated with the default triangulation, and
the elimination order given by this triangulation is used. The factors are kept as sets, as in
:cite:`madsen1999lazy`, and the messages are only collected towards the clique of the target (no binary join tree, no
distribution phase). Nothing is kept from one query to the next: VariableElimination is well suited to a few
posteriors on a large network, and `pyagrum.LazyPropagation` to many posteriors under the same evidence. There is
neither ``mpe`` nor ``evidenceProbability``.
