#ifndef CHECKER_H
#define CHECKER_H

#include "checker.h"
#include "string.h"
#include <ostream>
#include <cassert>
//#include <"symbol.h">

class symbol;
class Type;

class checker{	

	
	public:
		void openScope();
		void closeScope();
		symbol * declareFunc(const std::string &name, const Type &type);
		symbol * defineFunc(const std::string &name, const Type &type);
		symbol * declareVariable(const std::string &name, const Type &type);
		bool checkID(const std::string &name);

};


#endif
