

#if !defined(SKOOL_COCO_PARSER_H__)
#define SKOOL_COCO_PARSER_H__

#include <string>
#include <sstream>

#include <agrum/prm/PRMFactory.h>

#define  TRY(inst) try { inst; } catch (gum::Exception& e) { SemErr(e.getType());}


#include <agrum/prm/skool/Scanner.h>
using namespace std;

namespace gum {
namespace prm {
namespace skool {


class Errors {
public:
	int count;			// number of errors detected

	Errors();
	void SynErr(int line, int col, int n);
	void Error(int line, int col, const wchar_t *s);
	void Warning(int line, int col, const wchar_t *s);
	void Warning(const wchar_t *s);
	void Exception(const wchar_t *s);

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
		_system=13
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

	Token *t;			// last recognized token
	Token *la;			// lookahead token

gum::prm::PRMFactory* __factory;

void setFactory(gum::prm::PRMFactory* f) {
  __factory = f;
}

gum::prm::PRMFactory& factory() {
  if (__factory) {
    return *__factory;
  }
  GUM_ERROR(gum::OperationNotAllowed,"Please set a factory for scanning BIF file...");
}

bool isPath() {
  return (la->kind == _word) and (scanner->Peek()->kind == _dot);
}

bool isBaseType() {
  Token* n1 = scanner->Peek();
  Token* n2 = scanner->Peek();
  return (la->kind == _type) and (n1->kind == _word) and (n2->kind != _extends);
}

bool isBaseClass() {
  Token* n1 = scanner->Peek();
  Token* n2 = scanner->Peek();
  return (la->kind == _class) and (n1->kind == _word) and (n2->kind != _extends);
}

bool isBaseInterface() {
  Token* n1 = scanner->Peek();
  Token* n2 = scanner->Peek();
  return (la->kind == _interface) and (n1->kind == _word) and (n2->kind != _extends);
}

bool isClassOrInterface(std::string type) {
  std::string dot = ".";
  return factory().prm()->isClass(type) or factory().prm()->isClass(factory().currentPackage() + dot + type) or
         factory().prm()->isInterface(type) or factory().prm()->isInterface(factory().currentPackage() + dot + type) ;
}

//=====================


	Parser(Scanner *scanner);
	~Parser();
	void SemErr(const wchar_t* msg);

	void Skool();
	void Header();
	void Unit();
	void Package();
	void Import();
	void Ident(std::string& s);
	void Type();
	void Interface();
	void Class();
	void System();
	void BaseType();
	void SuperType();
	void Label(std::string& s);
	void InterfaceBody();
	void InterfaceStatement();
	void RefOrAttr(std::string& type, std::string& name);
	void Reference(std::string type, std::string name);
	void IdentSet(gum::Set<std::string>& set );
	void ClassBody();
	void ClassStatement();
	void RefOrParam(std::string type, std::string name);
	void Attribute(std::string type, std::string name);
	void Aggregate(std::string type, std::string name);
	void Parameter(std::string type, std::string name);
	void Dependecies();
	void FloatList(std::vector<float>& list );
	void FactorizedCPT();
	void IdentList(std::vector<std::string>& set );
	void CPTRule();
	void LabelList(std::vector<std::string>& list );
	void Function(std::string& name,
std::vector<std::string>& chains,
std::vector<std::string>& params );
	void SlotChain(std::string& s);
	void Number(float& val);
	void SystemDecl();
	void SysLeftValue(std::string& left, std::string& last);
	void SysInstDecl(std::string type);
	void SysArray(std::string array);
	void SysAffect(std::string inst, std::string ref);

	void Parse();

}; // end Parser

} // namespace
} // namespace
} // namespace


#endif // !defined(SKOOL_COCO_PARSER_H__)

