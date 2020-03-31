/*
 * File:	checker.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the semantic checker for Simple C.
 *
 *		If a symbol is redeclared, the redeclaration is discarded
 *		and the original declaration is retained.
 *
 *		Extra functionality:
 *		- inserting an undeclared symbol with the error type
 */

# include <iostream>
# include <unordered_set>
# include "lexer.h"
# include "checker.h"
# include "tokens.h"
# include "Symbol.h"
# include "Scope.h"
# include "Type.h"


using namespace std;

static unordered_set<string> defined;
static Scope *outermost, *toplevel;
static const Type error;
//static string functionName;


/*static string redefined = "redefinition of '%s'";
static string redeclared = "redeclaration of '%s'";
static string conflicting = "conflicting types for '%s'";
static string undeclared = "'%s' undeclared";
*/

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
 * Function:	openScope
 *
 * Description:	Create a scope and make it the new top-level scope.
 */

Scope *openScope()
{
    toplevel = new Scope(toplevel);

    if (outermost == nullptr)
	outermost = toplevel;

    return toplevel;
}


/*
 * Function:	closeScope
 *
 * Description:	Remove the top-level scope, and make its enclosing scope
 *		the new top-level scope.
 */

Scope *closeScope()
{
    Scope *old = toplevel;
    toplevel = toplevel->enclosing();
    return old;
}


/*
 * Function:	defineFunction
 *
 * Description:	Define a function with the specified NAME and TYPE.  A
 *		function is always defined in the outermost scope.
 */

Symbol *defineFunction(const string &name, const Type &type)
{
    if (defined.count(name) > 0) {
	//report(redefined, name);
	return outermost->find(name);
    }

    defined.insert(name);
    return declareFunction(name, type);
}


/*
 * Function:	declareFunction
 *
 * Description:	Declare a function with the specified NAME and TYPE.  A
 *		function is always declared in the outermost scope.  Any
 *		redeclaration is discarded.
 */

Symbol *declareFunction(const string &name, const Type &type)
{
    //cout << name << ": " << type << endl;
    Symbol *symbol = outermost->find(name);

    if (symbol == nullptr) {
	symbol = new Symbol(name, type);
	outermost->insert(symbol);

    } else if (type != symbol->type()) {
	//report(conflicting, name);
	delete type.parameters();

    } else
	delete type.parameters();

    return symbol;
}


/*
 * Function:	declareVariable
 *
 * Description:	Declare a variable with the specified NAME and TYPE.  Any
 *		redeclaration is discarded.
 */

Symbol *declareVariable(const string &name, const Type &type)
{
    //cout << name << ": " << type << endl;
    Symbol *symbol = toplevel->find(name);

    if (symbol == nullptr) {
	symbol = new Symbol(name, type);
	toplevel->insert(symbol);

    } else if (outermost != toplevel)
	{
		//report(redeclared, name);
	}

    else if (type != symbol->type())
	{
		//report(conflicting, name);
	}

    return symbol;
}


/*
 * Function:	checkIdentifier
 *
 * Description:	Check if NAME is declared.  If it is undeclared, then
 *		declare it as having the error type in order to eliminate
 *		future error messages.
 */

Symbol *checkIdentifier(const string &name)
{
    Symbol *symbol = toplevel->lookup(name);

    if (symbol == nullptr) {
	//report(undeclared, name);
	symbol = new Symbol(name, error);
	toplevel->insert(symbol);
    }

    return symbol;
}

/*Type add(bool &lvalue)
{
	Type left = mult();

	Type right = mult();
	std::cout << left << "+" << right << lvalue;

	return left;
}*/

/*Type checkAdd(Type &left, Type &right)
{

}*/

