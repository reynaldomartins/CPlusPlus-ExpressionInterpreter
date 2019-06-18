#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ExpressionInterpreter.h"

// Global variable with all Functions
CFunctionList FunctionList;

// Global Variable with all Error Codes
CErrorList ErrorList;

// Loading all Functions avaiable in the interpreter
CFunctionList::CFunctionList() {
	appendItem(new CFunction((char*) "SQRT", 1));
	appendItem(new CFunction((char*) "CEIL", 1));
	appendItem(new CFunction((char*) "TRUNC", 1));
	appendItem(new CFunction((char*) "ROUND", 1));
	appendItem(new CFunction((char*) "ABS", 1));
	appendItem(new CFunction((char*) "FACT", 1));
	appendItem(new CFunction((char*) "NEGAT", 1));
	appendItem(new CFunction((char*) "MOD", 2));
	appendItem(new CFunction((char*) "POW", 2));
	appendItem(new CFunction((char*) "MEAN", -1));
	appendItem(new CFunction((char*) "MEDIAN", -1));
	appendItem(new CFunction((char*) "MAX", -1));
	appendItem(new CFunction((char*) "MIN", -1));
};

// Loading all error codes that can be returned by the interpreter
CErrorList::CErrorList() {
	appendItem(new CError(ERR_OPEN_PARENTESIS_EXPECTED ,
					(char*) "It was expected a openning parentesis"));
	appendItem(new CError(ERR_CLOSE_PARENTESIS_EXPECTED,
					(char*) "It was expected a closing parentesis"));
	appendItem(new CError(ERR_NOT_FUNCTION,
						(char*) "Not valid function name"));
	appendItem(new CError(ERR_INVALID_SYMBOL,
						(char*) "Unexpected symbol"));
	appendItem(new CError(ERR_UNEXPECTED_END,
						(char*) "Unexpected end of expression"));
	appendItem(new CError(ERR_INVALID_NUM_PARAMS,
						(char*) "The number of Arguments provided to the function is wrong"));
	appendItem(new CError(ERR_EXPECTED_OPERATOR,
						(char*) "Unexpected symbol after a variable or constant"));
};

// createString function creates and allocate a string in the heap area dinamicaly
// I have decided to not use the C++ string class for a while 
// in order to do a stronger exercize about dinamically memory allocation 

char* createString(char* in_psString, char *in_psAddString) {
	int size;
	char* psString;

	size = strlen(in_psString);
	if (in_psAddString != NULL)
		size += strlen(in_psAddString);
	psString = (char*)malloc(size + 1);
	strcpy(psString, in_psString);
	if (in_psAddString != NULL)
		strcat(psString, in_psAddString);
	return psString;
}

// makeUpper function for a whole string
void makeUpper(char *in_psString) {
	int i=0;

	while (in_psString[i] != '\0')
	{
		in_psString[i] = toupper(in_psString[i]);
		i++;
	}
}

// verifies if a String is all alphabetic
// used to check if a token can be a variable which accepts just letters
bool isAlphaString(char* in_psString) {
	while (*in_psString != '\0') {
		if (isalpha(*in_psString) == 0)
			return false;
		in_psString++;
	}
	return true;
}

// used to verifiy if a token is valid number constant
bool isNumString(char* in_psString) {
	while (*in_psString != '\0') {
		if (isdigit(*in_psString) == 0 && *in_psString != '.')
			return false;
		in_psString++;
	}
	return true;
}

// quicksort recursive function
// used to sort a parameter list in order to find the median in the MEDIAN function
void quicksort(float *in_afNum, int in_iSize) {
	float *afNum = new float[in_iSize];
	float fTemp;
	int iIni = 0;
	int iEnd = in_iSize - 1;
	int i = 1;

	while (i < in_iSize)
	{
		if (in_afNum[i] < in_afNum[0])
		{
			afNum[iIni] = in_afNum[i];
			iIni++;
		}
		else if (in_afNum[i] >= in_afNum[0])
		{
			afNum[iEnd] = in_afNum[i];
			iEnd--;
		}
		i++;
	}
	afNum[iEnd] = in_afNum[0];
	if (in_iSize > 1)
	{
		quicksort(&afNum[0], iIni+1);
		memcpy((void*)& in_afNum[0], (void*)& afNum[0], sizeof(float) * (iIni + 1));
		quicksort(&afNum[iEnd + 1], in_iSize - iEnd - 1);
		memcpy((void*)& in_afNum[iEnd + 1], (void*)& afNum[iEnd + 1], sizeof(float) * (in_iSize - iEnd - 1));
	}
	else
		in_afNum[0] = afNum[0];
	delete[]afNum;
}

// CListItem Virtual Class to enclose all elements that are wanted to be in a list of linked elements
CListItem::CListItem() {
	m_pNext = NULL;
	m_pPrevious = NULL;
};

