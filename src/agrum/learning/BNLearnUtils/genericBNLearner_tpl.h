
namespace gum {

  namespace learning {

    template < typename GUM_SCALAR >
    genericBNLearner::Database::Database(
      const std::string&                 filename,
      const BayesNet< GUM_SCALAR >& bn ) {
      // assign to each column name in the database its position
      genericBNLearner::__checkFileName( filename );
      DBInitializerFromCSV<> initializer ( filename );
      const auto& xvar_names = initializer.variableNames ();
      std::size_t nb_vars = xvar_names.size();
      HashTable<std::string,std::size_t> var_names ( nb_vars );
      for ( std::size_t i = std::size_t(0); i < nb_vars; ++i )
        var_names.insert ( xvar_names[i], i );
       
      // we use the bn to insert the translators into the database table
      try {
        for ( auto node : bn.dag () ) {
          const Variable& var = bn.variable(node);
          __database.insertTranslator ( var, var_names[var.name()] );
        }
      }
      catch ( NotFound& ) {
        GUM_ERROR ( MissingVariableInDatabase,
                    "the database does not contain variable " );
      }
      
      // fill the database
      initializer.fillDatabase ( __database );
      
      // get the domain sizes of the variables
      __modalities = __database.domainSizes ();

      nb_vars = __database.nbVariables ();
      for ( std::size_t i = std::size_t(0); i < nb_vars; ++i )
        __name2nodeId.insert ( __database.variable(i).name(), i );

      
      // create the parser
      __parser = new DBRowGeneratorParser<> ( __database.handler (),
                                              DBRowGeneratorSet<> () );
    }
    

    template < typename GUM_SCALAR >
    genericBNLearner::Database::Database(
      const std::string&             filename,
      Database&                      score_database,
      const  BayesNet< GUM_SCALAR >& bn)
      : __database(genericBNLearner::__readFile(filename,bn)) {
    }


    template < typename GUM_SCALAR >
    BayesNet<GUM_SCALAR> genericBNLearner::Database::__BNVars () const {
      BayesNet<GUM_SCALAR> bn;
      const std::size_t nb_vars = __database.nbVariables ();
      for ( std::size_t i = 0; i < nb_vars; ++i ) {
        const DiscreteVariable& var = dynamic_cast<const DiscreteVariable&>
          ( __database.variable (i) );
        bn.add ( var );
      }
      return bn;
    }
    
        
      
    template < typename GUM_SCALAR >
    genericBNLearner::genericBNLearner(
      const std::string&                 filename,
      const gum::BayesNet< GUM_SCALAR >& bn )
      : __score_database ( filename, bn ) {
      GUM_CONSTRUCTOR(genericBNLearner);
    }

  
  }

}