Type checkLogical(const Type &left2, const Type &right2, const string &ops)
{	
	if(left2.isError() || right2.isError())
	{
		//cout << "Q";
		return Type();
	}

	Type left = left2.promote();
	Type right = right2.promote();
	

	if(ops == "||" || ops == "&&")
	{

		if(left.isPredicate() && right.isPredicate())
		{
			return Type(INT, 0);
		}
		else
		{
			//E5
			report(E5, ops);
			return Type();
		}
	}
	else if(ops == "==" || ops == "!=" || ops == ">=" || ops == "<=" || ops == ">" || ops == "<")
	{
		if(left.isCompatibleWith(right))
		{
			//cout << "K";
			//cout << ops;
			//cout << left << right;
			Type result(INT, 0);
			//cout << result;
			return result;
		}
		else
		{
			report(E5, ops);
			return Type();
		}	
	}
	else if(ops == "+" || ops == "-")
	{
		//cout << left;
		//cout << right;
		if(left.isNumeric() && right.isNumeric() && left.indirection() == 0 && right.indirection() == 0)
		{	
			if(left.specifier() == INT && right.specifier() == INT)
			{
				return Type(INT, 0);	
			}
			else if(left.specifier() == DOUBLE || right.specifier() == DOUBLE)
			{
				return Type(DOUBLE, 0);
			}
			else
			{
				//E5
				report(E5, ops);
				return Type();
			}
		}
		//else if(left.isNumeric() && right.isNumeric())
		//{
		else if(left.indirection() > 0 && right.indirection() == 0 && right.specifier() == INT && right.isFunction() == false && left.isFunction() == false)
		{
			return Type(left.specifier(), left.indirection());
		}
		else if(ops == "+" && right.indirection() > 0 && left.indirection() == 0 && left.specifier() == INT && right.isFunction() == false && left.isFunction() == false)
		{
			return Type(right.specifier(), right.indirection());
		}
		else if(ops == "-" && left.specifier() == right.specifier() && left.indirection() > 0 && right.indirection() > 0 && right.isFunction() == false && left.isFunction() == false)
		{
			return Type(INT, 0);
		}
		else
		{
			//E5
			report(E5, ops);
			return Type();
		}
		
		/*else
		{
			//E5
			report(E5, ops);
			return Type();
		}*/
	}
	else if(ops == "*" || ops == "/" || ops == "%")
	{
		if(ops == "*" || ops == "/")
		{
			if(left.isNumeric() && right.isNumeric() && (left.specifier() == DOUBLE || right.specifier() == DOUBLE))
			{
				return Type(DOUBLE, 0);
			}
			else if(left.isNumeric() && right.isNumeric())
			{
				return Type(INT, 0);
			}
			else
			{
				//E5
				report(E5, ops);
				return Type();
			}
		}
		else if(ops == "%")
		{
			if(left.specifier() == INT && right.specifier() == INT && left.isScalar() && right.isScalar())
			{
				return Type(INT, 0);
			}
			else
			{
				//E5
				report(E5, ops);
				return Type();
			}
		}
		else
		{
			//E5
			return Type();
		}
	}
	//cout << "P";
	return Type();
}

Type checkPrefix(Type &right2, const string &ops, bool &lvalue, const int &specifier, const int &indirection)
{
	if(right2.isError())
	{
		//cout << "L";
		return Type();
	}
	Type right = right2.promote(); 
	if(ops == "*")
	{
		if(right.indirection() > 0)
		{
			if(right.isArray())
			{
				//cout << "This shouldn't ever happen\n";
				return Type(right.specifier(), right.indirection() - 1, right.length());
			}
			else if(right.isFunction())
			{
				return Type(right.specifier(), right.indirection()-1, right.parameters()); 
			}
			else if(right.isScalar())
			{
				return Type(right.specifier(), right.indirection() - 1);
			}
			else
			{
				//Error E6
				report(E6, ops);
				return Type();
			}

		}
		else
		{
			//E6
			report(E6, ops);
			return Type();

		}
	}
	else if(ops == "&")
	{
		if(lvalue == true)
		{
			return Type(right2.specifier(), right2.indirection() + 1);
		}
		//Can functions and arrays be lvalues
		else
		{
			//E4

			report(E4);
			return Type();
		}
	}
	else if(ops == "!")
	{
		if(right2.isPredicate())
		{
			//Weird
			return Type(INT, 0);
		}
		else
		{
			//E6
			report(E6, ops);
			return Type();
		}
	}
	else if(ops == "-")
	{
		if(right.isNumeric())
		{
			return right;
		}
		else
		{
			//E6
			report(E6, ops);
			return Type();
		}
	}
	else if(ops == "sizeof")
	{
		if(right2.isFunction() == false)
		{
			return Type(INT, 0);
		}
		else
		{
			//E7
			report(E7);
			return Type();
		}
	}
	else if(ops == "typecast")
	{
		Type typecast(specifier, indirection);
		if(typecast.isError() || right2.isError())
		{
			return Type();
		}
		if( (right2.isNumeric() && typecast.isNumeric()) || (right2.indirection() > 0 && typecast.indirection() > 0) 
				|| ( right2.specifier() == INT && right2.indirection() == 0 && typecast.indirection() > 0)
		 		|| ( typecast.specifier() == INT && typecast.indirection() == 0 && right2.indirection() > 0)    ) 
		{
			return Type(specifier, indirection);
		}
		else
		{
			//E8
			report(E8);
			return Type();
		}
	}
	else
	{
		//cout << "what did you do\n";
	}
	//cout << "d";
		return Type();
	
}

