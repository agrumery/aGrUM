

#if !defined(SKOOL_COCO_PARSER_H__)
#define SKOOL_COCO_PARSER_H__

#include <string>

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
		_extends=10
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
	void BaseType();
	void SuperType();
	void Label(std::string& s);
	void Path(std::string& s);

	void Parse();

}; // end Parser

} // namespace
} // namespace
} // namespace


#endif // !defined(SKOOL_COCO_PARSER_H__)

