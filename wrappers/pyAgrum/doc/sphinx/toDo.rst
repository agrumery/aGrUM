Problèmes de type dans les signatures
-------------------------------------

gum::Idx -> int
gum::Size -> int
gum::NodeSet -> set

Problèmes d'exceptions
----------------------

RangeVar.setMinVal(new_minval) (resp. setMaxVal(new_maxval)) ne renvoie pas d'erreur si new_minval>maxval (resp. new_maxval<minval)

Problèmes avec doc de :
-----------------------
addLabel
addTick
%ignore gum::Potential::populate
%ignore gum::DAGmodel::property;
%ignore gum::DAGmodel::propertyWithDefault;
%ignore gum::DAGmodel::setProperty;

Utiliser un ignore, en plus de ne pas documenter la fonction, fait qu'elle n'est pas implémentée, ce qui pose problème dans les jeux de test

FillWith = populate ? Si oui il faut changer les tests.
Supprimer property et setProperty ?