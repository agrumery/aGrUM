#include <agrum/BN/BayesNet.h>
#include <iostream>

#include <agrum/learning/database/databaseFromCSV.h>
#include <agrum/learning/database/databaseVectInRAM.h>

#include <agrum/learning/database/DBCellTranslator.h>
#include <agrum/learning/database/DBCellTranslators/cellTranslatorCompactIntId.h>
#include <agrum/learning/database/DBRowTranslatorSet.h>

#include <agrum/learning/database/filteredRowGenerators/rowGeneratorIdentity.h>

#include <agrum/learning/scores_and_tests/scoreBDeu.h>

#include <agrum/learning/aprioris/aprioriSmoothing.h>

#include <agrum/learning/constraints/structuralConstraintDAG.h>
#include <agrum/learning/constraints/structuralConstraintDiGraph.h>

#include <agrum/learning/structureUtils/graphChangesGenerator4DiGraph.h>
#include <agrum/learning/structureUtils/graphChangesSelector4DiGraph.h>

#include <agrum/learning/greedyHillClimbing.h>
#include <agrum/learning/paramUtils/paramEstimatorML.h>


int main(int argc, char* argv[]) {
  std::cout << "Simple K-Cross-Validation with aGrUM" << std::endl << std::endl;

  gum::Idx k;
  if (argc < 2) {
    std::cout << "Please type 'example K' for a K-fold cross validation"
              << std::endl;
    std::cout << "...using K=3 by default" << std::endl << std::endl;
    k = 3;
  } else {
    k = atoi(argv[1]);
  }

  std::string                    csvfilename("../asia.csv");
  gum::learning::DatabaseFromCSV database(csvfilename);

  // K-fold Cross Validation Start
  int n = database.content().size();

  std::cout << "  K=" << k << " on " << csvfilename << " (size:" << n << ")"
            << std::endl
            << std::endl;


  // K-fold Cross Validation
  try {
    // Structure Learning
    const int nbCol =
      8;  // <-- has to be changed for each csv file (nb of variables in the csv)

    // will parse the database once
    gum::learning::DBRowTranslatorSet< gum::learning::CellTranslatorCompactIntId >
      translators;
    translators.insertTranslator(0, nbCol);

    gum::learning::FilteredRowGeneratorSet< gum::learning::RowGeneratorIdentity >
      generators;
    generators.insertGenerator();

    auto filter =
      gum::learning::make_DB_row_filter(database, translators, generators);

    std::vector< gum::Size > modalities = filter.modalities();

    gum::learning::AprioriSmoothing<> apriori;

    gum::learning::StructuralConstraintSetStatic<
      gum::learning::StructuralConstraintDAG >
      struct_constraint;

    gum::learning::GraphChangesGenerator4DiGraph< decltype(struct_constraint) >
      op_set(struct_constraint);

    gum::learning::GreedyHillClimbing search;

    int foldSize = database.content().size() / k;

    for (int fold = 0; fold < k; fold++) {
      gum::Idx fold_deb = fold * foldSize;
      gum::Idx fold_end = fold_deb + foldSize - 1;
      std::cout << "+ LEARNING on [" << fold_deb << "," << fold_end << "] : ";

      // LEARNING
      filter.handler().setRange(fold_deb, fold_end);
      gum::learning::ScoreBDeu<>        score(filter, modalities, apriori);
      gum::learning::ParamEstimatorML<> estimator(
        filter, modalities, apriori, score.internalApriori());
      gum::learning::GraphChangesSelector4DiGraph< decltype(score),
                                                   decltype(struct_constraint),
                                                   decltype(op_set) >
                              selector(score, struct_constraint, op_set);
      gum::Timer              timer;
      gum::BayesNet< double > bn = search.learnBN(selector,
                                                  estimator,
                                                  database.variableNames(),
                                                  modalities,
                                                  filter.translatorSet());
      std::cout << timer.step() << "s ";
      std::cout << bn.arcs().size() << " arcs" << std::endl;

      // TESTING
      gum::Instantiation I;

      for (auto& name : filter.variableNames()) {
        I.add(bn.variableFromName(name));
      }

      double LL = 0.0;
      std::cout << "  TESTING on ";

      if (fold_deb != 0) {
        std::cout << "[" << 0 << "," << fold_deb - 1 << "]";
        filter.handler().setRange(0, fold_deb - 1);
        filter.reset();

        while (filter.hasRows()) {
          int i = 0;

          for (auto item : filter.row().row())
            I.chgVal(i++, item);

          LL += bn.log2JointProbability(I);
        }
      }

      if (fold_end + 1 < database.content().size() - 1) {
        if (LL != 0.0) {
          std::cout << " U ";
        }

        std::cout << "[" << fold_end + 1 << "," << database.content().size() - 1
                  << "]";
        filter.handler().setRange(fold_end + 1, database.content().size() - 1);
        filter.reset();

        while (filter.hasRows()) {
          int i = 0;

          for (auto item : filter.row().row())
            I.chgVal(i++, item);

          LL += bn.log2JointProbability(I);
        }
      }

      std::cout << " : LL=" << LL << std::endl << std::endl;
    }
  } catch (const gum::Exception& ex) {
    std::cout << ex.errorContent() << std::endl;
  }
}
