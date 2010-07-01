

#if !defined(SKOOL_COCO_PARSER_H__)
#define SKOOL_COCO_PARSER_H__

#include <string>
#include <sstream>
#include <algorithm>
#include <fstream>

namespace std {
  #include<dirent.h>
}

#include <agrum/prm/PRMFactory.h>

#define  TRY(inst) try { inst; } catch (gum::Exception& e) { SemErr(e.getContent());}


#include <agrum/prm/skool/Scanner.h>
using namespace std;

namespace gum {
namespace prm {
namespace skool {


class Errors {
  public:
    int count;			// number of errors detected

    Errors();
    void SynErr(std::string file, int line, int n);
    void Error(std::string file, int line, const wchar_t *s);
}; // Errors

class Parser {
  private:
    	enum {
		_EOF=0,
		_integer=1,
		_float=2,
		_word=3,
		_eol=4,
		_dot=5,
		_comma=6,
		_colon=7,
		_semicolon=8,
		_type=9,
		_class=10,
		_interface=11,
		_extends=12,
		_system=13,
		_dependson=14,
		_default=15,
		_implements=16,
		_LEFT_CAST=17,
		_RIGHT_CAST=18
	};
	int maxT;

      Token *dummyToken;
    int errDist;
    int minErrDist;

    void SynErr(int n);
    void Get();
    void Expect(int n);
    bool StartOf(int s);
    void ExpectWeak(int n, int follow);
    bool WeakSeparator(int n, int syFol, int repFol);

  public:
    Scanner *scanner;
    Errors  *errors;

    Token *t;   // last recognized token
    Token *la//; he adds a ;   // lookahead token

    ;

gum::prm::PRMFactory* __factory;
std::vector<std::string> __class_path;
gum::Set<std::string> __imports;
std::string __file_name;

void SemErr(std::string s) {
  SemErr(widen(s).c_str());
}

void setFactory(gum::prm::PRMFactory* f) {
  __factory = f;
}

void setClassPath(const std::vector<std::string>& class_path) {
  __class_path = class_path;
}

void setImports(const gum::Set<std::string>& imports) {
  __imports = imports;
}

const gum::Set<std::string>& getImports() const {
  return __imports;
}

void addImports(const gum::Set<std::string>& imports) {
  for (gum::Set<std::string>::iterator iter = imports.begin(); iter != imports.end(); ++iter) {
    if (not __imports.exists(*iter)) {
      __imports.insert(*iter);
    }
  }
}

void setFileName(const std::string& file) {
  __file_name = file;
}

void import(std::string s) {
  if (not __imports.exists(s)) {
    __imports.insert(s);
    std::string sep = "/";
    std::string ext = ".skool";
    std::string import;
    std::ifstream file_test;
    std::replace(s.begin(), s.end(), '.', '/');
    bool found = false;
    for (std::vector<std::string>::iterator iter = __class_path.begin(); iter != __class_path.end(); ++iter) {
      import = (*iter) + sep + s + ext;
      file_test.open(import.c_str());
      if (file_test.is_open()) {
        file_test.close();
        found = true;
        Scanner s(import);
        Parser p(&s);
        p.setFactory(__factory);
        p.setClassPath(__class_path);
        p.setImports(__imports);
        p.setFileName(import);
        p.Parse();
        addImports(p.getImports());
        break;
      }
    }
    if (not found) {
      GUM_ERROR(gum::NotFound, "import not found");
    }
  }
}

gum::prm::PRMFactory& factory() {
  if (__factory) {
    return *__factory;
  }
  GUM_ERROR(OperationNotAllowed,"Please set a factory for scanning BIF file...");
}

bool isClass(std::string type) {
  std::string dot = ".";
  return factory().prm()->isClass(type) or factory().prm()->isClass(factory().currentPackage() + dot + type);
}

bool isClassOrInterface(std::string type) {
  std::string dot = ".";
  return factory().prm()->isClass(type) or factory().prm()->isClass(factory().currentPackage() + dot + type) or
         factory().prm()->isInterface(type) or factory().prm()->isInterface(factory().currentPackage() + dot + type) ;
}

void setRefOrParam(std::string s, std::string r) {
  size_t dot = s.find_first_of('.');
  if (dot != std::string::npos) {
    std::string instance = s.substr(0, dot);
    std::string member = s.substr(dot+1);
    gum::prm::System& sys = static_cast<gum::prm::System&>(*(factory().getCurrent()));
    gum::prm::ClassElementContainer* c = 0;
    if (sys.isInstance(instance)) {
      c = &(sys.get(instance).type());
    } else if (sys.isArray(instance)) {
      c = &(sys.getArrayType(instance));
    } else {
      GUM_ERROR(gum::NotFound, "unknown instance in system");
    }
    if (not c->exists(member)) {
      GUM_ERROR(gum::NotFound, "unknown member in instance");
    }
    if (c->obj_type() == gum::prm::PRMObject::prm_interface) {
      factory().setReferenceSlot(instance, member, r);
    } else {
      switch (c->get(member).elt_type()) {
        case gum::prm::ClassElement::prm_refslot:
          {
            factory().setReferenceSlot(instance, member, r);
            break;
          }
        case gum::prm::ClassElement::prm_attribute:
          {
            if (static_cast<gum::prm::Class*>(c)->isParameter(c->get(member).id())) {
              factory().setParameter(instance, member, r);
              break;
            } // If it is not a parameter goto to the default case
          }
        default: {
                   GUM_ERROR(WrongClassElement, "wrong ClassElement for this operation");
                 }
      }
    }
  } else {
    GUM_ERROR(gum::SyntaxError, "invalid right value");
  }
}

void setReferenceSlot(std::string s, std::string r) {
  size_t dot = s.find_first_of('.');
  if (dot != std::string::npos) {
    std::string instance = s.substr(0, dot);
    std::string member = s.substr(dot+1);
    gum::prm::System& sys = static_cast<gum::prm::System&>(*(factory().getCurrent()));
    gum::prm::ClassElementContainer* c = 0;
    if (sys.isInstance(instance)) {
      c = &(sys.get(instance).type());
    } else if (sys.isArray(instance)) {
      c = &(sys.getArrayType(instance));
    } else {
      GUM_ERROR(NotFound, "unknown instance in system");
    }
    if (not c->exists(member)) {
      GUM_ERROR(NotFound, "unknown member in instance");
    }
    factory().setReferenceSlot(instance, member, r);
  } else {
    GUM_ERROR(gum::SyntaxError, "invalid right value");
  }
}

//=====================

    Parser(Scanner *scanner);
    ~Parser();
    void SemErr(const wchar_t* msg);

    	void Skool();
	void Package();
	void Import();
	void Unit();
	void Ident(std::string& s);
	void Type();
	void Interface();
	void Class();
	void System();
	void Label(std::string& s);
	void Reference(std::string type, std::string name);
	void RefOrParam(std::string type, std::string name);
	void Attribute(std::string type, std::string name);
	void Aggregate(std::string type, std::string name);
	void Functions(std::string type, std::string name);
	void Parameter(std::string type, std::string name);
	void CastIdent(std::string& s);
	void Number(float& val);
	void CPTRule();
	void CPTRuleValue(std::string& s );
	void AggChains(std::vector<std::string>& chains );
	void AggLabels(std::vector<std::string>& labels );
	void NumberList(std::vector<float>& numbers );
	void ArrayDecl(std::string l1);

      void Parse();

}; // end Parser

} // namespace
} // namespace
} // namespace

#endif // !defined(SKOOL_COCO_PARSER_H__)

