%{ /*** C/C++ Declarations ***/

#include <stdio.h>
#include <vector>
#include <cstring>
%}

/*** yacc/bison Declarations ***/

%code requires{
#include <memory>
#include "waspcore/TreeNodePool.h"
#include "waspcore/wasp_node.h"
#include "waspcore/decl.h"
#include "waspcore/utils.h"
}

/* Require bison 3 or later */

%output "SnippetParser.cpp"
/* add debug output code to generated parser. disable this for release
 * versions. */
/*%debug*/

/* start symbol is named "start" */
%start start

/* write out a header file containing the token defines */
%defines
%require "3.7"
/* use newer C++ skeleton file */
%skeleton "lalr1.cc"

/* namespace to enclose parser in */
/* %name-prefix "wasp" */
%define api.namespace {wasp}
%define api.location.file "../waspcore/location.hh"
/* set the parser's class identifier */
%define api.parser.class {SnippetParser}

/* keep track of the current position within the input */
%locations
%initial-action
{
    // initialize the initial location object
    @$.begin.filename = @$.end.filename = &interpreter.stream_name();
    @$.begin.line = @$.end.line = interpreter.start_line();
    @$.begin.column = @$.end.column = interpreter.start_column();
    lexer = std::make_shared<SnippetLexerImpl>(interpreter,&input_stream);
    // lexer->set_debug(true); // Requires Snippet.lex %option debug uncommented
    // this->set_debug_level(1); // Requires Snippet.bison %debug option uncommented
};

/* The interpreter is passed by reference to the parser and to the SnippetLexer. This
 * provides a simple but effective pure interface, not relying on global
 * variables. */
%parse-param { class AbstractInterpreter& interpreter }
             {std::istream &input_stream}
             {std::shared_ptr<class SnippetLexerImpl> lexer}
/* verbose error messages */
%define parse.error verbose

 /*** BEGIN EXAMPLE - Change the Snippet grammar's tokens below ***/

//%define api.value.type {struct YYSTYPE}
%union{
    std::size_t token_index;
    std::size_t node_index;
    std::vector<size_t>* node_indices;
    // Objects have children and the child's type field's value is promoted
    // to the parent object's name. The pairing indicates the index to
    // the 'type' field in the vector. If no type field exist
    // the pair.first (index) = pair.second (vector>.size()
    std::pair<size_t, std::vector<size_t>*>* object_children;
}


%token                  END          0  "end of file"
%token                  EOL             "end of line"
%token                 UNKNOWN          "invalid token"
%token <token_index>  '$'
%token <token_index>  '{'
%token                 '}'
%token <token_index>   INTEGER         "integer"
%token <token_index>   COLON         ":"
%token <token_index>   TEXT          "text"
%type <node_index>  text snippet snippet_decl integer


%{

#include "SnippetInterpreter.h"
#include "SnippetLexer.h"
#include "waspcore/wasp_bug.h"

/* this "connects" the Snippet parser in the interpreter to the flex SnippetLexer class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the interpreter context. */
#undef yylex
#define yylex lexer->lex
%}

%% /*** Grammar Rules ***/

 /*** BEGIN - Change the Snippet grammar rules below ***/

text : TEXT
    {
        size_t token_index = $1;
        $$ = interpreter.push_leaf(wasp::SnippetType::TEXT,"txt", token_index);
    }
integer : INTEGER
    {
        size_t token_index = $1;
        $$ = interpreter.push_leaf(wasp::INTEGER, "pos", token_index);
    }
snippet_decl : '$'
    {
        size_t token_index = $1;
        $$ = interpreter.push_leaf(wasp::DECL, "$", token_index);
    }
snippet : snippet_decl integer
    {  // tab stop or simple placeholder
        std::vector<size_t> child_indices = {$1, $2};
        $$ = interpreter.push_parent(wasp::SnippetType::TABSTOP
                                    ,"tst" // tabstop
                                    ,child_indices);
    }
    | snippet_decl '{' integer '}'
    { // enclosed tab stop or simple placeholder
        std::vector<size_t> child_indices = {$1, $3};
        $$ = interpreter.push_parent(wasp::SnippetType::TABSTOP
                                    ,"tst" // tabstop
                                    ,child_indices);
    }
    | snippet_decl '{' integer ':' text '}'
    { // placeholder with default-selected text
        std::vector<size_t> child_indices = {$1, $3, $5};
        $$ = interpreter.push_parent(wasp::SnippetType::PLACEHOLDER
                                    ,"pho" // placeholder - tabstop and default text
                                    ,child_indices);
    }
    | snippet_decl '{' integer ':' '}'
    { // placeholder with no text... i.e., tabstop
        std::vector<size_t> child_indices = {$1, $3};
        $$ = interpreter.push_parent(wasp::SnippetType::TABSTOP
                                    ,"tst" // tabstop
                                    ,child_indices);
    }

start   : /** empty **/
        | start text{
            interpreter.push_staged_child($2);
        }
        | start snippet{
            interpreter.push_staged_child($2);
        }


 /*** END RULES - Change the Snippet grammar rules above ***/

%% /*** Additional Code ***/
namespace wasp{
void SnippetParser::error(const SnippetParser::location_type& l,
                           const std::string& m)
{
    interpreter.error_diagnostic()<<l<<": "<<m<<std::endl;
}
} // end of namespace
