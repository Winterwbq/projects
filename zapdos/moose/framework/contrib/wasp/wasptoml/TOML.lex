%top{
/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

}
%{ /*** C/C++ Declarations ***/

#include <string>
#include <sstream>
#include "TOMLLexer.h"
#include "TOMLInterpreter.h"

/* import the parser's token type into a local typedef */
typedef wasp::TOMLParser::token token;
typedef wasp::TOMLParser::token_type token_type;

/* By default yylex returns int, we use token_type. Unfortunately yyterminate
 * by default returns 0, which is not of token_type.
 * This logic ensures we continue to return END until the parser is done*/
#define yyterminate() {if (file_offset >0){rewind();} return token::END;}

%}

 /*** Flex Declarations and Options ***/

 /* enable c++ scanner class generation */
%option c++

%option prefix="TOML"
%option outfile="TOMLLexer.cpp"


 /* enable scanner to generate debug output. disable this for release
 * versions. */
%option debug

 /* no support for include files is planned */
%option yywrap nounput yymore

 /* enables the use of start condition stacks */
%option stack
%x raw_string
%x value_state
%x array_state
%s object_state



 /* The following paragraph suffices to track locations accurately. Each time
 * yylex is invoked, the begin position is moved onto the end position. */
%{
#define YY_USER_ACTION  if(eof_reached) yyterminate(); yylloc->columns(yyleng); file_offset+=yyleng;

//  Reduce duplicate code to allow the undoing of YY_USER_ACTION column and file_offset increments
#define do_yymore() { \
                    yymore(); \
                    yylloc->columns(-yyleng); \
                    file_offset-=yyleng; \
                    }
%}

%% /*** Regular Expressions Part ***/

 /* code to place at the beginning of yylex() */
%{
    // reset location
    yylloc->step();
%}
 /*** BEGIN EXAMPLE - Change the TOML lexer rules below ***/

 // TODO support raw/multi-line string using exclusive start condition for raw_string
 // TODO support boolean (true, false) as a value
 // TODO support 3.14159 as object 3 and nested 14159

<*>#[^\n]* {
    capture_token(yylval,wasp::COMMENT);
    return token::COMMENT;
    }
<INITIAL,object_state>[a-zA-Z0-9_\-]+ {
    capture_token(yylval,wasp::IDENTIFIER);
    return token::ID;
    }
<*>\"[^\"\n]*\" {
    capture_token(yylval,wasp::STRING);
    if (YY_START == value_state) yy_pop_state();
    return token::STRING;
    }
<*>'[^\'\n]*' {
    capture_token(yylval,wasp::STRING);
    if (YY_START == value_state) yy_pop_state();
    return token::STRING;
    }
<*>[\+\-]?[0-9]+ {
    capture_token(yylval,wasp::INTEGER);
    if (YY_START == value_state) yy_pop_state();
    return token::INTEGER;
    }
<*>[\+\-]?([0-9]+|[0-9]*\.[0-9]+)([eE][\+\-]?[0-9]+)? {
    capture_token(yylval,wasp::REAL);
    if (YY_START == value_state) yy_pop_state();
    return token::REAL;
    }
<INITIAL,value_state,array_state>\[ {
    capture_token(yylval,wasp::LBRACKET);
    if (YY_START == value_state)
    {
        yy_pop_state();
        // key = array, push array state
        yy_push_state(array_state);
    }
    else if (YY_START == array_state) // nested-array
    {
        yy_push_state(array_state);
    }
    return token::LBRACKET;
}

<value_state,array_state>true {
    capture_token(yylval,wasp::WASP_TRUE);
    if (YY_START == value_state) yy_pop_state();
    return token::TOKEN_TRUE;
}
<value_state,array_state>false {
    capture_token(yylval,wasp::WASP_FALSE);
    if (YY_START == value_state) yy_pop_state();
    return token::TOKEN_FALSE;
}

\[\[ {
    capture_token(yylval,wasp::DLBRACKET);
    return token::DLBRACKET;
}

<INITIAL,array_state>\] {
    capture_token(yylval,wasp::RBRACKET);
    if (YY_START == array_state) yy_pop_state();
    return token::RBRACKET;
}

\]\] {
    capture_token(yylval,wasp::DRBRACKET);
    return token::DRBRACKET;
}

<value_state,array_state>\{ {
    capture_token(yylval,wasp::LBRACE);
    if (YY_START == value_state) yy_pop_state();
    yy_push_state(object_state);
    return token::LBRACE;
}
<object_state>\} {
    capture_token(yylval,wasp::RBRACE);
    yy_pop_state();
    return token::RBRACE;
}

<*>, {
    capture_token(yylval,wasp::WASP_COMMA);
    return token::COMMA;
}

<*>\. {
    capture_token(yylval,wasp::SEPARATOR);
    return token::SEPARATOR;
}

<*>= {
    capture_token(yylval,wasp::ASSIGN);
    yy_push_state(value_state);
    return token::ASSIGN;
}

 /* gobble up white-spaces */
<*>[ \t\r]+ {
    yylloc->step();
}

 /* gobble up end-of-lines */
<*>\n {
    yylloc->lines(yyleng); yylloc->step();
    interpreter.push_line_offset(file_offset-yyleng);
}

 /* pass all other characters up to TOML*/
<*>. {
    return static_cast<token_type>(*yytext);
}
<*><<EOF>> {
    eof_reached = true;
    yyterminate();
}

 /*** END EXAMPLE - Change the TOML lexer rules above ***/

%% /*** Additional Code ***/

namespace wasp {

TOMLLexerImpl::TOMLLexerImpl(
                AbstractInterpreter & interpreter,
                std::istream* in,
                std::ostream* out)
    : TOMLFlexLexer(in, out)
    , interpreter(interpreter)
    , file_offset(0)
    , eof_reached(false)
{
}

TOMLLexerImpl::~TOMLLexerImpl()
{
}

void TOMLLexerImpl::set_debug(bool b)
{
    yy_flex_debug = b;
}
void TOMLLexerImpl::rewind()
{
    yyin.seekg(-yyleng,std::ios_base::cur);
    yyless(0);
}
void TOMLLexerImpl::capture_token(
        wasp::TOMLParser::semantic_type* yylval
        ,wasp::NODE type)
{
    std::size_t offset = file_offset - yyleng;
    yylval->token_index = interpreter.token_count();
    interpreter.push_token(yytext,type,offset);
}
} // end of namespace

/* This implementation of TOMLFlexLexer::yylex() is required to fill the
 * vtable of the class TOMLFlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the TOMLLexerImpl class instead. */

#ifdef yylex
#undef yylex
#endif

int TOMLFlexLexer::yylex()
{
    return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int TOMLFlexLexer::yywrap()
{
    return 1;
}