void CListItem::setNext(CListItem* in_pListItem) {
	m_pNext = in_pListItem;
};

void CListItem::setPrevious(CListItem* in_pListItem) {
	m_pPrevious = in_pListItem;
}

CListItem* CListItem::getNext() {
	return m_pNext;
};

CListItem* CListItem::getPrevious() {
	return m_pPrevious;
}

char* CListItem::getString() {
	return NULL;
}

void CListItem::print() {

}

// CList Virtual Class to enclose lists that can be inherited by more specific type of lists
CList::CList() {
	m_pFirstItem = NULL;
	m_pLastItem = NULL;
	m_pCurrentItem = NULL;
}

// Append a Item to the end of a list
void CList::appendItem(CListItem* in_pListItem) {
	if (m_pLastItem != NULL)
		// We will not append the first item in the list
		m_pLastItem->setNext(in_pListItem);
	else
		// We will append the first item in the list
		m_pFirstItem = in_pListItem;
	in_pListItem->setPrevious(m_pLastItem);
	m_pLastItem = in_pListItem;
};

// remove a item/element from the list
// this function is not aimed to delete the element removed
// all the links in the list are set in order to keep the list integrity
void CList::removeItem(CListItem* in_pListItem) {
	CListItem* pListItemPrevious = in_pListItem->getPrevious();
	CListItem* pListItemNext = in_pListItem->getNext();

	if (pListItemPrevious != NULL)
	{
		// It is not the First Item in the List
		pListItemPrevious->setNext(pListItemNext);
	}
	else
		// It is the first Item i the list
		m_pFirstItem = pListItemNext;
	if (pListItemNext != NULL)
		pListItemNext->setPrevious(pListItemPrevious);
}

CListItem* CList::movetoFirst() {
	m_pCurrentItem = m_pFirstItem;
	return m_pCurrentItem;
}

CListItem* CList::movetoNext() {
	m_pCurrentItem = m_pCurrentItem->getNext();
	return m_pCurrentItem;
	// Returning NULL means that we reached the end of the list
}

CListItem* CList::getFirst() {
	return m_pFirstItem;
}

CListItem* CList::getLast() {
	return m_pLastItem;
}

// When you order list to move the Current pointer is updated
CListItem* CList::getCurrent() {
	return m_pCurrentItem;
}

// CFunction and  CFunctionList
CFunction::CFunction(char* in_psFuncName, int in_iFuncArgs) {
	m_psFuncName = createString(in_psFuncName, NULL);
	m_iFuncArgs = in_iFuncArgs;
}

// Used to verify if a token is a valid function name
CFunction *CFunctionList::isFuncName(char *in_psName) {
	CFunction* pCurrent;

	pCurrent = (CFunction*) movetoFirst();
	while (pCurrent != NULL) {
		if (strcmp(pCurrent->getName(), in_psName) == 0)
			return pCurrent;
		pCurrent = (CFunction*) movetoNext();
	}
	return false;
}

// Given the function name, search in the Function list in order to get the number of arguments of the function
int CFunctionList::getArgs(char* in_psName) {
	CFunction* pCurrent;

	pCurrent = (CFunction*)movetoFirst();
	while (pCurrent != NULL) {
		if (strcmp(pCurrent->getName(), in_psName) == 0)
			return pCurrent->getArgs();
		pCurrent = (CFunction*)movetoNext();
	}
	return -1;
}

// CError and CErrorList
CError::CError(int in_iErrorCode, char* in_psErrorMessage) {
	m_iErrorCode = in_iErrorCode;
	m_psErrorMessage = createString(in_psErrorMessage, NULL);
}

// Given the error code, search the list for the 
char * CErrorList::getMessage(int in_iErrorCode)
{
	CError* pCurrent;

	pCurrent = (CError*)movetoFirst();
	while (pCurrent != NULL) {
		if (pCurrent->getCode() == in_iErrorCode)
			return pCurrent->getMessage();
		pCurrent = (CError*) movetoNext();
	}
	return NULL;
}


// Class CEntry store each data in a "compiled"/derivated stack, ready to be evaluated by the interpreter
void CEntry::print(void) {
	m_pEntryData->print();
};

// Class StackEntry register a list of the linked StackEntry which is loaded during grammar/derivation compilation
// The Stack entry is structured like a tree, having the operators and function as nodes,
// in a way that the evaluation process respects the precedence of the operators established in the grammar
CStackEntry::~CStackEntry() {
	CEntry* pCurrent;
	CEntry* pDelete;
	pCurrent = (CEntry*)movetoFirst();
	while (pCurrent != NULL)
	{
		pDelete = pCurrent;
		pCurrent = (CEntry*)movetoNext();
		delete pDelete;
	}
}

