
#ifndef TYPE_H
#define TYPE_H
#include <vector>
#include <ostream>
#include "tokens.h"

struct Parameters{

	bool variadic; //Ellipses or not
	std::vector<class Type> types;
};

class Type{
	enum {ARRAY, ERROR, FUNCTION, SCALAR};
	short _declarator, _specifier;
	unsigned _indirection;
	unsigned _length;
	Parameters *_parameters;

	public:
		Type();
		Type(int specifier, unsigned indirection);
		Type(int specifier, unsigned indirection, unsigned length);
		Type(int specifier, unsigned indirection, Parameters *paramaters);
		bool operator==(const Type &rhs) const;
		bool operator!=(const Type &rhs) const;

		int getSpecifier() const;
		unsigned getIndirection() const;
		void setDeclarator();
		bool isArray() const;
		unsigned getLength() const;
		unsigned getDeclarator() const;
		Parameters * getParameters() const;
};

std::ostream &operator<<(std::ostream &ostr, const Type & type);

#endif
