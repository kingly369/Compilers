/*
 * File:	parser.c
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the recursive-descent parser for
 *		Simple C.
 */

# include <cstdlib>
# include <iostream>
# include "checker.h"
# include "tokens.h"
# include "lexer.h"

using namespace std;

static Type expression(bool &lvalue);
static void statement();
static int lookahead, nexttoken;
static string lexbuf, nextbuf;
static Type functionName;

static string E1 = "break statement not within loop";
static string E2 = "invalid return type";
static string E3 = "invalid type for test expression";
static string E4 = "lvalue required in expression";
static string E5 = "invalid operands to binary %s";
static string E6 = "invalid operand to unary %s";
static string E7 = "invalid operand in sizeof expression";
static string E8 = "invalid operand in cast expression";
static string E9 = "called object is not a function";
static string E10 = "invalid arguments to called function";



/*
 * Function:	error
 *
 * Description:	Report a syntax error to standard error.
 */

static void error()
{
    if (lookahead == DONE)
	report("syntax error at end of file");
    else
	report("syntax error at '%s'", yytext);

    exit(EXIT_FAILURE);
}


/*
 * Function:	peek
 *
 * Description:	Return the next word from the lexer, but do not fetch it.
 */

static int peek()
{
    if (nexttoken == 0) {
	nexttoken = yylex();
	nextbuf = yytext;
    }

    return nexttoken;
}


/*
 * Function:	match
 *
 * Description:	Match the next token against the specified token.  A
 *		failure indicates a syntax error and will terminate the
 *		program since our parser does not do error recovery.
 */

static void match(int t)
{
    if (lookahead != t)
	error();

    if (nexttoken != 0) {
	lookahead = nexttoken;
	lexbuf = nextbuf;
	nexttoken = 0;
    } else {
	lookahead = yylex();
	lexbuf = yytext;
    }
}


/*
 * Function:	integer
 *
 * Description:	Match the next token as a number and return its value.
 */

static unsigned integer()
{
    string buf;


    buf = lexbuf;
    match(INTEGER);
    return strtoul(buf.c_str(), NULL, 0);
}


/*
 * Function:	identifier
 *
 * Description:	Match the next token as an identifier and return its name.
 */

static string identifier()
{
    string buf;


    buf = lexbuf;
    match(ID);
    return buf;
}


/*
 * Function:	closeParamScope
 *
 * Description:	Close the current scope, which should be the parameter
 *		scope of a function declaration.  Only the types, and not
 *		the symbols, need to saved as part of the declaration, so
 *		when the scope is closed, we can safely delete the scope
 *		and its symbols.
 */

static void closeParamScope()
{
    Scope *scope;


    scope = closeScope();

    for (auto symbol : scope->symbols())
	delete symbol;

    delete scope;
}


/*
 * Function:	isSpecifier
 *
 * Description:	Return whether the given token is a type specifier.
 */

static bool isSpecifier(int token)
{
    return token == CHAR || token == INT || token == DOUBLE;
}


/*
 * Function:	specifier
 *
 * Description:	Parse a type specifier.  Simple C has only char, int, and
 *		double types.
 *
 *		specifier:
 *		  char
 *		  int
 *		  double
 */

static int specifier()
{
    int typespec = ERROR;


    if (isSpecifier(lookahead)) {
	typespec = lookahead;
	match(lookahead);
    } else
	error();

    return typespec;
}


/*
 * Function:	pointers
 *
 * Description:	Parse pointer declarators (i.e., zero or more asterisks).
 *
 *		pointers:
 *		  empty
 *		  * pointers
 */

static unsigned pointers()
{
    unsigned count = 0;


    while (lookahead == '*') {
	match('*');
	count ++;
    }

    return count;
}


/*
 * Function:	declarator
 *
 * Description:	Parse a declarator, which in Simple C is either a scalar
 *		variable or an array, with optional pointer declarators.
 *
 *		declarator:
 *		  pointers identifier
 *		  pointers identifier [ integer ]
 */

static void declarator(int typespec)
{
    unsigned indirection;
    string name;


    indirection = pointers();
    name = identifier();

    if (lookahead == '[') {
	match('[');
	declareVariable(name, Type(typespec, indirection, integer()));
	match(']');
    } else
	declareVariable(name, Type(typespec, indirection));
}


