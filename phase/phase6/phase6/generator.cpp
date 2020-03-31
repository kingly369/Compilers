/*
 * File:	generator.cpp
 *
 * Description:	This file contains the public and member function
 *		definitions for the code generator for Simple C.
 *
 *		Extra functionality:
 *		- putting all the global declarations at the end
 */

# include <cassert>
# include <iostream>
# include "generator.h"
# include "machine.h"
# include "Tree.h"
# include "label.cpp"
# include "string.h"
# include <unordered_map>
# define FP(expr) ((expr)->type().isReal())
# define BYTE(expr) ((expr)->type().size() == 1)

using namespace std;
//static Label
static int offset;
static unsigned max_args;
//unordered_map<string, Label> listLabels;
std::unordered_map<int, std::string> listDoubles;
std::unordered_map<int, std::string> listStrings;
Label globalLabel;
Label returnLabel;
Label inLoop;
bool stringLabel = false;

bool leftDouble = false;
bool rightDouble = false;

int numberDouble = 0;
//list.insert({3, "tree");


/*class Label{
	unsigned counter

};
*/

/*
 * Function:	align (private)
 *
 * Description:	Return the number of bytes necessary to align the given
 *		offset on the stack.
 */
void assignTemp(Expression *expr);

static int align(int offset)
{
    if (offset % STACK_ALIGNMENT == 0)
	return 0;

    return STACK_ALIGNMENT - (abs(offset) % STACK_ALIGNMENT);
}


/*
 * Function:	operator << (private)
 *
 * Description:	Convenience function for writing the operand of an
 *		expression using the output stream operator.
 */

static ostream &operator <<(ostream &ostr, Expression *expr)
{
    expr->operand(ostr);
    return ostr;
}


/*
 * Function:	Expression::operand
 *
 * Description:	Write an expression as an operator to the specified stream.
 */

void Expression::operand(ostream &ostr) const
{

   // assert(offset != 0);
    ostr << offset << "(%ebp)";
}


/*
 * Function:	Identifier::operand
 *
 * Description:	Write an identifier as an operand to the specified stream.
 */

void Identifier::operand(ostream &ostr) const
{
    if (_symbol->offset == 0)
	ostr << global_prefix << _symbol->name();
    else
	ostr << _symbol->offset << "(%ebp)";
}


/*
 * Function:	Integer::operand
 *
 * Description:	Write an integer as an operand to the specified stream.
 */

void Integer::operand(ostream &ostr) const
{
    ostr << "$" << _value;
}


/*
 * Function:	Call::generate
 *
 * Description:	Generate code for a function call expression.
 */

void Call::generate()
{
    unsigned offset, size;


    /* Generate code for all arguments first. */
	//assignTemp(this);
    for (auto arg : _args)
	arg->generate();


    /* Move the arguments onto the stack. */

    offset = 0;
	assignTemp(this);
    for (auto arg : _args) {
	if (FP(arg)) {
	    cout << "\tfldl\t" << arg << endl;
	    cout << "\tfstpl\t" << offset << "(%esp)" << endl;
	} else {
	    cout << "\tmovl\t" << arg << ", %eax" << endl;
	    cout << "\tmovl\t%eax, " << offset << "(%esp)" << endl;
	}

	size = arg->type().size();
	offset += size;
    }

    if (offset > max_args)
	max_args = offset;


    /* Make the function call. */


    cout << "\tcall\t" << global_prefix << _id->name() << endl;
	cout << "\tmovl\t%eax, " << this << endl;
	//assignTemp(this);
    /* Save the return value */

/*# if 0
    assigntemp(this);

    if (FP(this))
	cout << "\tfstpl\t" << this << endl;
    else
	cout << "\tmovl\t%eax, " << this << endl;
# endif*/
}


/*
 * Function:	Block::generate
 *
 * Description:	Generate code for this block, which simply means we
 *		generate code for each statement within the block.
 */

void Block::generate()
{
    for (auto stmt : _stmts)
	{
		stmt->generate();
		//write(cout);
		//cout << "t" << endl;
		//cout << stmt->expr
	}

	//cout << "Q" << endl;
}


/*
 * Function:	Function::generate
 */

