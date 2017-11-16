%define ADD_CNINFERENCE_DOC(classname...)
%feature("docstring") gum::credal::classname::dynamicExpMax
"
Warnings
--------
TBD
"

%feature("docstring") gum::credal::classname::dynamicExpMin
"
Warnings
--------
TBD
"

%feature("docstring") gum::credal::classname::eraseAllEvidence
"
Warnings
--------
TBD
"

%feature("docstring") gum::credal::classname::insertEvidenceFile
"
Warnings
--------
TBD
"

%feature("docstring") gum::credal::classname::insertModalsFile
"
Warnings
--------
TBD
"

%feature("docstring") gum::credal::classname::makeInference
"
Warnings
--------
TBD
"

%feature("docstring") gum::credal::classname::marginalMax
"
Warnings
--------
TBD
"

%feature("docstring") gum::credal::classname::marginalMin
"
Warnings
--------
TBD
"

%feature("docstring") gum::credal::classname::saveInference
"
Warnings
--------
TBD
"

%feature("docstring") gum::credal::classname::setRepetitiveInd
"
Warnings
--------
TBD
"
%enddef

ADD_CNINFERENCE_DOC(CNMonteCarloSampling<double>)
ADD_CNINFERENCE_DOC(CNLoopyPropagation<double>)