/*
 * Function:	declaration
 *
 * Description:	Parse a local variable declaration.  Global declarations
 *		are handled separately since we need to detect a function
 *		as a special case.
 *
 *		declaration:
 *		  specifier declarator-list ;
 *
 *		declarator-list:
 *		  declarator
 *		  declarator , declarator-list
 */

static void declaration()
{
    int typespec;


    typespec = specifier();
    declarator(typespec);

    while (lookahead == ',') {
	match(',');
	declarator(typespec);
    }

    match(';');
}


/*
 * Function:	declarations
 *
 * Description:	Parse a possibly empty sequence of declarations.
 *
 *		declarations:
 *		  empty
 *		  declaration declarations
 */

static void declarations()
{
    while (isSpecifier(lookahead))
	declaration();
}


/*
 * Function:	primaryExpression
 *
 * Description:	Parse a primary expression.
 *
 *		primary-expression:
 *		  ( expression )
 *		  identifier ( expression-list )
 *		  identifier ( )
 *		  identifier
 *		  character
 *		  string
 *		  integer
 *		  real
 *
 *		expression-list:
 *		  expression
 *		  expression , expression-list
 */

static Type primaryExpression(bool &lvalue)
{
	int tmp;
	Type result;
	//bool isArray = false;
    if (lookahead == '(') {
	match('(');
	result = expression(lvalue);
	match(')');
	return result;
    } else if (lookahead == CHARACTER) {
	match(CHARACTER);
	tmp = CHAR;
	lvalue = false;
	return Type(INT, 0);
	
    } else if (lookahead == STRING) {
	match(STRING);
	tmp = STRING;
	string stringContent = lexbuf;
	//cout << "lexbuf: " << stringContent << endl;
	lvalue = false;
	return Type(CHAR, 0 , stringContent.size()-2);
    } else if (lookahead == INTEGER) {
	match(INTEGER);
	lvalue = false;
	return Type(INT, 0);
    } else if (lookahead == REAL) {
	match(REAL);
	lvalue = false;
	return Type(DOUBLE, 0);

    } else if (lookahead == ID) {
	Symbol *x = checkIdentifier(identifier());
	//cout << x->name();
	result = checkGetType(x->name());
	//cout << result;
	//result = result.promote();
	/*if(result.isError())
	{
		return Type();
	}*/	
	/*if(result.isScalar())
	{
		cout << "here";	
		lvalue = true;
		return result;
	}
	else if(result.isArray())
	{
		lvalue = false;
		return result;
	}
	else
	{
		result = checkFunctionCall(result);
		lvalue = false;
	}*/
/*	if(result.isScalar())
	{
		//cout << "here\n";
		//cout << "result " << result << "\n";	
		lvalue = true;
		return result;
	}
	else if(result.isArray())
	{
		lvalue = false;
		return result;
	}
	else
	{
		//cout << "T";
		//cout << result;
		result = checkFunctionCall(result);
		lvalue = false;
	}*/
	Parameters *parameters = new Parameters();
	if (lookahead == '(') {
	    match('(');
		//result = checkFunctionCall(result);
		lvalue = false;
	    if (lookahead != ')') {
		parameters->types.push_back(expression(lvalue));

		while (lookahead == ',') {
		    match(',');
		    parameters->types.push_back(expression(lvalue));
		}
	    }
		//result = checkFunctionCall(result, parameters);
		result = checkParameters(parameters, result);
		//cout << "R";
	    match(')');
	}
	
	if(result.isScalar())
	{
		//cout << "here\n";
		//cout << "result " << result << "\n";	
		lvalue = true;
		return result;
	}
	else if(result.isArray())
	{
		lvalue = false;
		return result;
	}
	else
	{
		//cout << "T";
		//cout << result;
		//result = checkFunctionCall(result);
		//lvalue = false;
	}

	return result;
    } else
	{error();}
	return result;
	
}


/*
 * Function:	postfixExpression
 *
 * Description:	Parse a postfix expression.
 *
 *		postfix-expression:
 *		  primary-expression
 *		  postfix-expression [ expression ]
 *		  postfix-expression ++
 *		  postfix-expression --
 */

