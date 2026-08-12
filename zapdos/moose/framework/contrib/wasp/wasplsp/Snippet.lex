%top{
/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

}
%{ /*** C/C++ Declarations ***/

#include <string>
#include <sstream>
#include "SnippetLexer.h"
#include "SnippetInterpreter.h"

/* import the parser's token type into a local typedef */
typedef wasp::SnippetParser::token token;
typedef wasp::SnippetParser::token_type token_type;

/* By default yylex returns int, we use token_type. Unfortunately yyterminate
 * by default returns 0, which is not of token_type.
 * This logic ensures we continue to return END until the parser is done*/
#define yyterminate() {if (file_offset >0){rewind();} return token::END;}

%}

 /*** Flex Declarations and Options ***/

 /* enable c++ scanner class generation */
%option c++

%option prefix="Snippet"
%option outfile="SnippetLexer.cpp"


 /* enable scanner to generate debug output. disable this for release
 * versions. */
 /*%option debug*/

 /* no support for include files is planned */
%option yywrap nounput yymore

 /* enables the use of start condition stacks */
%option stack
%x tabstop_state
%x placeholder_state
%x placeholder_colon_state
%x placeholder_any_state

INTEGER [0-9]+
COLON :
DOLLAR \$
LBRACE \{
RBRACE \}
ESCAPES \\[$\}]
 // TEXT is anything but the '$' unless escaped
TEXT ([^$]|\\$)+
 // TEXT that trails a match
ENCLOSED_TEXT ([^$\}]|{ESCAPES})+

 /* The following paragraph suffices to track locations accurately. Each time
 * yylex is invoked, the begin position is moved onto the end position. */
%{
#define YY_USER_ACTION  yylloc->columns(yyleng); file_offset+=yyleng;

%}

%% /*** Regular Expressions Part ***/

 /* code to place at the beginning of yylex() */
%{
    // reset location
    yylloc->step();
%}
 /*** BEGIN EXAMPLE - Change the Snippet lexer rules below ***/

<tabstop_state>{INTEGER} {
    capture_token(yylval, wasp::INTEGER);
    yy_pop_state(); // INTEGER immediately followed '$'
    return token::INTEGER;
}

<placeholder_state>{INTEGER} { // accessed via '${'
    capture_token(yylval, wasp::INTEGER);
    yy_push_state(placeholder_colon_state); // expect colon
    return token::INTEGER;
}

{DOLLAR} {
    // To be a tabstop or placeholder declarator the '$'
    // should be followed by an opening curly or an integer
    capture_token(yylval, wasp::DECL);
    yy_push_state(tabstop_state);
    return static_cast<token_type>(*yytext);
}

{TEXT} {
    capture_token(yylval, wasp::STRING);
    return token::TEXT;
}

<tabstop_state>{LBRACE} {
    yy_push_state(placeholder_state);
    return static_cast<token_type>(*yytext);
}

<tabstop_state>{RBRACE} {
    yy_pop_state(); // go back to start state
    return static_cast<token_type>(*yytext);
}

<placeholder_colon_state>{COLON} {
    // return individual character important to parser interpretation
    yy_pop_state(); // pop placeholder_colon_state
    yy_pop_state(); // pop placeholder_state, back to tabstop_state
    yy_push_state(placeholder_any_state);
    return static_cast<token_type>(*yytext);
}
<placeholder_colon_state>{RBRACE} { // found closing brace instead of colon (${int})
    // return individual character important to parser interpretation
    yy_pop_state(); // pop placeholder_colon_state
    yy_pop_state(); // pop placeholder_state, back to tabstop_state
    yy_pop_state(); // pop tabstop_state to exit
    return static_cast<token_type>(*yytext);
}
<placeholder_any_state>{ENCLOSED_TEXT} {
    yy_pop_state(); // pop placeholder_any_state, back to tabstop_state
    capture_token(yylval, wasp::STRING);
    return token::TEXT;
}
<placeholder_any_state>{RBRACE} { // found closing brace instead of text (${int:})
    // return individual character important to parser interpretation
    yy_pop_state(); // pop placeholder_any_state
    yy_pop_state(); // pop tabstop_state to exit
    return static_cast<token_type>(*yytext);
}

<*><<EOF>> {
    yyterminate();
}

 /*** END EXAMPLE - Change the Snippet lexer rules above ***/

%% /*** Additional Code ***/

namespace wasp {

SnippetLexerImpl::SnippetLexerImpl(
                AbstractInterpreter & interpreter,
                std::istream* in,
                std::ostream* out)
    : SnippetFlexLexer(in, out)
    , interpreter(interpreter)
    , file_offset(0)
{
}

SnippetLexerImpl::~SnippetLexerImpl()
{
}

void SnippetLexerImpl::set_debug(bool b)
{
    yy_flex_debug = b;
}
void SnippetLexerImpl::rewind()
{
    yyin.seekg(-yyleng,std::ios_base::cur);
    yyless(0);
}
void SnippetLexerImpl::capture_token(
        wasp::SnippetParser::semantic_type* yylval
        ,wasp::NODE type)
{
    std::size_t offset = file_offset - yyleng;
    yylval->token_index = interpreter.token_count();
    interpreter.push_token(yytext,type,offset);
}
} // end of namespace

/* This implementation of SnippetFlexLexer::yylex() is required to fill the
 * vtable of the class SnippetFlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the SnippetLexerImpl class instead. */

#ifdef yylex
#undef yylex
#endif

int SnippetFlexLexer::yylex()
{
    return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int SnippetFlexLexer::yywrap()
{
    return 1;
}
