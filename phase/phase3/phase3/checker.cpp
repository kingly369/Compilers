#include "string.h"
#include <iostream>
#include <cassert>
#include "scope.h"
#include "symbol.h"
#include "type.h"
#include "checker.h"
#include "lexer.h"

//#include <unordered_set>


//using namespace std;

typedef std::vector<string> unorderedVector;

unorderedVector defined;

static string E3 = "redeclaration of \'%s\'";
static string E2 = "redefinition of \'%s\'";
static string E1 = "conflicting types for \'%s\'";
static string E4 = "\'%s\' undeclared";

scope *topLevel = NULL;
scope *currentLevel = NULL;

void checker::openScope()
{
	//cout << "Opening Scope\n";
	if(topLevel == NULL)
	{
		topLevel = new scope(NULL);
		currentLevel = topLevel;
	}
	else
	{
		scope* newScope = new scope(currentLevel);
		currentLevel = newScope;
	}
}

void checker::closeScope()
{
	//cout << "Closing Scope\n";
	if(currentLevel->enclosing() == NULL)
	{
		//int i;
		
		/*for(i = 0; i < currentLevel->getSymbols().size() ; i++)
		{
			delete 
		}*/

		delete currentLevel;
		currentLevel = NULL;
		//currentLevel->_enclosing = NULL;
		topLevel = NULL;
	}

	else
	{	
		//delete currentLevel->getParamter
		scope * newScope = currentLevel->enclosing();

		delete currentLevel;	
		currentLevel = newScope;
	}
	//cout << "SCOPE CLOSED\n";
	//cout << currentLevel << '\n';	
}

symbol * checker::declareFunc(const string &name, const Type &type)
{
	//cout << "Declaring Function\n";
	symbol * sym = topLevel->find(name);

	if(sym == NULL)
	{
		/*if(checkID() == false)
		{
			cout << "undeclared function \n";
		}
		else*/
		//{
			sym = new symbol(type, name);
			topLevel->insert(sym);
			//cout << "Top Level : " << topLevel << '\n';
		//}
		//checkID(name)
	}
	else if(type != sym->getType())
	{
		//E1
		//std::cout << E1 << "\'" << name << "\'"<< "\n";
		report(E1, name);
	}
	/*else if(defined.find(name) == true )
	{
		//E2
	}*/
	//cout << currentLevel << '\n';
	//cout << name << '\n';
	return sym;
}

symbol * checker::declareVariable(const string & name, const Type &type)
{
	//cout << "he\n";
	//cout << currentLevel << '\n';
	//cout << name << '\n';
	symbol * sym = currentLevel->find(name);
	//cout << "sdf\n";
	if(sym == NULL)
	{
		sym = new symbol(type, name);
		currentLevel->insert(sym);
	}
	else if(currentLevel->enclosing() != NULL)
	{//youre in local
		//E3
		//report("redeclaration of '%s'", name);
		//std::cout << currentLevel->enclosing() << '\n';
		//std::cout << E3 << "\'" <<  name << "\'" << "\n";
		report(E3, name);
	}
	else if(type != sym->getType())
	{
		//youre in gkibak
		//E4
		//report("conflicting types for '%s'", name);
		//std::cout << E1 << "\'" << name << "\'" << "\n";
		report(E1, name);
	}
	//cout << currentLevel << '\n';
	//cout << name << '\n';	
	return sym;
}

