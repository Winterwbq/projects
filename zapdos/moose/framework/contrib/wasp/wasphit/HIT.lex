%top{
/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

}
%{ /*** C/C++ Declarations ***/

#include <string>
#include <sstream>
#include "HITLexer.h"
#include "HITInterpreter.h"

/* import the parser's token type into a local typedef */
typedef wasp::HITParser::token token;
typedef wasp::HITParser::token_type token_type;

/* By default yylex returns int, we use token_type. Unfortunately yyterminate
 * by default returns 0, which is not of token_type. 
 * This logic ensures we continue to return END until the parser is done*/
#define yyterminate() {if (file_offset >0){rewind();} return token::END;}

%}

 /*** Flex Declarations and Options ***/

 /* enable c++ scanner class generation */
%option c++

%option prefix="HIT"
%option outfile="HITLexer.cpp"


 /* enable scanner to generate debug output. disable this for release
 * versions. */
 /*%option debug*/

 /* no support for include files is planned */
%option yywrap nounput yymore

 /* enables the use of start condition stacks */
%option stack
%s object
%s param
%s assign
%s array
%x lbracket
%x rbracket
%x file_include
%x brace_expression_state
%x trailing_brace_expression_state

INTEGER -?[0-9]+
EXPONENT [eE][\+\-]?{INTEGER}
REAL {INTEGER}?\.{INTEGER}{EXPONENT}?|{INTEGER}\.({INTEGER}{EXPONENT}?)?|{INTEGER}\.?{EXPONENT}

DOUBLE_QUOTED_STRING \"([^\\\"]|\\\"|\\[^\"])*\"
SINGLE_QUOTE '
UNICODE [^\x00-\x7F]+
COMMENT #([^\n]|{UNICODE})*

 // Anything But Closing Brace allows consuming invalid/unknown tokens until newline or closing brace
