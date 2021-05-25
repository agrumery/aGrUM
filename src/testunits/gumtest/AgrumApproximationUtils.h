#ifndef AGRUM_AGRUMAPPROXIMATIONUTILS_H_H
#define AGRUM_AGRUMAPPROXIMATIONUTILS_H_H


template < typename GUM_SCALAR >
void unsharpen(const gum::BayesNet< GUM_SCALAR >& bn) {
  for (const auto nod : bn.nodes().asNodeSet()) {
    bn.cpt(nod).translate(1);
    bn.cpt(nod).normalizeAsCPT();
  }
}
template < typename GUM_SCALAR >
bool compareInference(const std::string&                       file,
                      int                                      line,
                      const gum::BayesNet< GUM_SCALAR >&       bn,
                      gum::LazyPropagation< GUM_SCALAR >&      lazy,
                      gum::ApproximateInference< GUM_SCALAR >& inf,
                      double                                   errmax = 5e-2) {

  GUM_SCALAR  err = static_cast< GUM_SCALAR >(0);
  std::string argstr = "";

  for (const auto& node : bn.nodes()) {

    if (!inf.BN().dag().exists(node)) continue;
    GUM_SCALAR e = (inf.posterior(node) - lazy.posterior(node)).abs().max();

    if (e > err) {
      err = e;
      argstr =
        bn.variable(node).name() + " (err=" + std::to_string(err) + ") : \n";
      argstr += "  lazy : " + lazy.posterior(node).toString() + "\n";
      argstr += "  inf : " + inf.posterior(node).toString() + " \n";
    }
  }
  /*if (err > errmax) {
     _GUM_PRINT(file, line, argstr);
     _GUM_PRINT(file, line, inf.messageApproximationScheme());
  }*/
  return err <= errmax;
}


#define GUM_APPROX_TEST_BEGIN_ITERATION \
  bool res = false;                     \
  (void)res;                            \
  for (int ii = 0; ii < 10; ii++) {

#define GUM_APPROX_TEST_END_ITERATION(ERRMAX)                        \
  if (compareInference(  __FILE__,   __LINE__, bn, lazy, inf, ERRMAX)) { \
    res = true;                                                      \
    break;                                                           \
  }                                                                  \
  std::cout << "![" <<   __LINE__ << "]" << std::flush;                \
  if (ii == 10) TS_FAIL("even with 10 tries.");                      \
  }
#endif  // AGRUM_AGRUMAPPROXIMATIONUTILS_H_H
