#include <agrum/BN/BayesNet.h>
#include <iostream>

#include <agrum/bn.h>

int main(int argc, char* argv[]) {
  std::cout << "Simple K-Cross-Validation with aGrUM" << std::endl << std::endl;

  std::size_t k;
  if (argc < 2) {
    std::cout << "Please type 'example K' for a K-fold cross validation"
              << std::endl;
    std::cout << "...using K=3 by default" << std::endl << std::endl;
    k = 3;
  } else {
    k = atoi(argv[1]);
  }

  std::string                           csvfilename("../asia.csv");
  gum::learning::DBInitializerFromCSV initializer(csvfilename);
  const auto&       var_names = initializer.variableNames();
  const std::size_t nb_vars = var_names.size();

  gum::learning::DBTranslatorSet                translator_set;
  gum::learning::DBTranslator4LabelizedVariable translator;
  for (std::size_t i = 0; i < nb_vars; ++i) {
    translator_set.insertTranslator(translator, i);
  }

  gum::learning::DatabaseTable database(translator_set);
  database.setVariableNames(initializer.variableNames());
  initializer.fillDatabase(database);
  database.reorder();

  
  // K-fold Cross Validation Start
  int n = database.nbRows();

  std::cout << "  K=" << k << " on " << csvfilename << " (size:" << n << ")"
            << std::endl
            << std::endl;


  // K-fold Cross Validation
  try {
    // Structure Learning
    const std::size_t nbCol = database.nbVariables ();

    gum::learning::DBRowGeneratorSet    genset;
    gum::learning::DBRowGeneratorParser parser(database.handler(), genset);
    gum::learning::SmoothingPrior     prior(database);

    gum::learning::StructuralConstraintSetStatic<
      gum::learning::StructuralConstraintDAG >
      struct_constraint;

    gum::learning::GraphChangesGenerator4DiGraph< decltype(struct_constraint) >
      op_set(struct_constraint);

    gum::learning::GreedyHillClimbing search;

    gum::learning::ScoreBIC score(parser, prior);
    gum::learning::ParamEstimatorML estimator(parser, prior,
                                                score.internalPrior());

    
    std::size_t foldSize = database.nbRows() / k;

    for (std::size_t fold = 0; fold < k; fold++) {
      // create the ranges of rows over which we perform the learning
      const std::size_t unfold_deb = fold * foldSize;
      const std::size_t unfold_end = unfold_deb + foldSize;

      std::vector< std::pair< std::size_t, std::size_t > > ranges;
      if ( fold == std::size_t(0) ) {
        ranges.push_back ( std::pair< std::size_t, std::size_t >
                           (unfold_end,n) );
      }
      else {
        ranges.push_back ( std::pair< std::size_t, std::size_t >
                           (std::size_t(0),unfold_deb) );
        
        if (fold != k-1) {
          ranges.push_back ( std::pair< std::size_t, std::size_t >
                             (unfold_end,n) );
        }
      }
      
      std::cout << "+ LEARNING on " << ranges << " : ";

      // LEARNING
      score.setRanges(ranges);
      estimator.setRanges(ranges);
      gum::learning::GraphChangesSelector4DiGraph< decltype(struct_constraint),
                                                   decltype(op_set) >
                              selector(score, struct_constraint, op_set);
      gum::Timer              timer;
      gum::BayesNet< double > bn = search.learnBN<double>(selector, estimator);
      std::cout << timer.step() << "s ";
      std::cout << bn.arcs().size() << " arcs" << std::endl;

      // TESTING
      gum::Instantiation I;

      for (auto& name : database.variableNames()) {
        I.add(bn.variableFromName(name));
      }

      double LL = 0.0;
      std::cout << "  TESTING on "
                << "[" << unfold_deb << "," << unfold_end << ")";

      parser.setRange(unfold_deb, unfold_end);      
      while (parser.hasRows()) {
        const gum::learning::DBRow< gum::learning::DBTranslatedValue >&
          row = parser.row();
        for (std::size_t i = 0; i < nbCol; ++i)
          I.chgVal(i, row[i].discr_val);

        LL += bn.log2JointProbability(I) * row.weight();
      }

      std::cout << " : LL=" << LL << std::endl << std::endl;
    }
  } catch (const gum::Exception& ex) {
    std::cout << ex.errorContent() << std::endl;
  }
}