// Start the evaluation of a derived expression
float CStackEntry::eval() {
	float fCalc=0;
	int i = 1;

	movetoFirst();
	while (findNextEvalNode() != NULL)
	{
		fCalc = calcEvalNode();
		printf("Calculation Stack after Evaluation Interaction #%i :\n", i++);
		printAll();
	}
	return fCalc;
}

// Find the next node in the stack/"tree" to be evaluated
CEntry* CStackEntry::findNextEvalNode() {

	int iDataType;

	while (true)
	{
		if (getCurrent() == NULL)
			return NULL;
		iDataType = ((CEntry*)getCurrent())->m_pEntryData->getType();
		if (iDataType ==  DATATYPE_OPER || iDataType == DATATYPE_FUNC)
return (CEntry*)getCurrent();
movetoNext();
	}
}

// Once the node was found it calculates the result using the constants in the stack
float CStackEntry::calcEvalNode() {
	int iDataType;
	iDataType = ((CEntry*)getCurrent())->m_pEntryData->getType();

	switch (iDataType)
	{
	case DATATYPE_OPER:
		return calcOper();
	case DATATYPE_FUNC:
		return calcFunc();
	}
	return 0;
}

// Return the calculated value of a function evaluation
// remark - This function could be optimized / breaked down for sack of clarity
float CStackEntry::calcFunc() {
	float fCalc;
	float fParam1;
	float fParam2;
	float fTemp;
	float * a_fParam = NULL;
	int iTemp;
	int iTemp2;
	int iNumArgs;
	char* psFuncName;
	CEntry* pCurrent;
	CEntry* pParam1;
	CEntry* pParam2;
	CFunction* pFunction;

	pCurrent = (CEntry*)getCurrent();
	psFuncName = ((CEntryDataFunc*)pCurrent->m_pEntryData)->m_psFuncName;
	pFunction = FunctionList.isFuncName(psFuncName);
	iNumArgs = pFunction->getArgs();

	// Once the parameters are obtained from the stack their are deleted and
	// the stack is reorganized using the removeItem 
	// The node element whihc contains the function is changed in order to receive a constant (as resulted from the evaluation) 

	// Evaluate functions with 1 parameters
	if (iNumArgs == 1)
	{
		pParam1 = (CEntry*)pCurrent->getPrevious();
		fParam1 = ((CEntryDataNum*)pParam1->m_pEntryData)->m_fNum;
		removeItem(pParam1);
		delete pParam1;

		if (strcmp(psFuncName, "SQRT") == 0)
			fCalc = sqrt(fParam1);
		else if (strcmp(psFuncName, "CEIL") == 0)
			fCalc = ceil(fParam1);
		else if (strcmp(psFuncName, "TRUNC") == 0)
			fCalc = floor(fParam1);
		else if (strcmp(psFuncName, "ABS") == 0)
			fCalc = abs(fParam1);
		else if (strcmp(psFuncName, "NEGAT") == 0)
			fCalc = -1 * fParam1;
		else if (strcmp(psFuncName, "ROUND") == 0)
			fCalc = round(fParam1);
		else if (strcmp(psFuncName, "FACT") == 0)
		{
			iTemp = 1;
			fTemp = 1;
			while (iTemp <= fParam1)
			{
				fTemp = fTemp * iTemp;
				iTemp++;
			}
			fCalc = fTemp;
		}
	}
	// Evaluate function with 2 parameters
	else if (iNumArgs == 2)
	{
		pParam1 = (CEntry*)pCurrent->getPrevious()->getPrevious();
		pParam2 = (CEntry*)pCurrent->getPrevious();
		fParam1 = ((CEntryDataNum*)pParam1->m_pEntryData)->m_fNum;
		fParam2 = ((CEntryDataNum*)pParam2->m_pEntryData)->m_fNum;
		removeItem(pParam1);
		removeItem(pParam2);
		delete pParam1;
		delete pParam2;

		if (strcmp(psFuncName, "MOD") == 0)
			fCalc = (float)(((int)round(fParam1)) % ((int)round(fParam2)));
		else if (strcmp(psFuncName, "POW") == 0)
			fCalc = pow(fParam1, fParam2);
	}
	// Evaluate functions that get variable quantity of arguments
	// During the parsing a delimiter is put in the stack to set the final of the parameter list 
	else if (iNumArgs == -1)
	{
		pParam1 = (CEntry*)pCurrent->getPrevious();
		fTemp = 0;
		iTemp = 0;
		// get parameter values until the delimiter is found
		while (pParam1->m_pEntryData->getType() != DATATYPE_PARAMTERMIN)
		{
			if (strcmp(psFuncName, "MEAN") == 0)
				fTemp = fTemp + ((CEntryDataNum*)pParam1->m_pEntryData)->m_fNum;
			else if (strcmp(psFuncName, "MEDIAN") == 0)
			{
				if (iTemp == 0)
				{
					iTemp2 = iTemp;
					pParam2 = pParam1;
					while (pParam2->m_pEntryData->getType() != DATATYPE_PARAMTERMIN)
					{
						pParam2 = (CEntry *) pParam2->getPrevious();
						iTemp2++;
					}
					a_fParam = new float[iTemp2];
				}
				a_fParam[iTemp] = ((CEntryDataNum*)pParam1->m_pEntryData)->m_fNum;
			}
			else if (strcmp(psFuncName, "MAX") == 0)
			{
				if (iTemp == 0)
					fTemp = ((CEntryDataNum*)pParam1->m_pEntryData)->m_fNum;
				else if (((CEntryDataNum*)pParam1->m_pEntryData)->m_fNum > fTemp)
					fTemp = ((CEntryDataNum*)pParam1->m_pEntryData)->m_fNum;
			}
			else if (strcmp(psFuncName, "MIN") == 0)
			{
				if (iTemp == 0)
					fTemp = ((CEntryDataNum*)pParam1->m_pEntryData)->m_fNum;
				else if (((CEntryDataNum*)pParam1->m_pEntryData)->m_fNum < fTemp)
					fTemp = ((CEntryDataNum*)pParam1->m_pEntryData)->m_fNum;
			}
			iTemp++;
			pParam2 = pParam1;
			pParam1 = (CEntry *) pParam1->getPrevious();
			removeItem(pParam2);
			delete pParam2;
		}
		if (strcmp(psFuncName, "MEAN") == 0)
			fCalc = fTemp / iTemp;
		else if (strcmp(psFuncName, "MEDIAN") == 0)
		{
			quicksort(a_fParam, iTemp);

			/*int i = 0;
			while (i < iTemp)
			{
				printf("%f,", a_fParam[i]);
				i++;
			}
			printf("\n");*/
				
			if (iTemp % 2 != 0)
				fCalc = a_fParam[(int) trunc(iTemp / 2)];
			else
				fCalc = (a_fParam[iTemp / 2] + a_fParam[iTemp / 2 - 1]) / 2;
			delete[]a_fParam;
		}
		else
			fCalc = fTemp;
		// Removing DATATYPE_PARAMTERMIN from the Stack
		removeItem(pParam1);
		delete pParam1;
	}
	// Set the node function as the resulting value
	delete pCurrent->m_pEntryData;
	pCurrent->m_pEntryData = new CEntryDataNum(fCalc);
	return fCalc;
}