void Function::generate()
{
    max_args = 0;
    offset = SIZEOF_REG * 2;
    allocate(offset);


    /* Generate our prologue. */

    cout << global_prefix << _id->name() << ":" << endl;
    cout << "\tpushl\t%ebp" << endl;
    cout << "\tmovl\t%esp, %ebp" << endl;
    cout << "\tsubl\t$" << _id->name() << ".size, %esp" << endl;


    /* Generate the body of this function. */

    _body->generate();


    /* Compute the proper stack frame size. */

    offset -= max_args;
    offset -= align(offset - SIZEOF_REG * 2);


    /* Generate our epilogue. */
	//Label exit;
	cout << returnLabel << ":" << endl;
	

    cout << "\tmovl\t%ebp, %esp" << endl;
    cout << "\tpopl\t%ebp" << endl;
    cout << "\tret" << endl << endl;

    cout << "\t.set\t" << _id->name() << ".size, " << -offset << endl;
    cout << "\t.globl\t" << global_prefix << _id->name() << endl << endl;
	
	cout << ".data" << endl;

	for(auto &x: listStrings)
	{
		cout << ".L" << x.first << ":\t.asciz\t";
		cout << "\"" << x.second << "\"" << endl;
	}
	for(auto &x : listDoubles)
	{
		cout << ".L" << x.first << ":\t.double\t" << x.second << endl;
	}
	returnLabel = Label();
	listStrings.clear();
	listDoubles.clear();
}


/*
 * Function:	generateGlobals
 *
 * Description:	Generate code for any global variable declarations.
 */

void generateGlobals(Scope *scope)
{
    const Symbols &symbols = scope->symbols();

    for (auto symbol : symbols)
	if (!symbol->type().isFunction()) {
	    cout << "\t.comm\t" << global_prefix << symbol->name() << ", ";
	    cout << symbol->type().size() << endl;
	}
}


/*
 * Function:	Assignment::generate
 *
 * Description:	Generate code for an assignment statement.
 *
 *		NOT FINISHED: Only works if the right-hand side is an
 *		integer literal and the left-hand side is an integer
 *		scalar.
 */

void Assignment::generate()
{
	cout << endl;
	
	_right->generate();
	_left->generate();
	//_right->generate();
	//_left->generate();
	Expression * child = _left->isDereference();
	//child->generate();
	//cout << "A" << endl;
	//assignTemp(this);
	if(child == nullptr)
	{	
		_left->generate();	
		if(_right->type().isReal())
		{
			//listDoubles
			if(stringLabel == true)
			{
				//cout << "\tleal\t" << globalLabel << ", %eax" << endl;
			   	cout << "\tfldl\t" << globalLabel << endl;
				cout << "\tfstpl\t" << _left << endl;
				stringLabel = false;
			}
			else
			{
				cout << "\tfldl\t" << _right << endl;
			
				cout << "\tfstpl\t" << _left << endl;
			}
		}
		else if(BYTE(_right))
		{
			cout << "\tmovb\t" << _right << ", %al" << endl;
			cout << "\tmovb\t%al, " << _left << endl;
		}
		else
		{
			cout << "\tmovl\t" << _right << ", %eax" << endl;
			cout << "\tmovl\t%eax, " << _left << endl;
			//cout << "R" << endl;
		}
	}
	else
	{
		//child->_lvalue = false;
		//cout << "T" << endl;
		//cout << child->type() << endl;
		child->generate();
		//cout << "hello3" << endl;
		if(_right->type().isReal())
		{
			if(stringLabel == true)
			{
				cout << "\tfldl\t" << globalLabel << endl;
				//cout << "\tmovl\t" << child << ", %eax" << endl;
				//cout << "\tfstpl\t(%eax)" << endl;
				cout << "\tfstpl\t" << child << endl;		
				//cout << "here" << endl;
				stringLabel = false;
			}
			else
			{
				//cout << "U" << endl;	
				cout << "\tfldl\t" << _right << endl;
				cout << "\tmovl\t" << child << ", %eax" << endl;
				cout << "\tfstpl\t(%eax)" << endl;		
				//cout << "\tfstpl\t" << child << endl;		
				//cout << "Here" << endl;
			}
		}
		else if(BYTE(_right))
		{
			cout << "\tmovl\t" << child << ", %eax" << endl;
			cout << "\tmovl\t" << _right << ", %ecx" << endl;
			cout << "\tmovl\t(%ecx), %ecx" << endl;
			cout << "\tmovb\t%cl, (%eax)" << endl;
		}
		else
		{
			cout << "\tmovl\t" << child << ", %eax" << endl;
			cout << "\tmovl\t" << _right << ", %ecx" << endl;
			//cout << "\tmovl\t(%eax)
			cout << "\tmovl\t%ecx, (%eax)" << endl;
			//cout << "\tmovl\t(%eax), ";
		}
	/*	cout << "\tmovl\t" << _right << ", %eax" << endl;
		cout << "\tmovl\t" << _left << ", %ecx" << endl;
		cout << "\tmovl\t%eax, (%ecx)" << endl;
		//cout << "\tmovl\t%ecx, " <*/
	}
	//stringLabel = false;
}

