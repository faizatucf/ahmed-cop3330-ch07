/*
 *  UCF COP3330 Fall 2021 Assignment 6 Solution
 *  Copyright 2021 Faiz Ahmed
 */


#include <iostream>
#include "std_lib_facilities.h"
using namespace std;

class Token
{
	public:
	char kind;
	double value;
	string name;
	Token(char ch) :kind(ch), value(0) { }
	Token(char ch, double val) :kind(ch), value(val) { }
	Token(char ch, string n) :kind(ch), name(n) { }
};

class Token_stream {
public:
	
	void putback(Token t);
	Token_stream();
	Token get();
	void ignore(char c);

	private:
	bool full;
	Token buffer;
	//void unget(Token t) { buffer = t; full = true; }

};



Token_stream::Token_stream():full(false), buffer(0) { }

const char let = 'L';
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';
const char con = 'C';
const string declkey = "let";
const string quitkey = "quit";

Token Token_stream::get()
{
	if (full) { full = false; return buffer; }
	char ch;
	cin.get(ch);
	while (isspace(ch))
	{
		if (ch == '\n')
			return Token(print);
		cin.get(ch);
	}

	switch (ch) {
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '%':
	case ';':
	case '=':
	case '/':
		return Token(ch);
	case '.':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	{	
		cin.putback(ch);
	double val;
	cin >> val;
	return Token(number, val);
	}

	default:
		if (isalpha(ch)) {
			string s;
			s += ch;
			while (cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch=='_')) 
			s+=ch;
			cin.putback(ch);
			if (s == declkey) return Token(let);
			if (s == quitkey) return Token(quit);
			return Token(name, s);
		}
		error("Bad token");
	}
}

void Token_stream::putback(Token t)
{
	if (full)
	cout<<"putback() into full buffer!";
	buffer = t;
	full = true;
}

void Token_stream::ignore(char c)
{
	if (full && c == buffer.kind) {
		full = false;
		return;
	}
	full = false;

	char ch = 0;
	while (cin >> ch)
		if (ch == c) return;
}

class Variable {
	public:
	string name;
	double value;
	bool is_const;
	Variable (string n, double v, bool va = true): name(n), value(v), is_const(va) { }
};

vector<Variable> var_table;

void get_value(string s, double d)
{
	for (Variable& v : var_table)
 		if (v.name == s) {
			  v.value = d;
 			 return;
		 }
 			error("get: undefined variable ", s);
}

class Symbol_table{
	public:
	double get(string s);
	void set(string s, double d);
	bool is_declared(string var);
	double declare(string var,double val,bool b);
	
	private:
	vector<Variable> var_table;
};

void set_value(string s, double d)
{
	for (int i = 0; i<var_table.size(); ++i)
		if (var_table[i].name == s)
		{
			if (var_table[i].is_const)
			{		
			cout<<s<<" is a constant";
			return;
			}
			var_table[i].value = d;
			return;
		}
	cout<<"set- it is an undefined variable "<<s;
}

//double var = 7.2;


double Symbol_table::get(string s)
{
	for (int i=0; i<var_table.size(); ++i)
		if (var_table[i].name == s)
			return var_table[i].value;
	cout<<"get- variable not defined "<<s;
}

void Symbol_table::set(string s, double d)
{
	for (int i = 0; i<var_table.size(); ++i)
		if (var_table[i].name == s)
		{
			if (var_table[i].is_const)
				cout<<s<<" is a constant";
			var_table[i].value = d;
			return;
		}
	cout<<"set: variable has not been defined "<<s;
}

bool Symbol_table::is_declared(string var)
{
	for (int i = 0; i<var_table.size(); ++i)
		if (var_table[i].name == var)
			return true;
	return false;
}

double Symbol_table::declare(string var, double val,bool b)
{
	if (is_declared(var))
		cout<<var<<" is declared two times";
	var_table.push_back(Variable(var,val,b));
	return val;
}

Symbol_table symt;

Token_stream ts;

double expression();

double primary()
{
	Token t = ts.get();
	switch (t.kind) {
	case '(':
	{	
	double d = expression();
	t = ts.get();
	if (t.kind != ')') error("'(' expected");
	return d;
	}
	case '-':
		return -primary();
	case '+':
		return t.value;
	case number:
		return t.value;
	case name:
	{
		Token t2 = ts.get();
		if (t2.kind == '=')
		{
			double d = expression();
			symt.set(t.name, d);
			return d;
		}
		else{
			ts.putback(t2);
			return symt.get(t.name);
		}
		//ts.putback(t2);
		//return symt.get(t.name);
	}
	default:
		error("primary expected");
	}
}

double term()
{
	double left = primary();
	Token t = ts.get();
	while (true) {
		switch (t.kind) {
		case '*':
			left *= primary();
			break;
		case '/':
		{	double d = primary();
		if (d == 0) error("divide by zero");
		left /= d;
		t = ts.get();
		break;
		}
		case '%':
		{
			double d = primary();
			int i1 = int(left);
			if (i1 != left)
				cout<<"left-hand operand of ""% is not int";
			int i2 = int(d);
			if (i2 != d)
				cout<<"right-hand operand ""of % is not int";
			if (i2 == 0)
			cout<<"%: divide by zero";
			left = i1%i2;
			t = ts.get();
			break;
		}

		default:
			ts.putback(t);
			return left;
		}
	}
}

double expression()
{
	double left = term();
	Token t = ts.get();
	while (true) {
		switch (t.kind) {
		case '+':
			left += term();
			t = ts.get();
			break;
		case '-':
			left -= term();
			t = ts.get();
			break;
		case '=':
			cout<<"use of '=' outside of a " "declaration";
		default:
			ts.putback(t);
			return left;
		}
	}
}

double declaration(bool b)
{
	Token t = ts.get();
	if (t.kind != name) 
		error("name expected in declaration");
	string name = t.name;
	Token t2 = ts.get();
	cout<<t2.kind<<"\n";
	if (t2.kind != '=') 
		error("= missing in declaration of ", name);
	double d = expression();
	symt.declare(name,d,b);
	return d;
}

double statement()
{
	Token t = ts.get();
	switch (t.kind) {
	case let:
		return declaration(false);
	case con:
		return declaration(true);
	default:
		ts.putback(t);
		return expression();
	}
}

void clean_up_mess()
{
	ts.ignore(print);
}

const string prompt = "> ";
const string result = "= ";

void calculate()
{

	while (cin) try {
		cout << prompt;
		Token t = ts.get();
		while (t.kind == print) t = ts.get();
		if (t.kind == quit) return;
		ts.putback(t);
		cout << result << statement() << endl;
	}
	catch (runtime_error& e) {
		cerr << e.what() << endl;
		clean_up_mess();
	}
}

int main()
{

try {
	symt.declare("e",2.7182818284,true);
	symt.declare("k",1000,true);
	symt.declare("pi",3.1415926535,true);
	calculate();
	return 0;
	}

catch (exception& e) {
	cerr << "exception: " << e.what() << endl;
	char c;
	while (cin >> c && c != ';');
	return 1;
	}

catch (...) {
	cerr << "exception\n";
	char c;
	while (cin >> c && c != ';');
	return 2;
	}

}