// Calculate a operation
// The node element whihc contains the operator is changed in order to receive a constant
// (as resulted from the evaluation)
float CStackEntry::calcOper() {
	float fParam1;
	float fParam2;
	float fCalc;
	char cOper;
	CEntry *pCurrent;
	CEntry* pParam1;
	CEntry* pParam2;

	pCurrent = (CEntry*)getCurrent();
	cOper = ((CEntryDataOper*) pCurrent->m_pEntryData)->m_cOper;

	pParam1 = (CEntry *) pCurrent->getPrevious()->getPrevious();
	pParam2 = (CEntry *) pCurrent->getPrevious();
	fParam1 = ((CEntryDataNum*) pParam1->m_pEntryData)->m_fNum;
	fParam2 = ((CEntryDataNum*)pParam2->m_pEntryData)->m_fNum;
	switch (cOper)
	{
		case '/':
			if (fParam2 == 0)
			{
				fCalc = 1/0.000000000000000001;
				printf("Division by 0 !\n");
			}
			fCalc = fParam1 / fParam2;
			break;
		case '*':
			fCalc = fParam1 * fParam2;
			break;
		case '+':
			fCalc = fParam1 + fParam2;
			break;
		case '-':
			fCalc = fParam1 - fParam2;
			break;
	}
	delete pCurrent->m_pEntryData;
	pCurrent->m_pEntryData = new CEntryDataNum(fCalc);
	removeItem(pParam1);
	removeItem(pParam2);
	delete pParam1;
	delete pParam2;
	return fCalc;
}

// For every variable found in the derivated stack, append the in the Variable stack
void CStackEntry::setVariables(CStackVar *in_pStackVar) {
	CEntry *pCurrent;
	char* psNameVar;
	
	pCurrent = (CEntry *)movetoFirst();
	while (pCurrent != NULL) 
	{
		if (pCurrent->m_pEntryData->getType() == DATATYPE_VAR)
		{
			psNameVar = ((CEntryDataVar*)pCurrent->m_pEntryData)->m_psVar;
			if (in_pStackVar->findVar(psNameVar) == NULL)
				in_pStackVar->appendItem(new CVar(psNameVar));
		}
		pCurrent = (CEntry*) movetoNext();
	}
}