Type checkPostfix(const Type &left2, const string & ops, const bool &lvalue, const Type &expr)
{
	if(left2.isError())
	{
		//cout << "M";
		return Type();
	}
	Type left = left2.promote();	
	Type expr2 = expr.promote();
	if(ops == "--" || ops == "++")
	{
		if(lvalue == true)
		{
			return left2;
		}
		else
		{
			//E4
			report(E4);
			return Type();
		}
	}
	else if(ops == "[]")
	{
		if(expr2.isInteger() && expr2.indirection() == 0 && left.indirection() > 0)
		{
			Type r = Type(left.specifier(), left.indirection() - 1);
			//r = r.promote();
			return r;//Type(left.specifier(), left.indirection() - 1);
		}
		else
		{
			//E5
			report(E5, ops);
			return Type();
		}
	}
	else
	{
		//cout << "what did you do\n";
		return Type();
	}
	return Type();
}	

Type checkGetType(const string &idName)
{
	Symbol *symbol = checkIdentifier(idName);
	return symbol->type();
}


/*Type checkFunctionCall(const Type &func, const Parameters *params)
{
	if(func.isFunction())
	{
		return Type(func.specifier(), func.indirection(), func.params);
	}
	cout << "F";
	return Type();
}*/
Type checkParameters(Parameters *params, const Type & idName)
{
	unsigned int i;
	/*if(idName.isError())
	{
		return Type();
	}*/
	/*for(i = 0 ; i < params->types.size(); i++)
	{
		if(params->types[i].isError())
		{
			return Type();
		}
		params->types[i] = params->types[i].promote();
		if(params->types[i].isPredicate() == false)
		{
			report(E10);
			
		}
	}*/
//pushback problems?	
//cout << idName;
	if(idName.isFunction())
	{
		for(i = 0 ; i < params->types.size(); i++)
		{
			/*if(params->types[i].isError())
			{
				return Type();
			}*/
			params->types[i] = params->types[i].promote();
			if(params->types[i].isPredicate() == false)
			{
				report(E10);
				//cout << "A";	
				return Type();
			}
		}
		Parameters *declParams = idName.parameters();
		//cout << "decl: " << declParams->types.size();
		//cout << "params: " << params->types.size();

		if(declParams->variadic == true)
		{
			if(declParams->types.size() <= params->types.size())
			{
				unsigned int k;
				for(k = 0; k < declParams->types.size(); k++)
				{

					if(declParams->types[k].isCompatibleWith(params->types[k]) == false)
					{
						//E10
						cout << "R";
						report(E10);
						//cout << "Decl param: " << declParams->types[k] << endl;
						//cout << "param: " << params->types[k] << endl;
						return Type();
					}
				}
				return Type(idName.specifier(), idName.indirection());
			}
			else
			{
				//E10
				//cout << "T";
				report(E10);
				return Type();
			}

		}
		else
		{
			if(declParams->types.size() == params->types.size())
			{
				unsigned int j;
				for(j = 0; j < params->types.size() ; j++)
				{
					

					if(declParams->types[j].isCompatibleWith(params->types[j]) == false)
					{
						//E10

						report(E10);
						//You can have multiple	
						return Type();
					}
				}
				//cout << "P";
				return Type(idName.specifier(), idName.indirection());
			}
			else
			{
				//E10
				report(E10);
				return Type();
			}
		}
	}
	else
	{
		//cout << "fasdf";
		report(E9);
	}
return Type();	
	
}


/*Type checkLogicalEquality(Type &left, Type &right)
{
	return left.isCompatibleWith(right);
}*/

