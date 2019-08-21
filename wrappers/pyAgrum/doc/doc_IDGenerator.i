%feature("docstring") IDGenerator
"
IDGenerator is used to easily generate influence diagrams.

IDGenerator() -> IDGenerator
    default constructor
"

%feature("docstring") IDGenerator::generate
"
Generate a new influence diagram given the parameters.

Parameters
----------
nbrNodes : int
	the number of node
arcDensity : double
	the density of arc (1 for a complete graph)
chanceNodeDensity : double
	the density of chance node
utilityNodeDensity : double
	the density of utility node
max_modality : int
	the maximum value for modalities

Returns
-------
pyAgrum.InfluenceDiagram
	the generated influence diagram
"