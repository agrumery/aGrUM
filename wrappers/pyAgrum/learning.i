
%extend gum::learning::BNLearner {
  gum::BayesNet<double> learnBN(const std::string filename) {
    return self->learnBN<double>(filename);
  }
}
