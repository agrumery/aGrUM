Problèmes de type
-----------------

gum::Idx -> int
gum::Size -> int
gum::NodeSet -> set

Inférence.BN devrait retourner un BayesNet ?

CredalNet.credalNet_currentCpt (pb de type de retour)
CredalNet.credalNet_srcCPt (pb de type de retour)
CredalNet.get_CPT_max (pb de type de retour)
CredalNet.get_CPT_min (pb de type de retour)
CredalNet.setCPT (pb de type de param)
CredalNet.setCPTs (pb de type de param)
InfluenceDiagram.utility (pb de type de retour)

Problèmes d'exceptions
----------------------

RangeVar.setMinVal(new_minval) (resp. setMaxVal(new_maxval)) ne renvoie pas d'erreur si new_minval>maxval (resp. new_maxval<minval)

Potential.add devrait envoyer une erreur lorsqu'on y passe une DiscretizedVariable sans tick.

Nouvelles fonctions
-------------------

- gum::LabelizedVariable::addTicks
- gum::DiscretizedVariable::addLabels

Fonctions à surcharger
----------------------

%feature("docstring") gum::classname::hasHardEvidence
devrait pouvoir prendre un id en param.

Problème de swig ?
------------------

gum::PRMexplorer::aggType 


gum.CredalNet.dynamicExceptations nécessaire pour faire fonctionner dynamicExpMax/Min