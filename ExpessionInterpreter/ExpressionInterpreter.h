// List of possible error codes that can be obtained during expression derivation / "compilation"
// Expression derivation == 0 means that the expression was compiled wihtout errors according to the established grammar 
// and can be evaluated

#define ERR_OPEN_PARENTESIS_EXPECTED 1
#define ERR_CLOSE_PARENTESIS_EXPECTED 2
#define ERR_NOT_FUNCTION 3
#define ERR_INVALID_SYMBOL 4
#define ERR_UNEXPECTED_END 5
#define ERR_INVALID_NUM_PARAMS 6
#define ERR_EXPECTED_OPERATOR 7

#define DELIMITERS " +-*/(),\x3"
#define VALID_AFTER "+-*/,)\x3"

char* createString(char* in_psString, char* in_psAddString);

class CListItem {
	CListItem* m_pNext;
	CListItem* m_pPrevious;

public:
	CListItem();
	virtual void setNext(CListItem*);
	virtual void setPrevious(CListItem*);
	virtual CListItem* getNext();
	virtual CListItem* getPrevious();
	virtual char* getString();
	virtual void print();
};

class CList {
	CListItem* m_pFirstItem;
	CListItem* m_pLastItem;


public:
	CListItem* m_pCurrentItem;

	CList();
	virtual void appendItem(CListItem*);
	virtual void removeItem(CListItem*);
	virtual CListItem* movetoFirst();
	virtual CListItem* movetoNext();
	virtual CListItem* getFirst();
	virtual CListItem* getLast();
	virtual CListItem* getCurrent();
	virtual void printAll();
};

// Functions list
// CFunction and CFunctionList Enables the parser to know what are the reserved words of a function
// and how many arguments shall be passed as parameters to the function
// -1 arguments means that the function accepts many parameters (such as AVG)
class CFunction : public CListItem {
	char* m_psFuncName;
	int m_iFuncArgs;

public:
	CFunction(char* , int);
	char* getName() { return m_psFuncName; };
	int getArgs() { return m_iFuncArgs; };
};

class CFunctionList : public CList
{
	public:
		CFunctionList();
		int getArgs(char *in_psFuncName);
		CFunction *isFuncName(char *in_psFuncName);
};


// Error list
// CError and CErrorList record all the messages to be returned upon a error during parsing
class CError : public CListItem {
	int m_iErrorCode;
	char* m_psErrorMessage;

public:
	CError(int, char *);
	char* getMessage() { return m_psErrorMessage; };
	int getCode() { return m_iErrorCode; }
};

class CErrorList : public CList
{
public:
	CErrorList();
	char *getMessage(int in_iErrorCode);
};


// CVar and CStackVar records all Variables found during the parsing and set the initial value for each variable as
// informed by the user

class CVar :public CListItem {
	char* m_psName;
	float m_fValue;

public:
	CVar(char* in_psName) { setName(in_psName); setValue(0); };
	~CVar() { delete[]m_psName; };
	void setValue(float in_fValue) { m_fValue = in_fValue; };
	void setName(char* in_psName) { m_psName = createString(in_psName, NULL); };
	char* getName() { return m_psName; };
	float getValue() { return m_fValue; };
};

class CStackVar : public CList {
public:
	~CStackVar();
	CVar* findVar(char* in_psVarName);
};

// Define Entry Types

#define DATATYPE_UNDEF 0
#define DATATYPE_NUM 1
#define DATATYPE_OPER 2
#define DATATYPE_VAR 3
#define DATATYPE_FUNC 4
#define DATATYPE_PARAMTERMIN 5
 
class CEntryData {
	public:
		virtual void print() {};
		~CEntryData() {};
		virtual int getType () { return DATATYPE_UNDEF; };
};

class CEntry : public CListItem {
public:
	CEntryData* m_pEntryData;

	~CEntry() { delete m_pEntryData; };
	void print();

};


class CEntryDataNum : public CEntryData  {
public:
	float m_fNum;

	CEntryDataNum(float in_fNum);
	virtual int getType() { return DATATYPE_NUM; };
	void print();
};

class CEntryDataOper : public CEntryData {
public:
	char m_cOper;

	CEntryDataOper(char in_cOper);
	void print();
	virtual int getType () { return DATATYPE_OPER; };
};

class CEntryDataVar : public CEntryData {
public:
	char *m_psVar;

	CEntryDataVar(char* in_psVar);
	~CEntryDataVar() { delete[]m_psVar; };
	virtual int getType () { return DATATYPE_VAR; };
	void print();
};

class CEntryDataFunc : public CEntryData {
public:
	char *m_psFuncName;

	CEntryDataFunc(char* m_psFuncName);
	~CEntryDataFunc() { delete[]m_psFuncName; };
	virtual int getType () { return DATATYPE_FUNC; };
	void print();
};

class CEntryDataParamTermin : public CEntryData {
public:
	void print() { printf("PARAMTERMIN\n"); };
	virtual int getType () { return DATATYPE_PARAMTERMIN; };
};

class CStackEntry : public CList {
public:
	CEntry* m_pCurrentOper;
	CEntry* m_pFirstParam;

	~CStackEntry();
	float eval(void);
	CEntry* findNextEvalNode();
	float calcEvalNode();
	float calcOper();
	float calcFunc();
	CEntry* appendNum(float);
	CEntry* appendVar(char *);
	CEntry* appendOper(char);
	CEntry* appendFunc(char*);
	CEntry* appendParamTermin();
	void setVariables(CStackVar*);
	void setVariablesValues(CStackVar*);
};


class CToken : public CListItem {
public:

	char *m_psTokenString;

	CToken(char *in_psTokenString);
	~CToken() { delete[]m_psTokenString; };
	char* getString();
	void print();
};

class CStackToken : public CList {
public:
	~CStackToken();
	int derivateExpression(CStackEntry*);
	int derivateSumSubtract(CStackEntry*);
	int derivateSumSubtract_DERIV(CStackEntry*);
	int derivateMultiple(CStackEntry*);
	int derivateMultiple_DERIV(CStackEntry*);
	int derivateDivision(CStackEntry*);
	int derivateDivision_DERIV(CStackEntry*);
	int derivateSimpleExpression(CStackEntry*);
	int derivateParamList(CStackEntry*, int *);
	int derivateParamList_DERIV(CStackEntry*, int *);
};

class CExpression {
public:
	char *m_psExp;
	CStackToken* m_pStackToken;
	CStackEntry* m_pStackEntry;
	CStackVar* m_pStackVar;

	CExpression(char* p_sExp);
	~CExpression();

	char* parse_stackNextToken(char* in_psParseIni);
	int parseExpression(void);
	char *parseToken(char* psParseIni);
	int compileExpression(void);
	void setVariables(void);
	void testInputVariables(void);
	float executeExpression(void);
};