// Replace all the variables in the stack for the constant values informed to them before the
// evauation process
void CStackEntry::setVariablesValues(CStackVar* in_pStackVar) {
	CEntry* pCurrent;
	CVar* pVar;
	char* psNameVar;
	float fValue;

	pCurrent = (CEntry*) movetoFirst();
	while (pCurrent != NULL)
	{
		if (pCurrent->m_pEntryData->getType() == DATATYPE_VAR)
		{
			psNameVar = ((CEntryDataVar*)pCurrent->m_pEntryData)->m_psVar;
			pVar = in_pStackVar->findVar(psNameVar);
			fValue = pVar->getValue();
			delete pCurrent->m_pEntryData;
			pCurrent->m_pEntryData = new CEntryDataNum(fValue);
		}
		pCurrent = (CEntry*) movetoNext();
	}
}

// Specific appending methods for each type of entry in the derived/"compiled" stack
CEntry* CStackEntry::appendVar(char *in_psVar) {
	CEntry *pEntry = new CEntry();
	pEntry->m_pEntryData = (CEntryData*) new CEntryDataVar(in_psVar);
	CList::appendItem(pEntry);
	return pEntry;
}

CEntry* CStackEntry::appendNum(float in_fNum) {
	CEntry* pEntry = new CEntry();
	pEntry->m_pEntryData = (CEntryData*) new CEntryDataNum(in_fNum);
	CList::appendItem(pEntry);
	return pEntry;
}

CEntry* CStackEntry::appendOper(char in_cOper) {
	CEntry* pEntry = new CEntry();
	pEntry->m_pEntryData = (CEntryData*) new CEntryDataOper(in_cOper);
	CList::appendItem(pEntry);
	return pEntry;
}

CEntry* CStackEntry::appendFunc(char* in_psFuncName) {
	CEntry* pEntry = new CEntry();
	pEntry->m_pEntryData = (CEntryData*) new CEntryDataFunc(in_psFuncName);
	CList::appendItem(pEntry);
	return pEntry;
}

CEntry* CStackEntry::appendParamTermin() {
	CEntry* pEntry = new CEntry();
	pEntry->m_pEntryData = (CEntryData*) new CEntryDataParamTermin();
	CList::appendItem(pEntry);
	return pEntry;
}

// CStackVar
CStackVar::~CStackVar() {
		CVar* pCurrent;
		CVar* pDelete;
		pCurrent = (CVar*)movetoFirst();
		while (pCurrent != NULL)
		{
			pDelete = pCurrent;
			pCurrent = (CVar*)movetoNext();
			delete pDelete;
		}
}

// Find a variable in the Var Stack by its name
CVar *CStackVar::findVar(char *in_psVarName)
{
	CVar* pCurrent;

	
	pCurrent = (CVar*) movetoFirst();

	while (pCurrent != NULL)
	{
		if (strcmp(pCurrent->getName(), in_psVarName) == 0)
			return pCurrent;
		pCurrent = (CVar*) movetoNext();
	}
	return NULL;
}

// Class CStackDataNum contain the Numeric (float) Data to be Evaluated
CEntryDataNum::CEntryDataNum(float in_fNum) {
	m_fNum = in_fNum;
};

void CEntryDataNum::print(void) {
	printf("%f\n", m_fNum);
};

// Class CStackDataOper contain the Operator Data to be Evaluated
CEntryDataOper::CEntryDataOper(char in_cOper) {
	m_cOper = in_cOper;
};

void CEntryDataOper::print(void) {
	printf("%c\n", m_cOper);
};

// Class CStackDataVar contain the Variable Data to be Evaluated

CEntryDataVar::CEntryDataVar(char* in_psVar) {
	m_psVar = createString(in_psVar, NULL);
};

void CEntryDataVar::print(void) {
	printf("%s\n", m_psVar);
};

// Class CStackDataFunc contain the Variable Data to be Evaluated

CEntryDataFunc::CEntryDataFunc(char* in_psFunc) {
	m_psFuncName = createString(in_psFunc, NULL);
};

void CEntryDataFunc::print(void) {
	printf("%s\n", m_psFuncName);
};


// CToken Class Methods
// Register and Link Tokens parsed
// A list of linked Tokens is not yet prepared for been Evaluated by the interpreter

CToken::CToken(char* in_psTokenString) {
	m_psTokenString = createString(in_psTokenString, NULL);
};

char *CToken::getString() {
	return m_psTokenString;
}

void CToken::print() {
	printf("%s\n", m_psTokenString);
}

// Class Stack Token
// It contains a list of linked tokens which can be derivated in order to build the Evaluation Stack 
// All the grammar derivation methods are enclosed in this class
// It contains the pointers which indicates where the recursive derivation process is 

CStackToken::~CStackToken() {
	CToken* pCurrent;
	CToken* pDelete;
	pCurrent = (CToken*)movetoFirst();
	while (pCurrent != NULL)
	{
		pDelete = pCurrent;
		pCurrent = (CToken*)movetoNext();
		delete pDelete;
	}
}

