Problèmes de type dans les signatures générées
----------------------------------------------

gum::Idx -> int
gum::Size -> int
gum::NodeSet -> set

Problèmes d'exceptions
----------------------

RangeVar.setMinVal(new_minval) (resp. setMaxVal(new_maxval)) ne renvoie pas d'erreur si new_minval>maxval (resp. new_maxval<minval)

Potential.add devrait envoyer une erreur lorsqu'on y passe une DiscretizedVariable sans tick.

Nouvelles fonctions
-------------------

- gum::LabelizedVariable::addTicks
- gum::DiscretizedVariable::addLabels