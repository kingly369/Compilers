#include <cassert>
//#include "tokens.h"
#include "type.h"
#include <iostream>


using namespace std;

//enum {ARRAY, FUNCTION, SCALAR};

Type::Type()
	:_declarator(ERROR)
{}

Type::Type(int specifier, unsigned indirection = 0)
	:_declarator(SCALAR), _specifier(specifier), _indirection(indirection)
{}

Type::Type(int specifier, unsigned indirection, unsigned length)
	:_declarator(ARRAY), _specifier(specifier), _indirection(indirection), _length(length)
{}

Type::Type(int specifier, unsigned indirection, Parameters *parameters)
	:_declarator(FUNCTION), _specifier(specifier), _indirection(indirection), _parameters(parameters)
{}

bool Type::operator==(const Type &rhs) const
{
/*	if (isArray())
	{
		if(rhs.getSpecifier() == getSpecifier() && rhs.getDeclarator() == getDeclarator() && rhs.getIndirection() == getIndirection() && rhs.getLength() == getLength())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if(rhs.getSpecifier() == getSpecifier() && rhs.getDeclarator() == getDeclarator() && rhs.getIndirection() == getIndirection())
		{
			return true;
		}
		return false;

	}
	return false;
*/

	if(_declarator != rhs._declarator)
	{
		return false;
	}
	if(_declarator == ERROR)
	{
		return true;
	}
	if(_specifier != rhs._specifier)
	{
		return false;
	}
	if(_indirection != rhs._indirection)
	{
		return false;
	}
	if(_declarator == SCALAR)
	{
		return true;
	}
	if(_declarator == ARRAY)
	{
		return _length == rhs._length;
	}
	assert(_declarator == FUNCTION);
	if(_parameters->variadic != rhs._parameters->variadic)
	{
		return false;
	}
	return _parameters->types == rhs._parameters->types;
}

bool Type::operator!=(const Type &rhs) const
{
	return !operator==(rhs);
}

int Type::getSpecifier() const
{
	return _specifier;
}

Parameters * Type::getParameters() const
{
	return _parameters;
}

unsigned Type::getIndirection() const
{
	return _indirection;
}

void Type::setDeclarator()
{
	_declarator = ERROR;
}

bool Type::isArray() const
{
	if(getDeclarator() == ARRAY)
	{
		return true;
	}
	return false;
}

unsigned Type::getLength() const
{
	return _length;
}

unsigned Type::getDeclarator() const
{
	return _declarator;
}

std::ostream &operator<<(std::ostream &ostr, const Type &type)
{
	ostr << "getSpecifier: " << type.getSpecifier() << "\n";
	int x = type.isArray();
	ostr << "isArray: " << type.isArray() << "\n";
	ostr << "getDeclarator: " << type.getDeclarator() << "\n";
	ostr << "indirection: " << type.getIndirection() << "\n";
	
	if(x != 0)
	{
		ostr << "length: " << type.getLength() << "\n";
	}
	return ostr;
}

/*int main()
{
	//Type x(INT, 0);
	struct Parameters *k;
	//vector<Type> a(1,x);
	//k->types = a;
	//k->variadic = false;
	Type x(INT, 0, k);
	//Type y(INT, 0, k);
	cout << x;
	return 1;
}
*/
