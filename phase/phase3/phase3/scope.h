#ifndef SCOPE_H
#define SCOPE_H

#include "symbol.h"
#include <vector>
#include <string>


typedef std::vector<symbol *> symbols;

class scope{
		
	typedef std::string string;
	scope *_enclosing; //
	symbols _symbols;

	public:
		scope(scope *enclosing);
		void insert(symbol *symbol);
		symbol *find(const string &name) const;
		symbol *lookup(const string &name) const;
		scope *enclosing() const; //getter
		const symbols & getSymbols() const;

};

#endif

