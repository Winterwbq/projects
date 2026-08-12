// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.



// First part of user prologue.
#line 1 "TOML.bison"
 /*** C/C++ Declarations ***/

#include <stdio.h>
#include <vector>
#include <cstring>

#line 48 "TOMLParser.cpp"


#include "TOMLParser.hpp"

// Second part of user prologue.
#line 104 "TOML.bison"


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


#line 151 "TOMLParser.cpp"



#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 36 "TOML.bison"
namespace wasp {
#line 245 "TOMLParser.cpp"

  /// Build a parser object.
  TOMLParser::TOMLParser (class AbstractInterpreter& interpreter_yyarg, std::istream &input_stream_yyarg, std::shared_ptr<class TOMLLexerImpl> lexer_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      interpreter (interpreter_yyarg),
      input_stream (input_stream_yyarg),
      lexer (lexer_yyarg)
  {}

  TOMLParser::~TOMLParser ()
  {}

  TOMLParser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/

  // basic_symbol.
  template <typename Base>
  TOMLParser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value (that.value)
    , location (that.location)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  TOMLParser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_MOVE_REF (location_type) l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  TOMLParser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_RVREF (value_type) v, YY_RVREF (location_type) l)
    : Base (t)
    , value (YY_MOVE (v))
    , location (YY_MOVE (l))
  {}


  template <typename Base>
  TOMLParser::symbol_kind_type
  TOMLParser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  template <typename Base>
  bool
  TOMLParser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  TOMLParser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    value = YY_MOVE (s.value);
    location = YY_MOVE (s.location);
  }

  // by_kind.
  TOMLParser::by_kind::by_kind () YY_NOEXCEPT
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  TOMLParser::by_kind::by_kind (by_kind&& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  TOMLParser::by_kind::by_kind (const by_kind& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {}

  TOMLParser::by_kind::by_kind (token_kind_type t) YY_NOEXCEPT
    : kind_ (yytranslate_ (t))
  {}



  void
  TOMLParser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  void
  TOMLParser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  TOMLParser::symbol_kind_type
  TOMLParser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }


  TOMLParser::symbol_kind_type
  TOMLParser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }



  // by_state.
  TOMLParser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  TOMLParser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  TOMLParser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  TOMLParser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  TOMLParser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  TOMLParser::symbol_kind_type
  TOMLParser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  TOMLParser::stack_symbol_type::stack_symbol_type ()
  {}

  TOMLParser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.value), YY_MOVE (that.location))
  {
#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  TOMLParser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.value), YY_MOVE (that.location))
  {
    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  TOMLParser::stack_symbol_type&
  TOMLParser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    return *this;
  }

  TOMLParser::stack_symbol_type&
  TOMLParser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  TOMLParser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    switch (yysym.kind ())
    {
      case symbol_kind::S_array: // array
#line 102 "TOML.bison"
                    { delete (yysym.value.node_indices); }
#line 459 "TOMLParser.cpp"
        break;

      case symbol_kind::S_object: // object
#line 102 "TOML.bison"
                    { delete (yysym.value.node_indices); }
#line 465 "TOMLParser.cpp"
        break;

      case symbol_kind::S_key: // key
#line 102 "TOML.bison"
                    { delete (yysym.value.node_indices); }
#line 471 "TOMLParser.cpp"
        break;

      case symbol_kind::S_declaration: // declaration
#line 102 "TOML.bison"
                    { delete (yysym.value.node_indices); }
#line 477 "TOMLParser.cpp"
        break;

      case symbol_kind::S_array_members: // array_members
#line 102 "TOML.bison"
                    { delete (yysym.value.node_indices); }
#line 483 "TOMLParser.cpp"
        break;

      case symbol_kind::S_object_members: // object_members
#line 102 "TOML.bison"
                    { delete (yysym.value.node_indices); }
#line 489 "TOMLParser.cpp"
        break;

      default:
        break;
    }
  }

#if YYDEBUG
  template <typename Base>
  void
  TOMLParser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  TOMLParser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  TOMLParser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  TOMLParser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  TOMLParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  TOMLParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  TOMLParser::debug_level_type
  TOMLParser::debug_level () const
  {
    return yydebug_;
  }

  void
  TOMLParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  TOMLParser::state_type
  TOMLParser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  TOMLParser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  TOMLParser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  TOMLParser::operator() ()
  {
    return parse ();
  }

  int
  TOMLParser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    // User initialization code.
