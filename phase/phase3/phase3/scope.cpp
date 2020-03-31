#include "symbol.h"
#include "scope.h"
#include <iostream>
#include <vector>
#include <string>

//using namespace std;

//int global = 0;
//int current = 0;


//typedef std::vector <scope> scopes;
typedef std::vector<symbol *> symbols;


scope::scope(scope *enclosing)
{
	_enclosing = enclosing;
}

void scope::insert(symbol *symbol)
{
	//int x = _Symbols.current
	_symbols.push_back(symbol);
	Type type = symbol->getType();
	if(type.getDeclarator() == 2)
	{
		//std::cout << "function\n";
		/*Parameters * params = type.getParameters();
		int i;
		for(i = 0; i < params->types.size(); i++)
		{
			symbol *tmp = new symbol(params->types[i], 
			_symbols.push_back(params->types[i]);
		}*/
	}
}

symbol * scope::find(const string &name) const
{
	//std::vector<Symbols *> :: iterator i;	
	//cout << "hasdf";
	int i;
	for(i = 0; i < (int)_symbols.size(); i++)
	{	
		symbol *tmp = _symbols[i];
		if(tmp->getName() == name)
		{
			return _symbols[i];
		}	
		//cout << "hello";
	}
	//cout << "hello";
	return 0;
}

symbol * scope::lookup(const string &name) const
{
	symbol * tmp;
	scope *tmpScope;
	if((tmp = scope::find(name)) != NULL)
	{
		return tmp;	
	}
	else
	{
		tmpScope = _enclosing;
	}
	//std::vector<Symbols *> :: iterator i;	
	//for(i = _Symbols.begin(); i != _Symbols.end(); i++)
	//{
	while(tmpScope != NULL)
	{
		if((tmp = tmpScope->find(name)) != NULL)
		{
			return tmp;	
		}
		else
		{
			tmpScope = tmpScope->_enclosing;
		}
	}
	//}
	return 0;
}


scope * scope::enclosing() const
{
	return _enclosing;
}

const symbols & scope::getSymbols() const
{
	int i;
	for(i = 0; i < (int)_symbols.size(); i++)
	{
		std::cout << "\n\n\n\n\n";
		std::cout << "Symbols: " << *_symbols[i] << "\n";
	}
	return _symbols;
}

/*int main()
{
	std::cout << "hello\n";
	return 0;
}*/