void CList::printAll() {
	CListItem* pListItem;
	CListItem* pCurrent;

	pCurrent = (CEntry*)getCurrent();

	pListItem = movetoFirst();
	while (pListItem != NULL) {
		pListItem->print();
		pListItem = movetoNext();
	}
	printf("\n");
	m_pCurrentItem = pCurrent;
}


// CStackToken contains the list of parsed tokens
// All methods of of derivation/"compilation" belongs to CStackToken
// The compiled Stack goes to in_pStackEntry passed as a parameters

// derivation
// [Expression] -> [Expression-sumsubtract]
int CStackToken::derivateExpression(CStackEntry* in_pStackEntry) {
	return derivateSumSubtract(in_pStackEntry);
}

// [Expression-sumsubtract] -> [Expression-multiply] [Expression-sumsubtract -DERIV]
int CStackToken::derivateSumSubtract(CStackEntry* in_pStackEntry) {
	int ret;

	ret = derivateMultiple(in_pStackEntry);
	if (ret == 0)
		ret = derivateSumSubtract_DERIV(in_pStackEntry);
	return ret;
}

// [Expression-sumsubtract -DERIV] -> + [Expression-multiply] [Expression-sumsubtract -DERIV] 
// [Expression - sumsubtract - DERIV] -> - [Expression - multiply][Expression - sumsubtract - DERIV]
// [Expression - sumsubtract - DERIV]->Empty
int CStackToken::derivateSumSubtract_DERIV(CStackEntry* in_pStackEntry) {
	char* psTokenString;
	CToken* pCurrent;
	int ret;

	pCurrent = (CToken*)getCurrent();
	if (pCurrent == NULL)
		return 0;
	psTokenString = pCurrent->getString();
	if (strcmp(psTokenString, "+") == 0 || strcmp(psTokenString, "-") == 0) {
		movetoNext();
		ret = derivateMultiple(in_pStackEntry);
		if (ret == 0)
		{
			in_pStackEntry->appendOper(psTokenString[0]);
			ret = derivateSumSubtract_DERIV(in_pStackEntry);
			return ret;
		}
		else
			return ret;
	}
	else
		return 0;
}

// [Expression-multiply] -> [Expression-divide] [Expression-multiply-DERIV] 
int CStackToken::derivateMultiple(CStackEntry* in_pStackEntry) {
	int ret;

	ret = derivateDivision(in_pStackEntry);
	if (ret == 0)
		ret = derivateMultiple_DERIV(in_pStackEntry);
	return ret;
}

// [Expression-multiply-DERIV] ->*[Expression-divide] [Expression-multiply-DERIV]
// [Expression - multiply - DERIV]->Empty
int CStackToken::derivateMultiple_DERIV(CStackEntry* in_pStackEntry) {
	char* psTokenString;
	CToken* pCurrent;
	int ret;

	pCurrent = (CToken *) getCurrent();
	if (pCurrent == NULL)
		return 0;
	psTokenString = pCurrent->getString();
	if (strcmp(psTokenString, "*") == 0) {
		movetoNext();
		ret = derivateDivision(in_pStackEntry);
		if (ret == 0)
		{
			in_pStackEntry->appendOper(psTokenString[0]);
			ret = derivateMultiple_DERIV(in_pStackEntry);		
			return ret;
		}
		else
			return ret;
	}
	else
		return 0;
}

// [Expression - divide]->[Expression - simple][Expression - divide - DERIV]
int CStackToken::derivateDivision(CStackEntry* in_pStackEntry) {
	int ret;

	ret = derivateSimpleExpression(in_pStackEntry);
	if (ret == 0)
		ret = derivateDivision_DERIV(in_pStackEntry);
	return ret;
}

//[Expression-divide-DERIV]-> / [Expression-simple] [Expression-divide-DERIV]
// [Expression - divide - DERIV]->Empty
int CStackToken::derivateDivision_DERIV(CStackEntry* in_pStackEntry) {
	char* psTokenString;
	int ret;
	CToken* pCurrent;

	pCurrent = (CToken*)getCurrent();
	if (pCurrent == NULL)
		return 0;
	psTokenString = pCurrent->getString();
	if (strcmp(psTokenString, "/") == 0) {
		movetoNext();
		ret = derivateSimpleExpression(in_pStackEntry);
		if (ret == 0)
		{
			in_pStackEntry->appendOper(psTokenString[0]);
			ret = derivateDivision_DERIV(in_pStackEntry);
			return ret;
		}
		else
			return ret;
	}
	else
		return 0;
}