void assignTemp(Expression *expr)
{
	offset -= expr->type().size();
	expr->offset = offset;
}

/*void Expression::operand(ostream &ostr) const
{
	assert(offset != 0);
	ostr << offset << "(%ebp)";
}*/

Expression* Expression::isDereference()
{
	return NULL;

}

Expression* Dereference::isDereference()
{
	return _expr;
}

void Multiply::generate()
{

	cout << endl;

	_left->generate();
	_right->generate();
	assignTemp(this);	
	
	if(FP(this))
	{
		cout << "\tfldl\t" << _left << endl;
		cout << "\tfmull\t" << _right << endl;
		cout << "\tfstpl\t" << this << endl;

	}	
	else
	{
		cout << "\tmovl\t" << _left << ", %eax" << endl;
		cout << "\timull\t" << _right << ", %eax" << endl;
		cout << "\tmovl\t%eax, " << this << endl;
	}


}

void Divide::generate()
{
	cout << endl;


	_left->generate();
	_right->generate();
	//cout << "k" << endl;
	assignTemp(this);	
	
	if(FP(this))
	{
		int p;
		//cout << _left->offset << endl;
		//cout << _right->offset << endl;
		if(_left->offset == 0)
		{
			stringLabel = true;
			leftDouble = true;	
			//p++;
		}
		else
		{
			stringLabel = false;
			leftDouble = false;
		}
		if(_right->offset == 0)
		{
			rightDouble = true;
		}
		else
		{
			rightDouble = false;
		}
		p = 0;
		for(auto &a : listDoubles)
		{
			if(p < a.first)
			{
				p = a.first;
			}
		}
		//cout << _left << endl;
		//cout << _right << endl;
		if(leftDouble == true && rightDouble == true)
		{
			//cout << "leal
			//cout << "here" << endl;

			cout << "\tfldl\t" << _left << endl;
			cout << "\tfdivl\t" << _right << endl;
			cout << "\tfstpl\t" << this << endl;
		}
		else if(leftDouble == false && rightDouble == true)
		{
			//cout << "here2" << endl;
			cout << "\tfldl\t" << _left << endl;
			cout << "\tfdivl\t.L" << p << endl;
			cout << "\tfstpl\t" << this << endl;
		
		}
		else if(leftDouble == true && rightDouble == false)
		{

			//cout << "here" << endl;

			cout << "\tfldl\t" << globalLabel << endl;
			cout << "\tfdivl\t" << _right << endl;
			cout << "\tfstpl\t" << this << endl;
		
		}
		else
		{
			cout << "\tfldl\t" << _left << endl;
			cout << "\tfdivl\t" << _right << endl;
			//cout << "\tcltd" << endl;
			//cout << "\tfdivp\t" << endl;
			cout << "\tfstpl\t" << this << endl;
		}
	}	
	else
	{
		cout << "\tmovl\t" << _left << ", %eax" << endl;
		cout << "\tcltd" << endl;
		cout << "\tmovl\t" << _right << ", %ecx" << endl;
		cout << "\tidivl\t" << "%ecx" << endl;
		cout << "\tmovl\t%eax, " << this << endl;
	}


stringLabel = false;



}



void Remainder::generate()
{
	cout << endl;


	_left->generate();
	_right->generate();
	assignTemp(this);	
	
	cout << "\tmovl\t" << _left << ", %eax" << endl;
	
	cout << "\tmovl\t%eax, %edx" << endl;
	cout << "\tmovl\t" << _right << ", %ecx" << endl;

	
	cout << "\tsarl\t$31, %edx" << endl;
	cout << "\tidivl\t" << "%ecx" << endl;
	cout << "\tmovl\t%edx, " << this << endl;


}



