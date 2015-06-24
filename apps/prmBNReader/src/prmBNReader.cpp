#include <iostream>
#include <string>
#include <regex>

#include <agrum/BN/io/BNReader.h>
#include <agrum/core/errorsContainer.h>

#include <agrum/PRM/PRM.h>
#include <agrum/PRM/o3prm/O3prmReader.h>

namespace gum {
  template <typename GUM_SCALAR> class O3prmBNReader : public BNReader<GUM_SCALAR> {
    public:
    O3prmBNReader(BayesNet<GUM_SCALAR> *bn, const std::string &filename)
        : BNReader<double>(bn, filename) {
      __bn = bn;
      __filename = filename;
    };

    ~O3prmBNReader(){};

    /// parse.
    /// @return the number of detected errors
    /// @throws IOError if file not exists
    int proceed(void) {
      prm::o3prm::O3prmReader<double> reader;
      reader.readFile(__filename);
      gum::prm::PRM<double> *prm = reader.prm();
      __errors = reader.errorsContainer();

      if (errors() == 0) {
        if (prm->isSystem("Asia")) {
          __generateBN(prm->system("Asia"));
        } else {
          if (prm->isClass("Asia")) {
            ParseError warn(
                false,
                "No instance Asia found but class found. Generating instance.",
                __filename, 0);
            __errors.add(warn);
            gum::prm::System<double> s("S_Asia");
            auto i = new gum::prm::Instance<double>("a", prm->getClass("Asia"));
            s.add(i);
            __generateBN(s);
          } else {
            ParseError err(true, "Neither instance nor class Asia.", __filename, 0);
            __errors.add(err);
          }
        }
      }
    };

    /// @{
    /// publishing Errors API

    /// # of errors
    Size errors() { return __errors.error_count; }
    /// # of errors
    Size warnings() { return __errors.warning_count; }

    /// line of ith error or warning
    unsigned int errLine(unsigned int i) { return __errors.error(i).line; }
    /// col of ith error or warning
    unsigned int errCol(unsigned int i) { return __errors.error(i).column; }
    /// type of ith error or warning
    bool errIsError(unsigned int i) { return __errors.error(i).is_error; }
    /// message of ith error or warning
    std::string errMsg(unsigned int i) { return __errors.error(i).msg; }

    /// send on std::cerr the list of errors
    void showElegantErrors(std::ostream &o = std::cerr) {
      __errors.elegantErrors(o);
    }

    /// send on std::cerr the list of errors or warnings
    void showElegantErrorsAndWarnings(std::ostream &o = std::cerr) {
      __errors.elegantErrorsAndWarnings(o);
    }

    /// send on std::cerr the number of errors and the number of warnings
    void showErrorCounts(std::ostream &o = std::cerr) {
      __errors.syntheticResults(o);
    }
    /// @}

    private:
    std::string __filename;
    BayesNet<GUM_SCALAR> *__bn;
    ErrorsContainer __errors;

    void __generateBN(prm::System<GUM_SCALAR> &system) {
      system.instantiate();
      BayesNetFactory<double> factory(__bn);
      system.groundedBN(factory);
    }
  };

} // gum

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
      std::cout << "#arcs : " << bn.sizeArcs() << std::endl << std::endl << std::endl
                << std::endl << std::endl << std::endl;
    }
  } catch (gum::Exception &e) {
    std::cout << " OK " << std::endl;
    std::cout << "========" << std::endl;
    GUM_SHOWERROR(e);
    std::cout << std::endl << std::endl;
  }
}

std::string getVariableName(std::string path,std::string type,std::string name) {
  return name;
}
void showBN(std::string filename) {
  gum::Set<std::string> names;

  try {
    gum::BayesNet<double> bn;
    gum::O3prmBNReader<double> reader(&bn, filename);
    std::cout<<"proceeding ..."<<std::endl;
    reader.proceed();
    std::cout<<"done ..."<<std::endl;
    reader.showElegantErrorsAndWarnings();

    std::regex re("([^\\(]+)(\\([^\\)]+\\))(.*)");
    std::smatch match;
    for (auto node : bn.nodes()) {
      // keeping the complete name in description
      bn.variable(node).setDescription(bn.variable(node).name());

      // trying to simplify the name
      if (std::regex_search(bn.variable(node).name(), match, re)) {
        if (match.size() != 4) {
          std::cout<<"ERROR : "<<bn.variable(node).name()<<std::endl;
        } else {
          std::string newNameRadical=getVariableName(match.str(1),match.str(2),match.str(3));
          std::string newName=newNameRadical;
          std::cout<<"  + trying to add "<<newName<<std::endl;

          // forcing newName to be unique
          int num=0;
          while (names.contains(newName)) {
            newName=newNameRadical+std::to_string(++num);
          }
          std::cout<<"      => adding "<<newName<<std::endl;
          names.insert(newName);

          bn.changeVariableName(node,newName);
        }

      }
      std::cout << std::endl;
    }

    std::cout << bn.toDot() << std::endl;
  } catch (gum::Exception &e) {
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

  //showBN("../data/AsiaWithSystem.o3prm");
  showBN("../data/AsiaClassAndSystemWithTwoClasses.o3prm");
}