static Type postfixExpression(bool &lvalue)
{

    Type left = primaryExpression(lvalue);
	Type result;
    while (1) {
	if (lookahead == '[') {
		//checkPostfix(left, "[]", 
	    match('[');
	    result = expression(lvalue);
	    match(']');
		left = checkPostfix(left, "[]", lvalue, result);  
		lvalue = true;
	//	cout << "check indirection greater than 1\n";
	//    cout << "index" << endl;

	} else if (lookahead == INC) {
	    match(INC);
		//cout << "check lvalue\n";
	    //cout << "inc" << endl;
		left = checkPostfix(left, "++", lvalue, result);
		lvalue = false;
	} else if (lookahead == DEC) {
	    match(DEC);
		//cout << "check lvalue\n";
	    //cout << "dec" << endl;
		left = checkPostfix(left, "--", lvalue, result);
		lvalue = false;
	} else
	    break;
    }
	return left;
}


/*
 * Function:	prefixExpression
 *
 * Description:	Parse a prefix expression.
 *
 *		prefix-expression:
 *		  postfix-expression
 *		  ! prefix-expression
 *		  - prefix-expression
 *		  * prefix-expression
 *		  & prefix-expression
 *		  sizeof prefix-expression
 *		  sizeof ( specifier pointers )
 *		  ( specifier pointers ) prefix-expression
 *
 *		This grammar is still ambiguous since "sizeof(type) * n"
 *		could be interpreted as a multiplicative expression or as a
 *		cast of a dereference.  The correct interpretation is the
 *		former, as the latter makes little sense semantically.  We
 *		resolve the ambiguity here by always consuming the "(type)"
 *		as part of the sizeof expression.
 */


//Type checkPrefix(Type &right2, const string &ops, const bool &lvalue, const int &specifier, const int &indirection)

static Type prefixExpression(bool &lvalue)
{
	Type pref;
	int spec;
	int indirection;
    if (lookahead == '!') {
	match('!');
	pref = prefixExpression(lvalue);
	//cout << "not" << endl;
	pref = checkPrefix(pref, "!", lvalue, spec, indirection);
	lvalue = false;
	//checkPredicate(pref);
	
    } else if (lookahead == '-') {
	match('-');
	pref = prefixExpression(lvalue);
	//cout << "neg" << endl;
	pref = checkPrefix(pref, "-", lvalue, spec, indirection);
	lvalue = false;
    } else if (lookahead == '*') {
	match('*');
	pref = prefixExpression(lvalue);
	//cout << "deref" << endl;
	//cout << "Checking type\n";
	pref = checkPrefix(pref, "*", lvalue, spec, indirection);
	lvalue = true;

    } else if (lookahead == '&') {
	match('&');
	pref = prefixExpression(lvalue);
	pref = checkPrefix(pref, "&", lvalue, spec, indirection);
	lvalue = false;
	//cout << "addr" << endl;
	//cout << "check lvalue\n";
	//cout << "check numeric\n";
    } else if (lookahead == SIZEOF) {
	match(SIZEOF);

	if (lookahead == '(' && isSpecifier(peek())) {
	    match('(');
	    spec = specifier();
	    indirection = pointers();
		Type tmp(spec, indirection);
	    match(')');
		pref = checkPrefix(tmp, "sizeof", lvalue, spec, indirection);
	} else
	{
		pref = prefixExpression(lvalue); 
		pref = checkPrefix(pref, "sizeof", lvalue, spec, indirection);
	}
	lvalue = false;
	//cout << "sizeof" << pref << endl;

    } else if (lookahead == '(' && isSpecifier(peek())) { //typecasting
	match('(');
	spec = specifier();
	indirection = pointers();
	match(')');
	pref = prefixExpression(lvalue);
	pref = checkPrefix(pref, "typecast", lvalue, spec, indirection);
	lvalue = false;
	//cout << "cast" << endl;

    } else
	{
		pref = postfixExpression(lvalue);
		//pref = checkPrefix(pref, lvalue, 
	}
	return pref;
}


/*
 * Function:	multiplicativeExpression
 *
 * Description:	Parse a multiplicative expression.
 *
 *		multiplicative-expression:
 *		  prefix-expression
 *		  multiplicative-expression * prefix-expression
 *		  multiplicative-expression / prefix-expression
 *		  multiplicative-expression % prefix-expression
 */
//Type checkLogical(const Type &left2, const Type &right2, const string &ops)