void Add::generate()
{
	cout << endl;


	_left->generate();
	_right->generate();
	assignTemp(this);	
	
	if(_left->type().isReal() && _right->type().isReal())
	{
		cout << "\tfldl\t" << _left << endl;
		//cout << "\tmovl\t" << _right << ", %eax" << endl;

		cout << "\tfaddl\t" << _right << endl;
		cout << "\tfstpl\t" << this << endl;
	}
	else if(_left->type().isReal() && _right->type().isInteger())
	{
		//cout << "F" << endl;
	}
	else if(_left->type().isInteger() && _right->type().isReal())
	{
		//cout << "D" << endl;
	}
	else if(_left->type().isInteger() && _right->type().isInteger())
	{
		
		cout << "\tmovl\t" << _left << ", %eax" << endl;
		cout << "\taddl\t" << _right << ", %eax" << endl;
		cout << "\tmovl\t%eax, " << this << endl;	
	}
	else if(_left->type().isPointer() && _right->type().isInteger())
	{
		//cout << "here" << endl;
		//cout << "here" << endl;
		cout << "\tmovl\t" << _right << ", %eax" << endl;
		cout << "\timull\t$" << scaleRight << ", %eax" << endl;
		cout << "\taddl\t" << _left << ", %eax" << endl;
		cout << "\tmovl\t%eax, " << this << endl;
		//cout << "\tmovl\t" << this << ", %ecx" << endl;
		//cout << "\tmovl\t%eax, (%ecx)" << endl;
	}
	else if(_left->type().isInteger() && _right->type().isPointer())
	{
		cout << "\tmovl\t" << _left << ", %eax" << endl;
		cout << "\timull\t$" << scaleLeft << ", %eax" << endl;
		cout << "\taddl\t" << _right << ", %eax" << endl;//scale
		cout << "\tmovl\t%eax, " << this << endl;	
	}
	else
	{
		//cout << "\n\n\nKKKK\n\n\n";
	}



}

void Subtract::generate()
{
	cout << endl;


	_left->generate();
	_right->generate();
	assignTemp(this);	
	
	if(_left->type().isReal() && _right->type().isReal())
	{
		cout << "\tfldl\t" << _left << endl;
		cout << "\tfsubl\t" << _right << endl;
		cout << "\tfstpl\t" << this << endl;
	}
	else if(_left->type().isReal() && _right->type().isInteger())
	{
		//cout << "W" << endl;
	}
	else if(_left->type().isInteger() && _right->type().isReal())
	{
		//cout << "R" << endl;
	}
	else if(_left->type().isInteger() && _right->type().isInteger())
	{

		cout << "\tmovl\t" << _left << ", %eax" << endl;
		cout << "\tsubl\t" << _right << ", %eax" << endl;
		cout << "\tmovl\t%eax, " << this << endl;	
	}
	else if(_left->type().isPointer() && _right->type().isInteger())
	{
	
		cout << "\tmovl\t" << _right << ", %eax" << endl;
		cout << "\timull\t$" << scaleResult << ", %eax" << endl;
		cout << "\tsubl\t" << _left << ", %eax" << endl;
		cout << "\tmovl\t%eax, " << this << endl;	
	}
	else if(_left->type().isPointer() && _right->type().isPointer())
	{
	
		cout << "\tmovl\t" << _left << ", %eax" << endl;
		cout << "\tsubl\t" << _right << ", %eax" << endl;
		cout << "\tmovl\t$" << scaleResult << ", %ecx" << endl;
		cout << "\tcltd\t" << endl;
		cout << "\tidivl\t%ecx" << endl;
		cout << "\tmovl\t%eax, " << this << endl;	
	}
	else
	{
		//cout << "\n\n\nKKKK\n\n\n";
	}


}



void LessThan::generate()
{
	cout << endl;


	_left->generate();
	_right->generate();
	assignTemp(this);	
	
	if(FP(this))
	{

		cout << "\tfldl\t" << _left << endl;
		cout << "\tfcompl\t" << _right << endl;
		cout << "\tfnstsw\t%eax" << endl;
		cout << "\tsahf\t" << endl;
		cout << "\tsetb\t%al" << endl;
		cout << "\tmovzbl\t%al, %eax" << endl;
		cout << "\tmovl\t%eax, " << this << endl;


	}
	else
	{	
		cout << "\tmovl\t" << _left << ", %eax" << endl;
		cout << "\tcmpl\t" << _right << ", %eax" << endl;
		cout << "\tsetl\t%al" << endl;
		cout << "\tmovzbl\t%al, %eax" << endl;
		cout << "\tmovl\t%eax, " << this << endl;
	}


}




