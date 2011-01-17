

#ifndef PRM_RUN_COCO_SCANNER_H__
#define PRM_RUN_COCO_SCANNER_H__

#include <agrum/core/cocoR/common.h>

using namespace gum;



class Token {
  public:
    int kind;     // token kind
    int pos;      // token position in the source text (starting at 0)
    int col;      // token column (starting at 1)
    int line;     // token line (starting at 1)
    wchar_t* val; // token value
    Token *next;  // ML 2005-03-11 Peek tokens are kept in linked list

    Token();
    ~Token();
};

class Buffer {
  // This Buffer supports the following cases:
  // 1) seekable stream (file)
  //    a) whole stream in buffer
  //    b) part of stream in buffer
  // 2) non seekable stream (network, console)
  private:
    unsigned char *buf; // input buffer
    int bufCapacity;    // capacity of buf
    int bufStart;       // position of first byte in buffer relative to input stream
    int bufLen;         // length of buffer
    int fileLen;        // length of input stream (may change if the stream is no file)
    int bufPos;         // current position in buffer
    FILE* stream;       // input stream (seekable)
    bool isUserStream;  // was the stream opened by the user?

    int ReadNextStreamChunk();
    bool CanSeek();     // true if stream can be seeked otherwise false

  public:
    static const int EoF = COCO_WCHAR_MAX + 1;

    Buffer(FILE* s, bool isUserStream);
    Buffer(const unsigned char* buf, int len);
    Buffer(Buffer *b);
    virtual ~Buffer();

    virtual void Close();
    virtual int Read();
    virtual int Peek();
    virtual wchar_t* GetString(int beg, int end);
    virtual int GetPos();
    virtual void SetPos(int value);
};

class UTF8Buffer : public Buffer {
  public:
    UTF8Buffer(Buffer *b) : Buffer(b) {};
    virtual int Read();
};

//-----------------------------------------------------------------------------------
// StartStates  -- maps characters to start states of tokens
//-----------------------------------------------------------------------------------
class StartStates {
  private:
    class Elem {
      public:
        int key, val;
        Elem *next;
        Elem(int key, int val) { this->key = key; this->val = val; next = NULL; }
    };

    Elem **tab;

  public:
    StartStates() { tab = new Elem*[128]; memset(tab, 0, 128 * sizeof(Elem*)); }
    virtual ~StartStates() {
      for (int i = 0; i < 128; ++i) {
        Elem *e = tab[i];
        while (e != NULL) {
          Elem *next = e->next;
          delete e;
          e = next;
        }
      }
      delete [] tab;
    }

    void set(int key, int val) {
      Elem *e = new Elem(key, val);
      int k = ((unsigned int) key) % 128;
      e->next = tab[k]; tab[k] = e;
    }

    int state(int key) {
      Elem *e = tab[((unsigned int) key) % 128];
      while (e != NULL && e->key != key) e = e->next;
      return e == NULL ? 0 : e->val;
    }
};

//-------------------------------------------------------------------------------------------
// KeywordMap  -- maps strings to integers (identifiers to keyword kinds)
//-------------------------------------------------------------------------------------------
class KeywordMap {
  private:
    class Elem {
      public:
        wchar_t *key;
        int val;
        Elem *next;
        Elem(const wchar_t *key, int val) { this->key = gum::coco_string_create(key); this->val = val; next = NULL; }
        virtual ~Elem() { gum::coco_string_delete(key); }
    };

    Elem **tab;

  public:
    KeywordMap() { tab = new Elem*[128]; memset(tab, 0, 128 * sizeof(Elem*)); }
    virtual ~KeywordMap() {
      for (int i = 0; i < 128; ++i) {
        Elem *e = tab[i];
        while (e != NULL) {
          Elem *next = e->next;
          delete e;
          e = next;
        }
      }
      delete [] tab;
    }

    void set(const wchar_t *key, int val) {
      Elem *e = new Elem(key, val);
      int k = gum::coco_string_hash(key) % 128;
      e->next = tab[k]; tab[k] = e;
    }

    int get(const wchar_t *key, int defaultVal) {
      Elem *e = tab[gum::coco_string_hash(key) % 128];
      while (e != NULL && !gum::coco_string_equal(e->key, key)) e = e->next;
      return e == NULL ? defaultVal : e->val;
    }
};

class Scanner {
  private:
    void *firstHeap;
    void *heap;
    void *heapTop;
    void **heapEnd;

    unsigned char EOL;
    int eofSym;
    int noSym;
    int maxT;
    int charSetSize;
    StartStates start;
    KeywordMap keywords;

    Token *t;         // current token
    wchar_t *tval;    // text of current token
    int tvalLength;   // length of text of current token
    int tlen;         // length of current token

    Token *tokens;    // list of tokens already peeked (first token is a dummy)
    Token *pt;        // current peek token

    int ch;           // current input character
    
      int pos;          // byte position of current character
    int line;         // line number of current character
    int col;          // column number of current character
    int oldEols;      // EOLs that appeared in a comment;

    void CreateHeapBlock();
    Token* CreateToken();
    void AppendVal(Token *t);

    void Init();
    void NextCh();
    void AddCh();
    	bool Comment0();
	bool Comment1();

      Token* NextToken();

  public:
    Buffer *buffer;   // scanner buffer

    Scanner(std::string filename);
    ~Scanner();
    Token* Scan();
    Token* Peek();
    void ResetPeek();
    std::string filename;

}; // end Scanner



#endif // !defined(SKOOL_COCO_SCANNER_H__)

