#include "Unobservation.h"
#include <iostream>

Unobservation::Unobservation (string node_path):Action (node_path) {
  type = tUnobservation;
}

Unobservation::~Unobservation () {
}
