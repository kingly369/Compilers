/*
 * File:	parser.c
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the recursive-descent parser for
 *		Simple C.
 */

//#define _GLIBCXX_USE_C99 1

# include <cstdlib>
# include <iostream>
# include <string>
# include "tokens.h"
# include "lexer.h"
# include "checker.h"
# include "scope.h"
# include "type.h"
# include "symbol.h"

using namespace std;

/*struct Parameters{

	bool variadic;
	std::Vector <class Type> types;
}

class Type
{
	unsigned _indirection;
	unsigned length;
	short _declarator, _specifier;
	//Parameters *_parameters;
	public:
		Type();
		Type(int specifer, 

}
*/

//class checker;

checker myChecker;

static void expression();
static void statement();
static int lookahead, nexttoken;
static string lexbuf, nextbuf;


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

int specifier()
{
	int typespec = lookahead;
	
	if(lookahead == INT || lookahead == DOUBLE || lookahead == CHAR)
	{
		match(lookahead);
	}

	return typespec;


}

/*static void specifier()
{
	if (isSpecifier(lookahead))
	match(lookahead);
    else
	error();
	

}*/


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
	unsigned counter = 0;
    while (lookahead == '*')
	{
		match('*');
		counter++;
	}
	return counter;
}

static string identifier()
{
	string name = lexbuf;
	//cout << "asf\n";
	match(ID);
	//cout << "fasdf\n";
	return name;	
}