static Type multiplicativeExpression(bool &lvalue)
{
    Type left = prefixExpression(lvalue);

    while (1) {
	if (lookahead == '*') {
	    match('*');
	    Type right = prefixExpression(lvalue);
		//cout << left << "\n * \n" << right << lvalue << '\n';
	    left = checkLogical(left, right, "*");
		//cout << "mul" << endl;

		lvalue = false;
	} else if (lookahead == '/') {
	    match('/');
	    Type right = prefixExpression(lvalue);
	    //cout << left << "\n / \n" << right << lvalue << '\n';
		left = checkLogical(left, right, "/");
		lvalue = false;
		//cout << "div" << endl;

	} else if (lookahead == '%') {
	    match('%');
	    Type right = prefixExpression(lvalue);
	    //cout << "rem" << endl;
		//cout << left << "\n % \n" << right << lvalue << '\n';
		left = checkLogical(left, right, "%");
		lvalue = false;

	} else
	    break;
    }
	return left;
}


/*
 * Function:	additiveExpression
 *
 * Description:	Parse an additive expression.
 *
 *		additive-expression:
 *		  multiplicative-expression
 *		  additive-expression + multiplicative-expression
 *		  additive-expression - multiplicative-expression
 */
//Type checkLogical(const Type &left2, const Type &right2, const string &ops)

static Type additiveExpression(bool &lvalue)
{
    Type left = multiplicativeExpression(lvalue);

    while (1) {
	if (lookahead == '+') {
	    match('+');
	    Type right = multiplicativeExpression(lvalue);
		//cout << left << " + " << right << " " << lvalue << '\n';
		//left = checkAdd(left, right);
	    //cout << "add" << endl;
		left = checkLogical(left, right, "+");
		lvalue = false;
	} else if (lookahead == '-') {
	    match('-');
	    Type right = multiplicativeExpression(lvalue);
	   // cout << left << " - " << right << lvalue << '\n';
		//left = checkAdd(left, right);
		//cout << "sub" << endl;
		left = checkLogical(left, right, "-");
		lvalue = false;
	} else
	    break;
    }
	return left;
}


/*
 * Function:	relationalExpression
 *
 * Description:	Parse a relational expression.  Note that Simple C does not
 *		have shift operators, so we go immediately to additive
 *		expressions.
 *
 *		relational-expression:
 *		  additive-expression
 *		  relational-expression < additive-expression
 *		  relational-expression > additive-expression
 *		  relational-expression <= additive-expression
 *		  relational-expression >= additive-expression
 */

static Type relationalExpression(bool &lvalue)
{
   Type left = additiveExpression(lvalue);

    while (1) {
	if (lookahead == '<') {
	    match('<');
	    Type right = additiveExpression(lvalue);
		left = checkLogical(left, right, "<");
		lvalue = false;
	    //cout << "ltn" << endl;

	} else if (lookahead == '>') {
	    match('>');
	    Type right = additiveExpression(lvalue);
	    left = checkLogical(left, right, ">");
		lvalue = false;
		//cout << "gtn" << endl;

	} else if (lookahead == LEQ) {
	    match(LEQ);
	    Type right = additiveExpression(lvalue);
	    left = checkLogical(left, right, "<=");
		//cout << "leq" << endl;
		lvalue = false;

	} else if (lookahead == GEQ) {
	    match(GEQ);
	    Type right = additiveExpression(lvalue);
		left = checkLogical(left, right, ">=");
		lvalue = false;
	   // cout << "geq" << endl;

	} else
	    break;
    }
	return left;
}


/*
 * Function:	equalityExpression
 *
 * Description:	Parse an equality expression.
 *
 *		equality-expression:
 *		  relational-expression
 *		  equality-expression == relational-expression
 *		  equality-expression != relational-expression
 */

static Type equalityExpression(bool &lvalue)
{
    Type left = relationalExpression(lvalue);
    while (1) {
	if (lookahead == EQL) {
	    match(EQL);
	    Type right = relationalExpression(lvalue);
		left = checkLogical(left, right, "==");
		
		lvalue = false;
		//cout << "lfet " << left.specifier() << endl;
	} else if (lookahead == NEQ) {
	    match(NEQ);
	    Type right = relationalExpression(lvalue);
	    //cout << "neq" << endl;
		left = checkLogical(left, right, "!=");
		lvalue = false;
	} else
	    break;
    }
	//cout << "D";
	//cout <<left;
	return left;
}


