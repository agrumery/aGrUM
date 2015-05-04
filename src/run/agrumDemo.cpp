#define GUM_TRACE_ON

#include <iostream>
#include <fstream>
#include <sstream>

#include <agrum/config.h>
#include <agrum/core/exceptions.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/learning/database/CSVParser.h>
#include <agrum/learning/BNLearner.h>

#define xstrfy(s) strfy(s)
#define strfy(x) #x

#define GET_PATH_STR(x) xstrfy(GUM_SRC_PATH) "/testunits/ressources/" x
#define GET_PATH_XSTR(x) xstrfy(GUM_SRC_PATH) "/testunits/ressources/" xstrfy(x)

// namespace gum {
//
//     /**
//      * learnParameters
//      */
//     BayesNet<GUM_SCALAR> learnParameters(std::string filename,const
//     BayesNet<GUM_SCALAR>& source,GUM_SCALAR smooth;
// }// gum

gum::BayesNet<double> buildBN() {
  gum::BayesNet<double> bn;
#define createBoolVar(s)                                                            \
  gum::LabelizedVariable(s, s, 0).addLabel("false").addLabel("true");
  auto s = createBoolVar("smoking?");
  auto l = createBoolVar("lung_cancer?");
  auto b = createBoolVar("bronchitis?");
  auto v = createBoolVar("visit_to_Asia?");
  auto t = createBoolVar("tuberculosis?");
  auto o = createBoolVar("tuberculos_or_cancer?");
  auto d = createBoolVar("dyspnoea?");
#undef createBoolVar

  bn.add(s);
  bn.add(l);
  bn.add(b);
  bn.add(v);
  bn.add(t);
  bn.add(o);
  bn.add(d);

  // uncorrect name is : will it be correctly handled ?f
  // auto p = createBoolVar("ZORBLOBO");
  // bn.add(p);

  return bn;
}

std::vector<std::string> loadNames(std::string filename) {
  std::ifstream in(filename, std::ifstream::in);

  if ((in.rdstate() & std::ifstream::failbit) != 0) {
    GUM_ERROR(gum::IOError, "File " << filename << " not found");
  }

  gum::learning::CSVParser parser(in);
  parser.next();

  return parser.current();
}

gum::BayesNet<double> learnParameters(std::string filename,
                                      const gum::BayesNet<double> &src,
                                      double smoothing = 1.0) {
  auto names = loadNames(GET_PATH_STR("asia.csv"));

  gum::NodeProperty<gum::Sequence<std::string>> modals;

  for (gum::Idx col = 0; col < names.size(); col++) {
    try {
      gum::NodeId graphId = src.idFromName(names[col]);

      modals.insert(col, gum::Sequence<std::string>());

      for (gum::Size i = 0; i < src.variable(graphId).domainSize(); ++i)
        modals[col].insert(src.variable(graphId).label(i));
    } catch (const gum::NotFound &e) {
      // no problem : a colonne which is not in the BN...
    }
  }

  gum::learning::BNLearner learner(filename, modals);

  learner.useScoreLog2Likelihood();
  learner.useAprioriSmoothing();
  return learner.learnParameters<double>(src);
}

int main(int argc, char *argv[]) {
  try {
    auto bn = buildBN();
    auto bn2 = learnParameters(GET_PATH_STR("asia3.csv"), bn, 1.0);
  } catch (const gum::Exception &e) {
    GUM_SHOWERROR(e);
  }
}
