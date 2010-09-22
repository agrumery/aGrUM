#include "Interrogation.h"
#include <iostream>

Interrogation::Interrogation (string node_path):Action (node_path) {
  type = tInterrogation;
}

Interrogation::~Interrogation () {
}
