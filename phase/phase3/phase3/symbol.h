#ifndef SYMBOL_H
#define SYMBOL_H

#include <iostream>
#include <string>
#include <vector>
#include "type.h"

using namespace std;

class symbol
{

	Type _type;
	string _name;

	public:
		symbol(Type type, string name);

		Type getType();
		string getName();


};
std::ostream &operator<<(std::ostream &ostr, symbol &symbolA);

#endif
