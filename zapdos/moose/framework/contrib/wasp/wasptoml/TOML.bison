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

/* Require biTOML 3 or later */

%output "TOMLParser.cpp"
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
%define api.parser.class {TOMLParser}

/* keep track of the current position within the input */
%locations
%initial-action
{
    // initialize the initial location object
    @$.begin.filename = @$.end.filename = &interpreter.stream_name();
    @$.begin.line = @$.end.line = interpreter.start_line();
    @$.begin.column = @$.end.column = interpreter.start_column();
    lexer = std::make_shared<TOMLLexerImpl>(interpreter,&input_stream);
    // lexer->set_debug(true); // Requires TOML.lex %option debug uncommented
    // this->set_debug_level(1); // Requires TOML.bison %debug option uncommented
};

/* The interpreter is passed by reference to the parser and to the TOMLLexer. This
 * provides a simple but effective pure interface, not relying on global
 * variables. */
%parse-param { class AbstractInterpreter& interpreter }
             {std::istream &input_stream}
             {std::shared_ptr<class TOMLLexerImpl> lexer}
/* verbose error messages */
%define parse.error verbose

 /*** BEGIN EXAMPLE - Change the TOML grammar's tokens below ***/

//%define api.value.type {struct YYSTYPE}
%union{
    std::size_t token_index;
    std::size_t node_index;
    std::vector<size_t>* node_indices;
}


%token                  END          0  "end of file"
%token                  EOL             "end of line"
%token                 UNKNOWN          "invalid token"
%token <token_index>   LBRACKET         "["
%token <token_index>   RBRACKET         "]"
%token <token_index>   DLBRACKET         "[["
%token <token_index>   DRBRACKET         "]]"
%token <token_index>   LBRACE           "{"
%token <token_index>   RBRACE           "}"
%token <token_index>   COMMA           "comma"
%token <token_index>   SEPARATOR           "dot"
%token <token_index>    TOKEN_TRUE            "true"
%token <token_index>    TOKEN_FALSE            "false"
%token <token_index>   ASSIGN           "="
%token <token_index>   REAL          "real"
%token <token_index>   INTEGER          "integer"
%token <token_index>   STRING          "string"
%token <token_index>   ID    "identifier"
%token <token_index>   COMMENT           "comment"
 // type token_index for token grouping forwarding - PRIMITIVE => REAL, BOOLEAN, STRING
%type <token_index> PRIMITIVE
%type <node_index>  lbracket rbracket dlbracket drbracket assign
%type <node_index>   keyed_primitive keyed_object keyed_array
%type <node_index>  comment  comma lbrace rbrace decl separator
%type <node_index>   primitive table array_table
%type <node_indices> array_members array declaration key
%type <node_indices> object object_members


%destructor { delete $$; } array_members array object object_members declaration key