#line 44 "TOML.bison"
{
    // initialize the initial location object
    yyla.location.begin.filename = yyla.location.end.filename = &interpreter.stream_name();
    yyla.location.begin.line = yyla.location.end.line = interpreter.start_line();
    yyla.location.begin.column = yyla.location.end.column = interpreter.start_column();
    lexer = std::make_shared<TOMLLexerImpl>(interpreter,&input_stream);
    // lexer->set_debug(true); // Requires TOML.lex %option debug uncommented
    // this->set_debug_level(1); // Requires TOML.bison %debug option uncommented
}

#line 637 "TOMLParser.cpp"


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            yyla.kind_ = yytranslate_ (yylex (&yyla.value, &yyla.location));
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;

      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // assign: "="
#line 206 "TOML.bison"
    {
        size_t assign_token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::ASSIGN,"="
                         ,assign_token_index);
    }
#line 779 "TOMLParser.cpp"
    break;

  case 3: // lbracket: "["
#line 213 "TOML.bison"
    {
        size_t token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::LBRACKET,"["
                         ,token_index);
    }
#line 789 "TOMLParser.cpp"
    break;

  case 4: // rbracket: "]"
#line 219 "TOML.bison"
    {
        size_t token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::RBRACKET,"]"
                         ,token_index);
    }
#line 799 "TOMLParser.cpp"
    break;

  case 5: // lbrace: "{"
#line 225 "TOML.bison"
    {
        size_t token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::LBRACE,"{"
                         ,token_index);
    }
#line 809 "TOMLParser.cpp"
    break;

  case 6: // rbrace: "}"
#line 231 "TOML.bison"
    {
        size_t token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::RBRACE,"}"
                         ,token_index);
    }
#line 819 "TOMLParser.cpp"
    break;

  case 7: // dlbracket: "[["
#line 237 "TOML.bison"
    {
        size_t token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::DLBRACKET,"[["
                         ,token_index);
    }
#line 829 "TOMLParser.cpp"
    break;

  case 8: // drbracket: "]]"
#line 243 "TOML.bison"
    {
        size_t token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::DRBRACKET,"]]"
                         ,token_index);
    }
#line 839 "TOMLParser.cpp"
    break;

  case 9: // comma: "comma"
#line 250 "TOML.bison"
    {
        size_t token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::WASP_COMMA,","
                         ,token_index);
    }
#line 849 "TOMLParser.cpp"
    break;

  case 10: // separator: "dot"
#line 256 "TOML.bison"
    {
        size_t token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::SEPARATOR,"."
                         ,token_index);
    }
#line 859 "TOMLParser.cpp"
    break;

  case 11: // comment: "comment"
#line 263 "TOML.bison"
    {
        size_t token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::COMMENT,"comment"
                         ,token_index);
    }
#line 869 "TOMLParser.cpp"
    break;

  case 12: // array: lbracket rbracket
