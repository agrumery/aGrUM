Problèmes de type
-----------------

CredalNet.credalNet_currentCpt (pb de type de retour)

CredalNet.credalNet_srcCPt (pb de type de retour)

CredalNet.get_binaryCPT_max (pb de type de retour)

CredalNet.get_binaryCPT_max (pb de type de retour)

CredalNet.setCPT (pb de type de param)

CredalNet.setCPTs (pb de type de param)

ShaferShenoyLIMIDInference.insertEvidence (pb type de param ?)

ShaferShenoyLIMIDInference.displayStrongJunctionTree (pb type de param ?)

LazyPropagation.setTriangulation prend en paramètre un objet Triangulation

BayesNet.variableNodeMap renvoie un objet non wrappé de type gum::VariableNodeMap

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

%feature("docstring") gum::classname::hasHardEvidence devrait pouvoir prendre un id en param.

Autres problèmes
----------------

gum::PRMexplorer::aggType ?

gum.CredalNet.dynamicExceptations nécessaire pour faire fonctionner dynamicExpMax/Min$

On n'arrive pas à documenter MarkovBlanket et EssentialGraph (alors qu'autodoc le fait)

Il faudrait elever les versions de addChanceNode et addUtilityNode qui prennent en paramètres des MultiDimImplementation