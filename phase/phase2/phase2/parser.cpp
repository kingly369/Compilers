//# include <cerno>
# include <cstdlib>
# include <iostream>
# include "string.h"
# include "lexer.h"
# include "tokens.h"

using namespace std;

int lookahead, nexttoken;
string lexbuf, nextbuf;

void pointers();
void specifier();
void parameter();
void parameters();
void declarations();
void statements();
void parameter_list();
//void declaration_list();
void statement();
void error(void)
{
	//cout << lexbuf << "\n";
	report("error at %s", yytext);
	exit(1);
}

int peek()
{
	if(!nexttoken)
	{
		nexttoken = yylex();
		nextbuf = yytext;
	}
	return nexttoken;
}



void match(int t)
{
	if(lookahead != t)
	{
		//cout << t << '\n';
		//cout << lexbuf << '\n';

		error();
	}

	if (nexttoken)
	{
		lookahead = nexttoken;
		lexbuf = nextbuf;
		nexttoken = 0;
	}
	else
	{
		lookahead = yylex();
		lexbuf = yytext;
	}
/*

	if (lookahead == t)
	{
		lookahead = yylex();
		//cout << lookahead;
	}
	else
	{
		cout << "Error matching" << '\n';
		cout << lookahead << '\n';
		cout << t << '\n';
		//error();
	}
	*/
}



void logicalOR();

void global_declarator()
{
	pointers();
	match(ID);
	if(lookahead == LPAREN)
	{
		match(lookahead);
		parameters();
		match(RPAREN);
	}
	else if(lookahead == LBRACK)
	{
		match(lookahead);
		match(INTEGER);
		match(RBRACK);
	}
}

void remaining_decls()
{
	if(lookahead == SEMI)
	{
		match(lookahead);
	}
	else if(lookahead == COMMA)
	{
		match(lookahead);
		global_declarator();
		remaining_decls();
	}
}

void function_or_global()
{
	specifier();
	pointers();
	match(ID);

	if(lookahead == LPAREN)
	{
		match(lookahead);
		parameters();
		match(RPAREN);
		if(lookahead == LBRACE)
		{
			match(lookahead);
			declarations();
			statements();
			match(RBRACE);
		}
		else
		{
			remaining_decls();
		}
	}
	else if(lookahead == LBRACK)
	{
		match(lookahead);
		match(INTEGER);
		match(RBRACK);
		remaining_decls();
	}
	else
	{
		remaining_decls();
	}
}



void expressionList()
{
	logicalOR();

	if(lookahead == COMMA)
	{
		match(lookahead);
		expressionList();
	}

}

void primaryExp()
{
	if(lookahead == STRING || lookahead == CHARACTER || lookahead == INTEGER || lookahead == REAL)
	{
		//cout <<"h\n";
		match(lookahead);
		//cout << "h\n";
	}
	//do match id
	else if(lookahead == ID)
	{
		match(lookahead);
		if(lookahead == LPAREN && peek() == RPAREN)
		{
			match(LPAREN);
			match(RPAREN);
		}
		else if(lookahead == LPAREN)
		{
			match(lookahead);
			expressionList();
			match(RPAREN);
		}
		else
		{
		}
	}
	else if(lookahead == LPAREN)
	{
		match(lookahead);
		logicalOR();
		match(RPAREN);
	}
	else if(lookahead == ID)
	{
		match(lookahead);
		if(lookahead == LPAREN)
		{
			match(lookahead);
			//logicalOR();
			match(RPAREN);
		}
	}
	else
	{
		//error();
		match(lookahead);
		//cout << "k\n";
	}
}

void logicalincrement()
{	
	//cout << lookahead;
	primaryExp();

	while (lookahead == INC || lookahead == DEC || lookahead == LBRACK)
	{
		int tmp = lookahead;
		//cout << "INC\n";
		//match(lookahead);
		/*if(lookahead != tmp)
		{
			logicalOR();
			match(RBRACK);
			logicalincrement();
		}
		else
		{
			match(lookahead);
			logicalincrement();
		}*/
		//primaryExp();
		if(tmp == INC)
		{
			cout << "inc\n";
			match(lookahead);
		}
		else if(tmp == DEC)
		{
			cout << "dec\n";
			match(lookahead);
		}
		else if(tmp == LBRACK)
		{
			//cout << "index\n";
			match(lookahead);
			logicalOR();
			cout << "index\n";
			match(RBRACK);
		}
		else
		{
			primaryExp();
		}
	}
}

