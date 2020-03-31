/*
 * File:	checker.h
 *
 * Description:	This file contains the public function declarations for the
 *		semantic checker for Simple C.
 */

# ifndef CHECKER_H
# define CHECKER_H
# include "Scope.h"
//# include "string.h" 

Scope *openScope();
Scope *closeScope();

Symbol *defineFunction(const std::string &name, const Type &type);
Symbol *declareFunction(const std::string &name, const Type &type);
Symbol *declareVariable(const std::string &name, const Type &type);
Symbol *checkIdentifier(const std::string &name);

//Type add(const &lvalue);
//Type checkParameters(Parameters *params, const Type & idName)
Type checkLogical(const Type &left2, const Type &right2, const std::string &ops);
Type checkPrefix(Type &right2, const std::string &ops, bool &lvalue, const int &specifier, const int &indirection);
Type checkPostfix(const Type &left2, const std::string & ops, const bool &lvalue, const Type &expr);
Type checkGetType(const std::string &idName);
Type checkFunctionCall(const Type &func);
Type checkParameters(Parameters *params, const Type & idName);

















# endif /* CHECKER_H */
