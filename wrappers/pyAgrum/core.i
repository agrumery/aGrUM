%ignore *::diffSet;
 
%ignore gum::Instantiation::setValsFrom;

%pythonprepend gum::List::append %{
        args[0].__disown__() #lets c++ take care of deleting it
%}


%pythonappend gum::List::__getitem__ %{
        val.__fill_distrib__()
%}

