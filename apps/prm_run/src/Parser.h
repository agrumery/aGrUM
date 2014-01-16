

#ifndef PRM_RUN_COCO_PARSER_H__
#define PRM_RUN_COCO_PARSER_H__



#include <agrum/core/exceptions.h>
#include <agrum/core/timer.h>
#include <agrum/BN/inference/BayesNetInference.h>
#include <agrum/BN/inference/variableElimination.h>
#include <agrum/BN/inference/VEWithBB.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/BayesNetInference.h>
#include <agrum/BN/inference/BayesNetInference.h>
#include <agrum/PRM/PRMInference.h>
#include <agrum/PRM/SVE.h>
#include <agrum/PRM/SVED.h>
#include <agrum/PRM/groundedInference.h>
#include <agrum/PRM/skool/SkoolReader.h>

#include "Scanner.h"

using namespace gum;



class Errors {
  public:
    // number of errors detected
    int count;
    Errors();
    void SynErr ( std::string file, int line, int n );
    void Warning ( std::string file, int line, const wchar_t* s );
    void Error ( std::string file, int line, const wchar_t* s );
}; // Errors

class Parser {
  private:

    enum {
      _EOF = 0,
      _integer = 1,
      _float = 2,
      _word = 3,
      _eol = 4,
      _package = 5,
      _import = 6,
      _request = 7,
      _query = 8,
      _unobserve = 9,
      _engine = 10,
      _grd_engine = 11,
    };
    int maxT;


    Token* dummyToken;
    int errDist;
    int minErrDist;

    void SynErr ( int n );
    void Get();
    void Expect ( int n );
    bool StartOf ( int s );
    void ExpectWeak ( int n, int follow );
    bool WeakSeparator ( int n, int syFol, int repFol );

    bool __syntax_flag;

  public:
    Scanner* scanner;
    Errors*  errors;

    Token* t;  // last recognized token
    Token* la; // lookahead token

    ;
//=====================


    Parser ( Scanner* scanner, std::ostream& out = std::cout );
    ~Parser();
    void warning ( const std::string& s );
    void error ( const std::string& s );

    void prm_run();
    void RequestBloc();
    void Observe();
    void Unobserve();
    void Query();
    void SetEngine();
    void SetGrdEngine();


    void Parse();

    // My stuff
    gum::prm::skool::SkoolReader reader;
    std::string package;
    std::string file_name;
    std::string query_name;
    std::string engine;
    std::string bn_engine;
    gum::prm::PRMInference* inf;
    bool verbose;
    std::ostream& log;
    // We need this when using grounded inference
    gum::BayesNetInference<gum::prm::prm_float>* bn;

    void import ( std::string import_name );
    std::string findSystemName ( const std::string& s );
    std::string findInstanceName ( const std::string& s, const std::string& sys );
    std::string findAttributeName ( const std::string& s, const gum::prm::Instance& instance );
    void addObservation ( const std::string& left_val, const std::string& right_val );
    void removeObervation ( const std::string name );
    void generateInfEngine ( const gum::prm::System& sys );
    void query ( const std::string& name );
    bool isInSyntaxMode() const;
    void setSyntaxMode ( bool f );
}; // end Parser



#endif // !defined(SKOOL_COCO_PARSER_H__)