// [Expression - simple] -> ([Expression])
// [Expression - simple] -> - [Expression - simple]
// [Expression - simple]->Constant
// [Expression - simple]->Variable
// [Expression - simple]->Function([Expression - parameters - list])
int CStackToken::derivateSimpleExpression(CStackEntry* in_pStackEntry) {
	char* psTokenString;
	int ret;
	float fTokenNum;
	CToken* pCurrent;
	CFunction* pFunction;
	int iNumParam = 0;
	int iNumArgs;

	pCurrent = (CToken*)getCurrent();
	if (pCurrent == NULL)
		return ERR_UNEXPECTED_END;
	psTokenString = pCurrent->getString();
	if (strcmp(psTokenString, "(") == 0) {
		movetoNext();
		ret = derivateExpression(in_pStackEntry);
		if (ret == 0) {
			psTokenString = getCurrent()->getString();
			if (strcmp(psTokenString, ")") == 0)
			{
				movetoNext();
				return 0;
			}
			else
				return ERR_CLOSE_PARENTESIS_EXPECTED;
		}
		else
			return ret;
	}
	else if (strcmp(psTokenString, "-") == 0) {
		movetoNext();
		ret = derivateSimpleExpression(in_pStackEntry);
		if (ret == 0)
			in_pStackEntry->appendFunc((char *) "NEGAT");
		return ret;
	}
	else if ((pFunction = FunctionList.isFuncName(psTokenString)) != NULL) {
		iNumArgs = pFunction->getArgs();
		psTokenString = movetoNext()->getString();
		if (strcmp(psTokenString, "(") == 0) {
			// It is a function wich accepts a variable number of arguments
			// so add a paramdelimiter into the stack before parsing the parameters
			if (iNumArgs == -1)
				in_pStackEntry->appendParamTermin();
			movetoNext();
			ret = derivateParamList(in_pStackEntry, &iNumParam);
			if (ret == 0) {
				psTokenString = getCurrent()->getString();
				if (strcmp(psTokenString, ")") == 0)
				{
					in_pStackEntry->appendFunc(pFunction->getName());
					if (iNumArgs != iNumParam && iNumArgs != -1)
						return ERR_INVALID_NUM_PARAMS;
					movetoNext();
					return 0;
				}
				else
					return ERR_CLOSE_PARENTESIS_EXPECTED;
			}
			else
				return ret;
		}
		else
			return ERR_OPEN_PARENTESIS_EXPECTED;
	}
	else 
	{
			fTokenNum = (float)atof(psTokenString);
			// Check if it is valid Const Number
			// if fTokenNum != 0, it means it is a valid number
			if (fTokenNum != 0)
				in_pStackEntry->appendNum(fTokenNum);
			else 
			{
				// fToken == 0, it means that it can be zero or a non num
				if (isNumString(psTokenString) == true)
					// It is a numeric, so it is zero
					in_pStackEntry->appendNum(fTokenNum);
				// Check if it is a valid Variable
				else
				{
					if (isAlphaString(psTokenString) == false)
						return ERR_INVALID_SYMBOL;
					in_pStackEntry->appendVar(psTokenString);
				}
					
			}
			movetoNext();
			pCurrent = (CToken *) getCurrent();
			// Check if after a Variable or a Constant Num comes a valid token for its state
			// If not return a error
			if (pCurrent != NULL)
				if (strpbrk(pCurrent->getString(), VALID_AFTER) == NULL)
					return ERR_EXPECTED_OPERATOR;
			return 0;
	}
}

// [Expression - parameters - list] ->[Expression][Expression - parameters - list - DERIV]
int CStackToken::derivateParamList(CStackEntry* in_pStackEntry, int* in_piNumParam) {
	int ret;

	ret = derivateExpression(in_pStackEntry);
	if (ret == 0)
		ret = derivateParamList_DERIV(in_pStackEntry, in_piNumParam);
	return ret;
}

// [Expression-parameters-list-DERIV] -> , [Expression]  [Expression-parameters-list-DERIV] 
// [Expression - parameters - list - DERIV]->Empty
int CStackToken::derivateParamList_DERIV(CStackEntry* in_pStackEntry, int * in_piNumParam) {
	char* psTokenString;
	int ret;
	CToken* pCurrent;

	(*in_piNumParam)++;
	pCurrent = (CToken*) getCurrent();
	// The end of the expression was reached inexectedçy when a comma was trully expected
	if (pCurrent == NULL)
		return ERR_UNEXPECTED_END;
	psTokenString = pCurrent->getString();
	if (strcmp(psTokenString, ",") == 0) {
		movetoNext();
		ret = derivateExpression(in_pStackEntry);
		if (ret == 0)
		{
			ret = derivateParamList_DERIV(in_pStackEntry, in_piNumParam);
			return ret;
		}
		else
			return ret;
	}
	else
		return 0;
}