void GreaterThan::generate()
{
	cout << endl;


	_left->generate();
	_right->generate();
	assignTemp(this);	

	
	if(FP(this))
	{

		cout << "\tfldl\t" << _left << endl;
		cout << "\tfcompl\t" << _right << endl;
		cout << "\tfnstsw\t%eax" << endl;
		cout << "\tsahf\t" << endl;
		cout << "\tseta\t%al" << endl;
		cout << "\tmovzbl\t%al, %eax" << endl;
		cout << "\tmovl\t%eax, " << this << endl;




	}
	else
	{	
		cout << "\tmovl\t" << _left << ", %eax" << endl;
		cout << "\tcmpl\t" << _right << ", %eax" << endl;
		cout << "\tsetg\t%al" << endl;
		cout << "\tmovzbl\t%al, %eax" << endl;
		cout << "\tmovl\t%eax, " << this << endl;
	}





}



void LessOrEqual::generate()
{
	cout << endl;



_left->generate();
	_right->generate();
	assignTemp(this);	

	
	if(FP(this))
	{

		cout << "\tfldl\t" << _left << endl;
		cout << "\tfcompl\t" << _right << endl;
		cout << "\tfnstsw\t%eax" << endl;
		cout << "\tsahf\t" << endl;
		cout << "\tsetbe\t%al" << endl;
		cout << "\tmovzbl\t%al, %eax" << endl;
		cout << "\tmovl\t%eax, " << this << endl;




	}
	else
	{	
		cout << "\tmovl\t" << _left << ", %eax" << endl;
		cout << "\tcmpl\t" << _right << ", %eax" << endl;
		cout << "\tsetle\t%al" << endl;
		cout << "\tmovzbl\t%al, %eax" << endl;
		cout << "\tmovl\t%eax, " << this << endl;
	}




}



void GreaterOrEqual::generate()
{
	cout << endl;


_left->generate();
	_right->generate();
	assignTemp(this);	

	
	if(FP(this))
	{

		cout << "\tfldl\t" << _left << endl;
		cout << "\tfcompl\t" << _right << endl;
		cout << "\tfnstsw\t%eax" << endl;
		cout << "\tsahf\t" << endl;
		cout << "\tsetae\t%al" << endl;
		cout << "\tmovzbl\t%al, %eax" << endl;
		cout << "\tmovl\t%eax, " << this << endl;


	}
	else
	{	
		cout << "\tmovl\t" << _left << ", %eax" << endl;
		cout << "\tcmpl\t" << _right << ", %eax" << endl;
		cout << "\tsetge\t%al" << endl;
		cout << "\tmovzbl\t%al, %eax" << endl;
		cout << "\tmovl\t%eax, " << this << endl;
	}





}



void LogicalAnd::generate()
{
	cout << endl;


	_left->generate();
	_right->generate();
	assignTemp(this);	

	
	if(FP(this))
	{
		



	}
	else
	{	
		cout << "\tmovl\t" << _left << ", %eax" << endl;
		cout << "\tcmpl\t$0, %eax" << endl;
		Label tmp, tmp2; 
		cout << "\tje\t";
		cout << tmp2 << endl;
		cout << "\tmovl\t" << _right << ", %eax" << endl;
		cout << "\tcmpl\t$0, %eax" << endl;
		cout << "\tje\t" << tmp2 << endl;
		cout << "\tmovl\t$1, " << this << endl;
		cout << "\tjmp\t" << tmp << endl;
		//Label tmp2();
		cout << tmp2 << ":" << endl;
	   	cout << "\tmovl\t$0, " << this << endl;
		//cout << "\tjmp\t"
		cout << tmp << ":" << endl;	
	}


}


void LogicalOr::generate()
{
	cout << endl;


	_left->generate();
	_right->generate();
	assignTemp(this);	
	
	if(FP(this))
	{
		//cout << "here" << endl;



	}
	else
	{	
		cout << "\tmovl\t" << _left << ", %eax" << endl;
		cout << "\tcmpl\t$0, %eax" << endl;
		Label tmp, tmp2;
		cout << "\tjne\t\t" << tmp << endl;
		cout << "\tmovl\t" << _right << ", %eax" << endl;
		cout << "\tcmpl\t$0, %eax" << endl;
		cout << "\tjne\t\t" << tmp << endl;
		cout << "\tmovl\t$0, " << this << endl;
		//Label tmp2();
		cout << "\tjmp\t" << tmp2 << endl;
		cout << tmp << ":" << endl;
		cout << "\tmovl\t$1, %eax" << endl;
		cout << "\tmovl\t%eax, " << this << endl;
		cout << "\tjmp\t" << tmp2 << endl;
		cout << tmp2 << ":" << endl;
	//	cou
	}



}



