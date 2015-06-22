#include <iostream>

#include <agrum/BN/io/BNReader.h>
#include <agrum/core/errorsContainer.h>

#include <agrum/PRM/PRM.h>
#include <agrum/PRM/o3prm/O3prmReader.h>

namespace gum {
template <typename GUM_SCALAR> class O3prmBNReader : public BNReader<GUM_SCALAR> {
  public:
  O3prmBNReader(BayesNet<GUM_SCALAR> *bn, const std::string &filename):BNReader<double>(bn,filename) {
    __bn=bn;
    __filename=filename;
  };

  ~O3prmBNReader() {};

  /// parse.
  /// @return the number of detected errors
  /// @throws IOError if file not exists
  int proceed(void) {
    prm::o3prm::O3prmReader<double> reader;
    reader.readFile(__filename);
    gum::prm::PRM<double> *prm = reader.prm();
    __errors=reader.errorsContainer();

    if (errors()==0) {
      if (prm->isSystem("Asia")) {
        __generateBN(prm->system("Asia"));
      } else {
        if (prm->isClass("Asia")) {
          ParseError warn(false,"No instance Asia found but class found. Generating instance.",__filename,-1);
          __errors.add(warn);
          gum::prm::System<double> s("S_Asia");
          gum::prm::Instance<double> i("a",prm->getClass("Asia"));
          __generateBN(s);
        } else {
          ParseError err(true,"Neither instance nor class Asia.",__filename,-1);
          __errors.add(err);
        }
      }
    }
  };

  /// @{
  /// publishing Errors API

  /// # of errors
  Size errors() {
    return __errors.error_count;
  }
  /// # of errors
  Size warnings() {
    return __errors.warning_count;
  }

  /// line of ith error or warning
  unsigned int errLine(unsigned int i) {
    return __errors.error(i).line;
  }
  /// col of ith error or warning
  unsigned int errCol(unsigned int i) {
    return __errors.error(i).colomn;
  }
  /// type of ith error or warning
  bool errIsError(unsigned int i) {
    return __errors.error(i).is_error;
  }
  /// message of ith error or warning
  std::string errMsg(unsigned int i) {
    return __errors.error(i).msg;
  }

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
  BayesNet<GUM_SCALAR>* __bn;
  ErrorsContainer __errors;

  void __generateBN(prm::System<GUM_SCALAR>& system) {
    system.instantiate();
    BayesNetFactory<double> factory(__bn);
    system.groundedBN ( factory );
  }
};

} // gum

void testWith(std::string filename){
  std::cout<<"========"<<std::endl;
  std::cout<<"=(test)= "<<filename<< " => ";

  gum::BayesNet<double> bn;
  gum::O3prmBNReader<double> reader(&bn,filename);

  try {
    reader.proceed();
  } catch (gum::Exception& e) {
    GUM_SHOWERROR(e);
  }
  if (bn.size()==8) {
    std::cout<<"OK"<<std::endl;
    std::cout<<"========"<<std::endl<<std::endl;
  } else {
    std::cout<<"Not OK : "<<std::endl;
    std::cout<<"========"<<std::endl<<std::endl;

    reader.showElegantErrorsAndWarnings();
    std::cout<<"---------begin TO DOT------------"<<std::endl;
    std::cout<<bn.toDot()<<std::endl;
    std::cout<<"---------end   TO DOT------------"<<std::endl;
    std::cout<<"#nodes : "<<bn.size()<<std::endl;
    std::cout<<"#arcs : "<<bn.sizeArcs()<<std::endl<<std::endl<<std::endl<<std::endl<<std::endl<<std::endl;
  }
}

int main(void) {
  testWith("../data/AsiaWithSystem.o3prm");
  testWith("../data/AsiaSystemOnly.o3prm");
  testWith("../data/AsiaClassOnly.o3prm");
  testWith("../data/AsiaClassAndSystemSameName.o3prm");
  testWith("../data/FileNotFound.o3prm");

  std::cout<<" ... End of tests ..."<<std::endl;
}
