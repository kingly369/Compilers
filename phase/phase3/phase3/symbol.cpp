#include "symbol.h"
#include <string>
#include <iostream>
#include "type.h"

using namespace std;

symbol::symbol(Type type, string name)
	:_type(type), _name(name)
{}

Type symbol::getType()
{
	return _type;
}

string symbol::getName()
{
	return _name;
}

std::ostream &operator<<(std::ostream &ostr, symbol &symbolA)
{
	ostr << "name: " << symbolA.getName() << "\n";
	ostr << "type: " << symbolA.getType() << "\n";
	return ostr;
}
/*int main()
{
	return 0;
}*/