ABCB [^ \]\t\r\n]+
ASSIGN =
OVERRIDE_ASSIGN :(override)?=
LBRACKET \[

 /* START matches '${', END matches '}', INNER matches other pieces in scope */
BRACE_EXPRESSION_START \$\{
BRACE_EXPRESSION_INNER ([^\$\}]|\$[^\{])+
BRACE_EXPRESSION_END \}
TRAILING_BRACE_EXPRESSION [^ \t\$\}\n'"]+

 /* VALUE and ARRAY strings allow '$' if followed by anything other than '{' */
 /* but anything illegal after '$' will get removed and return to the stream */
NORMAL_VALUE_STRING ([^ \'\"\n\t\r\[\]\#\$]|\$[^\{])([^ \n\t\r\[\]\#\$]|\$[^\{])*
NORMAL_ARRAY_STRING ([^ \"\n\t\r\;\\'\$]|\$[^\{]|\\'|\\[^'])+

PERIOD_OBJCT_STRING \.[^\/ \n\[\]\=\#\&][^ \n\[\]\=\#\&]+
NORMAL_OBJCT_STRING   [^\. \n\[\]\=\#\&][^ \n\[\]\=\#\&]*
VALUE_STRING {NORMAL_VALUE_STRING}
ARRAY_STRING {NORMAL_ARRAY_STRING}|{ASSIGN}
OBJCT_STRING {PERIOD_OBJCT_STRING}|{NORMAL_OBJCT_STRING}
PARAM_STRING [^ \'\"\=\n\t\r\[\]\#\&\;]*[^ \'\"\=\n\t\r\[\]\#\&\;\:]
RBRACKET \]
SEMICOLON ;
TOP_OBJECT_TERM \[" "*\]
SUB_OBJECT_TERM \[" "*\.\.\/" "*\]
OBJECT_TERM {TOP_OBJECT_TERM}|{SUB_OBJECT_TERM}
DOT_SLASH \.\/
INCLUDE_PATH [^ \t\n][^\n#\[]*


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

#define do_brace_end() { \
    yy_pop_state(); \
    bool in_array_state = YY_START == array; \
    bool in_assign_state = YY_START == assign; \
    \
    if (in_array_state || in_assign_state)  \
    { \
        capture_token(yylval, wasp::STRING); \
    } \
    \
    if (in_array_state) \
    { \
        return token::ARRAY_STRING; \
    } \
    else if (in_assign_state) \
    { \
        yy_pop_state();  \
        return token::VALUE_STRING; \
    } \
    }
%}

%% /*** Regular Expressions Part ***/

 /* code to place at the beginning of yylex() */
%{
    // reset location
    yylloc->step();
%}
 /*** BEGIN EXAMPLE - Change the HIT lexer rules below ***/

<INITIAL,object,param>{COMMENT} {
    capture_token(yylval,wasp::COMMENT);
    return token::COMMENT;
}

 /* syntax error - key = COMMENT. */
<assign>{COMMENT} {
    yy_pop_state(); // pop the assign state
    capture_token(yylval,wasp::COMMENT);
    // The parser needs to know about this invalid token
    return token::COMMENT;
}

!include {
    yy_push_state(file_include);
    capture_token(yylval,wasp::FILE);
    return token::FILE;
}
<file_include>{INCLUDE_PATH} {
    // file includes grab everyting starting after '!include' to
    // either a newline '\n' or a comment '#'
    yy_pop_state();
    capture_token(yylval,wasp::STRING);
    return token::STRING;
}

<INITIAL,object>{LBRACKET} {
    yy_push_state(lbracket);
    capture_token(yylval,wasp::LBRACKET);
    return token::LBRACKET;
}

 /* The assign condition is a syntax error which for the purpose
  of error recovery is captured here 
  E.g., the error scenario is follows
  key = [
 */
<assign>{LBRACKET} {
    yy_pop_state(); // pop assign
    rewind(); // put back so error recovery is possible
    // The parser needs to know about this invalid token    
    return token::LBRACKET;
}
<lbracket>{DOT_SLASH} {
    capture_token(yylval,wasp::DOT_SLASH);
    return token::DOT_SLASH;
}
<lbracket>{OBJCT_STRING} {
    yy_pop_state();
    yy_push_state(rbracket);
    capture_token(yylval,wasp::STRING);
    return token::OBJCT_STRING;
}
 /* Anything But Closing Bracket is a unknown/error*/ 
<rbracket>{ABCB} {
    capture_token(yylval,wasp::UNKNOWN);
    return token::UNKNOWN;
}
<rbracket>{RBRACKET} {
    yy_pop_state();
    yy_push_state(object);
    capture_token(yylval,wasp::RBRACKET);
    return token::RBRACKET;
}
 /* syntax error - [ block EOL. */
<lbracket,rbracket>\n {
    yy_pop_state(); // leave state in attempt to error recover
    // assume body of object does follow 
    yy_push_state(object);
    yylloc->lines(yyleng); yylloc->step();
    interpreter.push_line_offset(file_offset-yyleng);
    return token::EOL;
} 
<object>{OBJECT_TERM} {
    yy_pop_state();
    capture_token(yylval,wasp::OBJECT_TERM);
    return token::OBJECT_TERM;
}

<INITIAL,object>{PARAM_STRING} {
    yy_push_state(param);
    capture_token(yylval,wasp::STRING);
    return token::PARAM_STRING;
}
<param>{ASSIGN} {
    yy_pop_state();
    yy_push_state(assign);
    capture_token(yylval,wasp::ASSIGN);
    return token::ASSIGN;
}
<param>{OVERRIDE_ASSIGN} {
    yy_pop_state();
    yy_push_state(assign);
    capture_token(yylval,wasp::OVERRIDE_ASSIGN);
    return token::OVERRIDE_ASSIGN;
}
<array,assign,brace_expression_state>{BRACE_EXPRESSION_START} {
    yy_push_state(brace_expression_state);

    // append next token to this yytext
    do_yymore();
}
<brace_expression_state>{BRACE_EXPRESSION_INNER} {
    // append next token to this yytext
    do_yymore();
}

<brace_expression_state>{BRACE_EXPRESSION_END} {
    do_brace_end();

    // because we have not concluded our brace expression, 
    // append the next token to this yytext
    do_yymore();
}

<brace_expression_state>{BRACE_EXPRESSION_END}/[^ \t\n'\}"] {

    yy_pop_state(); // brace expression concluded, pop it
    // If we are no longer in brace_expression, push trailing state to ensure trailing information is included in the brace token
    if (YY_START != brace_expression_state)
        yy_push_state(trailing_brace_expression_state); 

    // because of the trailing non-whitespace we have
    // are not concluding
    // append the next token to this yytext
    do_yymore();
}
<brace_expression_state>{BRACE_EXPRESSION_END}/\$\{ {
    yy_pop_state(); // brace expression concluded, pop it
    // the brace_expression_state will be pushed 
    do_yymore();
}
<trailing_brace_expression_state>{TRAILING_BRACE_EXPRESSION} {
    do_brace_end();
}

<trailing_brace_expression_state>{TRAILING_BRACE_EXPRESSION}/\$\{ {
    yy_pop_state(); // leave trailing brace expression state
    // the brace_expression_state will be pushed 
    do_yymore(); 
}

<assign>{INTEGER} {
    yy_pop_state();
    capture_token(yylval,wasp::INTEGER);
    return token::INTEGER;
}
<assign>{REAL} {
    yy_pop_state();
    capture_token(yylval,wasp::REAL);
    return token::REAL;
}
 /* syntax error - key = EOL. */
<assign,param>\n {
    yy_pop_state(); // leave state in attempt to error recover
    yylloc->lines(yyleng); yylloc->step();
    interpreter.push_line_offset(file_offset-yyleng);
    return token::EOL;
} 
 /* Allow scenarios where quoted data after newline*/
<assign>\n/[ \t]*[\'\"] {
    yylloc->lines(yyleng); yylloc->step();
    interpreter.push_line_offset(file_offset-yyleng);
    yyless(1); // put back quote
    
    yylloc->step();
}
<assign>{VALUE_STRING} {

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // VALUE_STRING
    //  if the next to last character is '$' because the "\$[^\{]" part of the
    //  regular expression matched and the last character is one from this set
    //  [ ' ' , '\n' , '\t' , '\r' , '[' , ']' , '#' , '$' ]
    //  then put that character back because it is not allowed in this context
    //  and rewind column and file_offset increases from yy_user_action by one
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (yyleng >= 2 && yytext[yyleng-2] == '$' &&
         (yytext[yyleng-1] == ' '  || yytext[yyleng-1] == '\n' ||
          yytext[yyleng-1] == '\t' || yytext[yyleng-1] == '\r' ||
          yytext[yyleng-1] == '['  || yytext[yyleng-1] == ']'  ||
          yytext[yyleng-1] == '#'  || yytext[yyleng-1] == '$'))
    {
      yyless(yyleng-1);
      yylloc->columns(-1);
      file_offset-=1;
    }

    yy_pop_state();
    capture_token(yylval,wasp::STRING);
    return token::VALUE_STRING;
}
<assign>{VALUE_STRING}/\$\{ {
    // this VALUE_STRING is a part of the following brace
    // expression and should be captured accordingly
    do_yymore();
}
<assign>{DOUBLE_QUOTED_STRING} {
    yy_pop_state();
    capture_token(yylval,wasp::QUOTED_STRING);
    return token::QSTRING;
}
<INITIAL,object>{DOUBLE_QUOTED_STRING} {
    capture_token(yylval,wasp::QUOTED_STRING);
    return token::QSTRING;
}

<assign>{SINGLE_QUOTE} {
    yy_pop_state();
    yy_push_state(array);
    capture_token(yylval,wasp::QUOTE);
    return token::QUOTE;
}

<array>{SINGLE_QUOTE} {
    yy_pop_state();
    capture_token(yylval,wasp::QUOTE);
    return token::QUOTE;
}
<array>{INTEGER} {
    capture_token(yylval,wasp::INTEGER);
    return token::INTEGER;
}
<array>{REAL} {
    capture_token(yylval,wasp::REAL);
    return token::REAL;
}
<array>{ARRAY_STRING} {

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // ARRAY_STRING
    //  if the next to last character is '$' because the "\$[^\{]" part of the
    //  regular expression matched and the last character is one from this set
    //  [ ' ' , '"' , '\n' , '\t' , '\r' , ';' , '\\' , '\'' , '$' ]
    //  then put that character back because it is not allowed in this context
    //  and rewind column and file_offset increases from yy_user_action by one
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (yyleng >= 2 && yytext[yyleng-2] == '$' &&
         (yytext[yyleng-1] == ' '  || yytext[yyleng-1] == '"'  ||
          yytext[yyleng-1] == '\n' || yytext[yyleng-1] == '\t' ||
          yytext[yyleng-1] == '\r' || yytext[yyleng-1] == ';'  ||
          yytext[yyleng-1] == '\\' || yytext[yyleng-1] == '\'' ||
          yytext[yyleng-1] == '$'))
    {
      yyless(yyleng-1);
      yylloc->columns(-1);
      file_offset-=1;
    }

    capture_token(yylval,wasp::STRING);
    return token::ARRAY_STRING;
}
<array>{ARRAY_STRING}/\$\{ {
    // this ARRAY_STRING is a part of the following brace
    // expression and should be captured accordingly
    do_yymore();
}
<array>{DOUBLE_QUOTED_STRING} {
    capture_token(yylval,wasp::QUOTED_STRING);
    return token::QSTRING;
}
<array>{SEMICOLON} {
    capture_token(yylval,wasp::SEMICOLON);
    return token::SEMICOLON;
}
<INITIAL,object>{SINGLE_QUOTE} {
    yy_push_state(array);
    capture_token(yylval,wasp::QUOTE);
    return token::QUOTE;
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

 /* pass all other characters up to HIT*/
<*>. {
    return static_cast<token_type>(*yytext);
}
<*><<EOF>> {
    eof_reached = true;
    yyterminate();
}

 /*** END EXAMPLE - Change the HIT lexer rules above ***/

%% /*** Additional Code ***/

namespace wasp {

HITLexerImpl::HITLexerImpl(
                AbstractInterpreter & interpreter,
                std::istream* in,
                std::ostream* out)
    : HITFlexLexer(in, out)
    , interpreter(interpreter)
    , file_offset(0)
    , eof_reached(false)
{
}

HITLexerImpl::~HITLexerImpl()
{
}

void HITLexerImpl::set_debug(bool b)
{
    yy_flex_debug = b;
}
void HITLexerImpl::rewind()
{
    yyin.seekg(-yyleng,std::ios_base::cur);
    yyless(0);
}
void HITLexerImpl::capture_token(
        wasp::HITParser::semantic_type* yylval
        ,wasp::NODE type)
{
    std::size_t offset = file_offset - yyleng;
    yylval->token_index = interpreter.token_count();
    interpreter.push_token(yytext,type,offset);
}
} // end of namespace

/* This implementation of HITFlexLexer::yylex() is required to fill the
 * vtable of the class HITFlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the HITLexerImpl class instead. */

#ifdef yylex
#undef yylex
#endif

int HITFlexLexer::yylex()
{    
    return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int HITFlexLexer::yywrap()
{
    return 1;
}