%{

#include "TOMLInterpreter.h"
#include "TOMLLexer.h"
#include "waspcore/wasp_bug.h"

/**
 * Collect names of the given node set. E.g., x, y, z for follow scenarios
 *  x.y.z = {...}
 * x.y.z = [...]
 * [x.y.z]
 * [[x.y.z]]
 * If names are quotes the quotes will be stripped
 **/
void collect_names(std::vector<std::string>& names, 
    wasp::AbstractInterpreter & interpreter,
    const std::vector<size_t> & node_indices)
{
    for (auto node_index : node_indices)
    {
        auto type = interpreter.type(node_index);
        if (type == wasp::ASSIGN) break; // no declarators past assign
        if (type != wasp::DECL) continue; // skip separators, opening '[', '[['        
        std::string name = wasp::strip_quotes(interpreter.data(node_index));
        names.push_back(name);
    }
}
/**
* Push a declared type node and any parents as dictated by name hierarchy
* @param interpreter - the parse tree own to which the new nodes will be pushed
* @param node_indices - the nodes consituting the TOML component
* @param terminal_type - the type of the terminal node pushed
* @return size_t - the index of the last node created (topmost)
**/
size_t push_declared_type(wasp::AbstractInterpreter & interpreter,
                                 std::vector<size_t>       & node_indices,
                                 wasp::NODE terminal_type)
{
    
    std::vector<std::string> names;
    // collect names (decl nodes)
    collect_names(names, interpreter, node_indices);
    wasp_ensure(!names.empty());

    wasp::NODE type = terminal_type;
    // traverse hierarchy creating parent nodes
    for (size_t parent_index = 0; !names.empty(); names.pop_back())
    {
        parent_index = interpreter.push_parent(type, names.back().data(),
                                                node_indices);
        type = wasp::IDENTIFIER;
        node_indices = {parent_index};
    }
    return node_indices.back();
}

/**
* Push a declared staged type node and any parents as dictated by name hierarchy
* @param interpreter - the parse tree own to which the new nodes will be pushed
* @param node_indices - the nodes consituting the TOML component
* @param type - the type of node to push
* @return size_t - the index of the last node staged (lowest most)

**/
size_t push_staged_declared_type(wasp::AbstractInterpreter & interpreter,
                                 std::vector<size_t>       & node_indices,
                                 wasp::NODE type)
{
    
    std::vector<std::string> names;
    // collect names (decl nodes)
    collect_names(names, interpreter, node_indices);
    wasp_check(!names.empty());

    // traverse hierarchy creating parent nodes
    size_t staged_node_index = 0;
    for (size_t i = 0, count = names.size(); i < count; ++i)
    {
        // Because TOML constructs are based on what has previously been defined 
        // any hierarchy type cannot be determined (array vs object) so
        // we use wasp::IDENTIFIER as the node type

        bool placeholder_parent = i != count-1;
        staged_node_index = interpreter.push_staged(placeholder_parent? wasp::IDENTIFIER : type, names[i].data(),
                                            !placeholder_parent ? node_indices : std::vector<size_t>());
    }
    return staged_node_index;
}

/* this "connects" the TOML parser in the interpreter to the flex TOMLLexer class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the interpreter context. */
#undef yylex
#define yylex lexer->lex

%}

%% /*** Grammar Rules ***/

 /*** BEGIN - Change the TOML grammar rules below ***/

assign : ASSIGN
    {
        size_t assign_token_index = ($1);
        $$ = interpreter.push_leaf(wasp::ASSIGN,"="
                         ,assign_token_index);
    }

lbracket : LBRACKET
    {
        size_t token_index = ($1);
        $$ = interpreter.push_leaf(wasp::LBRACKET,"["
                         ,token_index);
    }
rbracket : RBRACKET
    {
        size_t token_index = ($1);
        $$ = interpreter.push_leaf(wasp::RBRACKET,"]"
                         ,token_index);
    }
lbrace : LBRACE
    {
        size_t token_index = ($1);
        $$ = interpreter.push_leaf(wasp::LBRACE,"{"
                         ,token_index);
    }
rbrace : RBRACE
    {
        size_t token_index = ($1);
        $$ = interpreter.push_leaf(wasp::RBRACE,"}"
                         ,token_index);
    }
dlbracket : DLBRACKET
    {
        size_t token_index = ($1);
        $$ = interpreter.push_leaf(wasp::DLBRACKET,"[["
                         ,token_index);
    }
drbracket : DRBRACKET
    {
        size_t token_index = ($1);
        $$ = interpreter.push_leaf(wasp::DRBRACKET,"]]"
                         ,token_index);
    }

comma : COMMA
    {
        size_t token_index = ($1);
        $$ = interpreter.push_leaf(wasp::WASP_COMMA,","
                         ,token_index);
    }
separator : SEPARATOR
    {
        size_t token_index = ($1);
        $$ = interpreter.push_leaf(wasp::SEPARATOR,"."
                         ,token_index);
    }

comment : COMMENT
    {
        size_t token_index = ($1);
        $$ = interpreter.push_leaf(wasp::COMMENT,"comment"
                         ,token_index);
    }