void parameters()
{
	if(lookahead == VOID)
	{
		match(lookahead);

	}
	else
	{
		parameter_list();
		//cout << "inside parameters " << lexbuf << "\n";
		if(lookahead == ELLIPSIS)
		{	
			//match(COMMA);
			//cout << lexbuf << '\n';
			//cout << lookahead << '\n';
			//cout << ELLIPSIS << '\n';
			match(ELLIPSIS);
		}
	}
}

void parameter_list()
{
	parameter();
	if(lookahead == COMMA)
	{
		match(lookahead);
		//cout << "fasdfs\n";
		parameter_list();
	}
	//cout << "fasg\n";
}

void parameter()
{
	specifier();
	pointers();
	if(lookahead == ID)
	{
		match(ID);
	}
	//cout << "fasdf\n";
}

void specifier()
{
	if(lookahead == INT || lookahead == CHAR || lookahead == DOUBLE)
	{
		match(lookahead);

	}
}

/*void statement()
{
	if(lookahead == LBRACE)
	{
	}
}
*/
void declarator()
{
	pointers();
	match(lookahead);
	if(lookahead == LBRACK)
	{
		match(lookahead);
		match(INTEGER);
		match(RBRACK);
	}
}

void declarator_list()
{
	declarator();
	if(lookahead == COMMA)
	{
		match(lookahead);
		declarator_list();
	}
}

int isSpecifier()
{
	if(lookahead == INT || lookahead == CHAR || lookahead == DOUBLE)
	{
		return 1;
	}
	return 0;
}

void declaration()
{
	specifier();
	declarator_list();
	match(SEMI);
}


void declarations()
{
	//cout << "inside " << lexbuf << "\n";
	if(isSpecifier() == 1)
	{
		//cout << "ASD\n";
		declaration();
		//cout << "pass decl\n";
		declarations();
		//cout << "pass decls\n";
	}
}

/*void isStatement()
{

}
*/
void statements()
{
	if(lookahead== LBRACE || lookahead == BREAK || lookahead == RETURN || lookahead == WHILE || lookahead == FOR || lookahead == IF || lookahead == SIZEOF || lookahead == ID || lookahead == REAL || lookahead == INTEGER || lookahead == CHARACTER || lookahead == STRING || lookahead == ELSE)
	{
		statement();
		statements();
	}
}

void assignment()
{
	//cout << "here\n";
	logicalOR();
	//cout << "inside assign\n";
	if(lookahead == ASSIGN)
	{
		//cout << "inside assignment \n";
		match(lookahead);
		logicalOR();
		//cout << "done assignment\n";
	}

}

void statement()
{
	//cout << "asdffsdafd\n";
	if(lookahead == LBRACE)
	{
		match(lookahead);
		//cout << "inside lbrace\n";
		declarations();
		//cout << "done declarations\n";
		statements();
		//cout << "done statements\n";
		
		match(RBRACE);
	}
	else if(lookahead == BREAK)
	{
		match(lookahead);
		match(SEMI);
	}
	else if(lookahead == RETURN)
	{
		match(lookahead);
		logicalOR();
		match(SEMI);
	}
	else if(lookahead == WHILE)
	{
		match(lookahead);
		match(LPAREN);
		logicalOR();
		match(RPAREN);
		statement();
	}
	else if(lookahead == FOR)
	{
		match(lookahead);
		match(LPAREN);
		assignment();
		match(SEMI);
		logicalOR();
		match(SEMI);
		//cout << "Here" << lexbuf << '\n';
		assignment();
		//cout << "Here1\n";
		match(RPAREN);
		statement();
	}
	else if(lookahead == IF)
	{
		//cout << lookahead << '\n';
		match(lookahead);
		match(LPAREN);
		logicalOR();
		match(RPAREN);
		//cout << "fadf\n";
		statement();
		//cout << "adfsa\n";
		if (lookahead == ELSE)
		{
			match(lookahead);
			//cout << "inside else\n";
			statement();
		}
		//cout << lexbuf;

	}
	else
	{
		assignment();
		match(SEMI);
	}
}