/*
 * Function:	logicalAndExpression
 *
 * Description:	Parse a logical-and expression.  Note that Simple C does
 *		not have bitwise-and expressions.
 *
 *		logical-and-expression:
 *		  equality-expression
 *		  logical-and-expression && equality-expression
 */

static Type logicalAndExpression(bool &lvalue)
{
   Type left = equalityExpression(lvalue);

    while (lookahead == AND) {
	match(AND);
	Type right = equalityExpression(lvalue);
	//cout << "and" << endl;
	left = checkLogical(left, right, "&&");
	lvalue = false;
    }
	return left;
}


/*
 * Function:	expression
 *
 * Description:	Parse an expression, or more specifically, a logical-or
 *		expression, since Simple C does not allow comma or
 *		assignment as an expression operator.
 *
 *		expression:
 *		  logical-and-expression
 *		  expression || logical-and-expression
 */

static Type expression(bool &lvalue)
{
    Type left = logicalAndExpression(lvalue);

    while (lookahead == OR) {
	match(OR);
	Type right = logicalAndExpression(lvalue);
	//cout << "or" << endl;
	left = checkLogical(left, right, "||");
	lvalue = false;
    }
	return left;
}


/*
 * Function:	statements
 *
 * Description:	Parse a possibly empty sequence of statements.  Rather than
 *		checking if the next token starts a statement, we check if
 *		the next token ends the sequence, since a sequence of
 *		statements is always terminated by a closing brace.
 *
 *		statements:
 *		  empty
 *		  statement statements
 */

static void statements()
{
    while (lookahead != '}')
	statement();
}


/*
 * Function:	Assignment
 *
 * Description:	Parse an assignment statement.
 *
 *		assignment:
 *		  expression = expression
 *		  expression
 */

static void assignment(bool &lvalue)
{
	//bool leftLValue;
	//bool rightLValue;
    Type left = expression(lvalue);
	Type right;
	if(lvalue == false && lookahead == '=')
	{
		//E4
		cout << 2;
		report(E4);
	}
    if (lookahead == '=') {
	match('=');
	right = expression(lvalue);
    }
	//cout << left << endl;
	//cout << right << endl;
	if(left.isError() || right.isError())
	{
		//cout << "Z";
		return;
	}
	if(left.isCompatibleWith(right) == false)
	{
		//E5
		report(E5, "=");
	}
}


/*
 * Function:	statement
 *
 * Description:	Parse a statement.  Note that Simple C has so few
 *		statements that we handle them all in this one function.
 *
 *		statement:
 *		  { declarations statements }
 *		  break ;
 *		  return expression ;
 *		  while ( expression ) statement
 *		  for ( assignment ; expression ; assignment ) statement
 *		  if ( expression ) statement
 *		  if ( expression ) statement else statement
 *		  assignment ;
 *
 *		This grammar still suffers from the "dangling-else"
 *		ambiguity.  We resolve it here by always consuming the
 *		"else" token, thus matching an else with the closest
 *		unmatched if.
 */

static int counter = 0;

static void statement()
{
	bool lvalue;
	//int counter = 0;
    if (lookahead == '{') {
	match('{');
	openScope();
	declarations();
	statements();
	closeScope();
	match('}');

    } else if (lookahead == BREAK) {
	

	if(counter <= 0)
	{
		//E1
		report(E1);
		//cout << "counter : " << counter <<endl;
	}
	match(BREAK);
	match(';');

    } else if (lookahead == RETURN) {
	match(RETURN);

	Type returnResult = expression(lvalue);
	if(functionName.isCompatibleWith(returnResult) == false)
	{
		//E2
		report(E2);
	}
	match(';');

    } else if (lookahead == WHILE) {
	match(WHILE);
	match('(');
	Type result = expression(lvalue);
	if(result.isPredicate() == false)
	{
		//E3
		report(E3);
	}
	match(')');
	counter++;
	statement();
	counter--;
    } else if (lookahead == FOR) {
	match(FOR);
	match('(');
	assignment(lvalue);
	match(';');
	Type result = expression(lvalue);
	if(result.isPredicate() == false)
	{
		//E3
		report(E3);
	}
	match(';');
	assignment(lvalue);
	match(')');
	counter++;
	statement();
	counter--;
    } else if (lookahead == IF) {
	match(IF);
	match('(');
	Type result = expression(lvalue);
	if(result.isPredicate() == false)
	{
		//E3
		report(E3);
	}
	match(')');
	statement();

	if (lookahead == ELSE) {
	    match(ELSE);
	    statement();
	}

    } else {
	assignment(lvalue);
	match(';');
    }
}