static unsigned integer()
{
	string name = lexbuf;
	match(INTEGER);
	return (unsigned) stoi(name);
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
//local variable
static void declarator(int typespec)
{
   unsigned indirection = pointers();
    //match(ID);
	string name = identifier();
    if (lookahead == '[') {
	match('[');
	unsigned length = integer();
	//match(INTEGER);
	match(']');
	//cout << "(" << typespec << ", " << name << ", "<< indirection << ", ARRAY)\n";
    Type arr(typespec, indirection, length);
	myChecker.declareVariable(name, arr);

	}
	else
	{
	//cout << "(" << typespec << ", " << name << ", " << indirection << ", SCALAR)\n";
	Type scalar(typespec, indirection);
	myChecker.declareVariable(name, scalar);	
	//cout << *(myChecker.declareVariable(name, scalar));
	}
	
	//return typespec;
}


/*
 * Function:	declaration
 *
 * Description:	Parse a local variable declaration.  Global declarations
 *		are handled separately since we need to detect a function
 *		as a special case.
 *
 *		declaration:
 *		  specifier declarator-list ';'
 *
 *		declarator-list:
 *		  declarator
 *		  declarator , declarator-list
 */

static void declaration()
{
	//cout << "open declaration\n";
    int typespec = specifier();
    declarator(typespec);
	//Type tmp(typespec, 
    while (lookahead == ',') {
	//cout << "open declaration\n";
	match(',');
	declarator(typespec);
	//cout << "close declaration\n";
    }
	//cout << "close declaration\n";
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

static void primaryExpression()
{
    if (lookahead == '(') {
	match('(');
	expression();
	match(')');

    } else if (lookahead == CHARACTER) {
	match(CHARACTER);

    } else if (lookahead == STRING) {
	match(STRING);

    } else if (lookahead == INTEGER) {
	match(INTEGER);

    } else if (lookahead == REAL) {
	match(REAL);

    } else if (lookahead == ID) {
	//match(ID);
	string name = identifier();
	myChecker.checkID(name);

	if (lookahead == '(') {
	    match('(');

	    if (lookahead != ')') {
		expression();

		while (lookahead == ',') {
		    match(',');
		    expression();
		}
	    }

	    match(')');
	}

    } else
	error();
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

static void postfixExpression()
{
    primaryExpression();

    while (1) {
	if (lookahead == '[') {
	    match('[');
	    expression();
	    match(']');
	    //cout << "index" << endl;

	} else if (lookahead == INC) {
	    match(INC);
	    //cout << "inc" << endl;

	} else if (lookahead == DEC) {
	    match(DEC);
	    //cout << "dec" << endl;

	} else
	    break;
    }
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

static void prefixExpression()
{
    if (lookahead == '!') {
	match('!');
	prefixExpression();
	//cout << "not" << endl;

    } else if (lookahead == '-') {
	match('-');
	prefixExpression();
	//cout << "neg" << endl;

    } else if (lookahead == '*') {
	match('*');
	prefixExpression();
	//cout << "deref" << endl;

    } else if (lookahead == '&') {
	match('&');
	prefixExpression();
	//cout << "addr" << endl;

    } else if (lookahead == SIZEOF) {
	match(SIZEOF);

	if (lookahead == '(' && isSpecifier(peek())) {
	    match('(');
	    specifier();
	    pointers();
	    match(')');
	} else
	    prefixExpression();

	//cout << "sizeof" << endl;

    } else if (lookahead == '(' && isSpecifier(peek())) {
	match('(');
	specifier();
	pointers();
	match(')');
	prefixExpression();
	//cout << "cast" << endl;

    } else
	postfixExpression();
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

static void multiplicativeExpression()
{
    prefixExpression();

    while (1) {
	if (lookahead == '*') {
	    match('*');
	    prefixExpression();
	    //cout << "mul" << endl;

	} else if (lookahead == '/') {
	    match('/');
	    prefixExpression();
	    //cout << "div" << endl;

	} else if (lookahead == '%') {
	    match('%');
	    prefixExpression();
	    //cout << "rem" << endl;

	} else
	    break;
    }
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

static void additiveExpression()
{
    multiplicativeExpression();

    while (1) {
	if (lookahead == '+') {
	    match('+');
	    multiplicativeExpression();
	    //cout << "add" << endl;

	} else if (lookahead == '-') {
	    match('-');
	    multiplicativeExpression();
	    //cout << "sub" << endl;

	} else
	    break;
    }
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

static void relationalExpression()
{
    additiveExpression();

    while (1) {
	if (lookahead == '<') {
	    match('<');
	    additiveExpression();
	    //cout << "ltn" << endl;

	} else if (lookahead == '>') {
	    match('>');
	    additiveExpression();
	    //cout << "gtn" << endl;

	} else if (lookahead == LEQ) {
	    match(LEQ);
	    additiveExpression();
	    //cout << "leq" << endl;

	} else if (lookahead == GEQ) {
	    match(GEQ);
	    additiveExpression();
	    //cout << "geq" << endl;

	} else
	    break;
    }
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

static void equalityExpression()
{
    relationalExpression();

    while (1) {
	if (lookahead == EQL) {
	    match(EQL);
	    relationalExpression();
	    //cout << "eql" << endl;

	} else if (lookahead == NEQ) {
	    match(NEQ);
	    relationalExpression();
	    //cout << "neq" << endl;

	} else
	    break;
    }
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

static void logicalAndExpression()
{
    equalityExpression();

    while (lookahead == AND) {
	match(AND);
	equalityExpression();
	//cout << "and" << endl;
    }
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

static void expression()
{
    logicalAndExpression();

    while (lookahead == OR) {
	match(OR);
	logicalAndExpression();
	//cout << "or" << endl;
    }
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
	//cout << "open statements\n";
    while (lookahead != '}')
	statement();
	//cout << "close statements\n";
	myChecker.closeScope();
	
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

static void assignment()
{
    expression();

    if (lookahead == '=') {
	match('=');
	expression();
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

static void statement()
{
    if (lookahead == '{') {
	//cout << "open statment\n";
	myChecker.openScope();
	match('{');
	declarations();
	statements();
	match('}');
	//cout << "close statement\n";
	//myChecker.closeScope();
    } else if (lookahead == BREAK) {
	match(BREAK);
	match(';');

    } else if (lookahead == RETURN) {
	match(RETURN);
	Type result= expression();
	match(';');

    } else if (lookahead == WHILE) {
	//cout << "open statement\n";
	match(WHILE);
	match('(');
	expression();
	match(')');
	statement();
	//cout << "close statement\n";
    } else if (lookahead == FOR) {
	//cout << "open statement\n";
	match(FOR);
	match('(');
	assignment();
	match(';');
	expression();
	match(';');
	assignment();
	match(')');
	statement();
	//cout << "close statement\n";

    } else if (lookahead == IF) {
	//cout << "open statement\n";
	match(IF);
	match('(');
	expression();
	match(')');
	statement();
	//cout << "close statement\n";

	if (lookahead == ELSE) {
	    match(ELSE);
	    statement();
	}

    } else {
	assignment();
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
    int typespec = specifier();
    unsigned indirection = pointers();
	string name = identifier();
    //match(ID);
	Type tmp(typespec, indirection);
	myChecker.declareVariable(name, tmp);
	return tmp;
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

static Parameters * parameters()
{
	//cout << "open params\n";
	Parameters *params = new Parameters;
    if (lookahead == VOID)
	match(VOID);

    else {
	//cout << "open decl\n";
	Type tmp = parameter();
	params->types.push_back(tmp);
	while (lookahead == ',') {
	    match(',');

	    if (lookahead == ELLIPSIS) {
		match(ELLIPSIS);
		params->variadic = true;
		break;
	    }

	    Type tmp2 = parameter();
		params->types.push_back(tmp2);
	}
	//cout << "close params\n";
    }
	//cout << "close params\n";
	//myChecker.closeScope();
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
	//cout << "in global decl\n";
    unsigned indirection = pointers();
    //match(ID);
	string name = identifier();
    if (lookahead == '[') {
	match('[');
	//match(INTEGER);
	unsigned length = integer();
	match(']');
	Type arr(typespec, indirection, length);
	//cout << length << '\n';
	myChecker.declareVariable(name, arr);
    } else if (lookahead == '(') {
	//cout << "open global\n";
	match('(');
	///Type arr(INT, 0, 10);
	//openScope();
	Parameters * params = parameters();
	Type func(typespec, indirection, params);
	myChecker.declareFunc(name, func);
	match(')');
	//cout << "close decl\n";
	//cout << "BOSING\n";
	//myChecker.closeScope();
    }
	else
	{
		Type scalar(typespec, indirection);
		myChecker.declareVariable(name, scalar);
	}
	//return params;
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
	//cout << "fasdfs\n";
    while (lookahead == ',') {
	match(',');
	//cout << "open decl\n";
	globalDeclarator(typespec);
	//cout << "close decl\n";
    }
	//cout << "close decl\n";
	//myChecker.closeScope();
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
	//cout << "open global\n";
    int typespec = specifier();
    unsigned indirection = pointers();
    string name = identifier();
	//match(ID);

    if (lookahead == '[') {
	match('[');
	unsigned length = integer();
	Type arr(typespec, indirection, length);
	//cout << arr;
	myChecker.declareVariable(name, arr);
	//match(INTEGER);
	match(']');
	remainingDeclarators(typespec);

    } else if (lookahead == '(') {
	//cout << "open function\n";
	match('(');
	myChecker.openScope();
	Parameters *params = parameters(); //first func
	//myChecker.closeScope();
	Type tmp(typespec, indirection, params);
	myChecker.declareFunc(name, tmp); //should go in global
	if(peek() == SEMI)
	{
		//cout << "CLOSING \n";
		myChecker.closeScope();
	}
	//myChecker.closeScope();

	match(')');

	if (lookahead == '{') {
	    myChecker.defineFunc(name, tmp); 
		match('{');
		//myChecker.defineFunc(name, tmp); 
	    //myChecker.openScope();
		
		declarations();
	    statements();
	    match('}');
		//cout << "close function\n";

	} else
	    remainingDeclarators(typespec);
		/*if(peek() == SEMI)
		{
			cout << "CLOSING\n";
			myChecker.closeScope();
		}*/
    } 
	
	else //scalar
	{
		Type x(typespec, indirection);
		//cout << x << '\n';
		myChecker.declareVariable(name, x);
		remainingDeclarators(typespec);
	}
		//cout << "close global\n";
}


/*
 * 
 * Function:	main
 *
 * Description:	Analyze the standard input stream.
 */

int main()
{
    lookahead = yylex();
	//cout << "open global\n";
	myChecker.openScope();
    while (lookahead != DONE)
	topLevelDeclaration();
	//cout << "close global\n";
	myChecker.closeScope();
    exit(EXIT_SUCCESS);
	
/*	Type x(INT, 0);
	Type y(INT, 0, 10);
	Type xx(CHAR, 0);
	Type doubleX(DOUBLE, 1);
	//std::vector<Type> types;
	//types.push_back(x);
	Parameters *tmp = new Parameters();
	tmp->types.push_back(x);
	tmp->variadic = true;
	Parameters *tmp2 = new Parameters();
	tmp2->types.push_back(x);
	tmp2->types.push_back(y);
	Parameters *tmp3 = new Parameters();
	


	Parameters *tmp4 = new Parameters();
	tmp4->types.push_back(x);

	
	Parameters *tmp5 = new Parameters();
	tmp5->types.push_back(x);
	tmp5->types.push_back(x);
	tmp5->variadic = true;

	Parameters *tmp6 = new Parameters();
	tmp6->types.push_back(x);
	//tmp5->types.push_back(x);
	tmp6->variadic = true;



	Type z(INT, 0, tmp); //functions
	Type a(INT, 0, tmp2); //functions
	Type vv(INT, 0, tmp5);
	Type vv2(INT, 0, tmp6);
	
	Type arr(INT, 0, 10);
	Type arr2(INT, 0, 5);
	Type g(INT, 0, tmp3);
	Type g2(INT, 1, tmp3);
	Type f(INT, 1, tmp4);

	symbol *tmpSymbol = new symbol(z,"john");
	symbol *tmpSymbol2 = new symbol(a, "cait");
	//symbol *tmpSymbol3 = new symbol(g, 
	scope *newScope = new scope(NULL);
	newScope->insert(tmpSymbol);
	newScope->insert(tmpSymbol2);
	scope *localScope = new scope(newScope);
	myChecker.openScope();
	cout << *(myChecker.declareVariable("x", x));
	cout << *myChecker.declareVariable("x", x);
	cout << *myChecker.declareFunc("v", z);
	cout << *myChecker.declareFunc("v", a);
	cout << *myChecker.declareFunc("v", z);

	cout << *myChecker.declareVariable("a", arr);
	cout << *myChecker.declareVariable("a", arr2);
	cout << *myChecker.declareVariable("x", doubleX);
	
   	cout << *myChecker.declareFunc("g", g); 
	cout << *myChecker.declareFunc("g", g2);

	cout << *myChecker.declareFunc("f", f);
	cout << *myChecker.defineFunc("f", f);

	cout << *myChecker.declareVariable("f", x);

	cout << *myChecker.declareVariable("h", x);
	cout << *myChecker.declareVariable("h", g);

	cout << *myChecker.defineFunc("vv", vv);
   	cout << *myChecker.declareFunc("vv", vv);
	cout << *myChecker.declareFunc("vv", vv2);
*/

}
