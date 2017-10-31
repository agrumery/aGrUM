#include <agrum/BN/inference/loopySamplingInference.h>

template class gum::LoopySamplingInference<float, gum::WeightedSampling>;
template class gum::LoopySamplingInference<double, gum::WeightedSampling>;

template class gum::LoopySamplingInference<float, gum::ImportanceSampling>;
template class gum::LoopySamplingInference<double, gum::ImportanceSampling>;

template class gum::LoopySamplingInference<float, gum::GibbsSampling>;
template class gum::LoopySamplingInference<double, gum::GibbsSampling>;

template class gum::LoopySamplingInference<float, gum::MonteCarloSampling>;
template class gum::LoopySamplingInference<double, gum::MonteCarloSampling>;