/*
 * Function:	parameter
 *
 * Description:	Parse a parameter, which in Simple C is always a scalar
 *		variable with optional pointer declarators.
 *
 *		parameter:
 *		  specifier pointers identifier
 */

static Type parameter()
{
    int typespec;
    unsigned indirection;
    string name;
    Type type;


    typespec = specifier();
    indirection = pointers();
    name = identifier();

    type = Type(typespec, indirection);
    declareVariable(name, type);
    return type;
}


/*
 * Function:	parameters
 *
 * Description:	Parse the parameters of a function, but not the opening or
 *		closing parentheses.
 *
 *		parameters:
 *		  void
 *		  parameter-list
 *		  parameter-list , ...
 */

static Parameters *parameters()
{
    Parameters *params;


    openScope();
    params = new Parameters;
    params->variadic = false;

    if (lookahead == VOID)
	match(VOID);

    else {
	params->types.push_back(parameter());

	while (lookahead == ',') {
	    match(',');

	    if (lookahead == ELLIPSIS) {
		params->variadic = true;
		match(ELLIPSIS);
		break;
	    }

	    params->types.push_back(parameter());
	}
    }

    return params;
}


/*
 * Function:	globalDeclarator
 *
 * Description:	Parse a declarator, which in Simple C is either a scalar
 *		variable, an array, or a function, with optional pointer
 *		declarators.
 *
 *		global-declarator:
 *		  pointers identifier
 *		  pointers identifier [ integer ]
 *		  pointers identifier ( parameters )
 */

static void globalDeclarator(int typespec)
{
    unsigned indirection;
    string name;


    indirection = pointers();
    name = identifier();

    if (lookahead == '[') {
	match('[');
	declareVariable(name, Type(typespec, indirection, integer()));
	match(']');

    } else if (lookahead == '(') {
	match('(');
	declareFunction(name, Type(typespec, indirection, parameters()));
	closeParamScope();
	match(')');

    } else
	declareVariable(name, Type(typespec, indirection));
}


/*
 * Function:	remainingDeclarators
 *
 * Description:	Parse any remaining global declarators after the first.
 *
 * 		remaining-declarators
 * 		  ;
 * 		  , global-declarator remaining-declarators
 */

static void remainingDeclarators(int typespec)
{
    while (lookahead == ',') {
	match(',');
	globalDeclarator(typespec);
    }

    match(';');
}


/*
 * Function:	topLevelDeclaration
 *
 * Description:	Parse a global declaration or function definition.
 *
 * 		global-or-function:
 * 		  specifier pointers identifier remaining-decls
 * 		  specifier pointers identifier [ integer ] remaining-decls
 * 		  specifier pointers identifier ( parameters ) remaining-decls 
 * 		  specifier pointers identifier ( parameters ) { ... }
 */

static void topLevelDeclaration()
{
    int typespec;
    unsigned indirection;
    Parameters *params;
    string name;


    typespec = specifier();
    indirection = pointers();
    name = identifier();

    if (lookahead == '[') {
	match('[');
	declareVariable(name, Type(typespec, indirection, integer()));
	match(']');
	remainingDeclarators(typespec);

    } else if (lookahead == '(') {
	match('(');
	params = parameters();
	match(')');

	if (lookahead == '{') {
		Type tmp(typespec, indirection);
		functionName = tmp;
	    defineFunction(name, Type(typespec, indirection, params));
	    match('{');
	    declarations();
	    statements();
	    closeScope();
	    match('}');

	} else {
	    closeParamScope();
	    declareFunction(name, Type(typespec, indirection, params));
	    remainingDeclarators(typespec);
	}

    } else {
	declareVariable(name, Type(typespec, indirection));
	remainingDeclarators(typespec);
    }
}


/*
 * Function:	main
 *
 * Description:	Analyze the standard input stream.
 */

int main()
{
    openScope();
    lookahead = yylex();

    while (lookahead != DONE)
	topLevelDeclaration();

    closeScope();
    exit(EXIT_SUCCESS);
}