void Equal::generate()
{
	cout << endl;


	_left->generate();
	_right->generate();
	assignTemp(this);	

	
	if(FP(this))
	{

		cout << "\tfldl\t" << _left << endl;
		cout << "\tfcompl\t" << _right << endl;
		cout << "\tfnstsw\t%eax" << endl;
		cout << "\tsahf\t" << endl;
		cout << "\tsete\t%al" << endl;
		cout << "\tmovzbl\t%al, %eax" << endl;
		cout << "\tmovl\t%eax, " << this << endl;

	}
	else
	{	
		cout << "\tmovl\t" << _left << ", %eax" << endl;
		cout << "\tcmpl\t" << _right << ", %eax" << endl;
		cout << "\tsete\t%al" << endl;
		cout << "\tmovzbl\t%al, %eax" << endl;
		cout << "\tmovl\t%eax, " << this << endl;
	}


}



void NotEqual::generate()
{
	cout << endl;


	_left->generate();
	_right->generate();
	assignTemp(this);	

	
	if(FP(this))
	{

		cout << "\tfldl\t" << _left << endl;
		cout << "\tfcompl\t" << _right << endl;
		cout << "\tfnstsw\t%eax" << endl;
		cout << "\tsahf\t" << endl;
		cout << "\tsetne\t%al" << endl;
		cout << "\tmovzbl\t%al, %eax" << endl;
		cout << "\tmovl\t%eax, " << this << endl;


	}
	else
	{	
		cout << "\tmovl\t" << _left << ", %eax" << endl;
		cout << "\tcmpl\t" << _right << ", %eax" << endl;
		cout << "\tsetne\t%al" << endl;
		cout << "\tmovzbl\t%al, %eax" << endl;
		cout << "\tmovl\t%eax, " << this << endl;
	}




}



void Negate::generate()
{
	cout << endl;


	_expr->generate();
	//_right->generate();
	assignTemp(this);	

	
	if(FP(this))
	{
		if(_expr->offset == 0)
		{
			stringLabel == true;
			//cout << "here" << endl;
		}
		else
		{
			stringLabel == false;
		}
		//cout << _expr->offset << endl;
		if(_expr->offset == 0)
		{
			cout << "\tfldl\t" << globalLabel << endl;
		   	cout << "\tfchs\t" << endl;
			cout << "\tfstpl\t" << this << endl;
			stringLabel = false;
		}
		else
		{
			//cout << "here" << endl;
			cout << "\tfldl\t" << _expr << endl;
	   		cout << "\tfchs" << endl;
   			cout << "\tfstpl\t" << this << endl;	   
		}

	}
	else
	{	
		cout << "\tmovl\t" << _expr << ", %eax" << endl;
		cout << "\tnegl\t%eax" << endl;
		cout << "\tmovl\t%eax, " << this << endl;
	}

}



void Dereference::generate()
{
	cout << endl;
	//cout << _expr->type() << endl;


	_expr->generate();
	//_right->generate();
	assignTemp(this);	
	//cout << _expr->type() << endl;
	//cout << "hello" << endl;	
	if(lvalue() == false)
	{

		//cout << "hello2" << endl;
		//_lvalue = false;
	}
	else
	{	
		cout << endl;
		//cout << "Rvalue" << endl;
		//cout << _expr->offset << endl;
		cout << "\tmovl\t" << _expr << ", %eax" << endl;
		cout << "\tmovl\t(%eax), %eax" << endl;
		cout << "\tmovl\t%eax, " << this << endl;

	}



}



