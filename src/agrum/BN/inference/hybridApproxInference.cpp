#include <agrum/BN/inference/hybridApproxInference.h>

template class gum::HybridApproxInference<float, gum::WeightedSampling>;
template class gum::HybridApproxInference<double, gum::WeightedSampling>;

template class gum::HybridApproxInference<float, gum::ImportanceSampling>;
template class gum::HybridApproxInference<double, gum::ImportanceSampling>;

template class gum::HybridApproxInference<float, gum::GibbsSampling>;
template class gum::HybridApproxInference<double, gum::GibbsSampling>;

template class gum::HybridApproxInference<float, gum::MonteCarloSampling>;
template class gum::HybridApproxInference<double, gum::MonteCarloSampling>;
