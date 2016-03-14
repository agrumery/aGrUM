#include <iostream>

#include <agrum/PRM/o3prm/O3prmBNReader.h>

void testWith(std::string filename, gum::Size nbNode) {
  try {
    std::cout << "========" << std::endl;
    std::cout << "=(test)= " << filename << " => ";

    gum::BayesNet<double> bn;
    gum::O3prmBNReader<double> reader(&bn, filename);
    reader.proceed();
    if (bn.size() == nbNode) {
      std::cout << "OK" << std::endl;
      std::cout << "========" << std::endl << std::endl;
    } else {
      std::cout << "Not OK : " << std::endl;
      std::cout << "========" << std::endl << std::endl;

      reader.showElegantErrorsAndWarnings();
      std::cout << "---------begin TO DOT------------" << std::endl;
      std::cout << bn.toDot() << std::endl;
      std::cout << "---------end   TO DOT------------" << std::endl;
      std::cout << "#nodes : " << bn.size() << std::endl;
      std::cout << "#arcs : " << bn.sizeArcs() << std::endl
                << std::endl
                << std::endl
                << std::endl
                << std::endl
                << std::endl;
    }
  } catch (gum::Exception& e) {
    std::cout << " OK " << std::endl;
    std::cout << "========" << std::endl;
    GUM_SHOWERROR(e);
    std::cout << std::endl << std::endl;
  }
}
void showBN(std::string filename) {
  gum::Set<std::string> names;

  try {
    gum::BayesNet<double> bn;
    gum::O3prmBNReader<double> reader(&bn, filename);
    if (reader.proceed() == 0) {
      std::cout << bn.toDot() << std::endl;
    } else {
      reader.showElegantErrorsAndWarnings();
      std::cout << std::endl
                << " Compiling \"" << filename << "\" failed." << std::endl
                << std::endl;
    }
  } catch (gum::Exception& e) {
    std::cout << "========" << std::endl;
    GUM_SHOWERROR(e);
    std::cout << std::endl << std::endl;
  }
}

int main(void) {
  /*
  testWith("../data/AsiaWithSystem.o3prm", 8);
  testWith("../data/AsiaSystemOnly.o3prm", 8);
  testWith("../data/AsiaClassOnly.o3prm", 8);
  testWith("../data/AsiaClassAndSystemSameName.o3prm", 8);
  testWith("../data/FileNotFound.o3prm", 0);
  std::cout << " ... End of tests ..." << std::endl;
  */

  // showBN("../data/AsiaWithSystem.o3prm");
  showBN("../data/AsiaClassAndSystemWithTwoClasses.o3prm");
}
