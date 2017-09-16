#include <agrum/BN/inference/hybridApproxInference.h>

template class gum::HybridApproxInference<float, gum::WeightedApproxInference>;
template class gum::HybridApproxInference<double, gum::WeightedApproxInference>;

template class gum::HybridApproxInference<float, gum::ImportanceApproxInference>;
template class gum::HybridApproxInference<double, gum::ImportanceApproxInference>;

template class gum::HybridApproxInference<float, gum::GibbsApproxInference>;
template class gum::HybridApproxInference<double, gum::GibbsApproxInference>;

template class gum::HybridApproxInference<float, gum::MonteCarloApproxInference>;
template class gum::HybridApproxInference<double, gum::MonteCarloApproxInference>;