array :
    lbracket rbracket
    {
        $$ = new std::vector<size_t>();
        $$->push_back($1);
        $$->push_back($2);
    }
    | lbracket END
    {
        error(@2, "array has unmatched left bracket!");
        YYERROR;
        $$ = nullptr;
    }
    | lbracket array_members END
    {
        std::string name = "object";
        auto last_component_type = interpreter.type($2->back());
        if( $2->size() ==0 ) error(@1, name+" has unmatched left bracket!");
        else if( last_component_type == wasp::OBJECT ) error(@1, name+" or one of its components has unmatched left bracket!");
        else error(@1, name+" has unmatched left bracket!");
        delete $2;
        YYERROR;
        $$ = nullptr;
    }
    | lbracket array_members rbracket
    {
            $array_members->insert($array_members->begin(),$1);
            $array_members->push_back($3);
            $$ = $array_members;
    }
object : lbrace rbrace
    {
        $$ = new std::vector<size_t>();
        $$->push_back($1);
        $$->push_back($2);
    }
    |  lbrace END
    {
        error(@2, "object has unmatched left brace!");
        YYERROR;
        $$ = nullptr;
    }
    |  lbrace object_members END
    {
        // TODO capture partial definition
        std::string name = "object";
        auto last_component_type = interpreter.type($2->back());
        if( $2->size() ==0 ) error(@1, name+" has unmatched left brace!");
        else if( last_component_type == wasp::OBJECT ) error(@1, name+" or one of its components has unmatched left brace!");
        else error(@1, name+" has unmatched left brace!");
        delete $2;
        YYERROR;
        $$ = nullptr;
    }
    | lbrace object_members rbrace
    {
        $object_members->insert($object_members->begin(),$1);
        $object_members->push_back($3);
        $$ = $object_members;
    }
decl : ID
    {
        auto token_index = ($1);
        std::string quote_less_data = interpreter.token_data(token_index);
        quote_less_data = wasp::strip_quotes(quote_less_data);
        $$ = interpreter.push_leaf(wasp::DECL
                                   ,"decl"
                                   ,token_index);
    }
key : decl
        {
            $$ = new std::vector<size_t>();
            $$->push_back($1);
        }
        | decl separator key
        {
            $$ = $3;            
            $$->insert($$->begin(), $separator);
            $$->insert($$->begin(), $decl);
        }
declaration : key assign
        {
            $$ = $1; 
            $$->push_back($2);
        }


PRIMITIVE : STRING | INTEGER | REAL | TOKEN_FALSE | TOKEN_TRUE

primitive : PRIMITIVE
{
    size_t token_index = ($1);
    $$ = interpreter.push_leaf(wasp::VALUE,"value"
                     ,token_index);
}
keyed_primitive : declaration primitive
    {
        $1->push_back($2);
        $$ = push_declared_type(interpreter, *$1, wasp::KEYED_VALUE);
        delete $1;
    }
keyed_object : declaration object
    {
        for( size_t i = 0; i < $2->size(); ++i )
        {
            $1->push_back($2->at(i));
        }
        $$ = push_declared_type(interpreter, *$1, wasp::KEYED_VALUE);
        delete $1;
        delete $2;
    }
keyed_array : declaration array
    {
        for( size_t i = 0; i < $2->size(); ++i )
        {
            $1->push_back($2->at(i));
        }
        $$ = push_declared_type(interpreter, *$1, wasp::KEYED_VALUE);
        delete $1;
        delete $2;
    }
