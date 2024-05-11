package rs.ac.bg.etf.pp1;

import java_cup.runtime.Symbol;

%%

%{

	boolean error_detected = false;

	// ukljucivanje informacije o poziciji tokena
	private Symbol new_symbol(int type) {
		return new Symbol(type, yyline+1, yycolumn);
	}

	// ukljucivanje informacije o poziciji tokena
	private Symbol new_symbol(int type, Object value) {
		return new Symbol(type, yyline+1, yycolumn, value);
	}

	public void report_error(String symbol_value, int line, int column) {
		this.error_detected = true;
		System.err.println(
			  "Lexic Error (" + symbol_value + ") on line " + line
			+ " at column " + column
		);
	}

	public boolean wasErrorDetected() {
		return this.error_detected;
	}

%}

%cup
%line
%column

%xstate COMMENT

%eofval{
	return new_symbol(sym.EOF);
%eofval}

%%

" " 	{ }
"\b" 	{ }
"\t" 	{ }
"\r\n" 	{ }
"\r" 	{ }
"\n" 	{ }
"\f" 	{ }


"program" { return new_symbol(sym.PROG, yytext()); }
"break" { return new_symbol(sym.BREAK, yytext()); }
"class" { return new_symbol(sym.CLASS, yytext()); }
"else" { return new_symbol(sym.ELSE, yytext()); }
"const" { return new_symbol(sym.CONST, yytext()); }
"if" { return new_symbol(sym.IF, yytext()); }
"new" { return new_symbol(sym.NEW, yytext()); }
"print" { return new_symbol(sym.PRINT, yytext()); }
"read" { return new_symbol(sym.READ, yytext()); }
"return" { return new_symbol(sym.RETURN, yytext()); }
"void" { return new_symbol(sym.VOID, yytext()); }
"extends" { return new_symbol(sym.EXTENDS, yytext()); }
"continue" { return new_symbol(sym.CONTINUE, yytext()); }
"for" { return new_symbol(sym.FOR, yytext()); }
"static" { return new_symbol(sym.STATIC, yytext()); }
"namespace" { return new_symbol(sym.NAMESPACE, yytext()); }

"+" { return new_symbol(sym.PLUS, yytext()); }
"-" { return new_symbol(sym.MINUS, yytext()); }
"*" { return new_symbol(sym.MULTIPLY, yytext()); }
"/" { return new_symbol(sym.DIVIDE, yytext()); }
"%" { return new_symbol(sym.MODULUS, yytext()); }
"==" { return new_symbol(sym.EQUALITY, yytext()); }
"!=" { return new_symbol(sym.INEQUALITY, yytext()); }
">" { return new_symbol(sym.GT, yytext()); }
">=" { return new_symbol(sym.GTE, yytext()); }
"<" { return new_symbol(sym.LT, yytext()); }
"<=" { return new_symbol(sym.LTE, yytext()); }
"&&" { return new_symbol(sym.AND, yytext()); }
"||" { return new_symbol(sym.OR, yytext()); }
"=" { return new_symbol(sym.EQUAL, yytext()); }
"++" { return new_symbol(sym.INCR, yytext()); }
"--" { return new_symbol(sym.DECR, yytext()); }
";" { return new_symbol(sym.SEMI, yytext()); }
":" { return new_symbol(sym.COLON, yytext()); }
"," { return new_symbol(sym.COMMA, yytext()); }
"." { return new_symbol(sym.DOT, yytext()); }
"(" { return new_symbol(sym.LPAREN, yytext()); }
")" { return new_symbol(sym.RPAREN, yytext()); }
"[" { return new_symbol(sym.LBRACKET, yytext()); }
"]" { return new_symbol(sym.RBRACKET, yytext()); }
"{" { return new_symbol(sym.LBRACE, yytext()); }
"}" { return new_symbol(sym.RBRACE, yytext()); }
"=>" { return new_symbol(sym.THICKARROW, yytext()); }

<YYINITIAL> "//" 		     { yybegin(COMMENT); }
<COMMENT> .      { yybegin(COMMENT); }
<COMMENT> "\r\n" { yybegin(YYINITIAL); }
<COMMENT> "\n" { yybegin(YYINITIAL); }
<COMMENT> "\r" { yybegin(YYINITIAL); }

[0-9]+  { return new_symbol(sym.NUMBER, Integer.parseInt(yytext())); }
("true"|"false") { return new_symbol(sym.BOOL, Boolean.valueOf(yytext())); }
\'\\b\' { return new_symbol(sym.CHAR, Character.valueOf('\b')); }
\'\\t\' { return new_symbol(sym.CHAR, Character.valueOf('\t')); }
\'\\n\' { return new_symbol(sym.CHAR, Character.valueOf('\n')); }
\'\\r\' { return new_symbol(sym.CHAR, Character.valueOf('\r')); }
\'.\' { return new_symbol(sym.CHAR, Character.valueOf(yytext().charAt(1))); }
[a-zA-Z][a-zA-Z0-9_]* 	{ return new_symbol (sym.IDENT, yytext()); }

. {
	report_error(yytext(), yyline + 1, yycolumn + 1);
  }