symbol * checker::defineFunc(const string & name, const Type &type)
{
	//cout << "Defining Function\n";
	//can functions be defined globally
	/*Symbol * sym = topLevel->lookup(name);
	if(sym == nullptr)
	{
		sym = new Symbol(name, type);
		defined->insert(sym);
	}
	else if(type != sym->getType())
	{
		//Error
	}
	else
	{
		
	}
*/
	unsigned i;
	for(i = 0; i < defined.size(); i++)
	{
		if(defined[i] == name)
		{
			//E2
			//std::cout << E2 << "\'" << name << "\'" <<"\n";
			report(E2, name);
			return topLevel->find(name);
		}
	}	
	/*if(defined.count(name) > 0)
	{
		//E2
		return topLevel->find(name);
	}*/
	//symbol * tmp = new symbol(type, name);
	defined.insert(defined.begin(),name);

	symbol * sym = topLevel->find(name);

	if(sym == NULL)
	{
		/*if(checkID() == false)
		{
			cout << "undeclared function \n";
		}
		else*/
		//{
			sym = new symbol(type, name);
			topLevel->insert(sym);
			//cout << "asdfsaf" << '\n';
		//}
		//checkID(name)
	}
	else if(type != sym->getType())
	{
		//E1
		//std::cout << E1 << "\'" << name << "\'" << "\n";
		report(E1, name);
	}
	/*else if(defined.find(name) == true )
	{
		//E2
	}*/
	//cout << currentLevel << '\n';
	//cout << name << "\n";
	return sym;
	
	//return declareFunc(name, type);

}

bool checker::checkID(const string &name)
{
	//cout << "Checking ID\n";
	symbol * sym = currentLevel->lookup(name);
	
	if(sym == NULL)
	{
		//Type *tmp = new Type(name, ERROR);
		//currentLevel.insert(tmp);
		//E4
		//cout << sym << '\n';
		report(E4, name);
		Type x(INT, 0);
		x.setDeclarator();
		symbol * errorSym = new symbol(x, name);
		currentLevel->insert(errorSym);
		//std::cout << "\'" <<  name << "\'" << E4 << '\n';	
		return false;
	}
	//else if(sym->getType()->getDeclarator() == ERROR)
	//{
	//	return false;
	//}
	return true;

}

/*
int main()
{
	//std::cout << "hello world";
	//openScope();
	Type x(INT, 0);
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
	symbol *tmpSymbol3 = new symbol(x, "Mary");
	localScope->insert(tmpSymbol3);
	//Type d = newScope->getSymbols()[0]->getType();
	//symbol* as = localScope->lookup("john2");
	openScope();

	*/
	/*cout << *declareVariable("x", x);
	cout << *declareVariable("x", x);
	cout << *declareFunc("v", z);
	cout << *declareFunc("v", a);
	cout << *declareFunc("v", z);

	cout << *declareVariable("a", arr);
	cout << *declareVariable("a", arr2);
	cout << *declareVariable("x", doubleX);
	
   	cout << *declareFunc("g", g); 
	cout << *declareFunc("g", g2);

	cout << *declareFunc("f", f);
	cout << *defineFunc("f", f);

	cout << *declareVariable("f", x);

	cout << *declareVariable("h", x);
	cout << *declareVariable("h", g);

	cout << *defineFunc("vv", vv);
   	cout << *declareFunc("vv", vv);
	cout << *declareFunc("vv", vv2);
*/
/*
	cout << *declareVariable("x", x);
	//openScope();
	cout << *defineFunc("f", vv);
	openScope();
	cout << *declareVariable("y", x);
	cout << *declareVariable("x", x);
	openScope();
	cout << *declareVariable("x", x);
	cout << *declareVariable("y", x);
	openScope();
	cout << *declareVariable("x", xx);
	
	closeScope();	
	
	closeScope();

	*/
	//openScope();
	

	


	//cout << currentLevel << '\n';
	//cout << topLevel << '\n';
	//symbol * as = topLevel->find("mom");
	/*cout << *declareVariable("x", x);
	cout << *declareVariable("x", x);

	cout << currentLevel->enclosing() << '\n';
	openScope();
	cout << currentLevel->enclosing() << '\n';
	cout << *declareVariable("mom",y);
	cout << *defineFunc("fun", z);
	cout << *defineFunc("fun", z);
	openScope();
	cout << *declareVariable("x", x);
	cout << *declareVariable("j", y);*/
	//cout << checkID("cat") << '\n';
	//cout << checkID("mom") << '\n';
	//cout << *declareFunc("j", z);
	//cout << *declareFunc("j", z);
//	currentLevel->getSymbols();
	//declareVariable("mom", z);
	//cout << currentLevel;;
	//std::cout << *localScope->lookup("john2") << "\n";
//	return 0;
//}


