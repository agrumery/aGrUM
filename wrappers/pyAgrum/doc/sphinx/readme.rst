Read me/To do
*************

Read me
-------

The following command is used in order to check the completion of the documentation :

``python act guideline``

**OPTIONS**
	-\-correction to display the undocumented methods

Lists of methods and classes to skip are defined in the missingDocs python script (in acttools).

To do
-----

Type issues
===========

CredalNet.credalNet_currentCpt (issue with return type)

CredalNet.credalNet_srcCPt (issue with return type)

CredalNet.get_CPT_max (issue with return type)

CredalNet.get_CPT_min (issue with return type)

CredalNet.setCPT (issue with parameter type)

CredalNet.setCPTs (issue with parameter type)

InfluenceDiagramInference.displayStrongJunctionTree (issue with parameter type)

LazyPropagation.setTriangulation (issue with parameter type, Triangulation is not a wrapped object)

BayesNet.variableNodeMap (issue with return type, VariableNodeMap is not a wrapped object)

Exception issues
================

RangeVar.setMinVal(new_minval) (resp. setMaxVal(new_maxval)) should raise an exception if new_minval>maxval (resp. new_maxval<minval)

Potential.add should raise an exception when a DiscretizedVariable is added without any tick.

Methods to add
==============

- gum::LabelizedVariable::addLabels

- gum::DiscretizedVariable::addTicks

Methods to overload
===================

hasHardEvidence with a node id in parameter (instead of a node name)

Other issues
============

gum.CredalNet.dynamicExceptations needed in order to make dynamicExpMax and dynamicExpMin work.

Can't document MarkovBlanket and EssentialGraph for unknown reason.

Version of addChanceNode and addUtilityNode with a multiDimImplementation as parameter should be disabled as the object isn't wrapped.