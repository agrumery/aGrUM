Problèmes de type dans les signatures
-------------------------------------

gum::Idx -> int
gum::Size -> int
gum::NodeSet -> set

Problèmes d'exceptions
----------------------

RangeVar.setMinVal(new_minval) (resp. setMaxVal(new_maxval)) ne renvoie pas d'erreur si new_minval>maxval (resp. new_maxval<minval)