void Address::generate()
{
	cout << endl;

	//cout << "here" << endl;
	//_expr->operand(cout);
	_expr->generate();
	//_right->generate();
	assignTemp(this);
	//cout << _expr << endl;
	Expression *child;
	//child = _expr->isDereference();
	//cout << _expr->isDereference() << endl;
	//_expr->isDereference();
	if((child = _expr->isDereference())) 
	{
		cout << "\tmovl\t" << child << ", %eax" << endl;
		cout << "\tmovl\t%eax, " << this << endl;
	}
	else
	{
		//cout << "\tmovl\t" << _expr << ", %eax" << endl;
		//Label x;
		//int tmp = x.number();
		//listStrings.insert({tmp, 
		/*std::unordered_map<int, std::string>::const_iterator got = listStrings.find(globalLabel.number());
		std::unordered_map<int, std::string>::const_iterator git = listDoubles.find(globalLabel.number());
		if(got == listStrings.end() || git == listDoubles.end())
		{
			stringLabel = true;
		}
		else
		{
			stringLabel = false;
		}
*/
		bool p;
		if(_expr->offset == 0)
		{
			p = true;
		}
		else
		{
			p = false;
		}
		//cout << _expr->offset << endl;
		//cout << offset << endl;
		if(stringLabel == true)
		{
			//cout << "a" << endl;
			
			cout << "\tleal\t" << globalLabel << ", %eax" << endl;
			//assignTemp(this);
			cout << "\tmovl\t%eax, " << this << endl; 
			stringLabel = false;
		}
		else
		{
			//cout << "\tmovl\t" << _expr << ", %eax" << endl;
			cout << "\tleal\t" << _expr << ", %eax" << endl;
			cout << "\tmovl\t%eax, " << this << endl;
		}
		//cout << "\tmovl\t%eax, " << this << endl;
	}

}


void Not::generate()
{
	cout << endl;


	_expr->generate();
	//_right->generate();
	assignTemp(this);	

	
	if(FP(this))
	{	
		cout << "\tfldl\t" << this << endl;
		cout << "\tftst\t" << endl;
		cout << "\tfnstsw %ax" << endl;
		cout << "\tfstp\t%st(0)" << endl;
		cout << "\tsahf" << endl;

	}
	else
	{	
		cout << "\tmovl\t" << _expr << ", %eax" << endl;
		cout << "\tcmpl\t$0" << ", %eax" << endl;
		cout << "\tsete\t%al" << endl;
		cout << "\tmovzbl\t%al, %eax" << endl;
		cout << "\tmovl\t%eax, " << this << endl;
	}

}



void Increment::generate()
{
	cout << endl;


	_expr->generate();
	assignTemp(this);

	if(FP(this))
	{
		cout << "\tfld1\t" << endl;
		cout << "\tfldl\t" << _expr << endl;
		cout << "\tfstl\t" << this << endl;
		cout << "\tfaddp\t%st(1), %st" << endl;
	  	cout << "\tfstpl\t" << _expr << endl; 	
	}
	else
	{
		cout << "\tmovl\t" << _expr << ", %eax" << endl;
		cout << "\tmovl\t%eax, " << this << endl;
		cout << "\taddl\t$" << scale << ", %eax" << endl;
		cout << "\tmovl\t%eax, " << _expr << endl;
	}


}



void Decrement::generate()
{
	cout << endl;


	_expr->generate();
	assignTemp(this);
	if(FP(this))
	{
		cout << "\tfld1\t" << endl;
		cout << "\tfldl\t" << _expr << endl;
		cout << "\tfstl\t" << this << endl;
		cout << "\tfsubp\t%st(1), %st" << endl;
	  	cout << "\tfstpl\t" << _expr << endl; 	
	}
	else
	{
		cout << "\tmovl\t" << _expr << ", %eax" << endl;
		cout << "\tmovl\t%eax, " <<  this << endl;
		cout << "\tsubl\t$" << scale << ", %eax" << endl;
		cout << "\tmovl\t%eax, " << _expr << endl;
	}


}



