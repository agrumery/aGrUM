A PAG (partial ancestral graph) represents a Markov equivalence class of maximal ancestral graphs (`pyagrum.MAG`): it
has their common skeleton, and a mark of the PAG is an arrowhead or a tail when this mark is in all the MAGs of the
class, a circle otherwise :cite:`zhang2008completeness`. Its edges are thus :math:`\to` (definitely directed),
:math:`\leftrightarrow` (a latent confounder), :math:`-` (selection bias), :math:`\circ\!\!\to` (possibly directed),
:math:`\circ\!\!-\!\!\circ` (completely uncertain) and :math:`-\!\!\circ`.

A PAG is the output of the FCI algorithm (`pyagrum.BNLearner.learnPAG`): the skeleton and the unshielded colliders come
from independence tests, then the orientation rules R1-R10 of :cite:`zhang2008completeness` are applied; these rules are
complete, i.e. every circle left by them is a tail in some MAG of the class and an arrowhead in another one.

Conventions of pyAgrum: a PAG is a marked graph (`pyagrum.MarkedGraph`) whose invariants are not checked, since FCI on
finite data may give a graph that represents no class. A PAG has no separation of its own: the MAGs of its class are
Markov equivalent, so the m-separation of the representative MAG given by `pyagrum.MAG` (``fromPAG``) is the one of
all of them, when the PAG is valid.
