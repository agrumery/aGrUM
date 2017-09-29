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
bool __compareInference(const gum::BayesNet< GUM_SCALAR >&       bn,
                        gum::LazyPropagation< GUM_SCALAR >&      lazy,
                        gum::ApproximateInference< GUM_SCALAR >& inf,
                        double                                   errmax = 5e-2) {

  GUM_SCALAR  err = static_cast< GUM_SCALAR >(0);
  std::string argstr = "";

  for (const auto& node : bn.nodes()) {

    if (!inf.BN().dag().exists(node)) continue;

    GUM_SCALAR e;
    try {

      e = lazy.posterior(node).KL(inf.posterior(node));

    } catch (gum::FatalError) {
      // 0 in a proba
      try {
        // we cannot use KL, we use quadratic error
        e = (inf.posterior(node) - lazy.posterior(node)).sq().sum();
      } catch (gum::FatalError) {
        e = std::numeric_limits< GUM_SCALAR >::infinity();
      }

    }

    catch (gum::NotFound e) {
      continue;
    }

    if (e > err) {
      err = e;
      argstr =
        bn.variable(node).name() + " (err=" + std::to_string(err) + ") : \n";
      argstr += "  lazy : " + lazy.posterior(node).toString() + "\n";
      argstr += "  inf : " + inf.posterior(node).toString() + " \n";
    }
  }
  /*if (err > errmax) {
    GUM_TRACE(argstr);
    GUM_TRACE(inf.messageApproximationScheme());
  }*/
  return err <= errmax;
}

#endif  // AGRUM_AGRUMAPPROXIMATIONUTILS_H_H
