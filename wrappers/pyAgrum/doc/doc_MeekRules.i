%feature("docstring") gum::MeekRules
"
MeekRules class applies the Meek rules to a mixed graph or PDAG (Partially Directed Acyclic Graph) in order to propagate orientation constraints and obtain a CPDAG (Completed Partially Directed Acyclic Graph) or a DAG (Directed Acyclic Graph).

The Meek Rules help complete the orientation of edges in a mixed graph, resulting in a CPDAG that reflects the possible causal relationships among variables while accounting for the observed conditional independencies in the data. The CPDAG is a more refined and directed version of the original PDAG (Partially Directed Acyclic Graph) obtained from the data.
"

%feature("docstring") gum::MeekRules::propagates
"
Propagates orientation constraints in the graph according to the Meek rules. The Meek rules are applied iteratively until no more orientation constraints can be propagated. This may lead to double orientation that are not resolved by the Meek rules. Then this method returns just a pyAgrum.MixedGraph.

Parameters
----------
mg : pyAgrum.MixedGraph
    The graph to be completed.

Returns
-------
pyAgrum.MixedGraph
    The completed graph, with possible double orientations.
"

%feature("docstring") gum::MeekRules::propagateToCPDAG
"
Propagates orientation constraints in the graph according to the Meek rules. The Meek rules are applied iteratively until no more orientation constraints can be propagated. And then arbitratily resolves double orientations. This method returns a `pyAgrum.PDAG` with the properties of CPDAG.

The arbitrary resolution of double orientations is not deterministic. It depends on the order of the edges in the graph. `pyAgrum.MeekRules.Choices` returns the list of the arbitrary choices made by the last execution of `pyAgrum.MeekRules.progagatesToCPDAG`.

Parameters
----------
mg : pyAgrum.MixedGraph
    The graph to be completed.

Returns
-------
pyAgrum.PDAG
    The completed `pyAgrum.PDAG` (CPDAG).
"

%feature("docstring") gum::MeekRules::propagateToDAG
"
Call propagateToCPDAG and then orients the remaining edges arbitrarily according to some heuristics. `pyAgrum.MeekRules.Choices` returns the list of the arbitrary choices made by the last execution of `pyAgrum.MeekRules.progagatesToDAG`.

Parameters
----------
mg : `pyAgrum.MixedGraph` or `pyAgrum.PDAG`
    The graph to be completed.

Returns
-------
`pyAgrum.DAG`
    The completed `pyAgrum.DAG`.
"

%feature("docstring") gum::MeekRules::choices
"
Returns the list of the arbitrary choices made by the last execution of `pyAgrum.MeekRules.progagatesToCPDAG` or `pyAgrum.MeekRules.progagatesToDAG.

Returns
-------
list of tuple
    The list of the arbitrary choices made by the last execution of `pyAgrum.MeekRules.progagatesToCPDAG` or `pyAgrum.MeekRules.progagatesToDAG`. Each tuple is `pyAgrum.Edge`.
"
