StructuralMetrics compares a learned graph to a reference one. Each pair of nodes is classified by its state in the
reference and in the learned graph (arc, undirected edge, or nothing; Bayesian networks are compared through their
essential graphs). For the graph, a true positive is a pair with the same arc or the same edge; a false positive a pair
linked in the learned graph but not in the same way (extra link, wrong orientation, arc instead of edge or the
reverse); a false negative a missing link. The skeleton measures ignore the orientations. Precision, recall and
F-score follow (:math:`F = 2PR/(P+R)`, 0 when undefined).

The structural Hamming distance (SHD) is the number of pairs counted as false positives or false negatives, i.e. the
number of additions, removals and changes of orientation that turn one graph into the other
:cite:`tsamardinos2006max`. The structural intervention distance (SID) :cite:`peters2015structural` counts, between
two DAGs, the ordered pairs :math:`(i, j)` for which adjusting on the parents of :math:`i` in the learned DAG does not
give the effect of an intervention on :math:`i` on :math:`j` in the reference DAG; it is asymmetric, and 0 when the
learned DAG is the reference with more arcs.