void pointers()
{
	if(lookahead != STAR)
	{
	}
	else
	{
		match(STAR);
		pointers();
	}
}


void logicalright()
{
	//while (lookahead == ADDR || lookahead == NOT || lookahead == MINUS || lookahead == STAR)
	//{
		int tmp = lookahead;
		//match(lookahead);
		if(tmp == ADDR)
		{
			match(lookahead);
			logicalright();	
		}
		else if(tmp == NOT)
		{
			match(lookahead);
			logicalright();
		}
		else if(tmp == MINUS)
		{
			match(lookahead);
			logicalright();
		}
		else if(tmp == STAR)
		{
			match(lookahead);
			logicalright();
		}
		else if(tmp == SIZEOF)
		{

			match(lookahead);
			/*if(lookahead == LPAREN)
			{
				match(lookahead);
			}*/
			if(lookahead == LPAREN && (peek() == INT || peek() == DOUBLE || peek() == CHAR))
			{
				match(lookahead);
				match(lookahead);
				pointers();
				cout << "sizeof\n";

				match(RPAREN);
			}
			else
			{
				logicalOR();
				cout << "sizeof\n";
			}	

		}
		else if(tmp == LPAREN && (peek() == INT || peek() == DOUBLE || peek() == CHAR))
		{
			//cout << nexttoken << "\n";
			match(LPAREN);
			match(lookahead);
			pointers();
			match(RPAREN);
			logicalOR();
			cout << "cast\n";
		}
		else
		{
			logicalincrement();
		}
		if(tmp == ADDR)
		{
			cout << "addr\n";	
		}
		else if(tmp == NOT)
		{
			cout << "not\n";
		}
		else if(tmp == MINUS)
		{
			cout << "neg\n";
		}
		else if(tmp == STAR)
		{
			cout << "deref\n";

		}
	//}
	//logicalincrement();
}

void logicalmul()
{
	logicalright();
	//cout << lookahead << "\n";	
	while(lookahead == STAR || lookahead == DIV || lookahead == REM)
	{
		int tmp = lookahead;
		//cout << tmp;
		match(lookahead);
		//cout << "Entering right\n";
		logicalright();
		//cout << "Finishing mul" << '\n';
		if(tmp == STAR)
		{
			cout << "mul\n";
		}
		else if(tmp == DIV)
		{
			cout << "div\n";
		}
		else
		{
			cout << "rem\n";
		}
	}
	
}

void logicalplus()
{
	logicalmul();

	while (lookahead == PLUS || lookahead == MINUS)
	{
		int tmp = lookahead;
		match(lookahead);
		logicalmul();
		if(tmp == PLUS)
		{
			cout << "add\n";
		}
		else
		{
			cout << "sub\n";
		}
	}
}

void logicalCMP()
{
	logicalplus();

	while (lookahead == LEQ || lookahead == LTN || lookahead == GEQ || lookahead == GTN)
	{
		int tmp = lookahead;
		match(lookahead);
		logicalplus();
		if(tmp == LEQ)
		{
			cout << "leq\n";
		}
		else if(tmp == LTN)
		{
			cout << "ltn\n";
		}
		else if(tmp == GEQ)
		{
			cout << "geq\n";
		}
		else
		{
			cout << "gtn\n";
		}
		//cout << lookahead << '\n';
	}
}

void logicalEQ()
{
	logicalCMP();

	while (lookahead == EQL || lookahead == NEQ)
	{
		int tmp = lookahead;
		match(lookahead);
		logicalCMP();
		if(tmp == EQL)
		{cout << "eql\n";}
		else
		{cout << "neq\n";}
	}
}
void logicalAND()
{
	logicalEQ();

	while (lookahead == AND)
	{
		match(lookahead);
		logicalEQ();
		cout << "and\n";
	}
}

void logicalOR()
{
	logicalAND();

	while (lookahead == OR)
	{
		match(lookahead);
		logicalAND();
		cout << "or" << '\n';
	}
}



int main()
{
	lookahead = yylex();
	//cout << ELLIPSIS << '\n';
	while(lookahead != DONE)
	{
		//logicalOR();
		//statements();
		//	parameters();
		function_or_global();
		//match(';');
		//cout << "looped\n";
	}
//	cout << "done\n";
	return 0;
}
