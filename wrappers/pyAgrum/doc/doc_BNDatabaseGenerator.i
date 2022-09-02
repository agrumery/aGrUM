%feature("docstring") gum::learning::BNDatabaseGenerator
"
BNDatabaseGenerator is used to easily generate databases from a pyAgrum.BayesNet.

Parameters
----------
bn: pyAgrum.BayesNet
  the Bayesian network used to generate data.
"

%feature("docstring") gum::learning::BNDatabaseGenerator::drawSamples
"
Generate and stock the part of the database compatible with the evidence, returns log2likelihood.

Parameters
----------
nbSamples : int
	the number of samples that will be generated
evs : gum::Instantiation | Dict[int|str,int|str] (optional)
  the evidence that will be observed by the resulting samples. Default is empty.

Warning
-------
nbSamples is not the size of the database but the number of generated samples. It may happen that the evidence is very
rare (or even impossible). In that cas the generated database may have only a few samples (even it may be empty).
"

%feature("docstring") gum::learning::BNDatabaseGenerator::toCSV
"
generates csv representing the generated database.

Parameters
----------
csvFilename: str
  the name of the csv file
useLabels: bool
  whether label or id in the csv file (default true)
append: bool
  append in the file or rewrite the file (default false)
csvSeparator: str
  separator in the csv file (default ',')
"

%feature("docstring") gum::learning::BNDatabaseGenerator::samplesNbRows
"
return the number of rows in the samples
"

%feature("docstring") gum::learning::BNDatabaseGenerator::samplesNbCols
"
return the number of columns in the samples
"
