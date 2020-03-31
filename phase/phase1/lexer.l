%{
/*
 * File:	lexer.l
 *
 * Description:	This file contains the flex description for the lexical
 *		analyzer for Simple C.
 *
 */

# include <cerrno>
# include <cstdlib>
# include <iostream>
# include "string.h"
//# include "lexer.h"

using namespace std;

void stringFunction(int f())
{
	char x;
	string tmp = "\"";
	while((x=f()) != '"')
	{
		tmp += x;	
	}
	tmp += "\"";
	cout << "string " << tmp << "\n";
}

void tmpFunction(int f())
{
	char x;
	bool a = false;
	//bool b = false;
	string message = "/*";
	while(true)
	{
		x = f();
		message += x;
		if(x=='*')
		{
			a = true;
		}
		else if(a && x =='/')
		{
			break;
	
		}
		else
		{
			a = false;
		}
	}
	//message += '/';
	//cout << "comment " << message << "\n";
	
}

%}

%option nounput noyywrap yylineno
%%
"auto" {cout << "keyword " << yytext << "\n";}
"break" {cout << "keyword " << yytext << "\n";}
"case" {cout << "keyword " << yytext << "\n";}
"char" {cout << "keyword " << yytext << "\n";}
"const" {cout << "keyword " << yytext << "\n";}
"continue" {cout << "keyword " << yytext << "\n";}
"default" {cout << "keyword " << yytext << "\n";}
"do" {cout << "keyword " << yytext << "\n";}
"double" {cout << "keyword " << yytext << "\n";}
"else" {cout << "keyword " << yytext << "\n";}
"enum" {cout << "keyword " << yytext << "\n";}
"extern" {cout << "keyword " << yytext << "\n";}
"float" {cout << "keyword " << yytext << "\n";}
"for" {cout << "keyword " << yytext << "\n";}
"goto" {cout << "keyword " << yytext << "\n";}
"if" {cout << "keyword " << yytext << "\n";}
"int" {cout << "keyword " << yytext << "\n";}
"long" {cout << "keyword " << yytext << "\n";}
"register" {cout << "keyword " << yytext << "\n";}
"return" {cout << "keyword " << yytext << "\n";}
"short" {cout << "keyword " << yytext << "\n";}
"signed" {cout << "keyword " << yytext << "\n";}
"sizeof" {cout << "keyword " << yytext << "\n";}
"static" {cout << "keyword " << yytext << "\n";}
"struct" {cout << "keyword " << yytext << "\n";}
"switch" {cout << "keyword " << yytext << "\n";}
"typedef" {cout << "keyword " << yytext << "\n";}
"union" {cout << "keyword " << yytext << "\n";}
"unsigned" {cout << "keyword " << yytext << "\n";}
"void" {cout << "keyword " << yytext << "\n";}
"volatile" {cout << "keyword " << yytext << "\n";}
"while" {cout << "keyword " << yytext << "\n";}

"/*" {tmpFunction(yyinput);}

[_a-zA-Z]([_a-zA-Z0-9]+)? {cout << "identifier " << yytext << "\n";} 

\"([^"\n\\]|\\.)*\" {cout << "string " << yytext << "\n";}
[0-9]+\.[0-9]+([eE][+-]?[0-9]+)? {cout << "real " << yytext << "\n";}
[0-9]+ {cout << "integer " << yytext << "\n";}
\'([^'\n\\]|\\.)+\' {cout << "character " << yytext << "\n";}

"\=" {cout << "operator " << yytext << "\n";}
"\|\|" {cout << "operator " << yytext << "\n";}
"&&" {cout << "operator " << yytext << "\n";}
"\|" {cout << "operator " << yytext << "\n";}
"\=\=" {cout << "operator " << yytext << "\n";}
"\!\=" {cout << "operator " << yytext << "\n";}
"\<" {cout << "operator " << yytext << "\n";}
"\>" {cout << "operator " << yytext << "\n";}
"\<\=" {cout << "operator " << yytext << "\n";}
"\>\=" {cout << "operator " << yytext << "\n";}
"\+" {cout << "operator " << yytext << "\n";}
"\-" {cout << "operator " << yytext << "\n";}
"\*" {cout << "operator " << yytext << "\n";}
"/" {cout << "operator " << yytext << "\n";}
"%" {cout << "operator " << yytext << "\n";}
"&" {cout << "operator " << yytext << "\n";}
"\!" {cout << "operator " << yytext << "\n";}
"\+\+" {cout << "operator " << yytext << "\n";}
"\-\-" {cout << "operator " << yytext << "\n";}
"\-\>" {cout << "operator " << yytext << "\n";}
"\(" {cout << "operator " << yytext << "\n";}
"\)" {cout << "operator " << yytext << "\n";}
"\[" {cout << "operator " << yytext << "\n";}
"\]" {cout << "operator " << yytext << "\n";}
"\{" {cout << "operator " << yytext << "\n";}
"\}" {cout << "operator " << yytext << "\n";}
";" {cout << "operator " << yytext << "\n";}
"\:" {cout << "operator " << yytext << "\n";}
"\." {cout << "operator " << yytext << "\n";}
"," {cout << "operator " << yytext << "\n";}
"\.\.\." {cout << "operator " << yytext << "\n";}

[ \t \n]+ {/*Do nothing*/}
. {}
%%

int main()
{
    yylex();
    return 0;
}