// CExpression Class contains all engines to load a expression, parse/tokeninize it, derive it/"compile", and Evaluate it
CExpression::CExpression(char* in_psExp) {
	char psAddString[] = "\x3";
	m_psExp = createString(in_psExp, psAddString);
	m_pStackToken = new CStackToken();
	m_pStackEntry = new CStackEntry();
	m_pStackVar = new CStackVar();
};

CExpression::~CExpression() {
	delete m_pStackToken;
	delete m_pStackEntry;
	delete m_pStackVar;
}

int CExpression::compileExpression() {
	int ret = 0;

	m_pStackToken->movetoFirst();
	while (m_pStackToken->getCurrent() != NULL)
	{
		ret = m_pStackToken->derivateExpression(m_pStackEntry);
		if (ret != 0)
			break;
	}
	if (ret == 0) {
		printf("Calculation Stack after Token Stack Derivation / Compilation :\n");
		m_pStackEntry->printAll();
	}
	return ret;
}

void CExpression::setVariables(void) {
	m_pStackEntry->setVariables(m_pStackVar);
}

float CExpression::executeExpression(void) {
	return m_pStackEntry->eval();
};

// Parse the next token using the delimiters definition
char* CExpression::parse_stackNextToken(char* in_psParseIni) {
	char* psParseNextIni;
	char* psParseEnd;
	char* psTokenString;

	psParseEnd = parseToken(in_psParseIni);
	if (in_psParseIni == psParseEnd) {
		if (*in_psParseIni != ' ')
		{
			psTokenString = (char*)malloc(2);
			memcpy(psTokenString, in_psParseIni, 1);
			psTokenString[1] = '\0';
			psParseNextIni = psParseEnd + 1;
		}
		else
			return psParseEnd + 1;
	}
	else {
		psTokenString = (char*)malloc(psParseEnd - in_psParseIni + 1);
		memcpy(psTokenString, in_psParseIni, psParseEnd - in_psParseIni + 1);
		psTokenString[psParseEnd - in_psParseIni] = '\0';
		psParseNextIni = psParseEnd;
	}
	m_pStackToken->appendItem(new CToken(psTokenString));
	return psParseNextIni;
}

// Parse a single token
char* CExpression::parseToken(char* psParseIni) {
	// Procura o fim do token, buscando o próximo delimitador
	return strpbrk(psParseIni, DELIMITERS);
}

// Do the whole parsing
int CExpression::parseExpression(void) {
	char* psParseIni;

	psParseIni = m_psExp;
	while (*psParseIni != '\x3') {
		psParseIni = this->parse_stackNextToken(psParseIni);
	}
	// m_pStackToken->appendItem((char *) "#");
	printf("Token Stack after Parsing :\n");
	m_pStackToken->printAll();
	return 0;
}

// ask the user the input valus for the variables found
void CExpression::testInputVariables() {
	CVar* pCurrent;
	char strvalue[20];
	float fValue;
	bool bVarFound = false;
	
	pCurrent = (CVar*) m_pStackVar->movetoFirst();
	while (pCurrent != NULL)
	{
		printf("Inform the value for variable (%s) : ", pCurrent->getName());
		std::cin >> strvalue;
		fValue = atoi(strvalue);
		pCurrent->setValue(fValue);
		pCurrent = (CVar*) m_pStackVar->movetoNext();
		bVarFound = true;
	}
	if (bVarFound == true)
	{
		m_pStackEntry->setVariablesValues(m_pStackVar);
		printf("\n");
		printf("Calculation Stack after Variables Setting :\n");
		m_pStackEntry->printAll();
	}
}


// Main Program

int main()
{
	CExpression* pExp;
	char sExpression[400];
	char sAnswer[2];
	int iRet;

	std::cout << "Interpreter Exercise - C++ Recap - by Reynaldo Martins\n\n";

	while (true)
	{
		printf("================================================\n");
		printf("===== Enter a Expression to be Calculated : ====\n");
		printf("================================================\n");
		printf("(type 'exit' to leave the program)\n");
		/* std::cin >> sExpression;*/
		while (true)
		{
			std::cin.getline(sExpression, sizeof(sExpression));
			if (strcmp(sExpression, "") != 0)
				break;
		}
		std::cout << "\n";
		makeUpper(sExpression);
		if (strcmp(sExpression, "EXIT") == 0)
			return 0;
		pExp = new CExpression((char*)sExpression);
		pExp->parseExpression();
		iRet = pExp->compileExpression();
		if (iRet != 0)
		{
			std::cout << "A error occoured while compiling the expression\n";
			printf("Error : %i (%s)\n", iRet, ErrorList.getMessage(iRet));
			if (pExp->m_pStackToken->getCurrent())
				pExp->m_pStackToken->getCurrent()->print();
			std::cout << "\n";
		}
		else
		{
			pExp->setVariables();
			pExp->testInputVariables();
			pExp->executeExpression();
			delete pExp;
		}
		std::cout << "\n";
	}
}