array_members :object
        {
            $$ = new std::vector<size_t>();
            size_t obj_i = interpreter.push_parent(wasp::OBJECT
                                        ,"value"
                                        ,*$1);
            $$->push_back(obj_i);
            delete $object;
        }
        | array_members comma object
        {
            $$ = $1;
            $$->push_back($2);
            size_t obj_i = interpreter.push_parent(wasp::OBJECT
                                        ,"value"
                                        ,*$3);
            $$->push_back(obj_i);
            delete $object;
        }
        | array
        {
            $$ = new std::vector<size_t>();
            size_t arr_i = interpreter.push_parent(wasp::ARRAY
                                        ,"value"
                                        ,*$1);
            $$->push_back(arr_i);
            delete $1;
        }
        | array_members comma array
        {
            $$ = $1;
            $$->push_back($2);
            size_t arr_i = interpreter.push_parent(wasp::ARRAY
                                        ,"value"
                                        ,*$3);
            $$->push_back(arr_i);
            delete $3;
        }
        | primitive
        {
            $$ = new std::vector<size_t>();
            $$->push_back($1);
        }
        | array_members comma primitive
        {
            $$ = $1;
            $$->push_back($2);
            $$->push_back($3);
        }
object_members : keyed_object
        {
            $$ = new std::vector<size_t>();
            $$->push_back($1);
        }
        | object_members comma keyed_object
        {
            $$ = $1;
            $$->push_back($2);
            $$->push_back($3);
        }
        | keyed_array
        {
            $$ = new std::vector<size_t>();
            $$->push_back($1);
        }
        | object_members comma keyed_array
        {
            $$ = $1;
            $$->push_back($2);
            $$->push_back($3);
        }
        | keyed_primitive
        {
            $$ = new std::vector<size_t>();
            $$->push_back($1);
        }
        | object_members comma keyed_primitive
        {
            $$ = $1;
            $$->push_back($2);
            $$->push_back($3);
        }
 // Table are a terminator-less construct, which pushes a stage/scope after which all
 // TOML constructs except other tables or array of table are children
table : lbracket key rbracket
    {
        // Check for prior staged table that may need to be committed.
        while (interpreter.staged_count() > 1)
        {
            interpreter.commit_staged(interpreter.staged_count()-1);
        }

        $2->insert($2->begin(), $lbracket);
        $2->push_back($rbracket);

        $$ = push_staged_declared_type(interpreter, *$2, wasp::TABLE);
        delete $2;
    }
 // Array Table are a terminator-less construct, which pushes a stage/scope after which all
 // TOML constructs except other tables or array of table are children
array_table : dlbracket key drbracket
    {
        // Check for prior staged table that may need to be committed.
        while (interpreter.staged_count() > 1)
        {
            interpreter.commit_staged(interpreter.staged_count()-1);
        }

        $2->insert($2->begin(), $dlbracket);
        $2->push_back($drbracket);

        $$ = push_staged_declared_type(interpreter, *$2, wasp::ARRAY_TABLE);
        delete $2;
    }
start   : /** empty **/
        | start comment{
            interpreter.push_staged_child($2);
        }
        | start keyed_primitive{
            interpreter.push_staged_child($2);
        }
        | start keyed_primitive error{
            interpreter.push_staged_child($2);
            interpreter.set_failed(true);
        }
        | start keyed_object{
            interpreter.push_staged_child($2);
        }
        | start keyed_object error{
            interpreter.push_staged_child($2);
            interpreter.set_failed(true);
        }
        | start keyed_array{
            interpreter.push_staged_child($2);
        }
        | start keyed_array error{
            interpreter.push_staged_child($2);
            interpreter.set_failed(true);
        }
        | start table{
            // node staging occurs for this rule
        }
        | start table error{
            // node staging occurs for this rule
            interpreter.set_failed(true);
        }
        | start array_table{
            // node staging occurs for this rule
        }
        | start array_table error{
            // node staging occurs for this rule
            interpreter.set_failed(true);
        }

 /*** END RULES - Change the TOML grammar rules above ***/

%% /*** Additional Code ***/
namespace wasp{
void TOMLParser::error(const TOMLParser::location_type& l,
                           const std::string& m)
{
    interpreter.error_diagnostic()<<l<<": "<<m<<std::endl;
}
} // end of namespace
