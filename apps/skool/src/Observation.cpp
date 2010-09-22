#include "Observation.h"
#include <iostream>

Observation::Observation (string node_path):Action (node_path) {
  type = tObservation;
}

Observation::Observation () {
}

Observation::~Observation () {
}
