

#if !defined(COCO_PARSER_H__)
#define COCO_PARSER_H__

#include <agrum/core/cast_unicode.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/BayesNetFactory.h>

#undef TRY
#define  TRY(inst) try { inst; } catch (gum::Exception& e) { SemErr(e.getType());}

#include <iostream>
#include <string>
#include <fstream>
#include "Scanner.h"

namespace gum {
namespace BIF {


class Errors {
	class Storing {
    private:
			std::vector<bool> is_errors;
			std::vector<std::wstring> filenames;
			std::vector<std::wstring> msgs;
			std::vector<unsigned int> cols;
			std::vector<unsigned int> lines;
	public:
		Storing() {};

		void add(const bool is_error,const std::wstring& filename,const std::wstring& msg,int line, int col) {
			is_errors.push_back(is_error);
			filenames.push_back(filename);
			msgs.push_back(msg);
			lines.push_back(line);
			cols.push_back(col);
		}
		const std::wstring filename(unsigned int i) {
			return (i<filenames.size())?filenames[i]:L"No filename";
		};
		const std::wstring msg(unsigned int i) {
			return (i<msgs.size())?msgs[i]:L"No error";
		};
		unsigned int line(unsigned int i) {
			return (i<lines.size())?lines[i]:-1;
		}
		unsigned int col(unsigned int i) {
			return (i<cols.size())?cols[i]:-1;
		}
		bool is_error(unsigned int i){
			return (i<is_errors.size())?is_errors[i]:-1;
		}
	};

private:
	mutable Storing storer;

public:
	int error_count;			// number of errors detected
	int warning_count;
  

	Errors();
	void SynErr(const std::wstring& filename,int line, int col, int n);
	void Error(const std::wstring& filename,int line, int col, const wchar_t *s);
	void Warning(const std::wstring& filename,int line, int col, const wchar_t *s);
	void Warning(const std::wstring& filename,const wchar_t *s);
	void Exception(const std::wstring& filename,const wchar_t *s);

	int count(void) const {return error_count+warning_count;}

  void add_error(const bool is_error,const std::wstring& filename,int lig,int col,const std::wstring& s) const;
  
	const std::wstring filename(int i) const {
		return storer.filename(i);
	};
	const std::wstring msg(int i) const {
		return storer.msg(i);
	};
	int line(int i) const {
		return storer.line(i);
	}
	int col(int i) const {
		return storer.col(i);
	}
	bool is_error(int i) const {
		return storer.is_error(i);
	}

	void showElegantErrors() {
		int nb_err=0;
		int no_line=1;
		int num_msg=0;
		std::ifstream ifs(narrow(filename(num_msg)).c_str());
		std::string temp;

		while( getline( ifs, temp ) ) {
			if (nb_err>error_count) break;
			while (no_line==line(num_msg)) {
				if (is_error(num_msg)) {
					std::cerr<<narrow(filename(num_msg))<<":"<<line(num_msg)<<std::endl;
					std::cerr<<temp<<std::endl;
					std::cerr<<std::string(col(num_msg)-1,' ')<<"^"<<" "<<narrow(msg(num_msg))<<std::endl<<std::endl;
					nb_err++;
				}
				num_msg++;
			}
			no_line++;
		}
	}
	void showElegantErrorsAndWarnings() {
		int nb_err=0;
		int no_line=1;
		int num_msg=0;
		std::ifstream ifs(narrow(filename(num_msg)).c_str());
		std::string temp;

		while( getline( ifs, temp ) ) {
			if (nb_err>error_count+warning_count) break;
			while (no_line==line(num_msg)) {
				std::cerr<<narrow(filename(num_msg))<<":"<<line(num_msg)<<std::endl;
				std::cerr<<temp<<std::endl;
				std::cerr<<std::string(col(num_msg)-1,' ')<<"^"<<" "<<narrow(msg(num_msg))<<std::endl<<std::endl;
				nb_err++;
				num_msg++;
			}
			no_line++;
		}
	}

	void showSyntheticResults() {
		std::cerr<<"Errors : "<<error_count<<std::endl;
		std::cerr<<"Warnings : "<<warning_count<<std::endl;
	}
}; // Errors

class Parser {
private:
	enum {
		_EOF=0,
		_ident=1,
		_integer=2,
		_number=3,
		_string=4,
		_largestring=5
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

gum::AbstractBayesNetFactory* __factory;

void setFactory(gum::AbstractBayesNetFactory* f) {
  __factory=f;
}

gum::AbstractBayesNetFactory& factory(void) {
  if (__factory) return *__factory;
  GUM_ERROR(gum::OperationNotAllowed,"Please set a factory for scanning BIF file...");
}

void SemErr(std::string s) {
  SemErr(widen(s).c_str());
}

void Warning(std::string s) {
  Warning(widen("Warning : "+s).c_str());
}

void __checkSizeOfProbabilityAssignation(const std::vector<float>&v,const std::string& var) {
  gum::Size s=(gum::Size)0;
  TRY(s=factory().varInBN(factory().variableId(var)).domainSize());
  if (v.size()<s)
    Warning("Not enough data in probability assignation for node "+var);
  if (v.size()>s)
    Warning("Too many data in probability assignation for node "+var);
}



//=====================


	Parser(Scanner *scanner);
	~Parser();
	void SemErr(const wchar_t* msg);
	void Warning(const wchar_t* msg);

	void BIF();
	void NETWORK();
	void VARIABLE();
	void PROBA();
	void IDENT(std::string& name);
	void STRING(std::string& str);
	void PROPERTY();
	void LABELIZE_VAR(int& nbrMod);
	void NBR(int& val);
	void MODALITY_LIST();
	void IDENT_OR_INTEGER(std::string& name);
	void LISTE_PARENTS(std::vector<std::string>& parents );
	void RAW_PROBA(std::vector<float>& v );
	void FACTORIZED_PROBA(std::string& var,const std::vector<std::string>& parents );
	void LISTE_FLOAT(std::vector<float>& v );
	void ASSIGNATION(const std::string& var,const std::vector<std::string>& parents,bool is_first );
	void LISTE_LABELS(const std::vector<std::string>& parents,std::vector<std::string>& labels, unsigned int num_label );
	void FLOAT(float& val);

	void Parse();

}; // end Parser

} // namespace
} // namespace


#endif // !defined(COCO_PARSER_H__)

