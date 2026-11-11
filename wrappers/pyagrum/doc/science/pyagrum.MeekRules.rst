MeekRules applies the four orientation rules of :cite:`meek1995causal` to a mixed graph. For an extendable PDAG
:cite:`dor1992simple`, the closure under these rules is unique and is computed with the algorithm of
:cite:`wienobst2021extendability`. The orientation into a DAG with small conditional probability tables
(``propagateToDAG``) removes potential sinks as in :cite:`dor1992simple` and, in the exact case, follows the
Clique-Picking recursion of :cite:`wienobst2021polynomial,wienobst2023polynomial`.