void Cast::generate()
{
	cout << endl;


	_expr->generate();
	assignTemp(this);
	//cout << _expr->type() << endl;
	if(_expr->type().isReal())
	{
		if(type().isInteger())
		{
			cout << "\tfldl\t" << _expr << endl;
			cout << "\tfisttpl\t" << this << endl;
			//cout << "h" << endl;
		}
		else if(type().isReal())
		{
			cout << "\tfldl\t" << _expr << endl;;
			cout << "\tfstpl\t" << this << endl;	
		}
		else
		{
			cout << "\tmovsbl\t" << _expr << ", %eax" << endl;
			cout << "\tmovl\t%eax, " << _expr << endl;
			cout << "\tfldl\t" << _expr << endl;
			cout << "\tfisttpl\t" << this << endl;
		}	
	}
	else if(_expr->type().size() == 1)
	{
		if(type().isInteger())
		{
			cout << "\tmovsbl\t" << _expr << ", %eax" << endl;
			cout << "\tmovl\t%eax, " << this << endl;
		}
		else if(type().isReal())
		{
			//cout << "\tmovsbl\t" << _expr << ", %eax" << endl;
			cout << "\tfild\t" << _expr << endl;
		   	cout << "\tfstpl\t" << this << endl;
		}
		else
		{
			cout << "\tmovb\t" << _expr << ", %al" << endl;
			cout << "\tmovb\t%al, " << this << endl; 
		}	
	}
	else
	{
		if(type().size() == 4)
		{
			//cout << "here" << endl;
			cout << "\tmovl\t" << _expr << ", %eax" << endl;
			cout << "\tmovl\t%eax, " << this << endl;
			
			//cout << "\tfild\t" << _expr << endl;
			//cout << "\tfisttpl\t" << this << endl;

		}
		else if(type().isReal())
		{
			//cout << "\tmovl\t" << _expr << ", %eax" << endl;
			cout << "\tfildl\t" << _expr << endl; 
			cout << "\tfstpl\t" << this << endl;
		}
		else
		{
			cout << "\tmovl\t" << _expr << ", %eax" << endl;
			cout << "\tmovl\t%eax, " << this << endl;
		}	
	}


}



void String::generate()
{
	cout << endl;


	Label tmp;
	int x = tmp.number();
	globalLabel = tmp;
	//std::pair<std::string&, unsigned &> obj(value(), x);
	string escaped = escapeString(value());
	
	listStrings.insert({x, escaped});
	stringLabel = true;
	//cout << "here" << endl;
}

void Real::generate()
{
	cout << endl;
	numberDouble++;
	assignTemp(this);
	//cout << "t" << endl;
	Label tmp;
	int x = tmp.number();
	globalLabel = tmp;
	listDoubles.insert({x, value()});
	stringLabel = true;
//	cout << "\tleal\t" << globalLabel <<  ", %eax" << endl;
//	cout << "\tmovl\t%eax, " << this << endl;
	//cout << "Hello" << endl;
}

/*void Expression::generate()
{
}
*/

void Return::generate()
{
	//cout << "returning" << endl;
	//cout << "gadfgdafgDF" << endl;
	_expr->generate();
	//assignTemp(this);
	if(_expr->type().isReal())
	{
		cout << "\tfldl\t" << _expr << endl;
		cout << "\tjmp\t" << returnLabel << endl;
	}
	else
	{
		cout << "\tmovl\t" << _expr << ", %eax" << endl;
		cout << "\tjmp\t" << returnLabel << endl;
	}
}

void Expression::test(const Label &label, bool ifTrue)
{
	cout << endl;
	generate();
	if(FP(this))
	{

		cout << "\tfldl\t" << this << endl;
		cout << "\tftst\t" << endl;
		cout << "\tfnstsw %ax" << endl;
		cout << "\tfstp\t%st(0)" << endl;
		cout << "\tsahf" << endl;
	}
	else
	{
		cout << "\tmovl\t" << this << ", %eax" << endl;
		cout << "\tcmpl\t$0, %eax" << endl;
	}
	cout << (ifTrue ? "\tjne\t" : "\tje\t") << label << endl;	
}

void Break::generate()
{
	cout << "\tjmp\t" << inLoop << endl;
}

void While::generate()
{
	Label loop, exit;
	inLoop = exit;
	cout << loop << ":" << endl;

	_expr->test(exit, false);
	_stmt->generate();

	cout << "\tjmp\t" << loop << endl;
	cout << exit << ":" << endl;

}



void For::generate()
{

	cout << endl;

	Label loop, exit;
	inLoop = exit;
	_init->generate();

	cout << loop << ":" << endl;
	
	_expr->test(exit, false);
	
	_stmt->generate();

	_incr->generate();

	cout << "\tjmp\t" << loop << endl;

	cout << exit << ":" << endl;

}

void If::generate()
{
	cout << endl;
	//cout << "here" << endl;
	Label ELSE, SKIP;
	//cout << IF << ":" << endl;

	_expr->test(ELSE, false);
	_thenStmt->generate();
	cout << "\tjmp\t" << SKIP << endl;
	
	cout << ELSE << ":" << endl;
	if(_elseStmt)
	{
		_elseStmt->generate();
	}
	cout << SKIP << ":" << endl;
}