#line 271 "TOML.bison"
    {
        (yylhs.value.node_indices) = new std::vector<size_t>();
        (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
        (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
    }
#line 879 "TOMLParser.cpp"
    break;

  case 13: // array: lbracket "end of file"
#line 277 "TOML.bison"
    {
        error(yystack_[0].location, "array has unmatched left bracket!");
        YYERROR;
        (yylhs.value.node_indices) = nullptr;
    }
#line 889 "TOMLParser.cpp"
    break;

  case 14: // array: lbracket array_members "end of file"
#line 283 "TOML.bison"
    {
        std::string name = "object";
        auto last_component_type = interpreter.type((yystack_[1].value.node_indices)->back());
        if( (yystack_[1].value.node_indices)->size() ==0 ) error(yystack_[2].location, name+" has unmatched left bracket!");
        else if( last_component_type == wasp::OBJECT ) error(yystack_[2].location, name+" or one of its components has unmatched left bracket!");
        else error(yystack_[2].location, name+" has unmatched left bracket!");
        delete (yystack_[1].value.node_indices);
        YYERROR;
        (yylhs.value.node_indices) = nullptr;
    }
#line 904 "TOMLParser.cpp"
    break;

  case 15: // array: lbracket array_members rbracket
#line 294 "TOML.bison"
    {
            (yystack_[1].value.node_indices)->insert((yystack_[1].value.node_indices)->begin(),(yystack_[2].value.node_index));
            (yystack_[1].value.node_indices)->push_back((yystack_[0].value.node_index));
            (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    }
#line 914 "TOMLParser.cpp"
    break;

  case 16: // object: lbrace rbrace
#line 300 "TOML.bison"
    {
        (yylhs.value.node_indices) = new std::vector<size_t>();
        (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
        (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
    }
#line 924 "TOMLParser.cpp"
    break;

  case 17: // object: lbrace "end of file"
#line 306 "TOML.bison"
    {
        error(yystack_[0].location, "object has unmatched left brace!");
        YYERROR;
        (yylhs.value.node_indices) = nullptr;
    }
#line 934 "TOMLParser.cpp"
    break;

  case 18: // object: lbrace object_members "end of file"
#line 312 "TOML.bison"
    {
        // TODO capture partial definition
        std::string name = "object";
        auto last_component_type = interpreter.type((yystack_[1].value.node_indices)->back());
        if( (yystack_[1].value.node_indices)->size() ==0 ) error(yystack_[2].location, name+" has unmatched left brace!");
        else if( last_component_type == wasp::OBJECT ) error(yystack_[2].location, name+" or one of its components has unmatched left brace!");
        else error(yystack_[2].location, name+" has unmatched left brace!");
        delete (yystack_[1].value.node_indices);
        YYERROR;
        (yylhs.value.node_indices) = nullptr;
    }
#line 950 "TOMLParser.cpp"
    break;

  case 19: // object: lbrace object_members rbrace
#line 324 "TOML.bison"
    {
        (yystack_[1].value.node_indices)->insert((yystack_[1].value.node_indices)->begin(),(yystack_[2].value.node_index));
        (yystack_[1].value.node_indices)->push_back((yystack_[0].value.node_index));
        (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    }
#line 960 "TOMLParser.cpp"
    break;

  case 20: // decl: "identifier"
#line 330 "TOML.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        std::string quote_less_data = interpreter.token_data(token_index);
        quote_less_data = wasp::strip_quotes(quote_less_data);
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::DECL
                                   ,"decl"
                                   ,token_index);
    }
#line 973 "TOMLParser.cpp"
    break;

  case 21: // key: decl
#line 339 "TOML.bison"
        {
            (yylhs.value.node_indices) = new std::vector<size_t>();
            (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
        }
#line 982 "TOMLParser.cpp"
    break;

  case 22: // key: decl separator key
#line 344 "TOML.bison"
        {
            (yylhs.value.node_indices) = (yystack_[0].value.node_indices);            
            (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->begin(), (yystack_[1].value.node_index));
            (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->begin(), (yystack_[2].value.node_index));
        }
#line 992 "TOMLParser.cpp"
    break;

  case 23: // declaration: key assign
#line 350 "TOML.bison"
        {
            (yylhs.value.node_indices) = (yystack_[1].value.node_indices); 
            (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
        }
#line 1001 "TOMLParser.cpp"
    break;

  case 24: // PRIMITIVE: "string"
#line 356 "TOML.bison"
            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 1007 "TOMLParser.cpp"
    break;

  case 25: // PRIMITIVE: "integer"
#line 356 "TOML.bison"
                     { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 1013 "TOMLParser.cpp"
    break;

  case 26: // PRIMITIVE: "real"
#line 356 "TOML.bison"
                               { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 1019 "TOMLParser.cpp"
    break;

  case 27: // PRIMITIVE: "false"
#line 356 "TOML.bison"
                                      { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 1025 "TOMLParser.cpp"
    break;

  case 28: // PRIMITIVE: "true"
#line 356 "TOML.bison"
                                                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 1031 "TOMLParser.cpp"
    break;

  case 29: // primitive: PRIMITIVE
#line 359 "TOML.bison"
{
    size_t token_index = ((yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_leaf(wasp::VALUE,"value"
                     ,token_index);
}
#line 1041 "TOMLParser.cpp"
    break;

  case 30: // keyed_primitive: declaration primitive
#line 365 "TOML.bison"
    {
        (yystack_[1].value.node_indices)->push_back((yystack_[0].value.node_index));
        (yylhs.value.node_index) = push_declared_type(interpreter, *(yystack_[1].value.node_indices), wasp::KEYED_VALUE);
        delete (yystack_[1].value.node_indices);
    }
#line 1051 "TOMLParser.cpp"
    break;

  case 31: // keyed_object: declaration object
#line 371 "TOML.bison"
    {
        for( size_t i = 0; i < (yystack_[0].value.node_indices)->size(); ++i )
        {
            (yystack_[1].value.node_indices)->push_back((yystack_[0].value.node_indices)->at(i));
        }
        (yylhs.value.node_index) = push_declared_type(interpreter, *(yystack_[1].value.node_indices), wasp::KEYED_VALUE);
        delete (yystack_[1].value.node_indices);
        delete (yystack_[0].value.node_indices);
    }
#line 1065 "TOMLParser.cpp"
    break;

  case 32: // keyed_array: declaration array
#line 381 "TOML.bison"
    {
        for( size_t i = 0; i < (yystack_[0].value.node_indices)->size(); ++i )
        {
            (yystack_[1].value.node_indices)->push_back((yystack_[0].value.node_indices)->at(i));
        }
        (yylhs.value.node_index) = push_declared_type(interpreter, *(yystack_[1].value.node_indices), wasp::KEYED_VALUE);
        delete (yystack_[1].value.node_indices);
        delete (yystack_[0].value.node_indices);
    }
#line 1079 "TOMLParser.cpp"
    break;

  case 33: // array_members: object
#line 391 "TOML.bison"
        {
            (yylhs.value.node_indices) = new std::vector<size_t>();
            size_t obj_i = interpreter.push_parent(wasp::OBJECT
                                        ,"value"
                                        ,*(yystack_[0].value.node_indices));
            (yylhs.value.node_indices)->push_back(obj_i);
            delete (yystack_[0].value.node_indices);
        }
#line 1092 "TOMLParser.cpp"
    break;

  case 34: // array_members: array_members comma object
#line 400 "TOML.bison"
        {
            (yylhs.value.node_indices) = (yystack_[2].value.node_indices);
            (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
            size_t obj_i = interpreter.push_parent(wasp::OBJECT
                                        ,"value"
                                        ,*(yystack_[0].value.node_indices));
            (yylhs.value.node_indices)->push_back(obj_i);
            delete (yystack_[0].value.node_indices);
        }
#line 1106 "TOMLParser.cpp"
    break;

  case 35: // array_members: array
#line 410 "TOML.bison"
        {
            (yylhs.value.node_indices) = new std::vector<size_t>();
            size_t arr_i = interpreter.push_parent(wasp::ARRAY
                                        ,"value"
                                        ,*(yystack_[0].value.node_indices));
            (yylhs.value.node_indices)->push_back(arr_i);
            delete (yystack_[0].value.node_indices);
        }
#line 1119 "TOMLParser.cpp"
    break;

  case 36: // array_members: array_members comma array
#line 419 "TOML.bison"
        {
            (yylhs.value.node_indices) = (yystack_[2].value.node_indices);
            (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
            size_t arr_i = interpreter.push_parent(wasp::ARRAY
                                        ,"value"
                                        ,*(yystack_[0].value.node_indices));
            (yylhs.value.node_indices)->push_back(arr_i);
            delete (yystack_[0].value.node_indices);
        }
#line 1133 "TOMLParser.cpp"
    break;

  case 37: // array_members: primitive
#line 429 "TOML.bison"
        {
            (yylhs.value.node_indices) = new std::vector<size_t>();
            (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
        }
#line 1142 "TOMLParser.cpp"
    break;

  case 38: // array_members: array_members comma primitive
#line 434 "TOML.bison"
        {
            (yylhs.value.node_indices) = (yystack_[2].value.node_indices);
            (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
            (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
        }
#line 1152 "TOMLParser.cpp"
    break;

  case 39: // object_members: keyed_object
#line 440 "TOML.bison"
        {
            (yylhs.value.node_indices) = new std::vector<size_t>();
            (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
        }
#line 1161 "TOMLParser.cpp"
    break;

  case 40: // object_members: object_members comma keyed_object
#line 445 "TOML.bison"
        {
            (yylhs.value.node_indices) = (yystack_[2].value.node_indices);
            (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
            (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
        }
#line 1171 "TOMLParser.cpp"
    break;

  case 41: // object_members: keyed_array
#line 451 "TOML.bison"
        {
            (yylhs.value.node_indices) = new std::vector<size_t>();
            (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
        }
#line 1180 "TOMLParser.cpp"
    break;

  case 42: // object_members: object_members comma keyed_array
#line 456 "TOML.bison"
        {
            (yylhs.value.node_indices) = (yystack_[2].value.node_indices);
            (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
            (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
        }
#line 1190 "TOMLParser.cpp"
    break;

  case 43: // object_members: keyed_primitive
#line 462 "TOML.bison"
        {
            (yylhs.value.node_indices) = new std::vector<size_t>();
            (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
        }
#line 1199 "TOMLParser.cpp"
    break;

  case 44: // object_members: object_members comma keyed_primitive
#line 467 "TOML.bison"
        {
            (yylhs.value.node_indices) = (yystack_[2].value.node_indices);
            (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
            (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
        }
#line 1209 "TOMLParser.cpp"
    break;

  case 45: // table: lbracket key rbracket
#line 475 "TOML.bison"
    {
        // Check for prior staged table that may need to be committed.
        while (interpreter.staged_count() > 1)
        {
            interpreter.commit_staged(interpreter.staged_count()-1);
        }

        (yystack_[1].value.node_indices)->insert((yystack_[1].value.node_indices)->begin(), (yystack_[2].value.node_index));
        (yystack_[1].value.node_indices)->push_back((yystack_[0].value.node_index));

        (yylhs.value.node_index) = push_staged_declared_type(interpreter, *(yystack_[1].value.node_indices), wasp::TABLE);
        delete (yystack_[1].value.node_indices);
    }
#line 1227 "TOMLParser.cpp"
    break;

  case 46: // array_table: dlbracket key drbracket
#line 491 "TOML.bison"
    {
        // Check for prior staged table that may need to be committed.
        while (interpreter.staged_count() > 1)
        {
            interpreter.commit_staged(interpreter.staged_count()-1);
        }

        (yystack_[1].value.node_indices)->insert((yystack_[1].value.node_indices)->begin(), (yystack_[2].value.node_index));
        (yystack_[1].value.node_indices)->push_back((yystack_[0].value.node_index));

        (yylhs.value.node_index) = push_staged_declared_type(interpreter, *(yystack_[1].value.node_indices), wasp::ARRAY_TABLE);
        delete (yystack_[1].value.node_indices);
    }
#line 1245 "TOMLParser.cpp"
    break;

  case 48: // start: start comment
#line 505 "TOML.bison"
                       {
            interpreter.push_staged_child((yystack_[0].value.node_index));
        }
#line 1253 "TOMLParser.cpp"
    break;

  case 49: // start: start keyed_primitive
#line 508 "TOML.bison"
                               {
            interpreter.push_staged_child((yystack_[0].value.node_index));
        }
#line 1261 "TOMLParser.cpp"
    break;

  case 50: // start: start keyed_primitive error
#line 511 "TOML.bison"
                                     {
            interpreter.push_staged_child((yystack_[1].value.node_index));
            interpreter.set_failed(true);
        }
#line 1270 "TOMLParser.cpp"
    break;

  case 51: // start: start keyed_object
#line 515 "TOML.bison"
                            {
            interpreter.push_staged_child((yystack_[0].value.node_index));
        }
#line 1278 "TOMLParser.cpp"
    break;

  case 52: // start: start keyed_object error
#line 518 "TOML.bison"
                                  {
            interpreter.push_staged_child((yystack_[1].value.node_index));
            interpreter.set_failed(true);
        }
#line 1287 "TOMLParser.cpp"
    break;

  case 53: // start: start keyed_array
#line 522 "TOML.bison"
                           {
            interpreter.push_staged_child((yystack_[0].value.node_index));
        }
#line 1295 "TOMLParser.cpp"
    break;

  case 54: // start: start keyed_array error
#line 525 "TOML.bison"
                                 {
            interpreter.push_staged_child((yystack_[1].value.node_index));
            interpreter.set_failed(true);
        }
#line 1304 "TOMLParser.cpp"
    break;

  case 55: // start: start table
#line 529 "TOML.bison"
                     {
            // node staging occurs for this rule
        }
#line 1312 "TOMLParser.cpp"
    break;

  case 56: // start: start table error
#line 532 "TOML.bison"
                           {
            // node staging occurs for this rule
            interpreter.set_failed(true);
        }
#line 1321 "TOMLParser.cpp"
    break;

  case 57: // start: start array_table
#line 536 "TOML.bison"
                           {
            // node staging occurs for this rule
        }
#line 1329 "TOMLParser.cpp"
    break;

  case 58: // start: start array_table error
#line 539 "TOML.bison"
                                 {
            // node staging occurs for this rule
            interpreter.set_failed(true);
        }
#line 1338 "TOMLParser.cpp"
    break;


#line 1342 "TOMLParser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  TOMLParser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  TOMLParser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

  std::string
  TOMLParser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  // TOMLParser::context.
  TOMLParser::context::context (const TOMLParser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  TOMLParser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    const int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        const int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        const int yychecklim = yylast_ - yyn + 1;
        const int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }

    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }






  int
  TOMLParser::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
  TOMLParser::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char TOMLParser::yypact_ninf_ = -32;

  const signed char TOMLParser::yytable_ninf_ = -58;

  const signed char
  TOMLParser::yypact_[] =
  {
     -32,    61,   -32,   -32,   -32,   -32,   -32,    -9,    -9,   -32,
       5,     6,    79,     8,    11,    40,    43,    51,    16,    17,
     -32,    -9,   -32,   -32,   -32,   -32,   -32,   -32,   -32,   -32,
      69,     4,   -32,   -32,   -32,   -32,   -32,   -32,   -32,   -32,
     -32,   -32,   -32,   -32,   -32,   -32,   -32,   -32,   -32,   -32,
     -32,    94,   -32,   -32,   -32,   -32,   -32,   -32,    54,   -32,
     -32,   -32,    79,   -32,   -32,    -9,   -32,   -32,   -32,   -32,
     -32,   -32
  };

  const signed char
  TOMLParser::yydefact_[] =
  {
      47,     0,     1,     3,     7,    20,    11,     0,     0,    48,
      21,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      10,     0,     2,    23,     5,    28,    27,    26,    25,    24,
       0,     0,    32,    31,    29,    30,    50,    52,    54,    56,
      58,     4,    45,     8,    46,    22,    13,    12,    35,    33,
      37,     0,    17,     6,    16,    43,    39,    41,     0,    14,
       9,    15,     0,    18,    19,     0,    36,    34,    38,    44,
      40,    42
  };

  const signed char
  TOMLParser::yypgoto_[] =
  {
     -32,   -32,    25,   -27,   -32,   -26,   -32,   -32,   -16,   -32,
     -32,   -25,   -24,   -32,    12,   -32,   -32,   -23,   -31,   -30,
     -29,   -32,   -32,   -32,   -32,   -32
  };

  const signed char
  TOMLParser::yydefgoto_[] =
  {
       0,    23,    30,    42,    31,    54,     8,    44,    62,    21,
       9,    32,    33,    10,    11,    12,    34,    35,    13,    14,
      15,    51,    58,    16,    17,     1
  };

  const signed char
  TOMLParser::yytable_[] =
  {
      55,    56,    57,    47,    52,    48,    49,    50,   -49,    36,
       5,   -51,    37,   -49,    53,   -49,   -51,    20,   -51,    18,
      19,    22,    41,     5,    61,    43,     7,   -49,   -49,     0,
     -51,   -51,    64,    45,    69,    70,    71,    66,    67,    68,
     -53,    38,    65,   -55,    39,   -53,     0,   -53,   -55,     0,
     -55,   -57,    40,     0,    63,     0,   -57,     0,   -57,   -53,
     -53,     2,   -55,   -55,    53,    60,     3,     0,     4,    46,
     -57,   -57,     0,     0,     3,    41,     0,     0,    24,     0,
       5,     6,    25,    26,     3,    27,    28,    29,    24,     0,
       0,     0,    25,    26,    59,    27,    28,    29,     0,     0,
      41,     0,     0,     0,     0,    60
  };

  const signed char
  TOMLParser::yycheck_[] =
  {
      31,    31,    31,    30,     0,    30,    30,    30,     0,     1,
      19,     0,     1,     5,    10,     7,     5,    12,     7,     7,
       8,    15,     6,    19,    51,     8,     1,    19,    20,    -1,
      19,    20,    58,    21,    65,    65,    65,    62,    62,    62,
       0,     1,    58,     0,     1,     5,    -1,     7,     5,    -1,
       7,     0,     1,    -1,     0,    -1,     5,    -1,     7,    19,
      20,     0,    19,    20,    10,    11,     5,    -1,     7,     0,
      19,    20,    -1,    -1,     5,     6,    -1,    -1,     9,    -1,
      19,    20,    13,    14,     5,    16,    17,    18,     9,    -1,
      -1,    -1,    13,    14,     0,    16,    17,    18,    -1,    -1,
       6,    -1,    -1,    -1,    -1,    11
  };

  const signed char
  TOMLParser::yystos_[] =
  {
       0,    46,     0,     5,     7,    19,    20,    23,    27,    31,
      34,    35,    36,    39,    40,    41,    44,    45,    35,    35,
      12,    30,    15,    22,     9,    13,    14,    16,    17,    18,
      23,    25,    32,    33,    37,    38,     1,     1,     1,     1,
       1,     6,    24,     8,    28,    35,     0,    24,    32,    33,
      38,    42,     0,    10,    26,    39,    40,    41,    43,     0,
      11,    24,    29,     0,    26,    29,    32,    33,    38,    39,
      40,    41
  };

  const signed char
  TOMLParser::yyr1_[] =
  {
       0,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    32,    32,    32,    33,    33,    33,    33,
      34,    35,    35,    36,    37,    37,    37,    37,    37,    38,
      39,    40,    41,    42,    42,    42,    42,    42,    42,    43,
      43,    43,    43,    43,    43,    44,    45,    46,    46,    46,
      46,    46,    46,    46,    46,    46,    46,    46,    46
  };

  const signed char
  TOMLParser::yyr2_[] =
  {
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     3,     3,     2,     2,     3,     3,
       1,     1,     3,     2,     1,     1,     1,     1,     1,     1,
       2,     2,     2,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     3,     3,     0,     2,     2,
       3,     2,     3,     2,     3,     2,     3,     2,     3
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const TOMLParser::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "\"end of line\"",
  "\"invalid token\"", "\"[\"", "\"]\"", "\"[[\"", "\"]]\"", "\"{\"",
  "\"}\"", "\"comma\"", "\"dot\"", "\"true\"", "\"false\"", "\"=\"",
  "\"real\"", "\"integer\"", "\"string\"", "\"identifier\"", "\"comment\"",
  "$accept", "assign", "lbracket", "rbracket", "lbrace", "rbrace",
  "dlbracket", "drbracket", "comma", "separator", "comment", "array",
  "object", "decl", "key", "declaration", "PRIMITIVE", "primitive",
  "keyed_primitive", "keyed_object", "keyed_array", "array_members",
  "object_members", "table", "array_table", "start", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
  TOMLParser::yyrline_[] =
  {
       0,   205,   205,   212,   218,   224,   230,   236,   242,   249,
     255,   262,   270,   276,   282,   293,   299,   305,   311,   323,
     329,   338,   343,   349,   356,   356,   356,   356,   356,   358,
     364,   370,   380,   390,   399,   409,   418,   428,   433,   439,
     444,   450,   455,   461,   466,   474,   490,   504,   505,   508,
     511,   515,   518,   522,   525,   529,   532,   536,   539
  };

  void
  TOMLParser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  TOMLParser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  TOMLParser::symbol_kind_type
  TOMLParser::yytranslate_ (int t) YY_NOEXCEPT
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const signed char
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20
    };
    // Last valid token kind.
    const int code_max = 275;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return static_cast <symbol_kind_type> (translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

#line 36 "TOML.bison"
} // wasp
#line 1917 "TOMLParser.cpp"

#line 546 "TOML.bison"
 /*** Additional Code ***/
namespace wasp{
void TOMLParser::error(const TOMLParser::location_type& l,
                           const std::string& m)
{
    interpreter.error_diagnostic()<<l<<": "<<m<<std::endl;
}
} // end of namespace
