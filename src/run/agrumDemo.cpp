#define GUM_TRACE_ON

#include <iostream>
#include <agrum/BN/BayesNet.h>

#include <agrum/learning/database/databaseVectInRAM.h>
#include <agrum/learning/database/databaseFromCSV.h>

#include <agrum/learning/database/DBCellTranslators/cellTranslatorCompactIntId.h>

#include <agrum/learning/database/filteredRowGenerators/rowGeneratorIdentity.h>

#include <agrum/learning/scores_and_tests/scoreBDeu.h>

#include <agrum/learning/aprioris/aprioriSmoothing.h>

#include <agrum/learning/constraints/structuralConstraintDiGraph.h>
#include <agrum/learning/constraints/structuralConstraintDAG.h>

#include <agrum/learning/structureUtils/graphChangesSelector4DiGraph.h>
#include <agrum/learning/structureUtils/graphChangesGenerator4DiGraph.h>

#include <agrum/learning/paramUtils/paramEstimatorML.h>
#include <agrum/learning/greedyHillClimbing.h>
int main(int argc, char *argv[]) {

  //**SPDLOG not installed** namespace spd = spdlog;

  //**SPDLOG not installed**try {
  // Create console, multithreaded logger
  //**SPDLOG not installed**auto console = spd::stdout_logger_mt("console");
  /**SPDLOG not installed** console->info*/ std::cout
      << ("Bayesian Network Build");

  int k = atoi(argv[2]);

  gum::learning::DatabaseFromCSV database(argv[1]);
  /**SPDLOG not installed** console->info*/ std::cout
      << "Number of rows: " << database.content().size() << std::endl;
  /**SPDLOG not installed** console->info*/ std::cout
      << "Number of variables: " << database.variableNames().size()
      << std::endl;

  // K-fold Cross Validation Start
  int n = database.content().size();

  std::vector<int> indexes(n);
  int i(0);
  std::generate(indexes.begin(), indexes.end(), [&] { return i++; });

  std::random_device rd;
  std::mt19937 g(rd());

  std::shuffle(indexes.begin(), indexes.end(), g);

  // K-fold Cross Validation
  try {
    // Structure Learning
    const int nbCol = 8; // 32 for your case

    // will parse the database once
    auto translators = gum::learning::make_translators(
        gum::learning::Create<gum::learning::CellTranslatorCompactIntId,
                              gum::learning::Col<0>, nbCol>());
    auto generators =
        gum::learning::make_generators(gum::learning::RowGeneratorIdentity());

    auto filter =
        gum::learning::make_DB_row_filter(database, translators, generators);
    std::vector<unsigned int> modalities = filter.modalities();

    gum::learning::AprioriSmoothing<> apriori;

    gum::learning::StructuralConstraintSetStatic<
        gum::learning::StructuralConstraintDAG> struct_constraint;

    gum::learning::GraphChangesGenerator4DiGraph<decltype(struct_constraint)>
        op_set(struct_constraint);

    gum::learning::GreedyHillClimbing search;

    int foldSize = database.content().size() / k;

    for (int fold = 0; fold < k; fold++) {
      unsigned int fold_deb = fold * foldSize;
      unsigned int fold_end = fold_deb + foldSize - 1;
      std::cout << "LEARNING [" << fold_deb << "," << fold_end << "]  ";

      // LEARNING
      filter.handler().setRange(fold_deb, fold_end);
      gum::learning::ScoreBDeu<> score(filter, modalities, apriori);
      gum::learning::ParamEstimatorML<> estimator(filter, modalities, apriori,
                                                  score.internalApriori());
      gum::learning::GraphChangesSelector4DiGraph<
          decltype(score), decltype(struct_constraint), decltype(op_set)>
          selector(score, struct_constraint, op_set);
      gum::Timer timer;
      gum::BayesNet<float> bn =
          search.learnBN(selector, estimator, database.variableNames(),
                         modalities, filter.translatorSet());
      std::cout << timer.step() << "s ";
      std::cout << bn.arcs().size() << " arcs" << std::endl;

      // TESTING
      gum::Instantiation I;

      for (auto &name : filter.variableNames()) {
        I.add(bn.variableFromName(name));
      }

      double LL = 0.0;
      std::cout << "TESTING ";

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

      std::cout << " LL=" << LL << std::endl << std::endl;
    }
  } catch (const gum::Exception &ex) {
    std::cout << ex.errorContent() << std::endl;
  }
}
