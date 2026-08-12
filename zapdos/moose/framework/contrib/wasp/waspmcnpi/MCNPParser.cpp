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
#line 1 "MCNPParser.bison"
 /*** C/C++ Declarations ***/

#include <stdio.h>
#include <string>
#include <vector>


#line 49 "MCNPParser.cpp"


#include "MCNPParser.hpp"

// Second part of user prologue.
#line 704 "MCNPParser.bison"


#include "MCNPInterpreter.h"
#include "MCNPLexer.h"

#include "waspcore/wasp_bug.h"

/* this "connects" the bison parser in the interpreter to the flex MCNPLexer class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the interpreter context. */
#undef yylex
#define yylex lexer->lex

void assign_names(class wasp::AbstractInterpreter& interpreter,
                    const std::vector<std::string>& names,
                    const std::vector<wasp::mcnpi::NODE>& types,
                    const std::vector<size_t>* tokens,
                     std::vector<size_t>& results);


#line 76 "MCNPParser.cpp"



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

#line 35 "MCNPParser.bison"
namespace wasp {
#line 170 "MCNPParser.cpp"

  /// Build a parser object.
  MCNPParser::MCNPParser (class AbstractInterpreter& interpreter_yyarg, std::istream &input_stream_yyarg, std::shared_ptr<class MCNPLexerImpl> lexer_yyarg)
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

  MCNPParser::~MCNPParser ()
  {}

  MCNPParser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/

  // basic_symbol.
  template <typename Base>
  MCNPParser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value (that.value)
    , location (that.location)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  MCNPParser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_MOVE_REF (location_type) l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  MCNPParser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_RVREF (value_type) v, YY_RVREF (location_type) l)
    : Base (t)
    , value (YY_MOVE (v))
    , location (YY_MOVE (l))
  {}


  template <typename Base>
  MCNPParser::symbol_kind_type
  MCNPParser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  template <typename Base>
  bool
  MCNPParser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  MCNPParser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    value = YY_MOVE (s.value);
    location = YY_MOVE (s.location);
  }

  // by_kind.
  MCNPParser::by_kind::by_kind () YY_NOEXCEPT
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  MCNPParser::by_kind::by_kind (by_kind&& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  MCNPParser::by_kind::by_kind (const by_kind& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {}

  MCNPParser::by_kind::by_kind (token_kind_type t) YY_NOEXCEPT
    : kind_ (yytranslate_ (t))
  {}



  void
  MCNPParser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  void
  MCNPParser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  MCNPParser::symbol_kind_type
  MCNPParser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }


  MCNPParser::symbol_kind_type
  MCNPParser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }



  // by_state.
  MCNPParser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  MCNPParser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  MCNPParser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  MCNPParser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  MCNPParser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  MCNPParser::symbol_kind_type
  MCNPParser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  MCNPParser::stack_symbol_type::stack_symbol_type ()
  {}

  MCNPParser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.value), YY_MOVE (that.location))
  {
#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  MCNPParser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.value), YY_MOVE (that.location))
  {
    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  MCNPParser::stack_symbol_type&
  MCNPParser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    return *this;
  }

  MCNPParser::stack_symbol_type&
  MCNPParser::stack_symbol_type::operator= (stack_symbol_type& that)
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
  MCNPParser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    switch (yysym.kind ())
    {
      case symbol_kind::S_comment_list: // comment_list
#line 638 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 384 "MCNPParser.cpp"
        break;

      case symbol_kind::S_execution_other_options: // execution_other_options
#line 633 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 390 "MCNPParser.cpp"
        break;

      case symbol_kind::S_execution_filenames: // execution_filenames
#line 633 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 396 "MCNPParser.cpp"
        break;

      case symbol_kind::S_cell_geom_expression: // cell_geom_expression
#line 633 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 402 "MCNPParser.cpp"
        break;

      case symbol_kind::S_cell_fill_data: // cell_fill_data
#line 634 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 408 "MCNPParser.cpp"
        break;

      case symbol_kind::S_cell_transform_data: // cell_transform_data
#line 637 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 414 "MCNPParser.cpp"
        break;

      case symbol_kind::S_cell_fill_list: // cell_fill_list
#line 633 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 420 "MCNPParser.cpp"
        break;

      case symbol_kind::S_cell_params: // cell_params
#line 633 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 426 "MCNPParser.cpp"
        break;

      case symbol_kind::S_like_but_cell_params: // like_but_cell_params
#line 639 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 432 "MCNPParser.cpp"
        break;

      case symbol_kind::S_number_list: // number_list
#line 634 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 438 "MCNPParser.cpp"
        break;

      case symbol_kind::S_number_paren_list: // number_paren_list
#line 638 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 444 "MCNPParser.cpp"
        break;

      case symbol_kind::S_number_paren_colon_list: // number_paren_colon_list
#line 638 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 450 "MCNPParser.cpp"
        break;

      case symbol_kind::S_value_paren_colon_list: // value_paren_colon_list
#line 638 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 456 "MCNPParser.cpp"
        break;

      case symbol_kind::S_value_token_list: // value_token_list
#line 639 "MCNPParser.bison"
                    { delete (yysym.value.token_indices); }
#line 462 "MCNPParser.cpp"
        break;

      case symbol_kind::S_number_token_list: // number_token_list
#line 637 "MCNPParser.bison"
                    { delete (yysym.value.token_indices); }
#line 468 "MCNPParser.cpp"
        break;

      case symbol_kind::S_value_list: // value_list
#line 638 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 474 "MCNPParser.cpp"
        break;

      case symbol_kind::S_value_comma_list: // value_comma_list
#line 636 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 480 "MCNPParser.cpp"
        break;

      case symbol_kind::S_string_list: // string_list
#line 634 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 486 "MCNPParser.cpp"
        break;

      case symbol_kind::S_uran_members: // uran_members
#line 634 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 492 "MCNPParser.cpp"
        break;

      case symbol_kind::S_material_options: // material_options
#line 634 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 498 "MCNPParser.cpp"
        break;

      case symbol_kind::S_comma_sep_list: // comma_sep_list
#line 635 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 504 "MCNPParser.cpp"
        break;

      case symbol_kind::S_activation_options: // activation_options
#line 635 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 510 "MCNPParser.cpp"
        break;

      case symbol_kind::S_particle_symbol_list: // particle_symbol_list
#line 635 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 516 "MCNPParser.cpp"
        break;

      case symbol_kind::S_mode_particle_symbol_list: // mode_particle_symbol_list
#line 636 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 522 "MCNPParser.cpp"
        break;

      case symbol_kind::S_tally_surface_list: // tally_surface_list
#line 635 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 528 "MCNPParser.cpp"
        break;

      case symbol_kind::S_f5_tally_members: // f5_tally_members
#line 636 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 534 "MCNPParser.cpp"
        break;

      case symbol_kind::S_f5a_tally_members: // f5a_tally_members
#line 636 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 540 "MCNPParser.cpp"
        break;

      case symbol_kind::S_tally_time_options: // tally_time_options
#line 637 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 546 "MCNPParser.cpp"
        break;

      case symbol_kind::S_sdef_value_list: // sdef_value_list
#line 638 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 552 "MCNPParser.cpp"
        break;

      case symbol_kind::S_data_sdef_members: // data_sdef_members
#line 639 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 558 "MCNPParser.cpp"
        break;

      case symbol_kind::S_data_kopts_options: // data_kopts_options
#line 639 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 564 "MCNPParser.cpp"
        break;

      case symbol_kind::S_data_rand_card_options: // data_rand_card_options
#line 639 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 570 "MCNPParser.cpp"
        break;

      case symbol_kind::S_data_vect_members: // data_vect_members
#line 638 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 576 "MCNPParser.cpp"
        break;

      case symbol_kind::S_data_esplt_pairs: // data_esplt_pairs
#line 637 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 582 "MCNPParser.cpp"
        break;

      case symbol_kind::S_data_tsplt_pairs: // data_tsplt_pairs
#line 637 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 588 "MCNPParser.cpp"
        break;

      case symbol_kind::S_data_ft_members: // data_ft_members
#line 634 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 594 "MCNPParser.cpp"
        break;

      case symbol_kind::S_tmesh_xmesh_members: // tmesh_xmesh_members
#line 643 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 600 "MCNPParser.cpp"
        break;

      case symbol_kind::S_data_tmesh_members: // data_tmesh_members
#line 635 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 606 "MCNPParser.cpp"
        break;

      case symbol_kind::S_data_fmesh_members: // data_fmesh_members
#line 647 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 612 "MCNPParser.cpp"
        break;

      case symbol_kind::S_data_mesh_members: // data_mesh_members
#line 674 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 618 "MCNPParser.cpp"
        break;

      case symbol_kind::S_data_tropt_members: // data_tropt_members
#line 651 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 624 "MCNPParser.cpp"
        break;

      case symbol_kind::S_data_pert_members: // data_pert_members
#line 655 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 630 "MCNPParser.cpp"
        break;

      case symbol_kind::S_data_embed_members: // data_embed_members
#line 659 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 636 "MCNPParser.cpp"
        break;

      case symbol_kind::S_data_embee_members: // data_embee_members
#line 663 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 642 "MCNPParser.cpp"
        break;

      case symbol_kind::S_data_ssw_members: // data_ssw_members
#line 663 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 648 "MCNPParser.cpp"
        break;

      case symbol_kind::S_data_ssr_members: // data_ssr_members
#line 670 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 654 "MCNPParser.cpp"
        break;

      case symbol_kind::S_data_fmult_members: // data_fmult_members
#line 678 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 660 "MCNPParser.cpp"
        break;

      case symbol_kind::S_data_kpert_members: // data_kpert_members
#line 682 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 666 "MCNPParser.cpp"
        break;

      case symbol_kind::S_data_ksen_members: // data_ksen_members
#line 686 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 672 "MCNPParser.cpp"
        break;

      case symbol_kind::S_data_ptrac_members: // data_ptrac_members
#line 690 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 678 "MCNPParser.cpp"
        break;

      case symbol_kind::S_data_column_wise_header_members: // data_column_wise_header_members
#line 695 "MCNPParser.bison"
                    { delete (yysym.value.token_indices); }
#line 684 "MCNPParser.cpp"
        break;

      case symbol_kind::S_data_column_wise_row_members: // data_column_wise_row_members
#line 695 "MCNPParser.bison"
                    { delete (yysym.value.token_indices); }
#line 690 "MCNPParser.cpp"
        break;

      case symbol_kind::S_data_column_wise_table: // data_column_wise_table
#line 696 "MCNPParser.bison"
                    { for(auto i = 0; i < (yysym.value.table_token_indices)->size(); ++i) delete (yysym.value.table_token_indices)->at(i); delete (yysym.value.table_token_indices); }
#line 696 "MCNPParser.cpp"
        break;

      case symbol_kind::S_data_bfld_members: // data_bfld_members
#line 700 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 702 "MCNPParser.cpp"
        break;

      case symbol_kind::S_material_zaid_pairs: // material_zaid_pairs
#line 703 "MCNPParser.bison"
                    { delete (yysym.value.node_indices); }
#line 708 "MCNPParser.cpp"
        break;

      default:
        break;
    }
  }

#if YYDEBUG
  template <typename Base>
  void
  MCNPParser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
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
  MCNPParser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  MCNPParser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  MCNPParser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  MCNPParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  MCNPParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  MCNPParser::debug_level_type
  MCNPParser::debug_level () const
  {
    return yydebug_;
  }

  void
  MCNPParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  MCNPParser::state_type
  MCNPParser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  MCNPParser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  MCNPParser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  MCNPParser::operator() ()
  {
    return parse ();
  }

  int
  MCNPParser::parse ()
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
#line 44 "MCNPParser.bison"
{
    // initialize the initial location object
    yyla.location.begin.filename = yyla.location.end.filename = &interpreter.stream_name();
    yyla.location.begin.line = yyla.location.end.line = interpreter.start_line();
    yyla.location.begin.column = yyla.location.end.column = interpreter.start_column();
    lexer = std::make_shared<MCNPLexerImpl>(interpreter,&input_stream);

    // lexer->set_debug(true);
    // set_debug_level(true);
}

#line 857 "MCNPParser.cpp"


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
  case 2: // comment: "comment"
#line 731 "MCNPParser.bison"
                 {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::COMMENT, "comment", (yystack_[0].value.token_index));}
#line 995 "MCNPParser.cpp"
    break;

  case 3: // comment_list: comment
#line 735 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 1004 "MCNPParser.cpp"
    break;

  case 4: // comment_list: comment_list comment
#line 740 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 1013 "MCNPParser.cpp"
    break;

  case 5: // assign: "="
#line 746 "MCNPParser.bison"
                {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::ASSIGN, "=", (yystack_[0].value.token_index));}
#line 1019 "MCNPParser.cpp"
    break;

  case 6: // PRIMITIVE: "string"
#line 749 "MCNPParser.bison"
            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 1025 "MCNPParser.cpp"
    break;

  case 7: // PRIMITIVE: "integer number"
#line 749 "MCNPParser.bison"
                     { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 1031 "MCNPParser.cpp"
    break;

  case 8: // PRIMITIVE: "double precision number"
#line 749 "MCNPParser.bison"
                               { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 1037 "MCNPParser.cpp"
    break;

  case 9: // PRIMITIVE: "value"
#line 749 "MCNPParser.bison"
                                        { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 1043 "MCNPParser.cpp"
    break;

  case 10: // PRIMITIVE: "no"
#line 749 "MCNPParser.bison"
                                                { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 1049 "MCNPParser.cpp"
    break;

  case 11: // NUMBER: "integer number"
#line 750 "MCNPParser.bison"
         { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 1055 "MCNPParser.cpp"
    break;

  case 12: // NUMBER: "double precision number"
#line 750 "MCNPParser.bison"
                   { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 1061 "MCNPParser.cpp"
    break;

  case 13: // int_value: "integer number"
#line 753 "MCNPParser.bison"
                   {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::VALUE, "value" ,(yystack_[0].value.token_index));}
#line 1067 "MCNPParser.cpp"
    break;

  case 14: // value: PRIMITIVE
#line 756 "MCNPParser.bison"
                 {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::VALUE, "value", (yystack_[0].value.token_index));}
#line 1073 "MCNPParser.cpp"
    break;

  case 15: // number: NUMBER
#line 758 "MCNPParser.bison"
               {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::VALUE, "value", (yystack_[0].value.token_index));}
#line 1079 "MCNPParser.cpp"
    break;

  case 16: // id: "integer number"
#line 760 "MCNPParser.bison"
            {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::ID, "id", (yystack_[0].value.token_index));}
#line 1085 "MCNPParser.cpp"
    break;

  case 17: // card_id: "integer identifier"
#line 762 "MCNPParser.bison"
            {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::ID, "id", (yystack_[0].value.token_index));}
#line 1091 "MCNPParser.cpp"
    break;

  case 18: // message_card: "message card"
#line 764 "MCNPParser.bison"
                           {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::MESSAGE_CARD, "decl", (yystack_[0].value.token_index));}
#line 1097 "MCNPParser.cpp"
    break;

  case 19: // execution_filename_key: "datapath"
#line 766 "MCNPParser.bison"
                                 {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_DATAPATH).data(), (yystack_[0].value.token_index));}
#line 1103 "MCNPParser.cpp"
    break;

  case 20: // execution_filename_key: "inp"
#line 767 "MCNPParser.bison"
    {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_INP).data(), (yystack_[0].value.token_index));}
#line 1109 "MCNPParser.cpp"
    break;

  case 21: // execution_filename_key: "outp"
#line 768 "MCNPParser.bison"
     {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_OUTP).data(), (yystack_[0].value.token_index));}
#line 1115 "MCNPParser.cpp"
    break;

  case 22: // execution_filename_key: "runtpe"
#line 769 "MCNPParser.bison"
       {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_RUNTPE).data(), (yystack_[0].value.token_index));}
#line 1121 "MCNPParser.cpp"
    break;

  case 23: // execution_filename_key: "wwinp"
#line 770 "MCNPParser.bison"
      {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_WWINP).data(), (yystack_[0].value.token_index));}
#line 1127 "MCNPParser.cpp"
    break;

  case 24: // execution_filename_key: "wwout"
#line 771 "MCNPParser.bison"
      {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_WWOUT).data(), (yystack_[0].value.token_index));}
#line 1133 "MCNPParser.cpp"
    break;

  case 25: // execution_filename_key: "wwone"
#line 772 "MCNPParser.bison"
      {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_WWONE).data(), (yystack_[0].value.token_index));}
#line 1139 "MCNPParser.cpp"
    break;

  case 26: // execution_filename_key: "partinp"
#line 773 "MCNPParser.bison"
        {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_PARTINP).data(), (yystack_[0].value.token_index));}
#line 1145 "MCNPParser.cpp"
    break;

  case 27: // execution_filename_key: "linkin"
#line 774 "MCNPParser.bison"
       {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_LINKIN).data(), (yystack_[0].value.token_index));}
#line 1151 "MCNPParser.cpp"
    break;

  case 28: // execution_filename_key: "linkout"
#line 775 "MCNPParser.bison"
        {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_LINKOUT).data(), (yystack_[0].value.token_index));}
#line 1157 "MCNPParser.cpp"
    break;

  case 29: // execution_filename_key: "format of senstitivity profile output file (KSENTAL)"
#line 776 "MCNPParser.bison"
        {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, "ksental", (yystack_[0].value.token_index));}
#line 1163 "MCNPParser.cpp"
    break;

  case 30: // execution_filename_key: "histp"
#line 777 "MCNPParser.bison"
      {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_HISTP).data(), (yystack_[0].value.token_index));}
#line 1169 "MCNPParser.cpp"
    break;

  case 31: // execution_filename_key: "com"
#line 778 "MCNPParser.bison"
    {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_COM).data(), (yystack_[0].value.token_index));}
#line 1175 "MCNPParser.cpp"
    break;

  case 32: // execution_filename_key: "comout"
#line 779 "MCNPParser.bison"
       {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_COMOUT).data(), (yystack_[0].value.token_index));}
#line 1181 "MCNPParser.cpp"
    break;

  case 33: // execution_filename_key: "plotm"
#line 780 "MCNPParser.bison"
      {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_PLOTM).data(), (yystack_[0].value.token_index));}
#line 1187 "MCNPParser.cpp"
    break;

  case 34: // execution_filename_key: "mctal"
#line 781 "MCNPParser.bison"
      {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_MCTAL).data(), (yystack_[0].value.token_index));}
#line 1193 "MCNPParser.cpp"
    break;

  case 35: // execution_filename_key: "meshtal"
#line 782 "MCNPParser.bison"
        {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_MESHTAL).data(), (yystack_[0].value.token_index));}
#line 1199 "MCNPParser.cpp"
    break;

  case 36: // execution_filename_key: "mdata"
#line 783 "MCNPParser.bison"
      {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_MDATA).data(), (yystack_[0].value.token_index));}
#line 1205 "MCNPParser.cpp"
    break;

  case 37: // execution_filename_key: "name"
#line 784 "MCNPParser.bison"
     {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_NAME).data(), (yystack_[0].value.token_index));}
#line 1211 "MCNPParser.cpp"
    break;

  case 38: // execution_filename_key: "srctp"
#line 785 "MCNPParser.bison"
      {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_SRCTP).data(), (yystack_[0].value.token_index));}
#line 1217 "MCNPParser.cpp"
    break;

  case 39: // execution_filename_key: "wssa"
#line 786 "MCNPParser.bison"
     {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_WSSA).data(), (yystack_[0].value.token_index));}
#line 1223 "MCNPParser.cpp"
    break;

  case 40: // execution_filename_key: "rssa"
#line 787 "MCNPParser.bison"
     {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_RSSA).data(), (yystack_[0].value.token_index));}
#line 1229 "MCNPParser.cpp"
    break;

  case 41: // filename_value: execution_filename_key value
#line 791 "MCNPParser.bison"
{
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                            ,interpreter.name((yystack_[1].value.node_index))
                            ,{(yystack_[1].value.node_index), (yystack_[0].value.node_index)});
}
#line 1239 "MCNPParser.cpp"
    break;

  case 42: // other_option_optional_int: "c"
#line 799 "MCNPParser.bison"
                                         {auto c = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[0].value.token_index)); (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE, symbol_name(symbol_kind::S_CONTINUE_RUN).data(), {c});}
#line 1245 "MCNPParser.cpp"
    break;

  case 43: // other_option_optional_int: "c" int_value
#line 800 "MCNPParser.bison"
                       {auto c = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index)); (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE, symbol_name(symbol_kind::S_CONTINUE_RUN).data(), {c, (yystack_[0].value.node_index)});}
#line 1251 "MCNPParser.cpp"
    break;

  case 44: // other_option_optional_int: "cn"
#line 801 "MCNPParser.bison"
                  {auto c = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[0].value.token_index)); (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE, symbol_name(symbol_kind::S_CONTINUE_RUN_TPE).data(), {c});}
#line 1257 "MCNPParser.cpp"
    break;

  case 45: // other_option_optional_int: "cn" int_value
#line 802 "MCNPParser.bison"
                           {auto c = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index)); (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE, symbol_name(symbol_kind::S_CONTINUE_RUN_TPE).data(), {c, (yystack_[0].value.node_index)});}
#line 1263 "MCNPParser.cpp"
    break;

  case 46: // other_option_optional_int: "dbug" int_value
#line 803 "MCNPParser.bison"
               {auto c = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index)); (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE, symbol_name(symbol_kind::S_DBUG).data(), {c, (yystack_[0].value.node_index)});}
#line 1269 "MCNPParser.cpp"
    break;

  case 47: // other_option_optional_int: "tasks" int_value
#line 804 "MCNPParser.bison"
                {auto c = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index)); (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE, symbol_name(symbol_kind::S_TASKS).data(), {c, (yystack_[0].value.node_index)});}
#line 1275 "MCNPParser.cpp"
    break;

  case 48: // execution_other_option: other_option_optional_int
#line 807 "MCNPParser.bison"
                        { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 1281 "MCNPParser.cpp"
    break;

  case 49: // execution_other_option: "devtest"
#line 808 "MCNPParser.bison"
         {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_DEVTEST).data(), (yystack_[0].value.token_index));}
#line 1287 "MCNPParser.cpp"
    break;

  case 50: // execution_other_option: "notek"
#line 809 "MCNPParser.bison"
       {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_NOTEK).data(), (yystack_[0].value.token_index));}
#line 1293 "MCNPParser.cpp"
    break;

  case 51: // execution_other_option: "fatal"
#line 810 "MCNPParser.bison"
       {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_FATAL).data(), (yystack_[0].value.token_index));}
#line 1299 "MCNPParser.cpp"
    break;

  case 52: // execution_other_option: "print"
#line 811 "MCNPParser.bison"
       {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_PRINT).data(), (yystack_[0].value.token_index));}
#line 1305 "MCNPParser.cpp"
    break;

  case 53: // execution_other_option: "balance"
#line 812 "MCNPParser.bison"
         {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_BALANCE).data(), (yystack_[0].value.token_index));}
#line 1311 "MCNPParser.cpp"
    break;

  case 54: // execution_other_option: "eol"
#line 813 "MCNPParser.bison"
         {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_MCNPEOL).data(), (yystack_[0].value.token_index));}
#line 1317 "MCNPParser.cpp"
    break;

  case 55: // execution_other_option: "input"
#line 814 "MCNPParser.bison"
       {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_INPUT).data(), (yystack_[0].value.token_index));}
#line 1323 "MCNPParser.cpp"
    break;

  case 56: // execution_other_option: "plot_geom"
#line 815 "MCNPParser.bison"
           {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_PLOT_GEOM).data(), (yystack_[0].value.token_index));}
#line 1329 "MCNPParser.cpp"
    break;

  case 57: // execution_other_option: "crosssection"
#line 816 "MCNPParser.bison"
    {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_XS).data(), (yystack_[0].value.token_index));}
#line 1335 "MCNPParser.cpp"
    break;

  case 58: // execution_other_option: "transport"
#line 817 "MCNPParser.bison"
           {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_TRANSPORT).data(), (yystack_[0].value.token_index));}
#line 1341 "MCNPParser.cpp"
    break;

  case 59: // execution_other_option: "plot_data"
#line 818 "MCNPParser.bison"
           {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_PLOT_DATA).data(), (yystack_[0].value.token_index));}
#line 1347 "MCNPParser.cpp"
    break;

  case 60: // execution_other_option: "partisn_input"
#line 819 "MCNPParser.bison"
               {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_PARTISN_INPUT).data(), (yystack_[0].value.token_index));}
#line 1353 "MCNPParser.cpp"
    break;

  case 61: // execution_other_options: execution_other_option
#line 823 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 1362 "MCNPParser.cpp"
    break;

  case 62: // execution_other_options: execution_other_options execution_other_option
#line 828 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 1371 "MCNPParser.cpp"
    break;

  case 63: // execution_filenames: filename_value
#line 835 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 1380 "MCNPParser.cpp"
    break;

  case 64: // execution_filenames: execution_filenames filename_value
#line 840 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 1389 "MCNPParser.cpp"
    break;

  case 65: // message_block: message_card execution_filenames
#line 847 "MCNPParser.bison"
{
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::OBJECT
                                ,"msg_block"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 1401 "MCNPParser.cpp"
    break;

  case 66: // message_block: message_card execution_other_options
#line 855 "MCNPParser.bison"
{
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::OBJECT
                                ,"msg_block"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 1413 "MCNPParser.cpp"
    break;

  case 67: // message_block: message_card execution_filenames execution_other_options
#line 863 "MCNPParser.bison"
{
    (yystack_[1].value.node_indices)->insert((yystack_[1].value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yystack_[1].value.node_indices)->insert((yystack_[1].value.node_indices)->end(), (yystack_[0].value.node_indices)->begin(), (yystack_[0].value.node_indices)->end());
    (yylhs.value.node_index) = interpreter.push_parent(wasp::OBJECT
                                ,"msg_block"
                                ,*(yystack_[1].value.node_indices));
    delete (yystack_[1].value.node_indices);
    delete (yystack_[0].value.node_indices);
}
#line 1427 "MCNPParser.cpp"
    break;

  case 68: // message_block: message_card
#line 873 "MCNPParser.bison"
{
    (yylhs.value.node_index) = interpreter.push_parent(wasp::OBJECT
                                ,"msg_block"
                                ,{(yystack_[0].value.node_index)});
}
#line 1437 "MCNPParser.cpp"
    break;

  case 69: // continue: "continue"
#line 880 "MCNPParser.bison"
{
    (yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::CONTINUE_PROBLEM, "continue", (yystack_[0].value.token_index));
}
#line 1445 "MCNPParser.cpp"
    break;

  case 70: // blank_line: "blank line delimiter"
#line 885 "MCNPParser.bison"
{
    (yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::BLANK_LINE, "BL", (yystack_[0].value.token_index));
}
#line 1453 "MCNPParser.cpp"
    break;

  case 71: // title_card: "title"
#line 891 "MCNPParser.bison"
{
    (yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::TITLE_CARD, "title", (yystack_[0].value.token_index));
}
#line 1461 "MCNPParser.cpp"
    break;

  case 72: // cell_id: "cell_id"
#line 896 "MCNPParser.bison"
{
    (yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::ID, "id", (yystack_[0].value.token_index));
}
#line 1469 "MCNPParser.cpp"
    break;

  case 73: // material_id: "material_id"
#line 901 "MCNPParser.bison"
{
    (yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::MATERIAL, "material", (yystack_[0].value.token_index));
}
#line 1477 "MCNPParser.cpp"
    break;

  case 74: // density: "density"
#line 906 "MCNPParser.bison"
{
    (yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DENSITY, "rho", (yystack_[0].value.token_index));
}
#line 1485 "MCNPParser.cpp"
    break;

  case 75: // geom_op_union: "union"
#line 911 "MCNPParser.bison"
{
    (yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::UNION, "U", (yystack_[0].value.token_index));
}
#line 1493 "MCNPParser.cpp"
    break;

  case 76: // geom_op_complement: "#"
#line 916 "MCNPParser.bison"
{
    (yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::COMPLEMENT, "#", (yystack_[0].value.token_index));
}
#line 1501 "MCNPParser.cpp"
    break;

  case 77: // geom_id: NUMBER
#line 921 "MCNPParser.bison"
{
    (yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::ID, "id", (yystack_[0].value.token_index));
}
#line 1509 "MCNPParser.cpp"
    break;

  case 78: // lparen: "("
#line 927 "MCNPParser.bison"
{
    (yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::LPAREN, "LP", (yystack_[0].value.token_index));
}
#line 1517 "MCNPParser.cpp"
    break;

  case 79: // rparen: ")"
#line 932 "MCNPParser.bison"
{
    (yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::RPAREN, "RP", (yystack_[0].value.token_index));
}
#line 1525 "MCNPParser.cpp"
    break;

  case 80: // cell_geom_expression: geom_id
#line 941 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 1534 "MCNPParser.cpp"
    break;

  case 81: // cell_geom_expression: cell_geom_expression cell_geom_expression
#line 946 "MCNPParser.bison"
{ // whitespace between geometry ids is intersection operator
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->end(), (yystack_[0].value.node_indices)->begin(), (yystack_[0].value.node_indices)->end());
    delete (yystack_[0].value.node_indices);
}
#line 1544 "MCNPParser.cpp"
    break;

  case 82: // cell_geom_expression: lparen cell_geom_expression rparen
#line 952 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->end(), (yystack_[0].value.node_index));
}
#line 1554 "MCNPParser.cpp"
    break;

  case 83: // cell_geom_expression: cell_geom_expression geom_op_union cell_geom_expression
#line 958 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[2].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->end(), (yystack_[0].value.node_indices)->begin(), (yystack_[0].value.node_indices)->end());
    delete (yystack_[0].value.node_indices);
}
#line 1565 "MCNPParser.cpp"
    break;

  case 84: // cell_geom_expression: geom_op_complement cell_geom_expression
#line 965 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[0].value.node_indices);
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->begin(), (yystack_[1].value.node_index));
}
#line 1574 "MCNPParser.cpp"
    break;

  case 85: // cell_geom: cell_geom_expression
#line 973 "MCNPParser.bison"
{

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::CELL_GEOM
                                ,"geom"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 1586 "MCNPParser.cpp"
    break;

  case 86: // cell_param_key: "vol"
#line 982 "MCNPParser.bison"
                   {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_VOL).data(), (yystack_[0].value.token_index));}
#line 1592 "MCNPParser.cpp"
    break;

  case 87: // cell_param_key: "pwt"
#line 983 "MCNPParser.bison"
    {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_PWT).data(), (yystack_[0].value.token_index));}
#line 1598 "MCNPParser.cpp"
    break;

  case 88: // cell_param_key: "nonu"
#line 984 "MCNPParser.bison"
     {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_NONU).data(), (yystack_[0].value.token_index));}
#line 1604 "MCNPParser.cpp"
    break;

  case 89: // cell_param_key: "pd"
#line 985 "MCNPParser.bison"
   {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_PD).data(), (yystack_[0].value.token_index));}
#line 1610 "MCNPParser.cpp"
    break;

  case 90: // cell_param_key: "tmp"
#line 986 "MCNPParser.bison"
    {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_TMP).data(), (yystack_[0].value.token_index));}
#line 1616 "MCNPParser.cpp"
    break;

  case 91: // cell_param_key: "universe"
#line 987 "MCNPParser.bison"
         {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_UNIVERSE).data(), (yystack_[0].value.token_index));}
#line 1622 "MCNPParser.cpp"
    break;

  case 92: // cell_param_key: "lat"
#line 988 "MCNPParser.bison"
    {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_LAT).data(), (yystack_[0].value.token_index));}
#line 1628 "MCNPParser.cpp"
    break;

  case 93: // cell_param_key: "cosy"
#line 989 "MCNPParser.bison"
     {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_COSY).data(), (yystack_[0].value.token_index));}
#line 1634 "MCNPParser.cpp"
    break;

  case 94: // cell_param_key: "bfcl"
#line 990 "MCNPParser.bison"
      {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_BFLCL).data(), (yystack_[0].value.token_index));}
#line 1640 "MCNPParser.cpp"
    break;

  case 95: // cell_fill_data: value
#line 995 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 1649 "MCNPParser.cpp"
    break;

  case 96: // cell_fill_data: value lparen number_token_list rparen
#line 1000 "MCNPParser.bison"
{
    std::vector<size_t> children = {(yystack_[3].value.node_index), (yystack_[2].value.node_index)};

    if ((yystack_[1].value.token_indices)->size() == 1 ) // only 'm' captured
        assign_names(interpreter, {"m"}, {mcnpi::ID}, (yystack_[1].value.token_indices), children);
    else
        assign_names(interpreter,
                            {"o", "o", "o",
                            "rm", "rm", "rm",
                            "rm", "rm", "rm",
                            "rm", "rm", "rm",
                            "m"},
                            {mcnpi::ORIGIN, mcnpi::ORIGIN, mcnpi::ORIGIN,
                            mcnpi::VALUE, mcnpi::VALUE, mcnpi::VALUE,
                            mcnpi::VALUE, mcnpi::VALUE, mcnpi::VALUE,
                            mcnpi::VALUE, mcnpi::VALUE, mcnpi::VALUE,
                            mcnpi::ID}, (yystack_[1].value.token_indices), children);
    children.push_back((yystack_[0].value.node_index));
    (yylhs.value.node_indices) = new std::vector<size_t>(children);
}
#line 1674 "MCNPParser.cpp"
    break;

  case 97: // cell_fill_data: "integer number" "union" "integer number" "integer number" "union" "integer number" "integer number" "union" "integer number" cell_fill_list
#line 1021 "MCNPParser.bison"
 {
    auto is = interpreter.push_leaf(wasp::mcnpi::VALUE, "istart", (yystack_[9].value.token_index));
    auto l1 = interpreter.push_leaf(wasp::mcnpi::VALUE, ":", (yystack_[8].value.token_index));
    auto ie = interpreter.push_leaf(wasp::mcnpi::VALUE, "iend", (yystack_[7].value.token_index));

    auto js = interpreter.push_leaf(wasp::mcnpi::VALUE, "jstart", (yystack_[6].value.token_index));
    auto l2 = interpreter.push_leaf(wasp::mcnpi::VALUE, ":", (yystack_[5].value.token_index));
    auto je = interpreter.push_leaf(wasp::mcnpi::VALUE, "jend", (yystack_[4].value.token_index));

    auto ks = interpreter.push_leaf(wasp::mcnpi::VALUE, "kstart", (yystack_[3].value.token_index));
    auto l3 = interpreter.push_leaf(wasp::mcnpi::VALUE, ":", (yystack_[2].value.token_index));
    auto ke = interpreter.push_leaf(wasp::mcnpi::VALUE, "kend", (yystack_[1].value.token_index));

    auto cfl = interpreter.push_parent(wasp::mcnpi::FILL_LIST, "list", *(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);

    (yylhs.value.node_indices) = new std::vector<size_t>();
    *(yylhs.value.node_indices) = {is, l1, ie, js, l2, je, ks, l3, ke, cfl};
 }
#line 1698 "MCNPParser.cpp"
    break;

  case 98: // cell_fill: "fill" assign cell_fill_data
#line 1044 "MCNPParser.bison"
 {
    auto n = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_FILL).data(), (yystack_[2].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), n);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , "fill"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
 }
#line 1712 "MCNPParser.cpp"
    break;

  case 99: // cell_fill: "fill" cell_fill_data
#line 1054 "MCNPParser.bison"
 {
    auto n = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_FILL).data(), (yystack_[1].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), n);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , "fill"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
 }
#line 1725 "MCNPParser.cpp"
    break;

  case 100: // cell_fill: "*" "fill" assign cell_fill_data
#line 1063 "MCNPParser.bison"
 {
    auto angle_flag = interpreter.push_leaf(wasp::mcnpi::DECL, "angle_flag", (yystack_[3].value.token_index));
    auto n = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_FILL).data(), (yystack_[2].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), n);
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), angle_flag);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , "fill"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
 }
#line 1741 "MCNPParser.cpp"
    break;

  case 101: // cell_fill: "*" "fill" cell_fill_data
#line 1075 "MCNPParser.bison"
 {
    auto angle_flag = interpreter.push_leaf(wasp::mcnpi::DECL, "angle_flag", (yystack_[2].value.token_index));
    auto n = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_FILL).data(), (yystack_[1].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices) ->begin(), n);
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices) ->begin(), angle_flag);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , "fill"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
 }
#line 1756 "MCNPParser.cpp"
    break;

  case 102: // cell_transform_data: id
#line 1087 "MCNPParser.bison"
                          {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 1765 "MCNPParser.cpp"
    break;

  case 103: // cell_transform_data: lparen NUMBER NUMBER NUMBER rparen
#line 1091 "MCNPParser.bison"
                                      {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[4].value.node_index));
    auto ox = interpreter.push_leaf(wasp::mcnpi::DECL, "o", (yystack_[3].value.token_index));
    auto oy = interpreter.push_leaf(wasp::mcnpi::DECL, "o", (yystack_[2].value.token_index));
    auto oz = interpreter.push_leaf(wasp::mcnpi::DECL, "o", (yystack_[1].value.token_index));
    (yylhs.value.node_indices)->push_back(ox);
    (yylhs.value.node_indices)->push_back(oy);
    (yylhs.value.node_indices)->push_back(oz);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 1781 "MCNPParser.cpp"
    break;

  case 104: // cell_transform_data: lparen NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER rparen
#line 1105 "MCNPParser.bison"
                                    {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[13].value.node_index));
    auto ox = interpreter.push_leaf(wasp::mcnpi::ORIGIN, "o", (yystack_[12].value.token_index));
    auto oy = interpreter.push_leaf(wasp::mcnpi::ORIGIN, "o", (yystack_[11].value.token_index));
    auto oz = interpreter.push_leaf(wasp::mcnpi::ORIGIN, "o", (yystack_[10].value.token_index));

    auto xx = interpreter.push_leaf(wasp::mcnpi::ROTATION, "xx", (yystack_[9].value.token_index));
    auto yx = interpreter.push_leaf(wasp::mcnpi::ROTATION, "yx", (yystack_[8].value.token_index));
    auto zx = interpreter.push_leaf(wasp::mcnpi::ROTATION, "zx", (yystack_[7].value.token_index));

    auto xy = interpreter.push_leaf(wasp::mcnpi::ROTATION, "xy", (yystack_[6].value.token_index));
    auto yy = interpreter.push_leaf(wasp::mcnpi::ROTATION, "yy", (yystack_[5].value.token_index));
    auto zy = interpreter.push_leaf(wasp::mcnpi::ROTATION, "zy", (yystack_[4].value.token_index));

    auto xz = interpreter.push_leaf(wasp::mcnpi::ROTATION, "xz", (yystack_[3].value.token_index));
    auto yz = interpreter.push_leaf(wasp::mcnpi::ROTATION, "yz", (yystack_[2].value.token_index));
    auto zz = interpreter.push_leaf(wasp::mcnpi::ROTATION, "zz", (yystack_[1].value.token_index));

    (yylhs.value.node_indices)->push_back(ox);
    (yylhs.value.node_indices)->push_back(oy);
    (yylhs.value.node_indices)->push_back(oz);

    (yylhs.value.node_indices)->push_back(xx);
    (yylhs.value.node_indices)->push_back(yx);
    (yylhs.value.node_indices)->push_back(zx);

    (yylhs.value.node_indices)->push_back(xy);
    (yylhs.value.node_indices)->push_back(yy);
    (yylhs.value.node_indices)->push_back(zy);

    (yylhs.value.node_indices)->push_back(xz);
    (yylhs.value.node_indices)->push_back(yz);
    (yylhs.value.node_indices)->push_back(zz);

    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 1823 "MCNPParser.cpp"
    break;

  case 105: // cell_transform_data: lparen NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER "integer number" rparen
#line 1145 "MCNPParser.bison"
                                            {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[14].value.node_index));
    auto ox = interpreter.push_leaf(wasp::mcnpi::ORIGIN, "o", (yystack_[13].value.token_index));
    auto oy = interpreter.push_leaf(wasp::mcnpi::ORIGIN, "o", (yystack_[12].value.token_index));
    auto oz = interpreter.push_leaf(wasp::mcnpi::ORIGIN, "o", (yystack_[11].value.token_index));

    auto xx = interpreter.push_leaf(wasp::mcnpi::ROTATION, "xx", (yystack_[10].value.token_index));
    auto yx = interpreter.push_leaf(wasp::mcnpi::ROTATION, "yx", (yystack_[9].value.token_index));
    auto zx = interpreter.push_leaf(wasp::mcnpi::ROTATION, "zx", (yystack_[8].value.token_index));

    auto xy = interpreter.push_leaf(wasp::mcnpi::ROTATION, "xy", (yystack_[7].value.token_index));
    auto yy = interpreter.push_leaf(wasp::mcnpi::ROTATION, "yy", (yystack_[6].value.token_index));
    auto zy = interpreter.push_leaf(wasp::mcnpi::ROTATION, "zy", (yystack_[5].value.token_index));

    auto xz = interpreter.push_leaf(wasp::mcnpi::ROTATION, "xz", (yystack_[4].value.token_index));
    auto yz = interpreter.push_leaf(wasp::mcnpi::ROTATION, "yz", (yystack_[3].value.token_index));
    auto zz = interpreter.push_leaf(wasp::mcnpi::ROTATION, "zz", (yystack_[2].value.token_index));

    auto m = interpreter.push_leaf(wasp::mcnpi::FLAG, "m", (yystack_[1].value.token_index));

    (yylhs.value.node_indices)->push_back(ox);
    (yylhs.value.node_indices)->push_back(oy);
    (yylhs.value.node_indices)->push_back(oz);

    (yylhs.value.node_indices)->push_back(xx);
    (yylhs.value.node_indices)->push_back(yx);
    (yylhs.value.node_indices)->push_back(zx);

    (yylhs.value.node_indices)->push_back(xy);
    (yylhs.value.node_indices)->push_back(yy);
    (yylhs.value.node_indices)->push_back(zy);

    (yylhs.value.node_indices)->push_back(xz);
    (yylhs.value.node_indices)->push_back(yz);
    (yylhs.value.node_indices)->push_back(zz);
    (yylhs.value.node_indices)->push_back(m);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 1867 "MCNPParser.cpp"
    break;

  case 106: // cell_transformation: "cell transformation (TRCL)" assign cell_transform_data
#line 1187 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto d = (yystack_[0].value.node_indices);
    d->insert(d->begin(), (yystack_[1].value.node_index));
    d->insert(d->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::CELL_PARAM
                                ,"trcl"
                                ,*d);
    delete d;
 }
#line 1882 "MCNPParser.cpp"
    break;

  case 107: // cell_transformation: "*" "cell transformation (TRCL)" assign cell_transform_data
#line 1198 "MCNPParser.bison"
 {
    auto angle_flag = interpreter.push_leaf(wasp::mcnpi::DECL, "angle_flag", (yystack_[3].value.token_index));
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto d = (yystack_[0].value.node_indices);
    d->insert(d->begin(), (yystack_[1].value.node_index));
    d->insert(d->begin(), decl);
    d->insert(d->begin(), angle_flag);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::CELL_PARAM
                                ,"trcl"
                                ,*d);
    delete d;
}
#line 1899 "MCNPParser.cpp"
    break;

  case 108: // cell_transformation: "cell transformation (TRCL)" cell_transform_data
#line 1211 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto d = (yystack_[0].value.node_indices);
    d->insert(d->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::CELL_PARAM
                                ,"trcl"
                                ,*d);
    delete d;
 }
#line 1913 "MCNPParser.cpp"
    break;

  case 109: // cell_transformation: "*" "cell transformation (TRCL)" cell_transform_data
#line 1221 "MCNPParser.bison"
 {
    auto angle_flag = interpreter.push_leaf(wasp::mcnpi::DECL, "angle_flag", (yystack_[2].value.token_index));
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto d = (yystack_[0].value.node_indices);
    d->insert(d->begin(), decl);
    d->insert(d->begin(), angle_flag);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::CELL_PARAM
                                ,"trcl"
                                ,*d);
    delete d;
}
#line 1929 "MCNPParser.cpp"
    break;

  case 110: // cell_fill_list: number_paren_list
#line 1233 "MCNPParser.bison"
                 { (yylhs.value.node_indices) = (yystack_[0].value.node_indices); }
#line 1935 "MCNPParser.cpp"
    break;

  case 111: // cell_param_particle_based_key: "imp"
#line 1237 "MCNPParser.bison"
                                   {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_IMP).data(), (yystack_[0].value.token_index));}
#line 1941 "MCNPParser.cpp"
    break;

  case 112: // cell_param_particle_based_key: "ext"
#line 1238 "MCNPParser.bison"
    {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_EXT).data(), (yystack_[0].value.token_index));}
#line 1947 "MCNPParser.cpp"
    break;

  case 113: // cell_param_particle_based_key: "dxc"
#line 1240 "MCNPParser.bison"
    {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_DXC).data(), (yystack_[0].value.token_index));}
#line 1953 "MCNPParser.cpp"
    break;

  case 114: // cell_param_particle_based_key: "cell-based lower weight-window bounds (WWNi)"
#line 1242 "MCNPParser.bison"
    {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, "wwn", (yystack_[0].value.token_index));}
#line 1959 "MCNPParser.cpp"
    break;

  case 115: // cell_param_particle_based_key: "forced-collision card (FCL)"
#line 1243 "MCNPParser.bison"
    {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, "fcl", (yystack_[0].value.token_index));}
#line 1965 "MCNPParser.cpp"
    break;

  case 116: // cell_param_particle_based_key: "elpt"
#line 1244 "MCNPParser.bison"
     {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_ELPT).data(), (yystack_[0].value.token_index));}
#line 1971 "MCNPParser.cpp"
    break;

  case 117: // cell_param_particle_based_key: "unc"
#line 1245 "MCNPParser.bison"
    {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_UNC).data(), (yystack_[0].value.token_index));}
#line 1977 "MCNPParser.cpp"
    break;

  case 118: // colon: ":"
#line 1249 "MCNPParser.bison"
              {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::COLON, ":", (yystack_[0].value.token_index));}
#line 1983 "MCNPParser.cpp"
    break;

  case 119: // particle_symbol: "particle symbol"
#line 1252 "MCNPParser.bison"
                                  {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::PARTICLE_SYMBOL, "pt", (yystack_[0].value.token_index));}
#line 1989 "MCNPParser.cpp"
    break;

  case 120: // acell_param: cell_param_key assign value
#line 1256 "MCNPParser.bison"
{
    std::string key = interpreter.name((yystack_[2].value.node_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , key.data()
                                ,{(yystack_[2].value.node_index), (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
}
#line 2000 "MCNPParser.cpp"
    break;

  case 121: // acell_param: cell_param_key value
#line 1263 "MCNPParser.bison"
{
    std::string key = interpreter.name((yystack_[1].value.node_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , key.data()
                                ,{(yystack_[1].value.node_index), (yystack_[0].value.node_index)});
}
#line 2011 "MCNPParser.cpp"
    break;

  case 122: // acell_param: cell_param_key card_id assign value
#line 1271 "MCNPParser.bison"
{
    std::string key = interpreter.name((yystack_[3].value.node_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , key.data()
                                ,{(yystack_[3].value.node_index), (yystack_[2].value.node_index), (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
}
#line 2022 "MCNPParser.cpp"
    break;

  case 123: // acell_param: cell_param_particle_based_key colon particle_symbol_list assign value
#line 1278 "MCNPParser.bison"
{
    std::string key = interpreter.name((yystack_[4].value.node_index));
    auto m = (yystack_[2].value.node_indices);
    m->insert(m->begin(), (yystack_[3].value.node_index));
    m->insert(m->begin(), (yystack_[4].value.node_index));
    m->push_back((yystack_[1].value.node_index));
    m->push_back((yystack_[0].value.node_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , key.data()
                                ,*m);
    delete m;
}
#line 2039 "MCNPParser.cpp"
    break;

  case 124: // acell_param: cell_param_particle_based_key card_id colon particle_symbol_list assign value
#line 1291 "MCNPParser.bison"
{
    std::string key = interpreter.name((yystack_[5].value.node_index));
    auto m = (yystack_[2].value.node_indices);
    m->insert(m->begin(), (yystack_[3].value.node_index));
    m->insert(m->begin(), (yystack_[4].value.node_index));
    m->insert(m->begin(), (yystack_[5].value.node_index));
    m->push_back((yystack_[1].value.node_index));
    m->push_back((yystack_[0].value.node_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , key.data()
                                ,*m);
    delete m;
}
#line 2057 "MCNPParser.cpp"
    break;

  case 125: // acell_param: cell_param_particle_based_key colon particle_symbol_list value
#line 1305 "MCNPParser.bison"
{
    std::string key = interpreter.name((yystack_[3].value.node_index));
    auto m = (yystack_[1].value.node_indices);
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), (yystack_[3].value.node_index));
    m->push_back((yystack_[0].value.node_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , key.data()
                                ,*m);
    delete m;
}
#line 2073 "MCNPParser.cpp"
    break;

  case 126: // acell_param: cell_param_particle_based_key card_id colon particle_symbol_list value
#line 1317 "MCNPParser.bison"
{
    std::string key = interpreter.name((yystack_[4].value.node_index));
    auto m = (yystack_[1].value.node_indices);
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), (yystack_[3].value.node_index));
    m->insert(m->begin(), (yystack_[4].value.node_index));
    m->push_back((yystack_[0].value.node_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , key.data()
                                ,*m);
    delete m;
}
#line 2090 "MCNPParser.cpp"
    break;

  case 127: // acell_param: cell_fill
#line 1329 "MCNPParser.bison"
  { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 2096 "MCNPParser.cpp"
    break;

  case 128: // acell_param: cell_transformation
#line 1330 "MCNPParser.bison"
  { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 2102 "MCNPParser.cpp"
    break;

  case 129: // like_but_cell_param: acell_param
#line 1332 "MCNPParser.bison"
                      { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 2108 "MCNPParser.cpp"
    break;

  case 130: // like_but_cell_param: "mat" assign value
#line 1334 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , "mat"
                                ,{decl, (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
}
#line 2119 "MCNPParser.cpp"
    break;

  case 131: // like_but_cell_param: "mat" value
#line 1340 "MCNPParser.bison"
            {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , "mat"
                                ,{decl, (yystack_[0].value.node_index)});
}
#line 2130 "MCNPParser.cpp"
    break;

  case 132: // like_but_cell_param: "rho" assign value
#line 1346 "MCNPParser.bison"
                   {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , "rho"
                                ,{decl, (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
}
#line 2141 "MCNPParser.cpp"
    break;

  case 133: // like_but_cell_param: "rho" value
#line 1352 "MCNPParser.bison"
             {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , "rho"
                                ,{decl, (yystack_[0].value.node_index)});
}
#line 2152 "MCNPParser.cpp"
    break;

  case 134: // cell_params: acell_param
#line 1361 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 2161 "MCNPParser.cpp"
    break;

  case 135: // cell_params: cell_params acell_param
#line 1366 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 2170 "MCNPParser.cpp"
    break;

  case 136: // like_but_cell_params: like_but_cell_param
#line 1373 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 2179 "MCNPParser.cpp"
    break;

  case 137: // like_but_cell_params: like_but_cell_params like_but_cell_param
#line 1378 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 2188 "MCNPParser.cpp"
    break;

  case 138: // cell_param: cell_params
#line 1384 "MCNPParser.bison"
{
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::CELL_PARAMS
                                ,"params"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 2199 "MCNPParser.cpp"
    break;

  case 139: // like_but_cell_param_object: like_but_cell_params
#line 1392 "MCNPParser.bison"
{
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::CELL_PARAMS
                                ,"params"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 2210 "MCNPParser.cpp"
    break;

  case 140: // cell_card: cell_id material_id density cell_geom
#line 1400 "MCNPParser.bison"
{
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::CELL_CARD
                                ,"cell"
                                ,{(yystack_[3].value.node_index), (yystack_[2].value.node_index), (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
}
#line 2220 "MCNPParser.cpp"
    break;

  case 141: // cell_card: cell_id material_id density cell_geom cell_param
#line 1406 "MCNPParser.bison"
{
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::CELL_CARD
                                ,"cell"
                                ,{(yystack_[4].value.node_index), (yystack_[3].value.node_index), (yystack_[2].value.node_index), (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
}
#line 2230 "MCNPParser.cpp"
    break;

  case 142: // cell_card: cell_id material_id cell_geom
#line 1412 "MCNPParser.bison"
{
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::CELL_CARD
                                ,"cell"
                                ,{(yystack_[2].value.node_index), (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
}
#line 2240 "MCNPParser.cpp"
    break;

  case 143: // cell_card: cell_id material_id cell_geom cell_param
#line 1418 "MCNPParser.bison"
{
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::CELL_CARD
                                ,"cell"
                                ,{(yystack_[3].value.node_index), (yystack_[2].value.node_index), (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
}
#line 2250 "MCNPParser.cpp"
    break;

  case 144: // cell_card: cell_id "like" "integer number" "but" like_but_cell_param_object
#line 1424 "MCNPParser.bison"
{
    auto l = interpreter.push_leaf(wasp::mcnpi::FLAG, "decl", (yystack_[3].value.token_index));
    auto cid = interpreter.push_leaf(wasp::mcnpi::CELL_ID, "cell", (yystack_[2].value.token_index));
    auto b = interpreter.push_leaf(wasp::mcnpi::FLAG, "but", (yystack_[1].value.token_index));
    auto lp = interpreter.push_parent(wasp::mcnpi::CELL_CARD
                                ,"like"
                                ,{l, cid, b, (yystack_[0].value.node_index)});
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::CELL_CARD
                                ,"cell"
                                ,{(yystack_[4].value.node_index), lp});

}
#line 2267 "MCNPParser.cpp"
    break;

  case 145: // cell_block: cell_card
#line 1438 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 2276 "MCNPParser.cpp"
    break;

  case 146: // cell_block: cell_block cell_card
#line 1443 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 2285 "MCNPParser.cpp"
    break;

  case 147: // PARCONE: "k/x"
#line 1448 "MCNPParser.bison"
          { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 2291 "MCNPParser.cpp"
    break;

  case 148: // PARCONE: "k/y"
#line 1448 "MCNPParser.bison"
                     { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 2297 "MCNPParser.cpp"
    break;

  case 149: // PARCONE: "k/z"
#line 1448 "MCNPParser.bison"
                                { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 2303 "MCNPParser.cpp"
    break;

  case 150: // TORUS: "tx"
#line 1450 "MCNPParser.bison"
        { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 2309 "MCNPParser.cpp"
    break;

  case 151: // TORUS: "ty"
#line 1450 "MCNPParser.bison"
                 { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 2315 "MCNPParser.cpp"
    break;

  case 152: // TORUS: "tz"
#line 1450 "MCNPParser.bison"
                          { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 2321 "MCNPParser.cpp"
    break;

  case 153: // surface_id: "surface_id"
#line 1452 "MCNPParser.bison"
                       {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::ID, "id", (yystack_[0].value.token_index));}
#line 2327 "MCNPParser.cpp"
    break;

  case 154: // surface_geom: "p" number_token_list
#line 1457 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::vector<size_t> children = {decl};
    auto t = (yystack_[0].value.token_indices);
    std::string name;
    // is it plane points?
    auto tt = wasp::mcnpi::VALUE;
    if (t->size() >= 9)
    {
        name = "plpts";
        assign_names(interpreter, {"xp","yp","zp","xp","yp","zp","xp","yp","zp"},
                                  {tt,tt,tt,tt,tt,tt,tt,tt,tt}, t, children);
    }
    else
    {
        name = "pl";
        assign_names(interpreter, {"a","b","c","d"},
                                  {tt,tt,tt,tt}, t, children);
    }
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::PLANE,name.data(),children);
    delete t;
 }
#line 2354 "MCNPParser.cpp"
    break;

  case 155: // surface_geom: "px" NUMBER
#line 1480 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto d = interpreter.push_leaf(wasp::mcnpi::VALUE, "d", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::XPLANE
                                ,"xpl"
                                ,{decl, d});
 }
#line 2366 "MCNPParser.cpp"
    break;

  case 156: // surface_geom: "py" NUMBER
#line 1488 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto d = interpreter.push_leaf(wasp::mcnpi::VALUE, "d", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::YPLANE
                                ,"ypl"
                                ,{decl, d});
 }
#line 2378 "MCNPParser.cpp"
    break;

  case 157: // surface_geom: "pz" NUMBER
#line 1496 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto d = interpreter.push_leaf(wasp::mcnpi::VALUE, "d", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::ZPLANE
                                ,"zpl"
                                ,{decl, d});
 }
#line 2390 "MCNPParser.cpp"
    break;

  case 158: // surface_geom: "so" NUMBER
#line 1504 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto r = interpreter.push_leaf(wasp::mcnpi::VALUE, "r", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::ORIGIN_SPHERE
                                ,"sph"
                                ,{decl, r});
 }
#line 2402 "MCNPParser.cpp"
    break;

  case 159: // surface_geom: "sph" number_token_list
#line 1512 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"xbar", "ybar", "zbar","r"},
                                  {tt,tt,tt,tt}, t, children);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::SPHERE
                                ,"sph"
                                ,children);
    delete t;
 }
#line 2420 "MCNPParser.cpp"
    break;

  case 160: // surface_geom: "sx" NUMBER NUMBER
#line 1526 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto x = interpreter.push_leaf(wasp::mcnpi::VALUE, "xbar", (yystack_[1].value.token_index));
    auto r = interpreter.push_leaf(wasp::mcnpi::VALUE, "r", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::SPHERE
                                ,"sph"
                                ,{decl, x, r});
 }
#line 2433 "MCNPParser.cpp"
    break;

  case 161: // surface_geom: "sy" NUMBER NUMBER
#line 1535 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto y = interpreter.push_leaf(wasp::mcnpi::VALUE, "ybar", (yystack_[1].value.token_index));
    auto r = interpreter.push_leaf(wasp::mcnpi::VALUE, "r", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::SPHERE
                                ,"sph"
                                ,{decl, y, r});
 }
#line 2446 "MCNPParser.cpp"
    break;

  case 162: // surface_geom: "sz" NUMBER NUMBER
#line 1544 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto z = interpreter.push_leaf(wasp::mcnpi::VALUE, "zbar", (yystack_[1].value.token_index));
    auto r = interpreter.push_leaf(wasp::mcnpi::VALUE, "r", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::SPHERE
                                ,"sph"
                                ,{decl, z, r});
 }
#line 2459 "MCNPParser.cpp"
    break;

  case 163: // surface_geom: "c/x" number_token_list
#line 1553 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"ybar", "zbar","r"},
                                  {tt,tt,tt}, t, children);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::CYLINDER
                                ,"cyl"
                                ,children);
    delete t;
 }
#line 2477 "MCNPParser.cpp"
    break;

  case 164: // surface_geom: "c/y" number_token_list
#line 1567 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"xbar", "zbar","r"},
                                  {tt,tt,tt}, t, children);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::CYLINDER
                                ,"cyl"
                                ,children);
    delete t;
 }
#line 2495 "MCNPParser.cpp"
    break;

  case 165: // surface_geom: "c/z" number_token_list
#line 1581 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"xbar", "ybar","r"},
                                  {tt,tt,tt}, t, children);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::CYLINDER
                                ,"cyl"
                                ,children);
    delete t;
 }
#line 2513 "MCNPParser.cpp"
    break;

  case 166: // surface_geom: "cx" NUMBER
#line 1595 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto r = interpreter.push_leaf(wasp::mcnpi::VALUE, "r", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::XCYLINDER
                                ,"xcyl"
                                ,{decl, r});
 }
#line 2525 "MCNPParser.cpp"
    break;

  case 167: // surface_geom: "cy" NUMBER
#line 1603 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto r = interpreter.push_leaf(wasp::mcnpi::VALUE, "r", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::YCYLINDER
                                ,"ycyl"
                                ,{decl, r});
 }
#line 2537 "MCNPParser.cpp"
    break;

  case 168: // surface_geom: "cz" NUMBER
#line 1611 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto r = interpreter.push_leaf(wasp::mcnpi::VALUE, "r", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::ZCYLINDER
                                ,"zcyl"
                                ,{decl, r});
 }
#line 2549 "MCNPParser.cpp"
    break;

  case 169: // surface_geom: PARCONE number_token_list
#line 1619 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"xbar", "ybar", "zbar","t", "sense"},
                                  {tt,tt,tt,tt,tt}, t, children);
    wasp_check(std::string(interpreter.token_data((yystack_[1].value.token_index))).find_first_of("xyzXYZ") != std::string::npos);
    std::string name(1,interpreter.token_data((yystack_[1].value.token_index))[2]); // obtain axis from 'k/a' where a is x,y,z
    name.append("pcone");
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    wasp::mcnpi::NODE type = wasp::mcnpi::XPARCONE;
    if (name.at(0) == 'y') type = wasp::mcnpi::YPARCONE;
    else if (name.at(0) == 'z') type = wasp::mcnpi::ZPARCONE;

    (yylhs.value.node_index) = interpreter.push_parent(type, name.data(), children);
    delete t;
 }
#line 2573 "MCNPParser.cpp"
    break;

  case 170: // surface_geom: "kx" number_token_list
#line 1639 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"xbar", "t", "sense"},
                                  {tt,tt,tt}, t, children);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::XCONE, "xcone", children);
    delete t;
 }
#line 2588 "MCNPParser.cpp"
    break;

  case 171: // surface_geom: "ky" number_token_list
#line 1650 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"ybar", "t", "sense"},
                                  {tt,tt,tt}, t, children);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::YCONE, "ycone", children);
    delete t;
 }
#line 2603 "MCNPParser.cpp"
    break;

  case 172: // surface_geom: "kz" number_token_list
#line 1661 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"zbar", "t", "sense"},
                                  {tt,tt,tt}, t, children);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::ZCONE, "zcone",children);
    delete t;
 }
#line 2618 "MCNPParser.cpp"
    break;

  case 173: // surface_geom: TORUS number_token_list
#line 1672 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"xbar", "ybar", "zbar", "a", "b", "c"},
                                  {tt,tt,tt,tt,tt, tt}, t, children);
    wasp_check(std::string(interpreter.token_data((yystack_[1].value.token_index))).find_first_of("xyzXYZ") != std::string::npos);
    std::string name(1,interpreter.token_data((yystack_[1].value.token_index))[1]); // obtain axis from 'ta' where a is x,y,z
    name.append("torus");
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    wasp::mcnpi::NODE type = wasp::mcnpi::XTORUS;
    if (name.at(0) == 'y') type = wasp::mcnpi::YTORUS;
    else if (name.at(0) == 'z') type = wasp::mcnpi::ZTORUS;

    (yylhs.value.node_index) = interpreter.push_parent(type, name.data(), children);
    delete t;
 }
#line 2642 "MCNPParser.cpp"
    break;

  case 174: // surface_geom: "sq" number_token_list
#line 1692 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"a","b","c","d","e","f","g","xbar","ybar","zbar"},
                                  {tt,tt,tt,tt,tt,tt,tt,tt,tt,tt}, t, children);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::SQUADRIC
                                ,"sq"
                                ,children);
    delete t;
 }
#line 2660 "MCNPParser.cpp"
    break;

  case 175: // surface_geom: "gq" number_token_list
#line 1706 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"a","b","c","d","e","f","g","h","j","k"},
                               {tt, tt, tt, tt, tt, tt, tt, tt, tt,tt}, t, children);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::GQUADRIC
                                ,"gq"
                                ,children);
    delete t;
 }
#line 2678 "MCNPParser.cpp"
    break;

  case 176: // surface_geom: "x" number_token_list
#line 1720 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"xp","rp","xp","rp","xp","rp"},
                               {tt, tt, tt, tt, tt, tt}, t, children);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::XPOINTS
                                ,"xpts"
                                ,children);
    delete t;
 }
#line 2696 "MCNPParser.cpp"
    break;

  case 177: // surface_geom: "y" number_token_list
#line 1734 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"yp","rp","yp","rp","yp","rp"},
                               {tt, tt, tt, tt, tt, tt}, t, children);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::YPOINTS
                                ,"ypts"
                                ,children);
    delete t;
 }
#line 2714 "MCNPParser.cpp"
    break;

  case 178: // surface_geom: "z" number_token_list
#line 1748 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"zp","rp","zp","rp","zp","rp"},
                               {tt, tt, tt, tt, tt, tt}, t, children);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::ZPOINTS
                                ,"zpts"
                                ,children);
    delete t;
 }
#line 2732 "MCNPParser.cpp"
    break;

  case 179: // surface_geom: "box" number_token_list
#line 1762 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"vx","vy","vz","ax","ay","az","ax","ay","az","ax","ay","az"},
                                  {tt,tt,tt,tt,tt,tt,tt,tt,tt,tt,tt,tt}, t, children);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::BOX
                                ,"box"
                                ,children);
    delete t;
 }
#line 2750 "MCNPParser.cpp"
    break;

  case 180: // surface_geom: "rpp" number_token_list
#line 1776 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"xmin","xmax","ymin","ymax","zmin","zmax"},
                                  {tt,tt,tt,tt,tt,tt}, t, children);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::RECTANGULAR_PPIPED
                                ,"rppiped"
                                ,children);
    delete t;
 }
#line 2768 "MCNPParser.cpp"
    break;

  case 181: // surface_geom: "rcc" number_token_list
#line 1790 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"vx","vy","vz","hx","hy","hz","r"},
                                  {tt,tt,tt,tt,tt,tt, tt}, t, children);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::RIGHT_CIRCULAR_CYLINDER
                                ,"rrectcyl"
                                ,children);
    delete t;
 }
#line 2786 "MCNPParser.cpp"
    break;

  case 182: // surface_geom: "rec" number_token_list
#line 1804 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    if (t->size() >= 12)
    {
    assign_names(interpreter, {"vx","vy","vz","hx","hy","hz","vx1","vy1","vz1","vx2","vy2","vz2"},
                                  {tt,tt,tt,tt,tt,tt,tt,tt,tt,tt,tt,tt}, t, children);
    }
    else
    {
        assign_names(interpreter, {"vx","vy","vz","hx","hy","hz","vx1","vy1","vz1", "r"},
                                  {tt,tt,tt,tt,tt,tt,tt,tt,tt,tt}, t, children);
    }

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::RIGHT_ELLIPTICAL_CYLINDER
                                ,"relipcyl"
                                ,children);
    delete t;
}
#line 2812 "MCNPParser.cpp"
    break;

  case 183: // surface_geom: "hex" number_token_list
#line 1826 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"vx","vy","vz","hx","hy","hz","rx","ry","rz","sx","sy","sz","tx","ty","tz"},
                              {tt,tt,tt,tt,tt,tt,tt,tt,tt,tt,tt,tt,tt,tt,tt}, t, children);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::HEX
                                ,"hex"
                                ,children);
    delete t;
 }
#line 2830 "MCNPParser.cpp"
    break;

  case 184: // surface_geom: "trc" number_token_list
#line 1840 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"vx","vy","vz","hx","hy","hz","r1","r2"},
                              {tt,tt,tt,tt,tt,tt,tt,tt}, t, children);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::CONE
                                ,"cone"
                                ,children);
    delete t;
 }
#line 2848 "MCNPParser.cpp"
    break;

  case 185: // surface_geom: "ell" number_token_list
#line 1854 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"vx","vy","vz","vx","vy","vz","rm"},
                              {tt,tt,tt,tt,tt,tt,tt}, t, children);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::ELLIPSOID
                                ,"ellipsoid"
                                ,children);
    delete t;
 }
#line 2866 "MCNPParser.cpp"
    break;

  case 186: // surface_geom: "wed" number_token_list
#line 1868 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;

    assign_names(interpreter, {"vx","vy", "vz", "vx1","vy1","vz1","vx2","vy2","vz2","vx3","vy3","vz3"},
                                  {tt,tt,tt,tt,tt,tt,tt,tt,tt,tt,tt,tt,}, t, children);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::WEDGE
                                ,"wedge"
                                ,children);
    delete t;
 }
#line 2885 "MCNPParser.cpp"
    break;

  case 187: // surface_geom: "arb" number_token_list
#line 1883 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"ax","ay","az","bx","by","bz",
                               "cx","cy","cz","dx","dy","dz",
                               "ex","ey","ez","fx","fy","fz",
                               "gx","gy","gz","hx","hy","hz",
                               "n","n","n","n","n","n",},
                              {tt,tt,tt,tt,tt,tt,
                              tt,tt,tt,tt,tt,tt,
                              tt,tt,tt,tt,tt,tt,
                              tt,tt,tt,tt,tt,tt,
                              tt,tt,tt,tt,tt,tt}, t, children);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::ARBITRARY_POLYHEDRON
                                ,"arbpoly"
                                ,children);
    delete t;
 }
#line 2911 "MCNPParser.cpp"
    break;

  case 188: // surface_card: surface_id surface_geom
#line 1906 "MCNPParser.bison"
 {
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::SURFACE_CARD
                                ,"surface"
                                ,{(yystack_[1].value.node_index), (yystack_[0].value.node_index)});
 }
#line 2921 "MCNPParser.cpp"
    break;

  case 189: // surface_card: surface_id "integer number" surface_geom
#line 1912 "MCNPParser.bison"
 {
    int n = std::stoi(interpreter.token_data((yystack_[1].value.token_index)));

    size_t ni = 0;
    if (n < 0)
    {
        ni = interpreter.push_leaf(wasp::mcnpi::SURFACE_ID, "periodic", (yystack_[1].value.token_index));
    }
    else
    {
        ni = interpreter.push_leaf(wasp::mcnpi::TRANSFORM_ID, "transform", (yystack_[1].value.token_index));
    }
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::SURFACE_CARD
                                ,"surface"
                                ,{(yystack_[2].value.node_index), ni, (yystack_[0].value.node_index)});
 }
#line 2942 "MCNPParser.cpp"
    break;

  case 190: // surface_block: surface_card
#line 1930 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 2951 "MCNPParser.cpp"
    break;

  case 191: // surface_block: surface_block surface_card
#line 1935 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 2960 "MCNPParser.cpp"
    break;

  case 192: // array_jump: "array jump (nJ)"
#line 1941 "MCNPParser.bison"
                         {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::ARRAY_JUMP, "jump", (yystack_[0].value.token_index));}
#line 2966 "MCNPParser.cpp"
    break;

  case 193: // array_multiply: "array multiply (xM)"
#line 1943 "MCNPParser.bison"
                                 {(yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::ARRAY_MULTIPLY, "multiply", (yystack_[0].value.token_index));}
#line 2972 "MCNPParser.cpp"
    break;

  case 194: // array_repeat: "array repeat (nR)"
#line 1947 "MCNPParser.bison"
 {
    (yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::ARRAY_REPEAT, "repeat", (yystack_[0].value.token_index));
 }
#line 2980 "MCNPParser.cpp"
    break;

  case 195: // array_log_interp: "array logarithmic interpolation (nLOG)"
#line 1952 "MCNPParser.bison"
 {
    (yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::ARRAY_LOG_INTERP, "log", (yystack_[0].value.token_index));
 }
#line 2988 "MCNPParser.cpp"
    break;

  case 196: // array_lin_interp: "array linear interpoloation (nI)"
#line 1957 "MCNPParser.bison"
 {
    (yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::ARRAY_LIN_INTERP, "linear", (yystack_[0].value.token_index));
 }
#line 2996 "MCNPParser.cpp"
    break;

  case 197: // number_list: number_or_sc
#line 1963 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 3005 "MCNPParser.cpp"
    break;

  case 198: // number_list: number_list number_or_sc
#line 1968 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 3014 "MCNPParser.cpp"
    break;

  case 199: // number_paren_list: number_or_sc
#line 1976 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 3023 "MCNPParser.cpp"
    break;

  case 200: // number_paren_list: number_paren_list number_or_sc
#line 1981 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 3032 "MCNPParser.cpp"
    break;

  case 201: // number_paren_list: lparen number_paren_list rparen
#line 1986 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 3042 "MCNPParser.cpp"
    break;

  case 202: // number_paren_list: number_paren_list lparen number_paren_list rparen
#line 1992 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[3].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[2].value.node_index));
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->end(), (yystack_[1].value.node_indices)->begin(), (yystack_[1].value.node_indices)->end());
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
    delete (yystack_[1].value.node_indices);
 }
#line 3054 "MCNPParser.cpp"
    break;

  case 203: // number_paren_colon_list: number_or_sc
#line 2001 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 3063 "MCNPParser.cpp"
    break;

  case 204: // number_paren_colon_list: number_paren_colon_list number_or_sc
#line 2006 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 3072 "MCNPParser.cpp"
    break;

  case 205: // number_paren_colon_list: lparen number_paren_colon_list rparen
#line 2011 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 3082 "MCNPParser.cpp"
    break;

  case 206: // number_paren_colon_list: number_paren_colon_list lparen number_paren_colon_list rparen
#line 2017 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[3].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[2].value.node_index));
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->end(), (yystack_[1].value.node_indices)->begin(), (yystack_[1].value.node_indices)->end());
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
    delete (yystack_[1].value.node_indices);
 }
#line 3094 "MCNPParser.cpp"
    break;

  case 207: // number_paren_colon_list: number_paren_colon_list colon number_paren_colon_list
#line 2025 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[2].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->end(), (yystack_[0].value.node_indices)->begin(), (yystack_[0].value.node_indices)->end());
    delete (yystack_[0].value.node_indices);
 }
#line 3105 "MCNPParser.cpp"
    break;

  case 208: // value_or_sc: value
#line 2032 "MCNPParser.bison"
              { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 3111 "MCNPParser.cpp"
    break;

  case 209: // value_or_sc: array_jump
#line 2032 "MCNPParser.bison"
                      { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 3117 "MCNPParser.cpp"
    break;

  case 210: // value_or_sc: array_lin_interp
#line 2032 "MCNPParser.bison"
                                   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 3123 "MCNPParser.cpp"
    break;

  case 211: // value_or_sc: array_log_interp
#line 2032 "MCNPParser.bison"
                                                      { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 3129 "MCNPParser.cpp"
    break;

  case 212: // value_or_sc: array_multiply
#line 2032 "MCNPParser.bison"
                                                                         { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 3135 "MCNPParser.cpp"
    break;

  case 213: // value_or_sc: array_repeat
#line 2032 "MCNPParser.bison"
                                                                                          { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 3141 "MCNPParser.cpp"
    break;

  case 214: // value_paren_colon_list: value_or_sc
#line 2035 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 3150 "MCNPParser.cpp"
    break;

  case 215: // value_paren_colon_list: value_paren_colon_list value_or_sc
#line 2040 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 3159 "MCNPParser.cpp"
    break;

  case 216: // value_paren_colon_list: lparen value_paren_colon_list rparen
#line 2045 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 3169 "MCNPParser.cpp"
    break;

  case 217: // value_paren_colon_list: value_paren_colon_list lparen value_paren_colon_list rparen
#line 2051 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[3].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[2].value.node_index));
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->end(), (yystack_[1].value.node_indices)->begin(), (yystack_[1].value.node_indices)->end());
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
    delete (yystack_[1].value.node_indices);
 }
#line 3181 "MCNPParser.cpp"
    break;

  case 218: // value_paren_colon_list: value_paren_colon_list colon value_paren_colon_list
#line 2059 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[2].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->end(), (yystack_[0].value.node_indices)->begin(), (yystack_[0].value.node_indices)->end());
    delete (yystack_[0].value.node_indices);
 }
#line 3192 "MCNPParser.cpp"
    break;

  case 219: // value_paren_colon_list: value_paren_colon_list lt value_or_sc
#line 2066 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[2].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 3202 "MCNPParser.cpp"
    break;

  case 220: // value_paren_colon_list: value_paren_colon_list lt lparen value_paren_colon_list rparen
#line 2072 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[4].value.node_indices);
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->end(), (yystack_[3].value.node_index));
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->end(), (yystack_[2].value.node_index));
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->end(), (yystack_[1].value.node_indices)->begin(), (yystack_[1].value.node_indices)->end());
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
    delete (yystack_[1].value.node_indices);
}
#line 3215 "MCNPParser.cpp"
    break;

  case 221: // value_paren_colon_list: value_paren_colon_list lbracket value_paren_colon_list rbracket
#line 2081 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[3].value.node_indices);
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->end(), (yystack_[2].value.node_index));
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->end(), (yystack_[1].value.node_indices)->begin(), (yystack_[1].value.node_indices)->end());
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
    delete (yystack_[1].value.node_indices);
}
#line 3227 "MCNPParser.cpp"
    break;

  case 222: // value_paren_colon_list: "string" assign value
#line 2089 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t>();
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto kv = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE, name.data(), {decl, (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
    (yylhs.value.node_indices)->push_back(kv);
}
#line 3241 "MCNPParser.cpp"
    break;

  case 223: // number_or_sc: number
#line 2099 "MCNPParser.bison"
               { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 3247 "MCNPParser.cpp"
    break;

  case 224: // number_or_sc: array_jump
#line 2099 "MCNPParser.bison"
                        { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 3253 "MCNPParser.cpp"
    break;

  case 225: // number_or_sc: array_lin_interp
#line 2099 "MCNPParser.bison"
                                     { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 3259 "MCNPParser.cpp"
    break;

  case 226: // number_or_sc: array_log_interp
#line 2099 "MCNPParser.bison"
                                                        { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 3265 "MCNPParser.cpp"
    break;

  case 227: // number_or_sc: array_multiply
#line 2099 "MCNPParser.bison"
                                                                           { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 3271 "MCNPParser.cpp"
    break;

  case 228: // number_or_sc: array_repeat
#line 2099 "MCNPParser.bison"
                                                                                            { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 3277 "MCNPParser.cpp"
    break;

  case 229: // NUMBER_LIST_TOKEN: NUMBER
#line 2101 "MCNPParser.bison"
                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3283 "MCNPParser.cpp"
    break;

  case 230: // NUMBER_LIST_TOKEN: "array jump (nJ)"
#line 2101 "MCNPParser.bison"
                             { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3289 "MCNPParser.cpp"
    break;

  case 231: // NUMBER_LIST_TOKEN: "array repeat (nR)"
#line 2101 "MCNPParser.bison"
                                          { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3295 "MCNPParser.cpp"
    break;

  case 232: // NUMBER_LIST_TOKEN: "array multiply (xM)"
#line 2101 "MCNPParser.bison"
                                                         { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3301 "MCNPParser.cpp"
    break;

  case 233: // NUMBER_LIST_TOKEN: "array linear interpoloation (nI)"
#line 2102 "MCNPParser.bison"
                     { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3307 "MCNPParser.cpp"
    break;

  case 234: // NUMBER_LIST_TOKEN: "array logarithmic interpolation (nLOG)"
#line 2102 "MCNPParser.bison"
                                        { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3313 "MCNPParser.cpp"
    break;

  case 235: // NUMBER_LIST_TOKEN: "Vn"
#line 2102 "MCNPParser.bison"
                                                           { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3319 "MCNPParser.cpp"
    break;

  case 236: // VALUE_LIST_TOKEN: NUMBER_LIST_TOKEN
#line 2104 "MCNPParser.bison"
                   { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3325 "MCNPParser.cpp"
    break;

  case 237: // VALUE_LIST_TOKEN: "value"
#line 2104 "MCNPParser.bison"
                                       { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3331 "MCNPParser.cpp"
    break;

  case 238: // VALUE_LIST_TOKEN: "string"
#line 2104 "MCNPParser.bison"
                                               { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3337 "MCNPParser.cpp"
    break;

  case 239: // VALUE_LIST_TOKEN: "library id (.?[0-9]{2}[tcdmgpuyehporsa])"
#line 2104 "MCNPParser.bison"
                                                        { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3343 "MCNPParser.cpp"
    break;

  case 240: // VALUE_LIST_TOKEN: "no"
#line 2104 "MCNPParser.bison"
                                                                 { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3349 "MCNPParser.cpp"
    break;

  case 241: // VALUE_LIST_TOKEN: ":"
#line 2104 "MCNPParser.bison"
                                                                      { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3355 "MCNPParser.cpp"
    break;

  case 242: // VALUE_COMMA_LIST_TOKEN: VALUE_LIST_TOKEN
#line 2105 "MCNPParser.bison"
                         { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3361 "MCNPParser.cpp"
    break;

  case 243: // VALUE_COMMA_LIST_TOKEN: ","
#line 2105 "MCNPParser.bison"
                                            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3367 "MCNPParser.cpp"
    break;

  case 244: // value_token_list: VALUE_LIST_TOKEN
#line 2108 "MCNPParser.bison"
 {
    (yylhs.value.token_indices) = new std::vector<size_t>();
    (yylhs.value.token_indices)->push_back((yystack_[0].value.token_index));
 }
#line 3376 "MCNPParser.cpp"
    break;

  case 245: // value_token_list: value_token_list VALUE_LIST_TOKEN
#line 2113 "MCNPParser.bison"
 {
    (yylhs.value.token_indices) = (yystack_[1].value.token_indices);
    (yylhs.value.token_indices)->push_back((yystack_[0].value.token_index));
 }
#line 3385 "MCNPParser.cpp"
    break;

  case 246: // number_token_list: NUMBER_LIST_TOKEN
#line 2120 "MCNPParser.bison"
 {
    (yylhs.value.token_indices) = new std::vector<size_t>();
    (yylhs.value.token_indices)->push_back((yystack_[0].value.token_index));
 }
#line 3394 "MCNPParser.cpp"
    break;

  case 247: // number_token_list: number_token_list NUMBER_LIST_TOKEN
#line 2125 "MCNPParser.bison"
 {
    (yylhs.value.token_indices) = (yystack_[1].value.token_indices);
    (yylhs.value.token_indices)->push_back((yystack_[0].value.token_index));
 }
#line 3403 "MCNPParser.cpp"
    break;

  case 248: // value_list_item: VALUE_LIST_TOKEN
#line 2131 "MCNPParser.bison"
{
    (yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::VALUE,"value",(yystack_[0].value.token_index));
}
#line 3411 "MCNPParser.cpp"
    break;

  case 249: // value_comma_list_item: VALUE_COMMA_LIST_TOKEN
#line 2136 "MCNPParser.bison"
{
    (yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::VALUE,"value",(yystack_[0].value.token_index));
}
#line 3419 "MCNPParser.cpp"
    break;

  case 250: // value_list: value_list_item
#line 2141 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 3428 "MCNPParser.cpp"
    break;

  case 251: // value_list: value_list value_list_item
#line 2146 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 3437 "MCNPParser.cpp"
    break;

  case 252: // value_comma_list: value_comma_list_item
#line 2152 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 3446 "MCNPParser.cpp"
    break;

  case 253: // value_comma_list: value_comma_list value_comma_list_item
#line 2157 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 3455 "MCNPParser.cpp"
    break;

  case 254: // string_list: "string"
#line 2163 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    auto s = interpreter.push_leaf(wasp::mcnpi::VALUE,"value",(yystack_[0].value.token_index));
    (yylhs.value.node_indices)->push_back(s);
 }
#line 3465 "MCNPParser.cpp"
    break;

  case 255: // string_list: string_list "string"
#line 2169 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    auto s = interpreter.push_leaf(wasp::mcnpi::VALUE,"value",(yystack_[0].value.token_index));
    (yylhs.value.node_indices)->push_back(s);
 }
#line 3475 "MCNPParser.cpp"
    break;

  case 256: // data_vol_card: "vol" number_list
#line 2176 "MCNPParser.bison"
                                {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(),decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"vol"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
 }
#line 3488 "MCNPParser.cpp"
    break;

  case 257: // data_vol_card: "vol" "no" number_list
#line 2184 "MCNPParser.bison"
                      {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto n = interpreter.push_leaf(wasp::mcnpi::NO, "no", (yystack_[1].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(),n);
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(),decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"vol"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
 }
#line 3503 "MCNPParser.cpp"
    break;

  case 258: // data_vol_card: "vol" "no"
#line 2194 "MCNPParser.bison"
          {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto n = interpreter.push_leaf(wasp::mcnpi::NO, "no", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"vol"
                                ,{decl, n});

 }
#line 3516 "MCNPParser.cpp"
    break;

  case 259: // data_area_card: "area" number_list
#line 2204 "MCNPParser.bison"
                                  {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(),decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"area"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 3529 "MCNPParser.cpp"
    break;

  case 260: // data_transform_card: "transformation (TRn)" card_id number_list
#line 2215 "MCNPParser.bison"
                                                         {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"trcl"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 3543 "MCNPParser.cpp"
    break;

  case 261: // data_transform_card: "*" "transformation (TRn)" card_id number_list
#line 2224 "MCNPParser.bison"
                                              {
    auto angle_flag = interpreter.push_leaf(wasp::mcnpi::DECL, "angle_flag", (yystack_[3].value.token_index));
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), angle_flag);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"trcl"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 3559 "MCNPParser.cpp"
    break;

  case 262: // data_transform_card: "transformation (TRn)" card_id "*" number_list
#line 2235 "MCNPParser.bison"
                                              {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    auto angle_flag = interpreter.push_leaf(wasp::mcnpi::DECL, "angle_flag", (yystack_[1].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), angle_flag);
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"trcl"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 3575 "MCNPParser.cpp"
    break;

  case 263: // data_universe_card: "universe" number_list
#line 2247 "MCNPParser.bison"
                                          {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"universe"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 3588 "MCNPParser.cpp"
    break;

  case 264: // data_lat_card: "lat" number_list
#line 2257 "MCNPParser.bison"
                                {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"lat"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 3601 "MCNPParser.cpp"
    break;

  case 265: // data_fill_card: "fill" number_list
#line 2266 "MCNPParser.bison"
                                  {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fill"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 3614 "MCNPParser.cpp"
    break;

  case 266: // uran_members: "integer number" NUMBER NUMBER NUMBER
#line 2276 "MCNPParser.bison"
 {
    auto id = interpreter.push_leaf(wasp::mcnpi::ID, "id", (yystack_[3].value.token_index));
    auto dx = interpreter.push_leaf(wasp::mcnpi::VALUE, "dx", (yystack_[2].value.token_index));
    auto dy = interpreter.push_leaf(wasp::mcnpi::VALUE, "dy", (yystack_[1].value.token_index));
    auto dz = interpreter.push_leaf(wasp::mcnpi::VALUE, "dz", (yystack_[0].value.token_index));
    (yylhs.value.node_indices) = new std::vector<size_t>();
    *(yylhs.value.node_indices) = {id, dx, dy, dz};
 }
#line 3627 "MCNPParser.cpp"
    break;

  case 267: // uran_members: uran_members "integer number" NUMBER NUMBER NUMBER
#line 2285 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[4].value.node_indices);
    auto id = interpreter.push_leaf(wasp::mcnpi::ID, "id", (yystack_[3].value.token_index));
    auto dx = interpreter.push_leaf(wasp::mcnpi::VALUE, "dx", (yystack_[2].value.token_index));
    auto dy = interpreter.push_leaf(wasp::mcnpi::VALUE, "dy", (yystack_[1].value.token_index));
    auto dz = interpreter.push_leaf(wasp::mcnpi::VALUE, "dz", (yystack_[0].value.token_index));
    *(yylhs.value.node_indices) = {id, dx, dy, dz};
 }
#line 3640 "MCNPParser.cpp"
    break;

  case 268: // data_uran_card: "random universe (URAN)" uran_members
#line 2295 "MCNPParser.bison"
                                   {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"uran"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 3653 "MCNPParser.cpp"
    break;

  case 269: // MATERIAL_SCALAR_KEY: GAS
#line 2304 "MCNPParser.bison"
                      { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3659 "MCNPParser.cpp"
    break;

  case 270: // MATERIAL_SCALAR_KEY: REFI
#line 2304 "MCNPParser.bison"
                            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3665 "MCNPParser.cpp"
    break;

  case 271: // MATERIAL_SCALAR_KEY: COND
#line 2304 "MCNPParser.bison"
                                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3671 "MCNPParser.cpp"
    break;

  case 272: // MATERIAL_SCALAR_KEY: ESTEP
#line 2304 "MCNPParser.bison"
                                           { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3677 "MCNPParser.cpp"
    break;

  case 273: // MATERIAL_SCALAR_KEY: HSTEP
#line 2304 "MCNPParser.bison"
                                                   { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3683 "MCNPParser.cpp"
    break;

  case 274: // MATERIAL_LIST_KEY: REFC
#line 2305 "MCNPParser.bison"
                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3689 "MCNPParser.cpp"
    break;

  case 275: // MATERIAL_LIST_KEY: REFS
#line 2305 "MCNPParser.bison"
                           { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3695 "MCNPParser.cpp"
    break;

  case 276: // amaterial_option: MATERIAL_LIST_KEY assign number_list
#line 2308 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
            [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::MATERIAL_PARAM
                                ,name.data()
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 3712 "MCNPParser.cpp"
    break;

  case 277: // amaterial_option: MATERIAL_LIST_KEY number_list
#line 2321 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
            [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::MATERIAL_PARAM
                                ,name.data()
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 3728 "MCNPParser.cpp"
    break;

  case 278: // amaterial_option: MATERIAL_SCALAR_KEY assign value
#line 2333 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
            [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::MATERIAL_PARAM
                                ,name.data()
                                ,{decl, (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
}
#line 3742 "MCNPParser.cpp"
    break;

  case 279: // amaterial_option: MATERIAL_SCALAR_KEY value
#line 2343 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
            [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::MATERIAL_PARAM
                                ,name.data()
                                ,{decl, (yystack_[0].value.node_index)});
}
#line 3756 "MCNPParser.cpp"
    break;

  case 280: // amaterial_option: "(N|P|PN|E|H|A|S|T|D)LIB" assign "library id (.?[0-9]{2}[tcdmgpuyehporsa])"
#line 2353 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto lid = interpreter.push_leaf(wasp::mcnpi::DECL, "lib_id", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::MATERIAL_PARAM
                                ,interpreter.token_data((yystack_[2].value.token_index))
                                ,{decl, (yystack_[1].value.node_index), lid});
}
#line 3768 "MCNPParser.cpp"
    break;

  case 281: // amaterial_option: "(N|P|PN|E|H|A|S|T|D)LIB" "library id (.?[0-9]{2}[tcdmgpuyehporsa])"
#line 2361 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto lid = interpreter.push_leaf(wasp::mcnpi::DECL, "lib_id", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::MATERIAL_PARAM
                                ,interpreter.token_data((yystack_[1].value.token_index))
                                ,{decl, lid});
}
#line 3780 "MCNPParser.cpp"
    break;

  case 282: // material_zaid_entry: "integer number" number
#line 2370 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::ID, "id", (yystack_[1].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::ZAID
                                ,"zaid"
                                ,{decl, (yystack_[0].value.node_index)});
}
#line 3791 "MCNPParser.cpp"
    break;

  case 283: // material_zaid_entry: "integer number" assign number
#line 2377 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::ID, "id", (yystack_[2].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::ZAID
                                ,"zaid"
                                ,{decl, (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
}
#line 3802 "MCNPParser.cpp"
    break;

  case 284: // material_zaid_entry: "integer number" "library id (.?[0-9]{2}[tcdmgpuyehporsa])" number
#line 2384 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::ID, "id", (yystack_[2].value.token_index));
    auto lib = interpreter.push_leaf(wasp::mcnpi::LIB_ID, "lib", (yystack_[1].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::ZAID
                                ,"zaid"
                                ,{decl, lib, (yystack_[0].value.node_index)});
}
#line 3814 "MCNPParser.cpp"
    break;

  case 285: // material_zaid_entry: "integer number" "library id (.?[0-9]{2}[tcdmgpuyehporsa])" assign number
#line 2392 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::ID, "id", (yystack_[3].value.token_index));
    auto lib = interpreter.push_leaf(wasp::mcnpi::LIB_ID, "lib", (yystack_[2].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::ZAID
                                ,"zaid"
                                ,{decl, lib, (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
}
#line 3826 "MCNPParser.cpp"
    break;

  case 286: // material_zaid_entry: "double precision number" number
#line 2400 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::ID, "id", (yystack_[1].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::ZAID
                                ,"zaid"
                                ,{decl, (yystack_[0].value.node_index)});
}
#line 3837 "MCNPParser.cpp"
    break;

  case 287: // material_zaid_entry: "double precision number" assign number
#line 2407 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::ID, "id", (yystack_[2].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::ZAID
                                ,"zaid"
                                ,{decl, (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
}
#line 3848 "MCNPParser.cpp"
    break;

  case 288: // material_options: amaterial_option
#line 2415 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 3857 "MCNPParser.cpp"
    break;

  case 289: // material_options: material_options amaterial_option
#line 2420 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 3866 "MCNPParser.cpp"
    break;

  case 290: // material_options: material_zaid_entry
#line 2425 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t>();
    *(yylhs.value.node_indices) = {(yystack_[0].value.node_index)};
}
#line 3875 "MCNPParser.cpp"
    break;

  case 291: // material_options: material_options material_zaid_entry
#line 2430 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 3884 "MCNPParser.cpp"
    break;

  case 292: // data_material_card: "material id (Mn)" id material_options
#line 2437 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"material"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 3898 "MCNPParser.cpp"
    break;

  case 293: // data_material_card: "material id (Mn)" id
#line 2447 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"material"
                                ,{decl, (yystack_[0].value.node_index)});
}
#line 3909 "MCNPParser.cpp"
    break;

  case 294: // data_cutoff_card: "time energy and weight cutoff card (CUT)" colon particle_symbol number_list
#line 2455 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"cutoff"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 3924 "MCNPParser.cpp"
    break;

  case 295: // data_cutoff_card: "time energy and weight cutoff card (CUT)" colon particle_symbol
#line 2466 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"cutoff"
                                ,{decl, (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
}
#line 3935 "MCNPParser.cpp"
    break;

  case 296: // data_elpt_card: "elpt" colon particle_symbol number_list
#line 2474 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"elpt"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 3950 "MCNPParser.cpp"
    break;

  case 297: // ACTIVATION_OPTION_KEY: "fission"
#line 2485 "MCNPParser.bison"
                        { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3956 "MCNPParser.cpp"
    break;

  case 298: // ACTIVATION_OPTION_KEY: "nonfiss"
#line 2485 "MCNPParser.bison"
                                  { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3962 "MCNPParser.cpp"
    break;

  case 299: // ACTIVATION_OPTION_KEY: "delayed neutron (DN)"
#line 2485 "MCNPParser.bison"
                                            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3968 "MCNPParser.cpp"
    break;

  case 300: // ACTIVATION_OPTION_KEY: "delayed gamma (DG)"
#line 2485 "MCNPParser.bison"
                                                 { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3974 "MCNPParser.cpp"
    break;

  case 301: // ACTIVATION_OPTION_KEY: "discrete delayed-gamma line threshold (THRESH)"
#line 2485 "MCNPParser.bison"
                                                      { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3980 "MCNPParser.cpp"
    break;

  case 302: // ACTIVATION_OPTION_KEY: "delayed neutron per interaction (DNBIAS)"
#line 2485 "MCNPParser.bison"
                                                               { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3986 "MCNPParser.cpp"
    break;

  case 303: // ACTIVATION_OPTION_KEY: "nap"
#line 2485 "MCNPParser.bison"
                                                                        { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3992 "MCNPParser.cpp"
    break;

  case 304: // ACTIVATION_OPTION_KEY: "delayed neutron energy bias (DNEB)"
#line 2486 "MCNPParser.bison"
                        { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 3998 "MCNPParser.cpp"
    break;

  case 305: // ACTIVATION_OPTION_KEY: "delayed gamma energy bias (DPEB)"
#line 2486 "MCNPParser.bison"
                               { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 4004 "MCNPParser.cpp"
    break;

  case 306: // ACTIVATION_OPTION_KEY: "delayed gamma energy cutoff (PECUT)"
#line 2486 "MCNPParser.bison"
                                      { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 4010 "MCNPParser.cpp"
    break;

  case 307: // ACTIVATION_OPTION_KEY: "spontaneos half-life cutoff (HLCUT)"
#line 2486 "MCNPParser.bison"
                                              { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 4016 "MCNPParser.cpp"
    break;

  case 308: // ACTIVATION_OPTION_KEY: "correlated or uncorrelated flag (SAMPLE)"
#line 2486 "MCNPParser.bison"
                                                      { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 4022 "MCNPParser.cpp"
    break;

  case 309: // comma: ","
#line 2488 "MCNPParser.bison"
              { (yylhs.value.node_index)=interpreter.push_leaf(wasp::mcnpi::DATA_COMMA, "comma", (yystack_[0].value.token_index));}
#line 4028 "MCNPParser.cpp"
    break;

  case 310: // comma_sep_list: PRIMITIVE
#line 2491 "MCNPParser.bison"
{
    auto v = interpreter.push_leaf(wasp::mcnpi::VALUE, "value", (yystack_[0].value.token_index));
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back(v);
}
#line 4038 "MCNPParser.cpp"
    break;

  case 311: // comma_sep_list: comma_sep_list comma PRIMITIVE
#line 2497 "MCNPParser.bison"
{
    auto v = interpreter.push_leaf(wasp::mcnpi::VALUE, "value", (yystack_[0].value.token_index));
    (yylhs.value.node_indices) = (yystack_[2].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
    (yylhs.value.node_indices)->push_back(v);
}
#line 4049 "MCNPParser.cpp"
    break;

  case 312: // activation_option: ACTIVATION_OPTION_KEY assign comma_sep_list
#line 2505 "MCNPParser.bison"
                                                                {
    auto d = interpreter.push_leaf(wasp::mcnpi::DECL,"decl",(yystack_[2].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), d);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,interpreter.token_data((yystack_[2].value.token_index))
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 4063 "MCNPParser.cpp"
    break;

  case 313: // activation_option: ACTIVATION_OPTION_KEY comma_sep_list
#line 2514 "MCNPParser.bison"
                                        {
    auto d = interpreter.push_leaf(wasp::mcnpi::DECL,"decl",(yystack_[1].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), d);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,interpreter.token_data((yystack_[1].value.token_index))
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 4076 "MCNPParser.cpp"
    break;

  case 314: // activation_options: activation_option
#line 2524 "MCNPParser.bison"
                                        {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 4085 "MCNPParser.cpp"
    break;

  case 315: // activation_options: activation_options activation_option
#line 2529 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 4094 "MCNPParser.cpp"
    break;

  case 316: // data_activation_card: "activation options (ACT)" activation_options
#line 2536 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"act"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 4107 "MCNPParser.cpp"
    break;

  case 317: // data_fq_card: "print hierarchy (FQn)" card_id string_list
#line 2545 "MCNPParser.bison"
                                       {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fq"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 4121 "MCNPParser.cpp"
    break;

  case 318: // data_fq_card: "print hierarchy (FQn)" string_list
#line 2554 "MCNPParser.bison"
                 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fq"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 4134 "MCNPParser.cpp"
    break;

  case 319: // data_fc_card: "tally comment card (FCn)" card_id comment_list
#line 2563 "MCNPParser.bison"
                                        {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fc"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 4148 "MCNPParser.cpp"
    break;

  case 320: // data_sc_card: "source comment card (SCn)" card_id comment_list
#line 2573 "MCNPParser.bison"
                                        {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"sc"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 4162 "MCNPParser.cpp"
    break;

  case 321: // particle_symbol_list: particle_symbol
#line 2583 "MCNPParser.bison"
                                       {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 4171 "MCNPParser.cpp"
    break;

  case 322: // particle_symbol_list: particle_symbol_list comma particle_symbol
#line 2589 "MCNPParser.bison"
                                             {
    (yylhs.value.node_indices) = (yystack_[2].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 4181 "MCNPParser.cpp"
    break;

  case 323: // mode_particle_symbol_list: particle_symbol
#line 2596 "MCNPParser.bison"
                                            {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 4190 "MCNPParser.cpp"
    break;

  case 324: // mode_particle_symbol_list: mode_particle_symbol_list particle_symbol
#line 2600 "MCNPParser.bison"
                                            {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 4199 "MCNPParser.cpp"
    break;

  case 325: // lt: "<"
#line 2606 "MCNPParser.bison"
{
    (yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::LT, "LT", (yystack_[0].value.token_index));
}
#line 4207 "MCNPParser.cpp"
    break;

  case 326: // rbracket: "]"
#line 2611 "MCNPParser.bison"
{
    (yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::RBRACKET, "RB", (yystack_[0].value.token_index));
}
#line 4215 "MCNPParser.cpp"
    break;

  case 327: // lbracket: "["
#line 2616 "MCNPParser.bison"
{
    (yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::LBRACKET, "LB", (yystack_[0].value.token_index));
}
#line 4223 "MCNPParser.cpp"
    break;

  case 328: // SURFACE_OR_CELL_TALLY: "integrated current surface tally card (F1)"
#line 2620 "MCNPParser.bison"
                        { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 4229 "MCNPParser.cpp"
    break;

  case 329: // SURFACE_OR_CELL_TALLY: "flux averaged surface tally card (F2)"
#line 2620 "MCNPParser.bison"
                                   { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 4235 "MCNPParser.cpp"
    break;

  case 330: // SURFACE_OR_CELL_TALLY: "flux averaged cell tally card (F4)"
#line 2620 "MCNPParser.bison"
                                              { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 4241 "MCNPParser.cpp"
    break;

  case 331: // SURFACE_OR_CELL_TALLY: "fission energy deposition cell tally card (F7)"
#line 2620 "MCNPParser.bison"
                                                         { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 4247 "MCNPParser.cpp"
    break;

  case 332: // SURFACE_OR_CELL_TALLY: "F8 tally card (F8)"
#line 2620 "MCNPParser.bison"
                                                                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 4253 "MCNPParser.cpp"
    break;

  case 333: // tally_surface_list: number_or_sc
#line 2623 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 4262 "MCNPParser.cpp"
    break;

  case 334: // tally_surface_list: number_or_sc colon number_or_sc
#line 2628 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[2].value.node_index));
    (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 4273 "MCNPParser.cpp"
    break;

  case 335: // tally_surface_list: lparen tally_surface_list rparen
#line 2635 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 4283 "MCNPParser.cpp"
    break;

  case 336: // tally_surface_list: tally_surface_list number_or_sc
#line 2641 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 4292 "MCNPParser.cpp"
    break;

  case 337: // tally_surface_list: tally_surface_list number_or_sc colon number_or_sc
#line 2646 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[3].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[2].value.node_index));
    (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 4303 "MCNPParser.cpp"
    break;

  case 338: // tally_surface_list: tally_surface_list lparen tally_surface_list rparen
#line 2653 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[3].value.node_indices);
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->end(), (yystack_[1].value.node_indices)->begin(), (yystack_[1].value.node_indices)->end());
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
    delete (yystack_[1].value.node_indices);
}
#line 4315 "MCNPParser.cpp"
    break;

  case 339: // tally_surface_list: tally_surface_list lt number_or_sc
#line 2661 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[2].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 4325 "MCNPParser.cpp"
    break;

  case 340: // tally_surface_list: tally_surface_list lt lparen tally_surface_list rparen
#line 2667 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[4].value.node_indices);
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->end(), (yystack_[3].value.node_index));
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->end(), (yystack_[2].value.node_index));
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->end(), (yystack_[1].value.node_indices)->begin(), (yystack_[1].value.node_indices)->end());
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
    delete (yystack_[1].value.node_indices);
}
#line 4338 "MCNPParser.cpp"
    break;

  case 341: // tally_surface_list: tally_surface_list lbracket tally_surface_list rbracket
#line 2676 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[3].value.node_indices);
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->end(), (yystack_[2].value.node_index));
    (yylhs.value.node_indices)->insert((yylhs.value.node_indices)->end(), (yystack_[1].value.node_indices)->begin(), (yystack_[1].value.node_indices)->end());
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
    delete (yystack_[1].value.node_indices);
}
#line 4350 "MCNPParser.cpp"
    break;

  case 342: // f5_tally_member: NUMBER NUMBER NUMBER NUMBER
#line 2686 "MCNPParser.bison"
{
    auto x = interpreter.push_leaf(wasp::mcnpi::VALUE, "x", (yystack_[3].value.token_index));
    auto y = interpreter.push_leaf(wasp::mcnpi::VALUE, "y", (yystack_[2].value.token_index));
    auto z = interpreter.push_leaf(wasp::mcnpi::VALUE, "z", (yystack_[1].value.token_index));
    auto r = interpreter.push_leaf(wasp::mcnpi::VALUE, "r", (yystack_[0].value.token_index));

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::TALLY_POINT
                                ,"point"
                                ,{x, y, z, r});
}
#line 4365 "MCNPParser.cpp"
    break;

  case 343: // f5a_tally_member: NUMBER NUMBER NUMBER
#line 2698 "MCNPParser.bison"
{
    auto ao = interpreter.push_leaf(wasp::mcnpi::VALUE, "o", (yystack_[2].value.token_index));
    auto r = interpreter.push_leaf(wasp::mcnpi::VALUE, "r", (yystack_[1].value.token_index));
    auto ro = interpreter.push_leaf(wasp::mcnpi::VALUE, "ro", (yystack_[0].value.token_index));

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::TALLY_POINT
                                ,"point"
                                ,{ao, r, ro});
}
#line 4379 "MCNPParser.cpp"
    break;

  case 344: // f5_tally_members: f5_tally_member
#line 2709 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 4388 "MCNPParser.cpp"
    break;

  case 345: // f5_tally_members: f5_tally_members f5_tally_member
#line 2714 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 4397 "MCNPParser.cpp"
    break;

  case 346: // f5a_tally_members: f5a_tally_member
#line 2720 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 4406 "MCNPParser.cpp"
    break;

  case 347: // f5a_tally_members: f5a_tally_members f5a_tally_member
#line 2725 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 4415 "MCNPParser.cpp"
    break;

  case 348: // data_ftally_card: SURFACE_OR_CELL_TALLY card_id colon particle_symbol_list tally_surface_list
#line 2731 "MCNPParser.bison"
                                                                                                {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[4].value.token_index));

    (yystack_[1].value.node_indices)->insert((yystack_[1].value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yystack_[1].value.node_indices)->insert((yystack_[1].value.node_indices)->begin(), (yystack_[3].value.node_index));
    (yystack_[1].value.node_indices)->insert((yystack_[1].value.node_indices)->begin(), decl);
    (yystack_[1].value.node_indices)->insert((yystack_[1].value.node_indices)->end(), (yystack_[0].value.node_indices)->begin(), (yystack_[0].value.node_indices)->end());
    std::string tally_name = interpreter.data((yystack_[3].value.node_index));
    // we only care about the last number of the INTEGER which dictates the type
    tally_name = tally_name.substr(tally_name.size()-1);
    tally_name += "_tally";
    // create Fn_TALLY
    tally_name = "f"+tally_name;
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,tally_name.data()
                                ,*(yystack_[1].value.node_indices));
    delete (yystack_[1].value.node_indices);
    delete (yystack_[0].value.node_indices);
}
#line 4439 "MCNPParser.cpp"
    break;

  case 349: // data_ftally_card: SURFACE_OR_CELL_TALLY card_id colon particle_symbol_list tally_surface_list "string"
#line 2750 "MCNPParser.bison"
                                                                                     {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[5].value.token_index));
    (yystack_[2].value.node_indices)->insert((yystack_[2].value.node_indices)->begin(), (yystack_[3].value.node_index));
    (yystack_[2].value.node_indices)->insert((yystack_[2].value.node_indices)->begin(), (yystack_[4].value.node_index));
    (yystack_[2].value.node_indices)->insert((yystack_[2].value.node_indices)->begin(), decl);
    (yystack_[2].value.node_indices)->insert((yystack_[2].value.node_indices)->end(), (yystack_[1].value.node_indices)->begin(), (yystack_[1].value.node_indices)->end());
    auto T = interpreter.push_leaf(wasp::mcnpi::FLAG, "accumulator", (yystack_[0].value.token_index));
    (yystack_[2].value.node_indices)->push_back(T);
    std::string tally_name = interpreter.data((yystack_[4].value.node_index));
    tally_name = tally_name.substr(tally_name.size()-1);
    tally_name += "_tally";
    tally_name = "f"+tally_name;
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,tally_name.data()
                                ,*(yystack_[2].value.node_indices));
    delete (yystack_[2].value.node_indices);
    delete (yystack_[1].value.node_indices);
}
#line 4462 "MCNPParser.cpp"
    break;

  case 350: // data_ftally_card: "F6 cell tally card (F6)" card_id tally_surface_list "string"
#line 2768 "MCNPParser.bison"
                                             {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    (yystack_[1].value.node_indices)->insert((yystack_[1].value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yystack_[1].value.node_indices)->insert((yystack_[1].value.node_indices)->begin(), decl);
    auto T = interpreter.push_leaf(wasp::mcnpi::FLAG, "accumulator", (yystack_[0].value.token_index));
    (yystack_[1].value.node_indices)->push_back(T);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"f6_tally"
                                ,*(yystack_[1].value.node_indices));
    delete (yystack_[1].value.node_indices);
}
#line 4478 "MCNPParser.cpp"
    break;

  case 351: // data_ftally_card: "F6 cell tally card (F6)" card_id tally_surface_list
#line 2779 "MCNPParser.bison"
                                      {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"f6_tally"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 4493 "MCNPParser.cpp"
    break;

  case 352: // data_ftally_card: "F6 cell tally card (F6)" card_id colon particle_symbol_list tally_surface_list
#line 2789 "MCNPParser.bison"
                                                                 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[4].value.token_index));
    (yystack_[1].value.node_indices)->insert((yystack_[1].value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yystack_[1].value.node_indices)->insert((yystack_[1].value.node_indices)->begin(), (yystack_[3].value.node_index));
    (yystack_[1].value.node_indices)->insert((yystack_[1].value.node_indices)->begin(), decl);
    (yystack_[1].value.node_indices)->insert((yystack_[1].value.node_indices)->end(), (yystack_[0].value.node_indices)->begin(), (yystack_[0].value.node_indices)->end());
    std::string tally_name = interpreter.data((yystack_[3].value.node_index));

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"f6_tally"
                                ,*(yystack_[1].value.node_indices));
    delete (yystack_[1].value.node_indices);
    delete (yystack_[0].value.node_indices);
}
#line 4512 "MCNPParser.cpp"
    break;

  case 353: // data_ftally_card: "F6 cell tally card (F6)" card_id colon particle_symbol_list tally_surface_list "string"
#line 2803 "MCNPParser.bison"
                                                                        {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[5].value.token_index));
    (yystack_[2].value.node_indices)->insert((yystack_[2].value.node_indices)->begin(), (yystack_[3].value.node_index));
    (yystack_[2].value.node_indices)->insert((yystack_[2].value.node_indices)->begin(), (yystack_[4].value.node_index));
    (yystack_[2].value.node_indices)->insert((yystack_[2].value.node_indices)->begin(), decl);
    (yystack_[2].value.node_indices)->insert((yystack_[2].value.node_indices)->end(), (yystack_[1].value.node_indices)->begin(), (yystack_[1].value.node_indices)->end());
    auto T = interpreter.push_leaf(wasp::mcnpi::FLAG, "accumulator", (yystack_[0].value.token_index));
    (yystack_[2].value.node_indices)->push_back(T);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"f6_tally"
                                ,*(yystack_[2].value.node_indices));
    delete (yystack_[2].value.node_indices);
    delete (yystack_[1].value.node_indices);
}
#line 4531 "MCNPParser.cpp"
    break;

  case 354: // data_ftally_card: "flux point or ring tally card (F5)" card_id colon particle_symbol f5_tally_members
#line 2818 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[4].value.token_index));

    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[3].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"f5_tally"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 4549 "MCNPParser.cpp"
    break;

  case 355: // data_ftally_card: "flux point or ring tally card (F5)" card_id colon particle_symbol f5_tally_members "string"
#line 2832 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[5].value.token_index));
    auto nd = interpreter.push_leaf(wasp::mcnpi::FLAG, "nd", (yystack_[0].value.token_index));
    (yystack_[1].value.node_indices)->insert((yystack_[1].value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yystack_[1].value.node_indices)->insert((yystack_[1].value.node_indices)->begin(), (yystack_[3].value.node_index));
    (yystack_[1].value.node_indices)->insert((yystack_[1].value.node_indices)->begin(), (yystack_[4].value.node_index));
    (yystack_[1].value.node_indices)->insert((yystack_[1].value.node_indices)->begin(), decl);
    (yystack_[1].value.node_indices)->push_back(nd);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"f5_tally"
                                ,*(yystack_[1].value.node_indices));
    delete (yystack_[1].value.node_indices);
}
#line 4567 "MCNPParser.cpp"
    break;

  case 356: // data_ftally_card: "ring detector tally card (F5[XYZ])" card_id "string" colon particle_symbol f5a_tally_members
#line 2846 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[5].value.token_index));
    auto axis = interpreter.push_leaf(wasp::mcnpi::AXIS, "axis", (yystack_[3].value.token_index));

    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), axis);
    m->insert(m->begin(), (yystack_[4].value.node_index));
    m->insert(m->begin(), decl);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"f5a_tally"
                                ,*m);
    delete m;
}
#line 4588 "MCNPParser.cpp"
    break;

  case 357: // data_ftally_card: "ring detector tally card (F5[XYZ])" card_id "string" colon particle_symbol f5a_tally_members "string"
#line 2863 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[6].value.token_index));
    auto axis = interpreter.push_leaf(wasp::mcnpi::AXIS, "axis", (yystack_[4].value.token_index));
    auto nd = interpreter.push_leaf(wasp::mcnpi::FLAG, "nd", (yystack_[0].value.token_index));

    auto m = (yystack_[1].value.node_indices);
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), (yystack_[3].value.node_index));
    m->insert(m->begin(), axis);
    m->insert(m->begin(), (yystack_[5].value.node_index));
    m->insert(m->begin(), decl);

    m->push_back(nd);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"f5a_tally"
                                ,*m);
    delete m;
}
#line 4612 "MCNPParser.cpp"
    break;

  case 358: // data_ftally_card: "ring detector tally card (F[XYZ]5)" card_id colon particle_symbol f5a_tally_members "string"
#line 2883 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[5].value.token_index));
    auto nd = interpreter.push_leaf(wasp::mcnpi::FLAG, "nd", (yystack_[0].value.token_index));

    auto m = (yystack_[1].value.node_indices);
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), (yystack_[3].value.node_index));
    m->insert(m->begin(), (yystack_[4].value.node_index));
    m->insert(m->begin(), decl);

    m->push_back(nd);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fa5_tally"
                                ,*m);
    delete m;
}
#line 4634 "MCNPParser.cpp"
    break;

  case 359: // data_ftally_card: "ring detector tally card (F[XYZ]5)" card_id colon particle_symbol f5a_tally_members
#line 2901 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[4].value.token_index));

    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), (yystack_[3].value.node_index));
    m->insert(m->begin(), decl);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fa5_tally"
                                ,*m);
    delete m;
}
#line 4653 "MCNPParser.cpp"
    break;

  case 360: // data_ftally_card: "image tally card (FI[CPR]n)" card_id colon particle_symbol NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER
#line 2919 "MCNPParser.bison"
                                                  {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[13].value.token_index));

    auto x1 = interpreter.push_leaf(wasp::mcnpi::VALUE, "x1", (yystack_[9].value.token_index));
    auto y1 = interpreter.push_leaf(wasp::mcnpi::VALUE, "y1", (yystack_[8].value.token_index));
    auto z1 = interpreter.push_leaf(wasp::mcnpi::VALUE, "z1", (yystack_[7].value.token_index));

    auto ro = interpreter.push_leaf(wasp::mcnpi::VALUE, "ro", (yystack_[6].value.token_index));

    auto x2 = interpreter.push_leaf(wasp::mcnpi::VALUE, "x2", (yystack_[5].value.token_index));
    auto y2 = interpreter.push_leaf(wasp::mcnpi::VALUE, "y2", (yystack_[4].value.token_index));
    auto z2 = interpreter.push_leaf(wasp::mcnpi::VALUE, "z2", (yystack_[3].value.token_index));

    auto f1 = interpreter.push_leaf(wasp::mcnpi::VALUE, "f1", (yystack_[2].value.token_index));
    auto f2 = interpreter.push_leaf(wasp::mcnpi::VALUE, "f2", (yystack_[1].value.token_index));
    auto f3 = interpreter.push_leaf(wasp::mcnpi::VALUE, "f3", (yystack_[0].value.token_index));

    std::string tally_name = interpreter.token_data((yystack_[13].value.token_index));
    tally_name += "_tally";
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,tally_name.data()
                                ,{decl, (yystack_[12].value.node_index), (yystack_[11].value.node_index), (yystack_[10].value.node_index),
                                   x1, y1, z1, ro, x2, y2, z2, f1, f2, f3});
}
#line 4682 "MCNPParser.cpp"
    break;

  case 361: // data_ftally_card: "*" data_ftally_card
#line 2944 "MCNPParser.bison"
{
    auto energy_x_weight_flag = interpreter.push_leaf(wasp::mcnpi::FLAG, "exw", (yystack_[1].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"exw_tally"
                                ,{energy_x_weight_flag, (yystack_[0].value.node_index)});
}
#line 4693 "MCNPParser.cpp"
    break;

  case 362: // data_ftally_card: "+" data_ftally_card
#line 2951 "MCNPParser.bison"
{
    auto jerks_flag = interpreter.push_leaf(wasp::mcnpi::FLAG, "jflag", (yystack_[1].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"jerks_tally"
                                ,{jerks_flag, (yystack_[0].value.node_index)});
}
#line 4704 "MCNPParser.cpp"
    break;

  case 363: // data_mode_card: "problem type (MODE)" mode_particle_symbol_list
#line 2959 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"problem_type"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
 }
#line 4717 "MCNPParser.cpp"
    break;

  case 364: // TALLY_TIME_OPTION_KEY: "ref starting time (CBEG)"
#line 2968 "MCNPParser.bison"
                        { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 4723 "MCNPParser.cpp"
    break;

  case 365: // TALLY_TIME_OPTION_KEY: "frequency of cycling (CFRQ)"
#line 2968 "MCNPParser.bison"
                               { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 4729 "MCNPParser.cpp"
    break;

  case 366: // TALLY_TIME_OPTION_KEY: "dead time interval (COFI)"
#line 2968 "MCNPParser.bison"
                                      { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 4735 "MCNPParser.cpp"
    break;

  case 367: // TALLY_TIME_OPTION_KEY: "alive time interval (CONI)"
#line 2968 "MCNPParser.bison"
                                             { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 4741 "MCNPParser.cpp"
    break;

  case 368: // TALLY_TIME_OPTION_KEY: "number of subdivisions ot use within alive time (CSUB)"
#line 2968 "MCNPParser.bison"
                                                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 4747 "MCNPParser.cpp"
    break;

  case 369: // TALLY_TIME_OPTION_KEY: "ref ending time (CEND)"
#line 2968 "MCNPParser.bison"
                                                           { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 4753 "MCNPParser.cpp"
    break;

  case 370: // tally_time_option: TALLY_TIME_OPTION_KEY assign number
#line 2970 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,interpreter.token_data((yystack_[2].value.token_index))
                                ,{decl, (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
}
#line 4764 "MCNPParser.cpp"
    break;

  case 371: // tally_time_option: TALLY_TIME_OPTION_KEY number
#line 2977 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,interpreter.token_data((yystack_[1].value.token_index))
                                ,{decl, (yystack_[0].value.node_index)});
}
#line 4775 "MCNPParser.cpp"
    break;

  case 372: // tally_time_options: tally_time_option
#line 2985 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 4784 "MCNPParser.cpp"
    break;

  case 373: // tally_time_options: tally_time_options tally_time_option
#line 2990 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 4793 "MCNPParser.cpp"
    break;

  case 374: // data_tally_time_card: "tally time (Tn)" card_id number_list
#line 2996 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_time"
                                ,*m);
    delete m;
 }
#line 4808 "MCNPParser.cpp"
    break;

  case 375: // data_tally_time_card: "tally time (Tn)" card_id number_list "string" "string"
#line 3007 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[4].value.token_index));

    auto m = (yystack_[2].value.node_indices);
    m->insert(m->begin(), (yystack_[3].value.node_index));
    m->insert(m->begin(), decl);

    std::string o1 = interpreter.token_data((yystack_[1].value.token_index));
    std::string o2 = interpreter.token_data((yystack_[0].value.token_index));
    if (o1.size() > 1)
        o1 = "no_total";
    else o1 = "cumulative";

    if (o2.size() > 1)
        o2 = "no_total";
    else o2 = "cumulative";

    auto o1i = interpreter.push_leaf(wasp::mcnpi::DECL, o1.data(), (yystack_[1].value.token_index));
    auto o2i = interpreter.push_leaf(wasp::mcnpi::DECL, o2.data(), (yystack_[0].value.token_index));


    m->push_back(o1i);
    m->push_back(o2i);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_time"
                                ,*m);
    delete m;
 }
#line 4841 "MCNPParser.cpp"
    break;

  case 376: // data_tally_time_card: "tally time (Tn)" card_id number_list "string"
#line 3036 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));

    auto m = (yystack_[1].value.node_indices);
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), decl);

    std::string o1 = interpreter.token_data((yystack_[0].value.token_index));

    if (o1.size() > 1)
        o1 = "no_total";
    else o1 = "cumulative";

    auto o1i = interpreter.push_leaf(wasp::mcnpi::DECL, o1.data(), (yystack_[0].value.token_index));

    m->push_back(o1i);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_time"
                                ,*m);
    delete m;
 }
#line 4867 "MCNPParser.cpp"
    break;

  case 377: // data_tally_time_card: "tally time (Tn)" card_id tally_time_options
#line 3058 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_time"
                                ,*m);
    delete m;
 }
#line 4882 "MCNPParser.cpp"
    break;

  case 378: // data_kcode_card: "criticality source card (KCODE)" number_token_list
#line 3070 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    static std::vector<std::string> names = {"nsrck", "rkk", "ikz", "kct", "msrk", "knrm", "mrkp", "kc8"};
    static std::vector<mcnpi::NODE> types = {mcnpi::VALUE, mcnpi::VALUE, mcnpi::VALUE, mcnpi::VALUE,
                                             mcnpi::VALUE, mcnpi::VALUE, mcnpi::VALUE, mcnpi::VALUE};
    std::vector<size_t> children;
    children.push_back(decl);
    assign_names(interpreter, names, types, (yystack_[0].value.token_indices), children);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"kcode"
                                ,children);
    delete (yystack_[0].value.token_indices);
 }
#line 4901 "MCNPParser.cpp"
    break;

  case 379: // data_nps_card: "history cutoff (NPS)" number_token_list
#line 3086 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    static std::vector<std::string> names = {"npp", "npsmg"};
    static std::vector<mcnpi::NODE> types = {mcnpi::VALUE, mcnpi::VALUE};
    std::vector<size_t> children;
    children.push_back(decl);
    assign_names(interpreter, names, types, (yystack_[0].value.token_indices), children);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"nps"
                                ,children);
    delete (yystack_[0].value.token_indices);
 }
#line 4919 "MCNPParser.cpp"
    break;

  case 380: // data_print_card: "print" number_list
#line 3101 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"print"
                                ,*m);
    delete m;
 }
#line 4933 "MCNPParser.cpp"
    break;

  case 381: // data_print_card: "print"
#line 3111 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"print"
                                ,{decl});
 }
#line 4944 "MCNPParser.cpp"
    break;

  case 382: // data_talnp_card: "negate printing of tallies (TALNP)" number_list
#line 3119 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"talnp"
                                ,*m);
    delete m;
 }
#line 4958 "MCNPParser.cpp"
    break;

  case 383: // data_talnp_card: "negate printing of tallies (TALNP)"
#line 3129 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"talnp"
                                ,{decl});
 }
#line 4969 "MCNPParser.cpp"
    break;

  case 384: // data_prdmp_card: "print and dump cycle (PRDMP)" number_token_list
#line 3137 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    static std::vector<std::string> names = {"ndp", "ndm", "mct", "ndmp", "dmmp"};
    static std::vector<mcnpi::NODE> types = {mcnpi::VALUE, mcnpi::VALUE, mcnpi::VALUE, mcnpi::VALUE, mcnpi::VALUE};
    std::vector<size_t> children;
    children.push_back(decl);
    assign_names(interpreter, names, types, (yystack_[0].value.token_indices), children);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"prdmp"
                                ,children);
    delete (yystack_[0].value.token_indices);
 }
#line 4987 "MCNPParser.cpp"
    break;

  case 385: // data_mt_card: "s(a,b) thermal neutron scattering (MTm)" card_id string_list
#line 3153 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"mt"
                                ,*m);
    delete m;
 }
#line 5002 "MCNPParser.cpp"
    break;

  case 386: // data_mt_card: "s(a,b) thermal neutron scattering (MTm)" card_id
#line 3164 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"mt"
                                ,{decl, (yystack_[0].value.node_index)});
 }
#line 5013 "MCNPParser.cpp"
    break;

  case 387: // data_imp_card: "imp" colon particle_symbol_list number_list
#line 3172 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    auto m = (yystack_[1].value.node_indices);
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), decl);
    m->insert(m->end(), (yystack_[0].value.node_indices)->begin(), (yystack_[0].value.node_indices)->end());
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"imp"
                                ,*m);
    delete m;
    delete (yystack_[0].value.node_indices);
 }
#line 5030 "MCNPParser.cpp"
    break;

  case 388: // data_ksrc_card: "criticality source points (KSRC)" number_list
#line 3186 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ksrc"
                                ,*m);
    delete m;
 }
#line 5044 "MCNPParser.cpp"
    break;

  case 389: // data_tally_energy_card: "tally energy card (En)" card_id number_list
#line 3198 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_energy"
                                ,*m);
    delete m;
 }
#line 5059 "MCNPParser.cpp"
    break;

  case 390: // data_tally_energy_card: "tally energy card (En)" card_id number_list "string" "string"
#line 3209 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[4].value.token_index));

    auto m = (yystack_[2].value.node_indices);
    m->insert(m->begin(), (yystack_[3].value.node_index));
    m->insert(m->begin(), decl);

    std::string o1 = interpreter.token_data((yystack_[1].value.token_index));
    std::string o2 = interpreter.token_data((yystack_[0].value.token_index));
    if (o1.size() > 1)
        o1 = "no_total";
    else o1 = "cumulative";

    if (o2.size() > 1)
        o2 = "no_total";
    else o2 = "cumulative";

    auto o1i = interpreter.push_leaf(wasp::mcnpi::DECL, o1.data(), (yystack_[1].value.token_index));
    auto o2i = interpreter.push_leaf(wasp::mcnpi::DECL, o2.data(), (yystack_[0].value.token_index));


    m->push_back(o1i);
    m->push_back(o2i);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_energy"
                                ,*m);
    delete m;
 }
#line 5092 "MCNPParser.cpp"
    break;

  case 391: // data_tally_energy_card: "tally energy card (En)" card_id number_list "string"
#line 3238 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));

    auto m = (yystack_[1].value.node_indices);
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), decl);

    std::string o1 = interpreter.token_data((yystack_[0].value.token_index));

    if (o1.size() > 1)
        o1 = "no_total";
    else o1 = "cumulative";

    auto o1i = interpreter.push_leaf(wasp::mcnpi::DECL, o1.data(), (yystack_[0].value.token_index));

    m->push_back(o1i);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_energy"
                                ,*m);
    delete m;
 }
#line 5118 "MCNPParser.cpp"
    break;

  case 392: // data_tally_energy_card: "tally energy card (En)" card_id colon particle_symbol number_list
#line 3260 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[4].value.token_index));

    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), (yystack_[3].value.node_index));
    m->insert(m->begin(), decl);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_energy"
                                ,*m);
    delete m;
 }
#line 5137 "MCNPParser.cpp"
    break;

  case 393: // data_tally_energy_card: "tally energy card (En)" "integer number" tally_time_options
#line 3275 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto id = interpreter.push_leaf(wasp::mcnpi::ID, "id", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), id);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_energy"
                                ,*m);
    delete m;
 }
#line 5153 "MCNPParser.cpp"
    break;

  case 394: // data_tally_multiplier_card: "tally multiplier card (FMn)" card_id number_paren_colon_list
#line 3289 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_multiplier"
                                ,*m);
    delete m;
 }
#line 5168 "MCNPParser.cpp"
    break;

  case 395: // data_tally_multiplier_card: "tally multiplier card (FMn)" card_id number_paren_colon_list "string" "string"
#line 3300 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[4].value.token_index));

    auto m = (yystack_[2].value.node_indices);
    m->insert(m->begin(), (yystack_[3].value.node_index));
    m->insert(m->begin(), decl);

    std::string o1 = interpreter.token_data((yystack_[1].value.token_index));
    std::string o2 = interpreter.token_data((yystack_[0].value.token_index));
    if (o1.size() > 1)
        o1 = "no_total";
    else o1 = "cumulative";

    if (o2.size() > 1)
        o2 = "no_total";
    else o2 = "cumulative";

    auto o1i = interpreter.push_leaf(wasp::mcnpi::DECL, o1.data(), (yystack_[1].value.token_index));
    auto o2i = interpreter.push_leaf(wasp::mcnpi::DECL, o2.data(), (yystack_[0].value.token_index));


    m->push_back(o1i);
    m->push_back(o2i);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_multiplier"
                                ,*m);
    delete m;
 }
#line 5201 "MCNPParser.cpp"
    break;

  case 396: // data_tally_multiplier_card: "tally multiplier card (FMn)" card_id number_paren_colon_list "string"
#line 3329 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));

    auto m = (yystack_[1].value.node_indices);
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), decl);

    std::string o1 = interpreter.token_data((yystack_[0].value.token_index));

    if (o1.size() > 1)
        o1 = "no_total";
    else o1 = "cumulative";

    auto o1i = interpreter.push_leaf(wasp::mcnpi::DECL, o1.data(), (yystack_[0].value.token_index));

    m->push_back(o1i);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_multiplier"
                                ,*m);
    delete m;
 }
#line 5227 "MCNPParser.cpp"
    break;

  case 397: // data_tally_multiplier_card: "+" data_tally_multiplier_card
#line 3351 "MCNPParser.bison"
 {
    auto f = interpreter.push_leaf(wasp::mcnpi::FLAG, "flag", (yystack_[1].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ftm"
                                ,{f, (yystack_[0].value.node_index)});
 }
#line 5238 "MCNPParser.cpp"
    break;

  case 398: // data_sd_card: "segment divisor card (SDn)" card_id number_paren_list
#line 3359 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"sd"
                                ,*m);
    delete m;
 }
#line 5253 "MCNPParser.cpp"
    break;

  case 399: // SDEF_SCALAR_VALUE_KEY: "surface number (SUR)"
#line 3371 "MCNPParser.bison"
                         { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 5259 "MCNPParser.cpp"
    break;

  case 400: // SDEF_SCALAR_VALUE_KEY: ERG
#line 3371 "MCNPParser.bison"
                                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 5265 "MCNPParser.cpp"
    break;

  case 401: // SDEF_SCALAR_VALUE_KEY: "time (TME)"
#line 3371 "MCNPParser.bison"
                                          { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 5271 "MCNPParser.cpp"
    break;

  case 402: // SDEF_SCALAR_VALUE_KEY: "cosine angle between VEC and particle direction (DIR)"
#line 3371 "MCNPParser.bison"
                                                     { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 5277 "MCNPParser.cpp"
    break;

  case 403: // SDEF_SCALAR_VALUE_KEY: "sign of surface normal (NRM)"
#line 3372 "MCNPParser.bison"
                         { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 5283 "MCNPParser.cpp"
    break;

  case 404: // SDEF_SCALAR_VALUE_KEY: "radial distance from the position from POS or AXS (RAD)"
#line 3372 "MCNPParser.bison"
                                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 5289 "MCNPParser.cpp"
    break;

  case 405: // SDEF_SCALAR_VALUE_KEY: "ext"
#line 3372 "MCNPParser.bison"
                                               { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 5295 "MCNPParser.cpp"
    break;

  case 406: // SDEF_SCALAR_VALUE_KEY: "cookie-cutter cell number (CCC)"
#line 3372 "MCNPParser.bison"
                                                     { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 5301 "MCNPParser.cpp"
    break;

  case 407: // SDEF_SCALAR_VALUE_KEY: "Area of surface (ARA)"
#line 3372 "MCNPParser.bison"
                                                                { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 5307 "MCNPParser.cpp"
    break;

  case 408: // SDEF_SCALAR_VALUE_KEY: WGT
#line 3373 "MCNPParser.bison"
                         { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 5313 "MCNPParser.cpp"
    break;

  case 409: // SDEF_SCALAR_VALUE_KEY: TR
#line 3373 "MCNPParser.bison"
                               { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 5319 "MCNPParser.cpp"
    break;

  case 410: // SDEF_SCALAR_VALUE_KEY: "rejection efficiency critera for position sampling (EFF)"
#line 3373 "MCNPParser.bison"
                                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 5325 "MCNPParser.cpp"
    break;

  case 411: // SDEF_SCALAR_VALUE_KEY: "x-coordinate of position (X)"
#line 3373 "MCNPParser.bison"
                                               { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 5331 "MCNPParser.cpp"
    break;

  case 412: // SDEF_SCALAR_VALUE_KEY: "y-coordinate of position (Y)"
#line 3373 "MCNPParser.bison"
                                                        { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 5337 "MCNPParser.cpp"
    break;

  case 413: // SDEF_SCALAR_VALUE_KEY: "z-coordinate of position (Z)"
#line 3373 "MCNPParser.bison"
                                                                 { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 5343 "MCNPParser.cpp"
    break;

  case 414: // SDEF_SCALAR_VALUE_KEY: "source particle type (PAR)"
#line 3373 "MCNPParser.bison"
                                                                          { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 5349 "MCNPParser.cpp"
    break;

  case 415: // SDEF_LIST_VALUE_KEY: "date to use for comic-ray and background sources (DAT)"
#line 3375 "MCNPParser.bison"
                       { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 5355 "MCNPParser.cpp"
    break;

  case 416: // SDEF_LIST_VALUE_KEY: "location of cosmic particle source (LOC)"
#line 3375 "MCNPParser.bison"
                                  { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 5361 "MCNPParser.cpp"
    break;

  case 417: // SDEF_LIST_VALUE_KEY: "Beam emittence parameters (BEM)"
#line 3375 "MCNPParser.bison"
                                             { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 5367 "MCNPParser.cpp"
    break;

  case 418: // SDEF_LIST_VALUE_KEY: "Beam aperature parameters (BAP)"
#line 3375 "MCNPParser.bison"
                                                        { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 5373 "MCNPParser.cpp"
    break;

  case 419: // SDEF_LIST_VALUE_KEY: "reference point for position sampling in vector notation (POS)"
#line 3375 "MCNPParser.bison"
                                                                   { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 5379 "MCNPParser.cpp"
    break;

  case 420: // SDEF_LIST_VALUE_KEY: VEC
#line 3375 "MCNPParser.bison"
                                                                              { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 5385 "MCNPParser.cpp"
    break;

  case 421: // SDEF_LIST_VALUE_KEY: AXS
#line 3375 "MCNPParser.bison"
                                                                                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 5391 "MCNPParser.cpp"
    break;

  case 422: // SDEF_LIST_VALUE_KEY: CEL
#line 3375 "MCNPParser.bison"
                                                                                          { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 5397 "MCNPParser.cpp"
    break;

  case 423: // sdef_value: value_list_item
#line 3377 "MCNPParser.bison"
             { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 5403 "MCNPParser.cpp"
    break;

  case 424: // sdef_value: SDEF_PREFIXED_VAR assign value
#line 3379 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    name = name.substr(1); // remove 'F' from prefix to obtain variable name
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                ,name.data()
                                ,{decl, (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
}
#line 5418 "MCNPParser.cpp"
    break;

  case 425: // sdef_value: SDEF_PREFIXED_VAR value
#line 3390 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    name = name.substr(1); // remove 'F' from prefix to obtain variable name
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                ,name.data()
                                ,{decl, (yystack_[0].value.node_index)});
}
#line 5433 "MCNPParser.cpp"
    break;

  case 426: // sdef_value_list: sdef_value
#line 3401 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t> ();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 5442 "MCNPParser.cpp"
    break;

  case 427: // sdef_value_list: sdef_value_list sdef_value
#line 3406 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 5451 "MCNPParser.cpp"
    break;

  case 428: // data_sdef_member: SDEF_SCALAR_VALUE_KEY assign sdef_value
#line 3414 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
 }
#line 5465 "MCNPParser.cpp"
    break;

  case 429: // data_sdef_member: SDEF_LIST_VALUE_KEY assign sdef_value_list
#line 3424 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 5483 "MCNPParser.cpp"
    break;

  case 430: // data_sdef_member: SDEF_SCALAR_VALUE_KEY sdef_value
#line 3438 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, (yystack_[0].value.node_index)});
 }
#line 5497 "MCNPParser.cpp"
    break;

  case 431: // data_sdef_member: SDEF_LIST_VALUE_KEY sdef_value_list
#line 3448 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 5514 "MCNPParser.cpp"
    break;

  case 432: // data_sdef_members: data_sdef_member
#line 3462 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 5523 "MCNPParser.cpp"
    break;

  case 433: // data_sdef_members: data_sdef_members data_sdef_member
#line 3467 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 5532 "MCNPParser.cpp"
    break;

  case 434: // data_sdef_card: "general source (SDEF)"
#line 3473 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"sdef"
                                ,{decl});
 }
#line 5543 "MCNPParser.cpp"
    break;

  case 435: // data_sdef_card: "general source (SDEF)" data_sdef_members
#line 3480 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));

    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"sdef"
                                ,*m);
    delete m;
 }
#line 5558 "MCNPParser.cpp"
    break;

  case 436: // data_si_card: "source information for distribution for n (SIn)" card_id "string" value_paren_colon_list
#line 3493 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    auto op = interpreter.push_leaf(wasp::mcnpi::DECL, "option", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), op);
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"si"
                                ,*m);
    delete m;
 }
#line 5575 "MCNPParser.cpp"
    break;

  case 437: // data_si_card: "source information for distribution for n (SIn)" card_id value_paren_colon_list
#line 3506 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"si"
                                ,*m);
    delete m;
 }
#line 5590 "MCNPParser.cpp"
    break;

  case 438: // data_sp_card: "source probability for distribution for n (SPn)" card_id number_token_list
#line 3518 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    // check for 'built-in' function flag '-f' (negative integer)
    auto fi = t->front();
    std::vector<size_t> m = {decl, (yystack_[1].value.node_index)};
    if (interpreter.token_data(fi)[0] == '-')
    {
        m.push_back(interpreter.push_leaf(wasp::mcnpi::FLAG, "function", fi));
        t->erase(t->begin()); // remove the function token
    }
    for (auto ti : *t)
    {
        m.push_back(interpreter.push_leaf(wasp::mcnpi::VALUE, "value", ti));
    }

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "sp", m);
    delete t;
 }
#line 5614 "MCNPParser.cpp"
    break;

  case 439: // data_sp_card: "source probability for distribution for n (SPn)" card_id "string" number_list
#line 3538 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    auto op = interpreter.push_leaf(wasp::mcnpi::DECL, "option", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), op);
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"sp"
                                ,*m);
    delete m;
 }
#line 5631 "MCNPParser.cpp"
    break;

  case 440: // data_sp_card: "source probability for distribution for n (SPn)" card_id "string"
#line 3551 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto op = interpreter.push_leaf(wasp::mcnpi::DECL, "option", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"sp"
                                ,{decl, (yystack_[1].value.node_index), op});
 }
#line 5643 "MCNPParser.cpp"
    break;

  case 441: // data_sb_card: "source bias for distribution for n (SBn)" card_id number_token_list
#line 3560 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    // check for 'built-in' function flag '-f' (negative integer)
    auto fi = t->front();
    std::vector<size_t> m = {decl, (yystack_[1].value.node_index)};
    if (interpreter.token_data(fi)[0] == '-')
    {
        m.push_back(interpreter.push_leaf(wasp::mcnpi::FLAG, "function", fi));
        t->erase(t->begin()); // remove the function token
    }
    for (auto ti : *t)
    {
        m.push_back(interpreter.push_leaf(wasp::mcnpi::VALUE, "value", ti));
    }

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "sb", m);
    delete t;
 }
#line 5667 "MCNPParser.cpp"
    break;

  case 442: // data_sb_card: "source bias for distribution for n (SBn)" card_id "string" number_list
#line 3580 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    auto op = interpreter.push_leaf(wasp::mcnpi::DECL, "option", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), op);
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"sb"
                                ,*m);
    delete m;
 }
#line 5684 "MCNPParser.cpp"
    break;

  case 443: // data_totnu_card: "total fission card (TOTNU)"
#line 3594 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"totnu"
                                ,{decl});
 }
#line 5695 "MCNPParser.cpp"
    break;

  case 444: // data_totnu_card: "total fission card (TOTNU)" "no"
#line 3601 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto n = interpreter.push_leaf(wasp::mcnpi::FLAG, "no", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"totnu"
                                ,{decl, n});
 }
#line 5707 "MCNPParser.cpp"
    break;

  case 445: // data_de_card: "dose energy card (DEn)" card_id number_list
#line 3611 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto t = (yystack_[0].value.node_indices);
    t->insert(t->begin(),(yystack_[1].value.node_index));
    t->insert(t->begin(),decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "de", *t);
    delete t;
 }
#line 5720 "MCNPParser.cpp"
    break;

  case 446: // data_de_card: "dose energy card (DEn)" card_id "string" number_list
#line 3620 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    auto interp = interpreter.push_leaf(wasp::mcnpi::FLAG, "interp", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.node_indices);
    t->insert(t->begin(),interp);
    t->insert(t->begin(),(yystack_[2].value.node_index));
    t->insert(t->begin(),decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "de", *t);
    delete t;
 }
#line 5735 "MCNPParser.cpp"
    break;

  case 447: // data_df_card: "dose function card (DFn)" card_id number_list
#line 3632 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto t = (yystack_[0].value.node_indices);
    t->insert(t->begin(),(yystack_[1].value.node_index));
    t->insert(t->begin(),decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "de", *t);
    delete t;
 }
#line 5748 "MCNPParser.cpp"
    break;

  case 448: // data_df_card: "dose function card (DFn)" card_id "string" number_list
#line 3641 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    auto interp = interpreter.push_leaf(wasp::mcnpi::FLAG, "interp", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.node_indices);
    t->insert(t->begin(),interp);
    t->insert(t->begin(),(yystack_[2].value.node_index));
    t->insert(t->begin(),decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "df", *t);
    delete t;
 }
#line 5763 "MCNPParser.cpp"
    break;

  case 449: // data_tmp_card: "tmp" card_id number_list
#line 3653 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto t = (yystack_[0].value.node_indices);
    t->insert(t->begin(),(yystack_[1].value.node_index));
    t->insert(t->begin(),decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "tmp", *t);
    delete t;
 }
#line 5776 "MCNPParser.cpp"
    break;

  case 450: // data_tmp_card: "tmp" number_list
#line 3662 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.node_indices);
    t->insert(t->begin(),decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "tmp", *t);
    delete t;
 }
#line 5788 "MCNPParser.cpp"
    break;

  case 451: // data_fs_card: "tally segment card (FSn)" card_id number_list "string" "string"
#line 3671 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[4].value.token_index));
    auto t = (yystack_[2].value.node_indices);
    t->insert(t->begin(),(yystack_[3].value.node_index));
    t->insert(t->begin(),decl);

    std::string o1 = interpreter.token_data((yystack_[1].value.token_index));
    std::string o2 = interpreter.token_data((yystack_[0].value.token_index));
    if (o1.find_first_of("tT") == std::string::npos)
        o1 = "total";
    else o1 = "cumulative";

    if (o2.find_first_of("tT") == std::string::npos)
        o2 = "total";
    else o2 = "cumulative";

    auto o1i = interpreter.push_leaf(wasp::mcnpi::FLAG, o1.data(), (yystack_[1].value.token_index));
    auto o2i = interpreter.push_leaf(wasp::mcnpi::FLAG, o2.data(), (yystack_[0].value.token_index));

    t->push_back(o1i);
    t->push_back(o2i);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "fs", *t);
    delete t;
 }
#line 5817 "MCNPParser.cpp"
    break;

  case 452: // data_fs_card: "tally segment card (FSn)" card_id number_list "string"
#line 3696 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    auto t = (yystack_[1].value.node_indices);
    t->insert(t->begin(),(yystack_[2].value.node_index));
    t->insert(t->begin(),decl);

    std::string o1 = interpreter.token_data((yystack_[0].value.token_index));
    if (o1.find_first_of("tT") == std::string::npos)
        o1 = "total";
    else o1 = "cumulative";

    auto o1i = interpreter.push_leaf(wasp::mcnpi::FLAG, o1.data(), (yystack_[0].value.token_index));

    t->push_back(o1i);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "fs", *t);
    delete t;
 }
#line 5839 "MCNPParser.cpp"
    break;

  case 453: // data_fs_card: "tally segment card (FSn)" card_id number_list
#line 3714 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto t = (yystack_[0].value.node_indices);
    t->insert(t->begin(),(yystack_[1].value.node_index));
    t->insert(t->begin(),decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "fs", *t);
    delete t;
 }
#line 5852 "MCNPParser.cpp"
    break;

  case 454: // data_fs_card: "tally segment card (FSn)" card_id colon particle_symbol number_list
#line 3723 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[4].value.token_index));
    auto t = (yystack_[0].value.node_indices);
    t->insert(t->begin(),(yystack_[1].value.node_index));
    t->insert(t->begin(),(yystack_[2].value.node_index));
    t->insert(t->begin(),(yystack_[3].value.node_index));
    t->insert(t->begin(),decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "fs", *t);
    delete t;
 }
#line 5867 "MCNPParser.cpp"
    break;

  case 455: // data_fs_card: "tally segment card (FSn)" number_list
#line 3734 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.node_indices);
    t->insert(t->begin(),decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "fs", *t);
    delete t;
 }
#line 5879 "MCNPParser.cpp"
    break;

  case 456: // data_tf_card: "tally fluctation card (TFn)" card_id number_list
#line 3743 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto t = (yystack_[0].value.node_indices);
    t->insert(t->begin(),(yystack_[1].value.node_index));
    t->insert(t->begin(),decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "tf", *t);
    delete t;
 }
#line 5892 "MCNPParser.cpp"
    break;

  case 457: // data_lost_card: "lost particle card (LOST)" number_list
#line 3753 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.node_indices);
    t->insert(t->begin(),decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "lost", *t);
    delete t;
 }
#line 5904 "MCNPParser.cpp"
    break;

  case 458: // data_hsrc_card: "shannon entropy card (HSRC)" number_token_list
#line 3762 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::vector<size_t> children = {decl};
    auto t = (yystack_[0].value.token_indices);
    assign_names(interpreter,
                            {"nx", "xmin", "xmax",
                            "ny", "ymin", "ymax",
                            "nz", "zmin", "zmax"},
                            {mcnpi::VALUE, mcnpi::VALUE, mcnpi::VALUE,
                            mcnpi::VALUE, mcnpi::VALUE, mcnpi::VALUE,
                            mcnpi::VALUE, mcnpi::VALUE, mcnpi::VALUE}, t, children);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "hsrc", children);
    delete t;
}
#line 5923 "MCNPParser.cpp"
    break;

  case 459: // data_xs_card: "crosssection" card_id material_zaid_entry value_token_list
#line 3778 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    std::vector<size_t> children = {decl,(yystack_[2].value.node_index), (yystack_[1].value.node_index)};
    auto t = (yystack_[0].value.token_indices);

    std::vector<std::string> node_names = {"table_name", "awr", "file_name",
                                        "address_route",  "file_type", "table_length",
                                        "record_length", "ner", "temp", "ptf"};
    if (t->size() > 1)
    {
        // check if ZAID.abx was specified and promote as needed
        // at(0) = INTEGER, at(1) == LIB_ID
        if (interpreter.token_type(t->at(1)) == wasp::mcnpi::LIB_ID)
        {
            auto id = interpreter.push_leaf(wasp::mcnpi::ID, "id", t->at(0));
            auto lib = interpreter.push_leaf(wasp::mcnpi::LIB_ID, "lib", t->at(1));
            auto p = interpreter.push_parent(wasp::mcnpi::ZAID
                                ,node_names.front().data()
                                ,{id, lib});
            children.push_back(p);
            node_names.erase(node_names.begin());
            t->erase(t->begin(), t->begin()+2); // remove zaid and abx tokens
        }
    }
    std::vector<wasp::mcnpi::NODE> node_types(node_names.size(), mcnpi::VALUE);
    assign_names(interpreter, node_names, node_types, t, children);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "xs", children);
    delete t;
}
#line 5957 "MCNPParser.cpp"
    break;

  case 460: // data_phys_card: "particle physics options card (PHYS)" colon particle_symbol number_token_list
#line 3809 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    std::vector<size_t> children = {decl,(yystack_[2].value.node_index),(yystack_[1].value.node_index)};
    auto t = (yystack_[0].value.token_indices);
    std::string ps = interpreter.data((yystack_[1].value.node_index));
    std::vector<std::string> node_names;
    std::vector<mcnpi::NODE> node_types;
    wasp_check(ps.size() == 1); // particle type should be single character per lexer rule
    switch (ps.front())
    {
        case 'n':
        case 'N':
            node_names = {"emax", "emcnf", "iunr", "unused", "unused", "unused","coilf",
                         "ngam", "unused", "unused", "i_int_model", "i_els_model"};
            node_types = std::vector<mcnpi::NODE>(node_names.size(), mcnpi::VALUE);
            break;
        case 'p':
        case 'P':
            node_names = {"emcpf", "ides", "nocoh", "ispn", "nodop", "unused","fism"};
            node_types = std::vector<mcnpi::NODE>(node_names.size(), mcnpi::VALUE);
            break;
        case 'e':
        case 'E':
            node_names = {"emax", "ides", "iphot", "ibad", "istrg", "bnum",
                      "xnum",  "rnok", "enum", "numb",
                      "i_mcs_model", "unused", "unused", "efac",
                      "electron_method_boundary", "ckvnum"};
            node_types = std::vector<mcnpi::NODE>(node_names.size(), mcnpi::VALUE);
            break;
        case 'h':
        case 'H':
            node_names = {"emax", "ean", "tabl", "unused", "istrg", "unused",
                      "recl",  "unused", "unused", "unused",
                      "i_mcs_model", "i_int_model", "i_els_model", "efac",
                      "unused", "ckvnum", "drp"};
            node_types = std::vector<mcnpi::NODE>(node_names.size(), mcnpi::VALUE);
            break;
        default:
            node_names = {"emax", "unused", "unused", "unused", "istrg",
                      "unused", "xmunum", "xmugam", "unused", "unused",
                      "i_mcs_model", "i_int_model", "i_els_model", "efac",
                      "unused", "ckvnum", "drp"};
            node_types = std::vector<mcnpi::NODE>(node_names.size(), mcnpi::VALUE);
            break;
    }
    assign_names(interpreter, node_names, node_types, t, children);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "phys", children);
    delete t;
}
#line 6011 "MCNPParser.cpp"
    break;

  case 461: // KOPTS_OPTION: "number of cycles in every outer iteration (BLOCKSIZE=n)"
#line 3859 "MCNPParser.bison"
                { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6017 "MCNPParser.cpp"
    break;

  case 462: // KOPTS_OPTION: "calculate point-kinetic parameters (KINETICS=YES/NO)"
#line 3859 "MCNPParser.bison"
                            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6023 "MCNPParser.cpp"
    break;

  case 463: // KOPTS_OPTION: "calculate detailed precursor information (PRECURSOR=YES/NO)"
#line 3859 "MCNPParser.bison"
                                       { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6029 "MCNPParser.cpp"
    break;

  case 464: // KOPTS_OPTION: "format of senstitivity profile output file (KSENTAL)"
#line 3859 "MCNPParser.bison"
                                                   { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6035 "MCNPParser.cpp"
    break;

  case 465: // KOPTS_OPTION: "turn on FMAT (FMAT=YES/NO)"
#line 3859 "MCNPParser.bison"
                                                             { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6041 "MCNPParser.cpp"
    break;

  case 466: // KOPTS_OPTION: "FMATSKIP=n"
#line 3860 "MCNPParser.bison"
                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6047 "MCNPParser.cpp"
    break;

  case 467: // KOPTS_OPTION: "FMATSPACE=s"
#line 3860 "MCNPParser.bison"
                               { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6053 "MCNPParser.cpp"
    break;

  case 468: // KOPTS_OPTION: "FMATACCEL=YES/NO"
#line 3860 "MCNPParser.bison"
                                           { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6059 "MCNPParser.cpp"
    break;

  case 469: // KOPTS_OPTION: "FMATREDUCE=YES/NO"
#line 3860 "MCNPParser.bison"
                                                       { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6065 "MCNPParser.cpp"
    break;

  case 470: // KOPTS_OPTION: FMATNX
#line 3860 "MCNPParser.bison"
                                                                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6071 "MCNPParser.cpp"
    break;

  case 471: // KOPTS_OPTION: FMATNY
#line 3861 "MCNPParser.bison"
                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6077 "MCNPParser.cpp"
    break;

  case 472: // KOPTS_OPTION: FMATNZ
#line 3861 "MCNPParser.bison"
                             { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6083 "MCNPParser.cpp"
    break;

  case 473: // data_kopts_option: KOPTS_OPTION value
#line 3864 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, name.data(), {decl, (yystack_[0].value.node_index)});
}
#line 6095 "MCNPParser.cpp"
    break;

  case 474: // data_kopts_option: KOPTS_OPTION assign value
#line 3872 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, name.data(), {decl, (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
}
#line 6107 "MCNPParser.cpp"
    break;

  case 475: // data_kopts_options: data_kopts_option
#line 3881 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t> ();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 6116 "MCNPParser.cpp"
    break;

  case 476: // data_kopts_options: data_kopts_options data_kopts_option
#line 3886 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 6125 "MCNPParser.cpp"
    break;

  case 477: // data_kopts_card: "criticality calculation options card (KOPTS)" data_kopts_options
#line 3893 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.node_indices);
    t->insert(t->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "kopts", *t);
    delete t;
}
#line 6137 "MCNPParser.cpp"
    break;

  case 478: // data_tally_cosine_card: "tally cosine card (Cn)" card_id number_list
#line 3903 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_cosine"
                                ,*m);
    delete m;
 }
#line 6152 "MCNPParser.cpp"
    break;

  case 479: // data_tally_cosine_card: "tally cosine card (Cn)" card_id number_list "string" "string"
#line 3914 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[4].value.token_index));

    auto m = (yystack_[2].value.node_indices);
    m->insert(m->begin(), (yystack_[3].value.node_index));
    m->insert(m->begin(), decl);

    std::string o1 = interpreter.token_data((yystack_[1].value.token_index));
    std::string o2 = interpreter.token_data((yystack_[0].value.token_index));
    if (o1.size() > 1)
        o1 = "no_total";
    else o1 = "cumulative";

    if (o2.size() > 1)
        o2 = "no_total";
    else o2 = "cumulative";

    auto o1i = interpreter.push_leaf(wasp::mcnpi::DECL, o1.data(), (yystack_[1].value.token_index));
    auto o2i = interpreter.push_leaf(wasp::mcnpi::DECL, o2.data(), (yystack_[0].value.token_index));


    m->push_back(o1i);
    m->push_back(o2i);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_cosine"
                                ,*m);
    delete m;
 }
#line 6185 "MCNPParser.cpp"
    break;

  case 480: // data_tally_cosine_card: "tally cosine card (Cn)" card_id number_list "string"
#line 3943 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));

    auto m = (yystack_[1].value.node_indices);
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), decl);

    std::string o1 = interpreter.token_data((yystack_[0].value.token_index));

    if (o1.size() > 1)
        o1 = "no_total";
    else o1 = "cumulative";

    auto o1i = interpreter.push_leaf(wasp::mcnpi::DECL, o1.data(), (yystack_[0].value.token_index));

    m->push_back(o1i);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_cosine"
                                ,*m);
    delete m;
 }
#line 6211 "MCNPParser.cpp"
    break;

  case 481: // data_tally_cosine_card: "*" data_tally_cosine_card
#line 3965 "MCNPParser.bison"
 {
    auto a = interpreter.push_leaf(wasp::mcnpi::FLAG, "degrees", (yystack_[1].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"c_degrees"
                                ,{a, (yystack_[0].value.node_index)});
 }
#line 6222 "MCNPParser.cpp"
    break;

  case 482: // data_mgopt_card: "multigroup adjoint transport option card (MGOPT)" "string" number_token_list
#line 3973 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto mcal = interpreter.push_leaf(wasp::mcnpi::FLAG, "mcal", (yystack_[1].value.token_index));
    std::vector<size_t> children = {decl, mcal};
    auto t = (yystack_[0].value.token_indices);
    std::vector<std::string> node_names = {"igm", "iplt", "isb", "icw", "fnw", "rim"};
    std::vector<mcnpi::NODE> node_types(node_names.size(), wasp::mcnpi::VALUE);
    assign_names(interpreter, node_names, node_types, t, children);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "mgopt", children);
    delete t;
}
#line 6238 "MCNPParser.cpp"
    break;

  case 483: // RAND_OPTION: "random number generator type (GEN)"
#line 3986 "MCNPParser.bison"
               { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6244 "MCNPParser.cpp"
    break;

  case 484: // RAND_OPTION: "random number generator seed (SEED)"
#line 3986 "MCNPParser.bison"
                     { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6250 "MCNPParser.cpp"
    break;

  case 485: // RAND_OPTION: "number of random numbers between source particles (STRIDE)"
#line 3986 "MCNPParser.bison"
                            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6256 "MCNPParser.cpp"
    break;

  case 486: // RAND_OPTION: HIST
#line 3986 "MCNPParser.bison"
                                     { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6262 "MCNPParser.cpp"
    break;

  case 487: // data_rand_card_option: RAND_OPTION value
#line 3989 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, name.data(), {decl, (yystack_[0].value.node_index)});
}
#line 6274 "MCNPParser.cpp"
    break;

  case 488: // data_rand_card_option: RAND_OPTION assign value
#line 3997 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, name.data(), {decl, (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
}
#line 6286 "MCNPParser.cpp"
    break;

  case 489: // data_rand_card_options: data_rand_card_option
#line 4006 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t> ();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 6295 "MCNPParser.cpp"
    break;

  case 490: // data_rand_card_options: data_rand_card_options data_rand_card_option
#line 4011 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 6304 "MCNPParser.cpp"
    break;

  case 491: // data_rand_card: "random number generation card (RAND)" data_rand_card_options
#line 4018 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.node_indices);
    t->insert(t->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "rand", *t);
    delete t;
}
#line 6316 "MCNPParser.cpp"
    break;

  case 492: // data_rand_card: "random number generation card (RAND)"
#line 4026 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "rand", {decl});
}
#line 6325 "MCNPParser.cpp"
    break;

  case 493: // data_wwe_card: "weight-window energy or time intervals (WWE)" colon particle_symbol number_list
#line 4032 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"wwe"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
 }
#line 6340 "MCNPParser.cpp"
    break;

  case 494: // data_wwg_card: "weight-window generation card (WWG)" number_token_list
#line 4044 "MCNPParser.bison"
 {
    // it ic wg J J J J iE
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    static std::vector<std::string> names = {"it", "ic", "wg", "unused", "unused", "unused", "unused", "iE"};
    static std::vector<mcnpi::NODE> types(names.size(), wasp::mcnpi::VALUE);
    std::vector<size_t> children;
    children.push_back(decl);
    assign_names(interpreter, names, types, (yystack_[0].value.token_indices), children);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"wwg"
                                ,children);
    delete (yystack_[0].value.token_indices);
 }
#line 6359 "MCNPParser.cpp"
    break;

  case 495: // data_wwge_card: "weight-window generation energy or time bounds (WWGE)" colon particle_symbol number_list
#line 4060 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"wwge"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
 }
#line 6374 "MCNPParser.cpp"
    break;

  case 496: // data_wwgt_card: "weight-window generation time bounds (WWGT)" colon particle_symbol number_list
#line 4072 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"wwgt"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
 }
#line 6389 "MCNPParser.cpp"
    break;

  case 497: // data_wwn_card: "cell-based lower weight-window bounds (WWNi)" card_id colon particle_symbol number_list
#line 4084 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[4].value.token_index));

    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[3].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"wwn"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
 }
#line 6407 "MCNPParser.cpp"
    break;

  case 498: // data_wwn_card: "cell-based lower weight-window bounds (WWNi)" colon particle_symbol number_list
#line 4098 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));

    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"wwn"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
 }
#line 6424 "MCNPParser.cpp"
    break;

  case 499: // data_wwp_card: "weight-window parameter card (WWP)" colon particle_symbol_list number_token_list
#line 4112 "MCNPParser.bison"
 {
    // wupn wsurvn mxspln mwhere switchn mtime wnorm etsplt wu
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    auto m = (yystack_[1].value.node_indices);
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), decl);
    static std::vector<std::string> names = {"wupn", "wsurvn", "mxspln", "mwhere", "switchn", "mtime", "wnorm", "etsplt", "wu"};
    static std::vector<mcnpi::NODE> types(names.size(), wasp::mcnpi::VALUE);
    assign_names(interpreter, names, types, (yystack_[0].value.token_indices), *m);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"wwp"
                                ,*m);
    delete m;
    delete (yystack_[0].value.token_indices);
 }
#line 6445 "MCNPParser.cpp"
    break;

  case 500: // data_wwt_card: "weight-window time intervals (WWT)" colon particle_symbol number_list
#line 4130 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"wwt"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
 }
#line 6460 "MCNPParser.cpp"
    break;

  case 501: // data_dbcn_card: "debug information card (DBCN)" number_token_list
#line 4142 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::vector<size_t> children; children.push_back(decl);
    auto t = (yystack_[0].value.token_indices);
    std::vector<std::string> node_names(100); for (int i = 1; i <= 100; i++)node_names[i-1] = "x"+std::to_string(i);
    std::vector<wasp::mcnpi::NODE> node_types(node_names.size(), mcnpi::VALUE);
    assign_names(interpreter, node_names, node_types, t, children);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"dbcn"
                                ,children);
    delete t;
 }
#line 6477 "MCNPParser.cpp"
    break;

  case 502: // data_fcl_card: "forced-collision card (FCL)" colon particle_symbol number_list
#line 4156 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fcl"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
 }
#line 6492 "MCNPParser.cpp"
    break;

  case 503: // data_var_card: "variance reduction control card (VAR)" "russion roulette key (RR)" assign value
#line 4168 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    auto rd = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto rr = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE, "rr", {rd, (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"var"
                                ,{decl, rr});
}
#line 6505 "MCNPParser.cpp"
    break;

  case 504: // data_var_card: "variance reduction control card (VAR)" "russion roulette key (RR)" value
#line 4177 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto rd = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto rr = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE, "rr", {rd, (yystack_[0].value.node_index)});
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"var"
                                ,{decl, rr});
}
#line 6518 "MCNPParser.cpp"
    break;

  case 505: // data_cf_card: "cell-flagging card (CFn)" card_id number_list
#line 4186 "MCNPParser.bison"
                                                 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"cf"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 6532 "MCNPParser.cpp"
    break;

  case 506: // data_ext_card: EXT_CARD colon particle_symbol value_token_list
#line 4198 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl, (yystack_[2].value.node_index), (yystack_[1].value.node_index)};
    for (auto ti : *t) children.push_back(interpreter.push_leaf(wasp::mcnpi::VALUE, "value", ti));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ext"
                                ,children);
    delete t;
 }
#line 6547 "MCNPParser.cpp"
    break;

  case 507: // data_vect_member: "Vn" number number number
#line 4211 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"vn"
                                ,{decl, (yystack_[2].value.node_index), (yystack_[2].value.node_index), (yystack_[1].value.node_index)});
}
#line 6558 "MCNPParser.cpp"
    break;

  case 508: // data_vect_members: data_vect_member
#line 4219 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 6567 "MCNPParser.cpp"
    break;

  case 509: // data_vect_members: data_vect_members data_vect_member
#line 4224 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 6576 "MCNPParser.cpp"
    break;

  case 510: // data_vect_card: "vector input card (VECT)" data_vect_members
#line 4230 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.node_indices);
    t->insert(t->begin(), decl);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"vect"
                                ,*t);
    delete t;
 }
#line 6591 "MCNPParser.cpp"
    break;

  case 511: // data_esplt_pair: NUMBER NUMBER
#line 4242 "MCNPParser.bison"
 {
    auto r = interpreter.push_leaf(wasp::mcnpi::VALUE, "r", (yystack_[1].value.token_index));
    auto e = interpreter.push_leaf(wasp::mcnpi::VALUE, "e", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::PAIR
                                ,"re"
                                ,{r,e});
 }
#line 6603 "MCNPParser.cpp"
    break;

  case 512: // data_esplt_pairs: data_esplt_pair
#line 4251 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 6612 "MCNPParser.cpp"
    break;

  case 513: // data_esplt_pairs: data_esplt_pairs data_esplt_pair
#line 4256 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 6621 "MCNPParser.cpp"
    break;

  case 514: // data_esplt_card: "energy splitting and roulette card (ESPLT)" colon particle_symbol data_esplt_pairs
#line 4262 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    auto t = (yystack_[0].value.node_indices);
    t->insert(t->begin(), (yystack_[1].value.node_index));
    t->insert(t->begin(), (yystack_[2].value.node_index));
    t->insert(t->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"esplt"
                                ,*t);
    delete t;
 }
#line 6637 "MCNPParser.cpp"
    break;

  case 515: // data_tsplt_pair: NUMBER NUMBER
#line 4275 "MCNPParser.bison"
 {
    auto t = interpreter.push_leaf(wasp::mcnpi::VALUE, "t", (yystack_[1].value.token_index));
    auto e = interpreter.push_leaf(wasp::mcnpi::VALUE, "e", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::PAIR
                                ,"te"
                                ,{t,e});
 }
#line 6649 "MCNPParser.cpp"
    break;

  case 516: // data_tsplt_pairs: data_tsplt_pair
#line 4284 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 6658 "MCNPParser.cpp"
    break;

  case 517: // data_tsplt_pairs: data_tsplt_pairs data_tsplt_pair
#line 4289 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 6667 "MCNPParser.cpp"
    break;

  case 518: // data_tsplt_card: "time splitting and roulette card (TSPLT)" colon particle_symbol data_tsplt_pairs
#line 4295 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    auto t = (yystack_[0].value.node_indices);
    t->insert(t->begin(), (yystack_[1].value.node_index));
    t->insert(t->begin(), (yystack_[2].value.node_index));
    t->insert(t->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tsplt"
                                ,*t);
    delete t;
 }
#line 6683 "MCNPParser.cpp"
    break;

  case 519: // data_dd_card: "detector diagnostics card (DD)" number_list
#line 4308 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.node_indices);
    t->insert(t->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"dd"
                                ,*t);
    delete t;
 }
#line 6697 "MCNPParser.cpp"
    break;

  case 520: // data_dd_card: "detector diagnostics card (DD)" card_id number_list
#line 4318 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto t = (yystack_[0].value.node_indices);
    t->insert(t->begin(), (yystack_[1].value.node_index));
    t->insert(t->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"dd"
                                ,*t);
    delete t;
}
#line 6712 "MCNPParser.cpp"
    break;

  case 521: // data_dxt_card: "dxtran card (DXT)" colon particle_symbol number_token_list
#line 4330 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    std::vector<size_t> children = {decl, (yystack_[2].value.node_index), (yystack_[1].value.node_index)};
    size_t i = 0;
    auto t = (yystack_[0].value.token_indices);
    // process xi,yi,zi,rii,roi
    for( ;i < t->size(); )
    {
        if (i < t->size()) children.push_back(interpreter.push_leaf(wasp::mcnpi::VALUE, "x", t->at(i)));
        ++i;
        if (i < t->size()) children.push_back(interpreter.push_leaf(wasp::mcnpi::VALUE, "y", t->at(i)));
        ++i;
        if (i < t->size()) children.push_back(interpreter.push_leaf(wasp::mcnpi::VALUE, "z", t->at(i)));
        ++i;
        if (i < t->size()) children.push_back(interpreter.push_leaf(wasp::mcnpi::VALUE, "ri", t->at(i)));
        ++i;
        if (i < t->size()) children.push_back(interpreter.push_leaf(wasp::mcnpi::VALUE, "ro", t->at(i)));
        ++i;
        // if there are fewer than 5 elements left,
        // it is the remaining dwc1,dwc2,dpwt elements,
        // break out of the
        if (t->size() < i+5) //
        {
            break;
        }
    }
    // process last dxtran cards

    if (t->size() < i+5) //
    {
        if (i < t->size()) children.push_back(interpreter.push_leaf(wasp::mcnpi::VALUE, "dwc1", t->at(i)));
        ++i;
        if (i < t->size()) children.push_back(interpreter.push_leaf(wasp::mcnpi::VALUE, "dwc2", t->at(i)));
        ++i;
        if (i < t->size()) children.push_back(interpreter.push_leaf(wasp::mcnpi::VALUE, "dpwt", t->at(i)));
        ++i;
    }
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"dxt"
                                ,children);
    delete (yystack_[0].value.token_indices);
 }
#line 6759 "MCNPParser.cpp"
    break;

  case 522: // data_cm_card: "cosine multiplier card (CMn)" card_id number_list
#line 4374 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"cm"
                                ,*m);
    delete m;
 }
#line 6774 "MCNPParser.cpp"
    break;

  case 523: // data_cm_card: "*" "cosine multiplier card (CMn)" card_id number_list
#line 4385 "MCNPParser.bison"
 {
    auto f = interpreter.push_leaf(wasp::mcnpi::FLAG, "flag", (yystack_[3].value.token_index));
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    m->insert(m->begin(), f);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"cm"
                                ,*m);
    delete m;
 }
#line 6791 "MCNPParser.cpp"
    break;

  case 524: // FT_MEMBER_KEY: FRV
#line 4398 "MCNPParser.bison"
                { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6797 "MCNPParser.cpp"
    break;

  case 525: // FT_MEMBER_KEY: GEB
#line 4398 "MCNPParser.bison"
                      { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6803 "MCNPParser.cpp"
    break;

  case 526: // FT_MEMBER_KEY: TMC
#line 4398 "MCNPParser.bison"
                          { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6809 "MCNPParser.cpp"
    break;

  case 527: // FT_MEMBER_KEY: INC
#line 4398 "MCNPParser.bison"
                              { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6815 "MCNPParser.cpp"
    break;

  case 528: // FT_MEMBER_KEY: ICD
#line 4398 "MCNPParser.bison"
                                  { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6821 "MCNPParser.cpp"
    break;

  case 529: // FT_MEMBER_KEY: SCX
#line 4398 "MCNPParser.bison"
                                      { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6827 "MCNPParser.cpp"
    break;

  case 530: // FT_MEMBER_KEY: SCD
#line 4398 "MCNPParser.bison"
                                          { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6833 "MCNPParser.cpp"
    break;

  case 531: // FT_MEMBER_KEY: ELC
#line 4398 "MCNPParser.bison"
                                              { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6839 "MCNPParser.cpp"
    break;

  case 532: // FT_MEMBER_KEY: PTT
#line 4398 "MCNPParser.bison"
                                                  { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6845 "MCNPParser.cpp"
    break;

  case 533: // FT_MEMBER_KEY: PHL
#line 4398 "MCNPParser.bison"
                                                      { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6851 "MCNPParser.cpp"
    break;

  case 534: // FT_MEMBER_KEY: CAP
#line 4398 "MCNPParser.bison"
                                                          { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6857 "MCNPParser.cpp"
    break;

  case 535: // FT_MEMBER_KEY: RES
#line 4398 "MCNPParser.bison"
                                                              { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6863 "MCNPParser.cpp"
    break;

  case 536: // FT_MEMBER_KEY: TAG
#line 4398 "MCNPParser.bison"
                                                                  { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6869 "MCNPParser.cpp"
    break;

  case 537: // FT_MEMBER_KEY: LET
#line 4398 "MCNPParser.bison"
                                                                      { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6875 "MCNPParser.cpp"
    break;

  case 538: // FT_MEMBER_KEY: ROC
#line 4398 "MCNPParser.bison"
                                                                          { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6881 "MCNPParser.cpp"
    break;

  case 539: // FT_MEMBER_KEY: PDS
#line 4399 "MCNPParser.bison"
             { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6887 "MCNPParser.cpp"
    break;

  case 540: // FT_MEMBER_KEY: FFT
#line 4399 "MCNPParser.bison"
                 { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6893 "MCNPParser.cpp"
    break;

  case 541: // FT_MEMBER_KEY: "com"
#line 4399 "MCNPParser.bison"
                     { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 6899 "MCNPParser.cpp"
    break;

  case 542: // data_ft_member: FT_MEMBER_KEY number_list
#line 4401 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 6916 "MCNPParser.cpp"
    break;

  case 543: // data_ft_member: FT_MEMBER_KEY
#line 4414 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[0].value.token_index));
    std::string name = interpreter.token_data((yystack_[0].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl});
 }
#line 6930 "MCNPParser.cpp"
    break;

  case 544: // data_ft_members: data_ft_member
#line 4425 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 6939 "MCNPParser.cpp"
    break;

  case 545: // data_ft_members: data_ft_members data_ft_member
#line 4430 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 6948 "MCNPParser.cpp"
    break;

  case 546: // data_ft_card: "special treatment tallies card (FT)" card_id data_ft_members
#line 4436 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ft"
                                ,*m);
    delete m;
 }
#line 6963 "MCNPParser.cpp"
    break;

  case 547: // data_sf_card: "surface-flagging card (SF)" card_id number_list
#line 4448 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"sf"
                                ,*m);
    delete m;
 }
#line 6978 "MCNPParser.cpp"
    break;

  case 548: // data_ds_card: "dependent source distribution card (DS)" card_id "string" value_list
#line 4460 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    auto op = interpreter.push_leaf(wasp::mcnpi::DECL, "option", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), op);
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ds"
                                ,*m);
    delete m;
 }
#line 6995 "MCNPParser.cpp"
    break;

  case 549: // data_fu_card: "tallyx card (FU)" card_id
#line 4474 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fu"
                                ,{decl, (yystack_[0].value.node_index)});
 }
#line 7006 "MCNPParser.cpp"
    break;

  case 550: // data_fu_card: "tallyx card (FU)" card_id number_list
#line 4481 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_time"
                                ,*m);
    delete m;
 }
#line 7021 "MCNPParser.cpp"
    break;

  case 551: // data_fu_card: "tallyx card (FU)" card_id number_list "string" "string"
#line 4492 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[4].value.token_index));

    auto m = (yystack_[2].value.node_indices);
    m->insert(m->begin(), (yystack_[3].value.node_index));
    m->insert(m->begin(), decl);

    std::string o1 = interpreter.token_data((yystack_[1].value.token_index));
    std::string o2 = interpreter.token_data((yystack_[0].value.token_index));
    if (o1.size() > 1)
        o1 = "no_total";
    else o1 = "cumulative";

    if (o2.size() > 1)
        o2 = "no_total";
    else o2 = "cumulative";

    auto o1i = interpreter.push_leaf(wasp::mcnpi::DECL, o1.data(), (yystack_[1].value.token_index));
    auto o2i = interpreter.push_leaf(wasp::mcnpi::DECL, o2.data(), (yystack_[0].value.token_index));


    m->push_back(o1i);
    m->push_back(o2i);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fu"
                                ,*m);
    delete m;
 }
#line 7054 "MCNPParser.cpp"
    break;

  case 552: // data_fu_card: "tallyx card (FU)" card_id number_list "string"
#line 4521 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));

    auto m = (yystack_[1].value.node_indices);
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), decl);

    std::string o1 = interpreter.token_data((yystack_[0].value.token_index));

    if (o1.size() > 1)
        o1 = "no_total";
    else o1 = "cumulative";

    auto o1i = interpreter.push_leaf(wasp::mcnpi::DECL, o1.data(), (yystack_[0].value.token_index));

    m->push_back(o1i);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fu"
                                ,*m);
    delete m;
 }
#line 7080 "MCNPParser.cpp"
    break;

  case 553: // data_em_card: "energy multiplier card (EM)" card_id number_list
#line 4544 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"em"
                                ,*m);
    delete m;
 }
#line 7095 "MCNPParser.cpp"
    break;

  case 554: // data_bbrem_card: "bremsstrahlung biasing card (BBREM)" number_token_list
#line 4556 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto t = (yystack_[0].value.token_indices);
    std::vector<size_t> children = {decl};
    std::vector<std::string> names(49); for(size_t i = 0; i < 49; ++i) names[i] = "b";
    for (size_t i = 0; i < t->size(); ++i) names.push_back("m");
    std::vector<mcnpi::NODE> nodes(names.size(), mcnpi::VALUE);
    assign_names(interpreter, names, nodes, t, children);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"bbrem"
                                ,children);
    delete t;
 }
#line 7113 "MCNPParser.cpp"
    break;

  case 555: // CORX: CORA
#line 4570 "MCNPParser.bison"
       { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7119 "MCNPParser.cpp"
    break;

  case 556: // CORX: CORB
#line 4570 "MCNPParser.bison"
              { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7125 "MCNPParser.cpp"
    break;

  case 557: // CORX: CORC
#line 4570 "MCNPParser.bison"
                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7131 "MCNPParser.cpp"
    break;

  case 558: // tmesh_corx_card: CORX card_id number_list
#line 4572 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 7149 "MCNPParser.cpp"
    break;

  case 559: // XMESH_FLAG_KEYWORD: TRAKS
#line 4586 "MCNPParser.bison"
                      { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7155 "MCNPParser.cpp"
    break;

  case 560: // XMESH_FLAG_KEYWORD: FLUX
#line 4586 "MCNPParser.bison"
                             { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7161 "MCNPParser.cpp"
    break;

  case 561: // XMESH_FLAG_KEYWORD: POPUL
#line 4586 "MCNPParser.bison"
                                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7167 "MCNPParser.cpp"
    break;

  case 562: // XMESH_FLAG_KEYWORD: PEDEP
#line 4586 "MCNPParser.bison"
                                            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7173 "MCNPParser.cpp"
    break;

  case 563: // XMESH_FLAG_KEYWORD: TOTAL
#line 4586 "MCNPParser.bison"
                                                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7179 "MCNPParser.cpp"
    break;

  case 564: // XMESH_FLAG_KEYWORD: DEDX
#line 4586 "MCNPParser.bison"
                                                            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7185 "MCNPParser.cpp"
    break;

  case 565: // XMESH_FLAG_KEYWORD: RECOL
#line 4586 "MCNPParser.bison"
                                                                   { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7191 "MCNPParser.cpp"
    break;

  case 566: // XMESH_FLAG_KEYWORD: TLEST
#line 4586 "MCNPParser.bison"
                                                                           { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7197 "MCNPParser.cpp"
    break;

  case 567: // XMESH_FLAG_KEYWORD: EDLCT
#line 4586 "MCNPParser.bison"
                                                                                   { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7203 "MCNPParser.cpp"
    break;

  case 568: // XMESH_VALUE_KEYWORD: MFACT
#line 4588 "MCNPParser.bison"
                       { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7209 "MCNPParser.cpp"
    break;

  case 569: // XMESH_VALUE_KEYWORD: TRANS
#line 4588 "MCNPParser.bison"
                               { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7215 "MCNPParser.cpp"
    break;

  case 570: // tmesh_xmesh_member: XMESH_FLAG_KEYWORD
#line 4591 "MCNPParser.bison"
 {
    std::string name = interpreter.token_data((yystack_[0].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_leaf(wasp::mcnpi::FLAG, name.data(), (yystack_[0].value.token_index));
 }
#line 7226 "MCNPParser.cpp"
    break;

  case 571: // tmesh_xmesh_member: XMESH_VALUE_KEYWORD assign number
#line 4598 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::MATERIAL_PARAM
                                ,name.data()
                                ,{decl, (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
}
#line 7240 "MCNPParser.cpp"
    break;

  case 572: // tmesh_xmesh_member: DOSE number_token_list
#line 4608 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    static std::vector<std::string> names = {"ic", "int", "iu", "fac"};
    static std::vector<mcnpi::NODE> types(names.size(), mcnpi::VALUE);
    std::vector<size_t> children = {decl};
    assign_names(interpreter, names, types, (yystack_[0].value.token_indices), children);

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"dose"
                                ,children);
    delete (yystack_[0].value.token_indices);
}
#line 7257 "MCNPParser.cpp"
    break;

  case 573: // tmesh_xmesh_members: tmesh_xmesh_member
#line 4623 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 7266 "MCNPParser.cpp"
    break;

  case 574: // tmesh_xmesh_members: tmesh_xmesh_members tmesh_xmesh_member
#line 4628 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 7275 "MCNPParser.cpp"
    break;

  case 575: // XMESH: "cylindrical mesh (CMESH)"
#line 4633 "MCNPParser.bison"
         { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7281 "MCNPParser.cpp"
    break;

  case 576: // XMESH: "rectilinear mesh (RMESH)"
#line 4633 "MCNPParser.bison"
                 { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7287 "MCNPParser.cpp"
    break;

  case 577: // XMESH: "spherical mesh (SMESH)"
#line 4633 "MCNPParser.bison"
                         { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7293 "MCNPParser.cpp"
    break;

  case 578: // tmesh_xmesh_card: XMESH card_id colon particle_symbol tmesh_xmesh_members
#line 4636 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[4].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), (yystack_[3].value.node_index));
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[4].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 7313 "MCNPParser.cpp"
    break;

  case 579: // tmesh_xmesh_card: XMESH card_id
#line 4652 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, (yystack_[0].value.node_index)});
 }
#line 7327 "MCNPParser.cpp"
    break;

  case 580: // tmesh_xmesh_card: XMESH card_id tmesh_xmesh_members
#line 4662 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 7345 "MCNPParser.cpp"
    break;

  case 581: // data_tmesh_member: tmesh_corx_card
#line 4676 "MCNPParser.bison"
                     { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 7351 "MCNPParser.cpp"
    break;

  case 582: // data_tmesh_member: tmesh_xmesh_card
#line 4676 "MCNPParser.bison"
                                        { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 7357 "MCNPParser.cpp"
    break;

  case 583: // data_tmesh_members: data_tmesh_member
#line 4679 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 7366 "MCNPParser.cpp"
    break;

  case 584: // data_tmesh_members: data_tmesh_members data_tmesh_member
#line 4684 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) =(yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 7375 "MCNPParser.cpp"
    break;

  case 585: // data_tmesh_card: "superimposed mesh tally card (TMESH)" data_tmesh_members "mesh tally terminator (ENDMD)"
#line 4690 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto term = interpreter.push_leaf(wasp::mcnpi::TERMINATOR, "endmd", (yystack_[0].value.token_index));
    auto m = (yystack_[1].value.node_indices);
    m->insert(m->begin(), decl);
    m->push_back(term);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tmesh"
                                ,*m);
    delete m;
 }
#line 7391 "MCNPParser.cpp"
    break;

  case 586: // FMESH_KEY: GEOM
#line 4702 "MCNPParser.bison"
            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7397 "MCNPParser.cpp"
    break;

  case 587: // FMESH_KEY: ORIGIN
#line 4702 "MCNPParser.bison"
                   { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7403 "MCNPParser.cpp"
    break;

  case 588: // FMESH_KEY: AXS
#line 4702 "MCNPParser.bison"
                            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7409 "MCNPParser.cpp"
    break;

  case 589: // FMESH_KEY: VEC
#line 4702 "MCNPParser.bison"
                                  { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7415 "MCNPParser.cpp"
    break;

  case 590: // FMESH_KEY: IMESH
#line 4702 "MCNPParser.bison"
                                        { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7421 "MCNPParser.cpp"
    break;

  case 591: // FMESH_KEY: IINTS
#line 4702 "MCNPParser.bison"
                                                { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7427 "MCNPParser.cpp"
    break;

  case 592: // FMESH_KEY: JMESH
#line 4702 "MCNPParser.bison"
                                                        { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7433 "MCNPParser.cpp"
    break;

  case 593: // FMESH_KEY: JINTS
#line 4702 "MCNPParser.bison"
                                                                { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7439 "MCNPParser.cpp"
    break;

  case 594: // FMESH_KEY: KMESH
#line 4703 "MCNPParser.bison"
            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7445 "MCNPParser.cpp"
    break;

  case 595: // FMESH_KEY: KINTS
#line 4703 "MCNPParser.bison"
                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7451 "MCNPParser.cpp"
    break;

  case 596: // FMESH_KEY: EMESH
#line 4703 "MCNPParser.bison"
                            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7457 "MCNPParser.cpp"
    break;

  case 597: // FMESH_KEY: EINTS
#line 4703 "MCNPParser.bison"
                                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7463 "MCNPParser.cpp"
    break;

  case 598: // FMESH_KEY: ENORM
#line 4703 "MCNPParser.bison"
                                            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7469 "MCNPParser.cpp"
    break;

  case 599: // FMESH_KEY: FMESH_TMESH
#line 4703 "MCNPParser.bison"
                                                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7475 "MCNPParser.cpp"
    break;

  case 600: // FMESH_KEY: TINTS
#line 4703 "MCNPParser.bison"
                                                                  { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7481 "MCNPParser.cpp"
    break;

  case 601: // FMESH_KEY: TNORM
#line 4703 "MCNPParser.bison"
                                                                          { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7487 "MCNPParser.cpp"
    break;

  case 602: // FMESH_KEY: FACTOR
#line 4704 "MCNPParser.bison"
            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7493 "MCNPParser.cpp"
    break;

  case 603: // FMESH_KEY: MCNP_OUT
#line 4704 "MCNPParser.bison"
                     { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7499 "MCNPParser.cpp"
    break;

  case 604: // FMESH_KEY: TR
#line 4704 "MCNPParser.bison"
                                { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7505 "MCNPParser.cpp"
    break;

  case 605: // FMESH_KEY: TYPE
#line 4704 "MCNPParser.bison"
                                     { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7511 "MCNPParser.cpp"
    break;

  case 606: // FMESH_KEY: KCLEAR
#line 4704 "MCNPParser.bison"
                                            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7517 "MCNPParser.cpp"
    break;

  case 607: // FMESH_KEY: INC
#line 4704 "MCNPParser.bison"
                                                     { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7523 "MCNPParser.cpp"
    break;

  case 608: // data_fmesh_member: FMESH_KEY assign value_list
#line 4707 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 7541 "MCNPParser.cpp"
    break;

  case 609: // data_fmesh_member: FMESH_KEY value_list
#line 4721 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                ,name.data()
                                ,*m);
    delete m;
  }
#line 7558 "MCNPParser.cpp"
    break;

  case 610: // data_fmesh_members: data_fmesh_member
#line 4735 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 7567 "MCNPParser.cpp"
    break;

  case 611: // data_fmesh_members: data_fmesh_members data_fmesh_member
#line 4740 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 7576 "MCNPParser.cpp"
    break;

  case 612: // data_fmesh_card: "superimposed fmesh mesh tally card (FMESH)" card_id colon particle_symbol data_fmesh_members
#line 4746 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[4].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), (yystack_[3].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fmesh"
                                ,*m);
    delete m;
 }
#line 7593 "MCNPParser.cpp"
    break;

  case 613: // MESH_KEY: GEOM
#line 4759 "MCNPParser.bison"
           { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7599 "MCNPParser.cpp"
    break;

  case 614: // MESH_KEY: ORIGIN
#line 4759 "MCNPParser.bison"
                  { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7605 "MCNPParser.cpp"
    break;

  case 615: // MESH_KEY: AXS
#line 4759 "MCNPParser.bison"
                           { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7611 "MCNPParser.cpp"
    break;

  case 616: // MESH_KEY: REF
#line 4759 "MCNPParser.bison"
                                 { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7617 "MCNPParser.cpp"
    break;

  case 617: // MESH_KEY: VEC
#line 4759 "MCNPParser.bison"
                                       { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7623 "MCNPParser.cpp"
    break;

  case 618: // MESH_KEY: IMESH
#line 4759 "MCNPParser.bison"
                                             { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7629 "MCNPParser.cpp"
    break;

  case 619: // MESH_KEY: IINTS
#line 4759 "MCNPParser.bison"
                                                     { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7635 "MCNPParser.cpp"
    break;

  case 620: // MESH_KEY: JMESH
#line 4759 "MCNPParser.bison"
                                                             { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7641 "MCNPParser.cpp"
    break;

  case 621: // MESH_KEY: JINTS
#line 4759 "MCNPParser.bison"
                                                                     { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7647 "MCNPParser.cpp"
    break;

  case 622: // MESH_KEY: KMESH
#line 4760 "MCNPParser.bison"
            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7653 "MCNPParser.cpp"
    break;

  case 623: // MESH_KEY: KINTS
#line 4760 "MCNPParser.bison"
                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7659 "MCNPParser.cpp"
    break;

  case 624: // data_mesh_member: MESH_KEY assign value_list
#line 4763 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 7677 "MCNPParser.cpp"
    break;

  case 625: // data_mesh_member: MESH_KEY value_list
#line 4777 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                ,name.data()
                                ,*m);
    delete m;
  }
#line 7694 "MCNPParser.cpp"
    break;

  case 626: // data_mesh_members: data_mesh_member
#line 4791 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 7703 "MCNPParser.cpp"
    break;

  case 627: // data_mesh_members: data_mesh_members data_mesh_member
#line 4796 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 7712 "MCNPParser.cpp"
    break;

  case 628: // data_mesh_card: "superimposed mesh variable card (MESH)" data_mesh_members
#line 4802 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"mesh"
                                ,*m);
    delete m;
 }
#line 7726 "MCNPParser.cpp"
    break;

  case 629: // data_thtme_card: "thermal times card (THTME)" number_list
#line 4813 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"thtme"
                                ,*m);
    delete m;
 }
#line 7740 "MCNPParser.cpp"
    break;

  case 630: // data_tm_card: "time multiplier card (TMn)" card_id number_list
#line 4824 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tm"
                                ,*m);
    delete m;
 }
#line 7755 "MCNPParser.cpp"
    break;

  case 631: // data_mx_card: "material nuclide substitution card (MX)" card_id colon particle_symbol value_list
#line 4836 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[4].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), (yystack_[3].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"mx"
                                ,*m);
    delete m;
 }
#line 7772 "MCNPParser.cpp"
    break;

  case 632: // data_mphys_card: "model physics control card (MPHYS)" "string"
#line 4850 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto f = interpreter.push_leaf(wasp::mcnpi::DECL, "flag", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"mphys"
                                ,{decl, f});
 }
#line 7784 "MCNPParser.cpp"
    break;

  case 633: // TROPT_KEY: MCSCAT
#line 4858 "MCNPParser.bison"
             { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7790 "MCNPParser.cpp"
    break;

  case 634: // TROPT_KEY: ELOSS
#line 4858 "MCNPParser.bison"
                      { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7796 "MCNPParser.cpp"
    break;

  case 635: // TROPT_KEY: NREACT
#line 4858 "MCNPParser.bison"
                              { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7802 "MCNPParser.cpp"
    break;

  case 636: // TROPT_KEY: NESCAT
#line 4858 "MCNPParser.bison"
                                        { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7808 "MCNPParser.cpp"
    break;

  case 637: // TROPT_KEY: GENXS
#line 4858 "MCNPParser.bison"
                                                 { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7814 "MCNPParser.cpp"
    break;

  case 638: // data_tropt_member: TROPT_KEY assign value
#line 4861 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
 }
#line 7829 "MCNPParser.cpp"
    break;

  case 639: // data_tropt_member: TROPT_KEY value
#line 4872 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });

    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, (yystack_[0].value.node_index)});
 }
#line 7844 "MCNPParser.cpp"
    break;

  case 640: // data_tropt_members: data_tropt_member
#line 4884 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 7853 "MCNPParser.cpp"
    break;

  case 641: // data_tropt_members: data_tropt_members data_tropt_member
#line 4889 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 7862 "MCNPParser.cpp"
    break;

  case 642: // data_tropt_card: "transport options card (TROPT)" data_tropt_members
#line 4895 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tropt"
                                ,*m);
    delete m;
 }
#line 7876 "MCNPParser.cpp"
    break;

  case 643: // PERT_SCALAR_KEY: "mat"
#line 4905 "MCNPParser.bison"
                   { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7882 "MCNPParser.cpp"
    break;

  case 644: // PERT_SCALAR_KEY: "rho"
#line 4905 "MCNPParser.bison"
                         { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7888 "MCNPParser.cpp"
    break;

  case 645: // PERT_SCALAR_KEY: METHOD
#line 4905 "MCNPParser.bison"
                               { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7894 "MCNPParser.cpp"
    break;

  case 646: // PERT_LIST_KEY: CEL
#line 4906 "MCNPParser.bison"
                 { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7900 "MCNPParser.cpp"
    break;

  case 647: // PERT_LIST_KEY: ERG
#line 4906 "MCNPParser.bison"
                       { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7906 "MCNPParser.cpp"
    break;

  case 648: // PERT_LIST_KEY: RXN
#line 4906 "MCNPParser.bison"
                             { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 7912 "MCNPParser.cpp"
    break;

  case 649: // data_pert_member: PERT_LIST_KEY assign value_comma_list
#line 4909 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 7930 "MCNPParser.cpp"
    break;

  case 650: // data_pert_member: PERT_LIST_KEY value_comma_list
#line 4923 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 7947 "MCNPParser.cpp"
    break;

  case 651: // data_pert_member: PERT_SCALAR_KEY assign value
#line 4936 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
 }
#line 7961 "MCNPParser.cpp"
    break;

  case 652: // data_pert_member: PERT_SCALAR_KEY value
#line 4946 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, (yystack_[0].value.node_index)});
 }
#line 7975 "MCNPParser.cpp"
    break;

  case 653: // data_pert_members: data_pert_member
#line 4958 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 7984 "MCNPParser.cpp"
    break;

  case 654: // data_pert_members: data_pert_members data_pert_member
#line 4963 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 7993 "MCNPParser.cpp"
    break;

  case 655: // data_pert_card: "tally perturbation card (PERT)" card_id colon particle_symbol_list data_pert_members
#line 4969 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[4].value.token_index));
    auto m = (yystack_[1].value.node_indices);
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), (yystack_[3].value.node_index));
    m->insert(m->begin(), decl);
    auto m2 = (yystack_[0].value.node_indices);
    m->insert(m->end(), m2->begin(), m2->end());
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"pert"
                                ,*m);
    delete m;
    delete m2;
 }
#line 8012 "MCNPParser.cpp"
    break;

  case 656: // data_lca_card: LCA number_token_list
#line 4985 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::vector<size_t> children = {decl};
    std::vector<std::string> names = {"ielas", "ipreq", "iexisa", "ichoic", "jcoul", "nexite", "npidk", "noact", "icem", "ilaq", "nevtype"};
    std::vector<mcnpi::NODE> types(names.size(), mcnpi::VALUE);
    assign_names(interpreter, names, types, (yystack_[0].value.token_indices), children);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"lca"
                                ,children);
    delete (yystack_[0].value.token_indices);
 }
#line 8028 "MCNPParser.cpp"
    break;

  case 657: // data_lcb_card: LCB number_token_list
#line 4998 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::vector<size_t> children = {decl};
    std::vector<std::string> names = {"flenb1", "flenb2", "flenb3", "flenb4", "flenb5", "flenb6", "ctofe", "flim0"};
    std::vector<mcnpi::NODE> types(names.size(), mcnpi::VALUE);
    assign_names(interpreter, names, types, (yystack_[0].value.token_indices), children);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"lcb"
                                ,children);
    delete (yystack_[0].value.token_indices);
 }
#line 8044 "MCNPParser.cpp"
    break;

  case 658: // data_lcc_card: LCC number_token_list
#line 5011 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::vector<size_t> children = {decl};
    std::vector<std::string> names = {"stincl", "v0incl", "xfoisaincl", "npaulincl", "nosurfincl", "unused", "unused", "ecutincl", "ebankincl", "ebankabla"};
    std::vector<mcnpi::NODE> types(names.size(), mcnpi::VALUE);
    assign_names(interpreter, names, types, (yystack_[0].value.token_indices), children);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"lcc"
                                ,children);
    delete (yystack_[0].value.token_indices);
 }
#line 8060 "MCNPParser.cpp"
    break;

  case 659: // data_lea_card: LEA number_token_list
#line 5024 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::vector<size_t> children = {decl};
    std::vector<std::string> names = {"ipht", "icc", "nobalc", "nobale", "ifbrk", "ilvden", "ievap", "nofis"};
    std::vector<mcnpi::NODE> types(names.size(), mcnpi::VALUE);
    assign_names(interpreter, names, types, (yystack_[0].value.token_indices), children);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"lea"
                                ,children);
    delete (yystack_[0].value.token_indices);
 }
#line 8076 "MCNPParser.cpp"
    break;

  case 660: // data_leb_card: LEB number_token_list
#line 5037 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::vector<size_t> children = {decl};
    std::vector<std::string> names = {"yzere", "bzere", "yzero", "bzero"};
    std::vector<mcnpi::NODE> types(names.size(), mcnpi::VALUE);
    assign_names(interpreter, names, types, (yystack_[0].value.token_indices), children);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"leb"
                                ,children);
    delete (yystack_[0].value.token_indices);
 }
#line 8092 "MCNPParser.cpp"
    break;

  case 661: // data_pikmt_card: "photon production bias card (PIKMT)" number_list
#line 5054 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"pikmt"
                                ,*m);
    delete m;
 }
#line 8106 "MCNPParser.cpp"
    break;

  case 662: // data_dxc_card: "dxc" card_id colon particle_symbol number_list
#line 5065 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[4].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[3].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"dxc"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 8122 "MCNPParser.cpp"
    break;

  case 663: // data_dxc_card: "dxc" colon particle_symbol number_list
#line 5077 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"dxc"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 8137 "MCNPParser.cpp"
    break;

  case 664: // data_unc_card: "unc" colon particle_symbol number_list
#line 5089 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[1].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), (yystack_[2].value.node_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"unc"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 8152 "MCNPParser.cpp"
    break;

  case 665: // data_nonu_card: "nonu"
#line 5101 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"nonu"
                                ,{decl});
}
#line 8163 "MCNPParser.cpp"
    break;

  case 666: // data_nonu_card: "nonu" number_list
#line 5108 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    (yystack_[0].value.node_indices)->insert((yystack_[0].value.node_indices)->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"nonu"
                                ,*(yystack_[0].value.node_indices));
    delete (yystack_[0].value.node_indices);
}
#line 8176 "MCNPParser.cpp"
    break;

  case 667: // data_notrn_card: "direct-only neutron-particle point detector card (NOTRN)"
#line 5118 "MCNPParser.bison"
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"notrn"
                                ,{decl});
}
#line 8187 "MCNPParser.cpp"
    break;

  case 668: // EMBED_SCALAR_KEY: BACKGROUND
#line 5125 "MCNPParser.bison"
                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8193 "MCNPParser.cpp"
    break;

  case 669: // EMBED_SCALAR_KEY: MESHGEO
#line 5125 "MCNPParser.bison"
                                 { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8199 "MCNPParser.cpp"
    break;

  case 670: // EMBED_SCALAR_KEY: MGEOIN
#line 5125 "MCNPParser.bison"
                                           { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8205 "MCNPParser.cpp"
    break;

  case 671: // EMBED_SCALAR_KEY: MEEOUT
#line 5125 "MCNPParser.bison"
                                                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8211 "MCNPParser.cpp"
    break;

  case 672: // EMBED_SCALAR_KEY: MEEIN
#line 5125 "MCNPParser.bison"
                                                             { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8217 "MCNPParser.cpp"
    break;

  case 673: // EMBED_SCALAR_KEY: CALC_VOLS
#line 5125 "MCNPParser.bison"
                                                                     { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8223 "MCNPParser.cpp"
    break;

  case 674: // EMBED_SCALAR_KEY: DEBUG
#line 5125 "MCNPParser.bison"
                                                                                 { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8229 "MCNPParser.cpp"
    break;

  case 675: // EMBED_SCALAR_KEY: FILETYPE
#line 5126 "MCNPParser.bison"
                   { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8235 "MCNPParser.cpp"
    break;

  case 676: // EMBED_SCALAR_KEY: GMVFILE
#line 5126 "MCNPParser.bison"
                              { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8241 "MCNPParser.cpp"
    break;

  case 677: // EMBED_SCALAR_KEY: LENGTH
#line 5126 "MCNPParser.bison"
                                        { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8247 "MCNPParser.cpp"
    break;

  case 678: // EMBED_SCALAR_KEY: MCNPUMFILE
#line 5126 "MCNPParser.bison"
                                                 { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8253 "MCNPParser.cpp"
    break;

  case 679: // EMBED_LIST_KEY: MATCELL
#line 5127 "MCNPParser.bison"
                  { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8259 "MCNPParser.cpp"
    break;

  case 680: // EMBED_LIST_KEY: OVERLAP
#line 5127 "MCNPParser.bison"
                            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8265 "MCNPParser.cpp"
    break;

  case 681: // data_embed_member: EMBED_LIST_KEY assign value_comma_list
#line 5130 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 8283 "MCNPParser.cpp"
    break;

  case 682: // data_embed_member: EMBED_LIST_KEY value_comma_list
#line 5144 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 8300 "MCNPParser.cpp"
    break;

  case 683: // data_embed_member: EMBED_SCALAR_KEY assign value
#line 5157 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
 }
#line 8314 "MCNPParser.cpp"
    break;

  case 684: // data_embed_member: EMBED_SCALAR_KEY value
#line 5167 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, (yystack_[0].value.node_index)});
 }
#line 8328 "MCNPParser.cpp"
    break;

  case 685: // data_embed_members: data_embed_member
#line 5179 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 8337 "MCNPParser.cpp"
    break;

  case 686: // data_embed_members: data_embed_members data_embed_member
#line 5184 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 8346 "MCNPParser.cpp"
    break;

  case 687: // data_embed_card: EMBED_CARD card_id data_embed_members
#line 5190 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"embed"
                                ,*m);
    delete m;
 }
#line 8361 "MCNPParser.cpp"
    break;

  case 688: // EMBEE_SCALAR_KEY: EMBED
#line 5201 "MCNPParser.bison"
                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8367 "MCNPParser.cpp"
    break;

  case 689: // EMBEE_SCALAR_KEY: ENERGY
#line 5201 "MCNPParser.bison"
                            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8373 "MCNPParser.cpp"
    break;

  case 690: // EMBEE_SCALAR_KEY: TIME
#line 5201 "MCNPParser.bison"
                                     { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8379 "MCNPParser.cpp"
    break;

  case 691: // EMBEE_SCALAR_KEY: "mat"
#line 5201 "MCNPParser.bison"
                                            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8385 "MCNPParser.cpp"
    break;

  case 692: // EMBEE_SCALAR_KEY: ATOM
#line 5201 "MCNPParser.bison"
                                                  { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8391 "MCNPParser.cpp"
    break;

  case 693: // EMBEE_SCALAR_KEY: FACTOR
#line 5201 "MCNPParser.bison"
                                                         { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8397 "MCNPParser.cpp"
    break;

  case 694: // EMBEE_SCALAR_KEY: MTYPE
#line 5201 "MCNPParser.bison"
                                                                  { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8403 "MCNPParser.cpp"
    break;

  case 695: // EMBEE_SCALAR_KEY: ERRORS
#line 5201 "MCNPParser.bison"
                                                                          { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8409 "MCNPParser.cpp"
    break;

  case 696: // EMBEE_SCALAR_KEY: COMMENT_FLAG
#line 5201 "MCNPParser.bison"
                                                                                   { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8415 "MCNPParser.cpp"
    break;

  case 697: // EMBEE_LIST_KEY: LIST
#line 5202 "MCNPParser.bison"
                  { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8421 "MCNPParser.cpp"
    break;

  case 698: // data_embee_member: EMBEE_LIST_KEY assign value_comma_list
#line 5205 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 8439 "MCNPParser.cpp"
    break;

  case 699: // data_embee_member: EMBEE_LIST_KEY value_comma_list
#line 5219 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 8456 "MCNPParser.cpp"
    break;

  case 700: // data_embee_member: EMBEE_SCALAR_KEY assign value
#line 5232 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
 }
#line 8470 "MCNPParser.cpp"
    break;

  case 701: // data_embee_member: EMBEE_SCALAR_KEY value
#line 5242 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, (yystack_[0].value.node_index)});
 }
#line 8484 "MCNPParser.cpp"
    break;

  case 702: // data_embee_members: data_embee_member
#line 5254 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 8493 "MCNPParser.cpp"
    break;

  case 703: // data_embee_members: data_embee_members data_embee_member
#line 5259 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 8502 "MCNPParser.cpp"
    break;

  case 704: // data_embee_card: EMBEE card_id colon particle_symbol_list data_embee_members
#line 5265 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[4].value.token_index));
    auto m = (yystack_[1].value.node_indices);
    auto e = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), (yystack_[3].value.node_index));
    m->insert(m->begin(), decl);
    m->insert(m->end(), e->begin(), e->end());
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"embee"
                                ,*m);
    delete m;
    delete e;
 }
#line 8521 "MCNPParser.cpp"
    break;

  case 705: // data_embeb_card: EMBEB card_id number_list
#line 5281 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"embeb"
                                ,*m);
    delete m;
 }
#line 8536 "MCNPParser.cpp"
    break;

  case 706: // data_embeb_card: EMBEB card_id colon particle_symbol number_list
#line 5292 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[4].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), (yystack_[3].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"embeb"
                                ,*m);
    delete m;
 }
#line 8553 "MCNPParser.cpp"
    break;

  case 707: // data_embem_card: EMBEM card_id number_list
#line 5306 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"embem"
                                ,*m);
    delete m;
 }
#line 8568 "MCNPParser.cpp"
    break;

  case 708: // data_embtb_card: EMBTB card_id number_list
#line 5318 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"embtb"
                                ,*m);
    delete m;
 }
#line 8583 "MCNPParser.cpp"
    break;

  case 709: // data_embtm_card: EMBTM card_id number_list
#line 5330 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"embtm"
                                ,*m);
    delete m;
 }
#line 8598 "MCNPParser.cpp"
    break;

  case 710: // data_embde_card: EMBDE card_id number_list
#line 5342 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"embde"
                                ,*m);
    delete m;
 }
#line 8613 "MCNPParser.cpp"
    break;

  case 711: // data_embdf_card: EMBDF card_id number_list
#line 5354 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"embdf"
                                ,*m);
    delete m;
 }
#line 8628 "MCNPParser.cpp"
    break;

  case 712: // data_void_card: MCNP_VOID
#line 5366 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"void"
                                ,{decl});
 }
#line 8639 "MCNPParser.cpp"
    break;

  case 713: // data_void_card: MCNP_VOID number_list
#line 5373 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"void"
                                ,*m);
    delete m;
 }
#line 8653 "MCNPParser.cpp"
    break;

  case 714: // data_drxs_card: DRXS
#line 5384 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"drxs"
                                ,{decl});
 }
#line 8664 "MCNPParser.cpp"
    break;

  case 715: // data_drxs_card: DRXS number_list
#line 5391 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"drxs"
                                ,*m);
    delete m;
 }
#line 8678 "MCNPParser.cpp"
    break;

  case 716: // data_ctme_card: CTME number
#line 5402 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ctme"
                                ,{decl, (yystack_[0].value.node_index)});
 }
#line 8689 "MCNPParser.cpp"
    break;

  case 717: // data_rdum_card: RDUM
#line 5410 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"rdum"
                                ,{decl});
 }
#line 8700 "MCNPParser.cpp"
    break;

  case 718: // data_rdum_card: RDUM number_list
#line 5417 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"rdum"
                                ,*m);
    delete m;
 }
#line 8714 "MCNPParser.cpp"
    break;

  case 719: // data_idum_card: IDUM
#line 5428 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"idum"
                                ,{decl});
 }
#line 8725 "MCNPParser.cpp"
    break;

  case 720: // data_idum_card: IDUM number_list
#line 5435 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"idum"
                                ,*m);
    delete m;
 }
#line 8739 "MCNPParser.cpp"
    break;

  case 721: // pty: PTY assign particle_symbol_list
#line 5446 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"pty"
                                ,*m);
    delete m;
 }
#line 8754 "MCNPParser.cpp"
    break;

  case 722: // pty: PTY particle_symbol_list
#line 5457 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"pty"
                                ,*m);
    delete m;
 }
#line 8768 "MCNPParser.cpp"
    break;

  case 723: // SSW_KEY: SYM
#line 5467 "MCNPParser.bison"
           { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8774 "MCNPParser.cpp"
    break;

  case 724: // SSW_KEY: CEL
#line 5467 "MCNPParser.bison"
                 { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8780 "MCNPParser.cpp"
    break;

  case 725: // data_ssw_member: SSW_KEY assign number_list
#line 5470 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 8798 "MCNPParser.cpp"
    break;

  case 726: // data_ssw_member: SSW_KEY number_list
#line 5484 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 8815 "MCNPParser.cpp"
    break;

  case 727: // data_ssw_member: pty
#line 5496 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 8821 "MCNPParser.cpp"
    break;

  case 728: // data_ssw_members: data_ssw_member
#line 5498 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 8830 "MCNPParser.cpp"
    break;

  case 729: // data_ssw_members: data_ssw_members data_ssw_member
#line 5503 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 8839 "MCNPParser.cpp"
    break;

  case 730: // data_ssw_card: SSW
#line 5509 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ssw"
                                ,{decl});
 }
#line 8850 "MCNPParser.cpp"
    break;

  case 731: // data_ssw_card: SSW number_paren_list
#line 5516 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ssw"
                                ,*m);
    delete m;
 }
#line 8864 "MCNPParser.cpp"
    break;

  case 732: // data_ssw_card: SSW number_paren_list data_ssw_members
#line 5526 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[1].value.node_indices);
    m->insert(m->begin(), decl);
    m->insert(m->end(), (yystack_[0].value.node_indices)->begin(), (yystack_[0].value.node_indices)->end());
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ssw"
                                ,*m);
    delete m;
 }
#line 8879 "MCNPParser.cpp"
    break;

  case 733: // data_ssw_card: SSW data_ssw_members
#line 5537 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ssw"
                                ,*m);
    delete m;
 }
#line 8893 "MCNPParser.cpp"
    break;

  case 734: // SSR_KEY: OLD
#line 5547 "MCNPParser.bison"
           { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8899 "MCNPParser.cpp"
    break;

  case 735: // SSR_KEY: NEW
#line 5547 "MCNPParser.bison"
                 { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8905 "MCNPParser.cpp"
    break;

  case 736: // SSR_KEY: WGT
#line 5547 "MCNPParser.bison"
                       { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8911 "MCNPParser.cpp"
    break;

  case 737: // SSR_KEY: PSC
#line 5547 "MCNPParser.bison"
                             { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8917 "MCNPParser.cpp"
    break;

  case 738: // SSR_KEY: POA
#line 5547 "MCNPParser.bison"
                                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8923 "MCNPParser.cpp"
    break;

  case 739: // SSR_KEY: COL
#line 5547 "MCNPParser.bison"
                                          { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8929 "MCNPParser.cpp"
    break;

  case 740: // SSR_KEY: CEL
#line 5547 "MCNPParser.bison"
                                                { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8935 "MCNPParser.cpp"
    break;

  case 741: // SSR_KEY: BCW
#line 5547 "MCNPParser.bison"
                                                      { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8941 "MCNPParser.cpp"
    break;

  case 742: // SSR_KEY: AXS
#line 5547 "MCNPParser.bison"
                                                            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8947 "MCNPParser.cpp"
    break;

  case 743: // SSR_KEY: TR
#line 5547 "MCNPParser.bison"
                                                                  { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8953 "MCNPParser.cpp"
    break;

  case 744: // SSR_KEY: "ext"
#line 5547 "MCNPParser.bison"
                                                                       { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 8959 "MCNPParser.cpp"
    break;

  case 745: // data_ssr_member: SSR_KEY assign value_comma_list
#line 5551 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 8977 "MCNPParser.cpp"
    break;

  case 746: // data_ssr_member: SSR_KEY value_comma_list
#line 5565 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 8994 "MCNPParser.cpp"
    break;

  case 747: // data_ssr_member: pty
#line 5577 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 9000 "MCNPParser.cpp"
    break;

  case 748: // data_ssr_members: data_ssr_member
#line 5581 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 9009 "MCNPParser.cpp"
    break;

  case 749: // data_ssr_members: data_ssr_members data_ssr_member
#line 5586 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 9018 "MCNPParser.cpp"
    break;

  case 750: // data_ssr_card: SSR
#line 5593 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ssr"
                                ,{decl});
 }
#line 9029 "MCNPParser.cpp"
    break;

  case 751: // data_ssr_card: SSR data_ssr_members
#line 5600 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ssr"
                                ,*m);
    delete m;
 }
#line 9043 "MCNPParser.cpp"
    break;

  case 752: // FMULT_SCALAR_KEY: WIDTH
#line 5611 "MCNPParser.bison"
                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9049 "MCNPParser.cpp"
    break;

  case 753: // FMULT_SCALAR_KEY: METHOD
#line 5611 "MCNPParser.bison"
                            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9055 "MCNPParser.cpp"
    break;

  case 754: // FMULT_SCALAR_KEY: DATA
#line 5611 "MCNPParser.bison"
                                     { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9061 "MCNPParser.cpp"
    break;

  case 755: // FMULT_SCALAR_KEY: SHIFT
#line 5611 "MCNPParser.bison"
                                            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9067 "MCNPParser.cpp"
    break;

  case 756: // FMULT_LIST_KEY: SFNU
#line 5612 "MCNPParser.bison"
                  { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9073 "MCNPParser.cpp"
    break;

  case 757: // FMULT_LIST_KEY: WATT
#line 5612 "MCNPParser.bison"
                         { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9079 "MCNPParser.cpp"
    break;

  case 758: // data_fmult_member: FMULT_LIST_KEY assign value_comma_list
#line 5615 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 9097 "MCNPParser.cpp"
    break;

  case 759: // data_fmult_member: FMULT_LIST_KEY value_comma_list
#line 5629 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 9114 "MCNPParser.cpp"
    break;

  case 760: // data_fmult_member: FMULT_SCALAR_KEY assign value
#line 5642 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
 }
#line 9128 "MCNPParser.cpp"
    break;

  case 761: // data_fmult_member: FMULT_SCALAR_KEY value
#line 5652 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, (yystack_[0].value.node_index)});
 }
#line 9142 "MCNPParser.cpp"
    break;

  case 762: // data_fmult_members: data_fmult_member
#line 5664 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 9151 "MCNPParser.cpp"
    break;

  case 763: // data_fmult_members: data_fmult_members data_fmult_member
#line 5669 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 9160 "MCNPParser.cpp"
    break;

  case 764: // data_fmult_card: "multiplicity constant card (FMULT)" data_fmult_members
#line 5675 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fmult"
                                ,*m);
    delete m;
 }
#line 9174 "MCNPParser.cpp"
    break;

  case 765: // data_fmult_card: "multiplicity constant card (FMULT)" "integer number" data_fmult_members
#line 5685 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto z = interpreter.push_leaf(wasp::mcnpi::ZAID, "zaid", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), z);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fmult"
                                ,*m);
    delete m;
 }
#line 9190 "MCNPParser.cpp"
    break;

  case 766: // KPERT_SCALAR_KEY: LINEAR
#line 5697 "MCNPParser.bison"
                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9196 "MCNPParser.cpp"
    break;

  case 767: // KPERT_LIST_KEY: CEL
#line 5698 "MCNPParser.bison"
                  { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9202 "MCNPParser.cpp"
    break;

  case 768: // KPERT_LIST_KEY: "mat"
#line 5698 "MCNPParser.bison"
                        { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9208 "MCNPParser.cpp"
    break;

  case 769: // KPERT_LIST_KEY: "rho"
#line 5698 "MCNPParser.bison"
                              { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9214 "MCNPParser.cpp"
    break;

  case 770: // KPERT_LIST_KEY: ISO
#line 5698 "MCNPParser.bison"
                                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9220 "MCNPParser.cpp"
    break;

  case 771: // KPERT_LIST_KEY: RXN
#line 5698 "MCNPParser.bison"
                                          { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9226 "MCNPParser.cpp"
    break;

  case 772: // KPERT_LIST_KEY: ERG
#line 5698 "MCNPParser.bison"
                                                { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9232 "MCNPParser.cpp"
    break;

  case 773: // data_kpert_member: KPERT_LIST_KEY assign value_comma_list
#line 5701 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 9250 "MCNPParser.cpp"
    break;

  case 774: // data_kpert_member: KPERT_LIST_KEY value_comma_list
#line 5715 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 9267 "MCNPParser.cpp"
    break;

  case 775: // data_kpert_member: KPERT_SCALAR_KEY assign value
#line 5728 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
 }
#line 9281 "MCNPParser.cpp"
    break;

  case 776: // data_kpert_member: KPERT_SCALAR_KEY value
#line 5738 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, (yystack_[0].value.node_index)});
 }
#line 9295 "MCNPParser.cpp"
    break;

  case 777: // data_kpert_members: data_kpert_member
#line 5750 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 9304 "MCNPParser.cpp"
    break;

  case 778: // data_kpert_members: data_kpert_members data_kpert_member
#line 5755 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 9313 "MCNPParser.cpp"
    break;

  case 779: // data_kpert_card: "reactivity perturbation card (KPERT)" card_id data_kpert_members
#line 5761 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"kpert"
                                ,*m);
    delete m;
 }
#line 9328 "MCNPParser.cpp"
    break;

  case 780: // KSEN_SCALAR_KEY: LEGENDRE
#line 5772 "MCNPParser.bison"
                   { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9334 "MCNPParser.cpp"
    break;

  case 781: // KSEN_SCALAR_KEY: CONSTRAIN
#line 5772 "MCNPParser.bison"
                              { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9340 "MCNPParser.cpp"
    break;

  case 782: // KSEN_LIST_KEY: ISO
#line 5773 "MCNPParser.bison"
                 { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9346 "MCNPParser.cpp"
    break;

  case 783: // KSEN_LIST_KEY: RXN
#line 5773 "MCNPParser.bison"
                       { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9352 "MCNPParser.cpp"
    break;

  case 784: // KSEN_LIST_KEY: MT
#line 5773 "MCNPParser.bison"
                             { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9358 "MCNPParser.cpp"
    break;

  case 785: // KSEN_LIST_KEY: ERG
#line 5773 "MCNPParser.bison"
                                  { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9364 "MCNPParser.cpp"
    break;

  case 786: // KSEN_LIST_KEY: EIN
#line 5773 "MCNPParser.bison"
                                        { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9370 "MCNPParser.cpp"
    break;

  case 787: // KSEN_LIST_KEY: COS
#line 5773 "MCNPParser.bison"
                                              { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9376 "MCNPParser.cpp"
    break;

  case 788: // data_ksen_member: KSEN_LIST_KEY assign value_comma_list
#line 5776 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 9394 "MCNPParser.cpp"
    break;

  case 789: // data_ksen_member: KSEN_LIST_KEY value_comma_list
#line 5790 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 9411 "MCNPParser.cpp"
    break;

  case 790: // data_ksen_member: KSEN_SCALAR_KEY assign value
#line 5803 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
 }
#line 9425 "MCNPParser.cpp"
    break;

  case 791: // data_ksen_member: KSEN_SCALAR_KEY value
#line 5813 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, (yystack_[0].value.node_index)});
 }
#line 9439 "MCNPParser.cpp"
    break;

  case 792: // data_ksen_members: data_ksen_member
#line 5825 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 9448 "MCNPParser.cpp"
    break;

  case 793: // data_ksen_members: data_ksen_members data_ksen_member
#line 5830 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 9457 "MCNPParser.cpp"
    break;

  case 794: // data_ksen_card: "keff sensitivity coefficients card (KSEN)" card_id "string" data_ksen_members
#line 5836 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    auto xs = interpreter.push_leaf(wasp::mcnpi::FLAG, "xs", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), xs);
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ksen"
                                ,*m);
    delete m;
 }
#line 9474 "MCNPParser.cpp"
    break;

  case 795: // data_pwt_card: "pwt" number_list
#line 5850 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"pwt"
                                ,*m);
    delete m;
 }
#line 9488 "MCNPParser.cpp"
    break;

  case 796: // PTRAC_SCALAR_KEY: COINC
#line 5860 "MCNPParser.bison"
                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9494 "MCNPParser.cpp"
    break;

  case 797: // PTRAC_SCALAR_KEY: WRITE
#line 5860 "MCNPParser.bison"
                            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9500 "MCNPParser.cpp"
    break;

  case 798: // PTRAC_SCALAR_KEY: MEPH
#line 5860 "MCNPParser.bison"
                                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9506 "MCNPParser.cpp"
    break;

  case 799: // PTRAC_SCALAR_KEY: MAX
#line 5860 "MCNPParser.bison"
                                           { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9512 "MCNPParser.cpp"
    break;

  case 800: // PTRAC_SCALAR_KEY: FILE
#line 5860 "MCNPParser.bison"
                                                 { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9518 "MCNPParser.cpp"
    break;

  case 801: // PTRAC_SCALAR_KEY: BUFFER
#line 5860 "MCNPParser.bison"
                                                        { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9524 "MCNPParser.cpp"
    break;

  case 802: // PTRAC_LIST_KEY: TALLY
#line 5861 "MCNPParser.bison"
                   { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9530 "MCNPParser.cpp"
    break;

  case 803: // PTRAC_LIST_KEY: SURFACE
#line 5861 "MCNPParser.bison"
                           { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9536 "MCNPParser.cpp"
    break;

  case 804: // PTRAC_LIST_KEY: CEL
#line 5861 "MCNPParser.bison"
                                     { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9542 "MCNPParser.cpp"
    break;

  case 805: // PTRAC_LIST_KEY: "history cutoff (NPS)"
#line 5861 "MCNPParser.bison"
                                           { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9548 "MCNPParser.cpp"
    break;

  case 806: // PTRAC_LIST_KEY: TYPE
#line 5861 "MCNPParser.bison"
                                                 { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9554 "MCNPParser.cpp"
    break;

  case 807: // PTRAC_LIST_KEY: FILTER
#line 5861 "MCNPParser.bison"
                                                        { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9560 "MCNPParser.cpp"
    break;

  case 808: // PTRAC_LIST_KEY: EVENT
#line 5861 "MCNPParser.bison"
                                                                 { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9566 "MCNPParser.cpp"
    break;

  case 809: // PTRAC_LIST_KEY: VALUE_KEY
#line 5861 "MCNPParser.bison"
                                                                         { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9572 "MCNPParser.cpp"
    break;

  case 810: // data_ptrac_member: PTRAC_LIST_KEY assign value_comma_list
#line 5864 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 9590 "MCNPParser.cpp"
    break;

  case 811: // data_ptrac_member: PTRAC_LIST_KEY value_comma_list
#line 5878 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 9607 "MCNPParser.cpp"
    break;

  case 812: // data_ptrac_member: PTRAC_SCALAR_KEY assign value
#line 5891 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
 }
#line 9621 "MCNPParser.cpp"
    break;

  case 813: // data_ptrac_member: PTRAC_SCALAR_KEY value
#line 5901 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, (yystack_[0].value.node_index)});
 }
#line 9635 "MCNPParser.cpp"
    break;

  case 814: // data_ptrac_members: data_ptrac_member
#line 5913 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 9644 "MCNPParser.cpp"
    break;

  case 815: // data_ptrac_members: data_ptrac_members data_ptrac_member
#line 5918 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 9653 "MCNPParser.cpp"
    break;

  case 816: // data_ptrac_card: "particle track output (PTRAC)" data_ptrac_members
#line 5924 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ptrac"
                                ,*m);
    delete m;
 }
#line 9667 "MCNPParser.cpp"
    break;

  case 817: // data_column_wise_header_members: "string"
#line 5935 "MCNPParser.bison"
 {
    (yylhs.value.token_indices) = new std::vector<size_t>();
    (yylhs.value.token_indices)->push_back((yystack_[0].value.token_index));
 }
#line 9676 "MCNPParser.cpp"
    break;

  case 818: // data_column_wise_header_members: data_column_wise_header_members "string"
#line 5940 "MCNPParser.bison"
 {
    (yylhs.value.token_indices) = (yystack_[1].value.token_indices);
    (yylhs.value.token_indices)->push_back((yystack_[0].value.token_index));
 }
#line 9685 "MCNPParser.cpp"
    break;

  case 819: // data_column_wise_header_members: data_column_wise_header_members "integer identifier"
#line 5945 "MCNPParser.bison"
 {
    (yylhs.value.token_indices) = (yystack_[1].value.token_indices);
    (yylhs.value.token_indices)->push_back((yystack_[0].value.token_index));
 }
#line 9694 "MCNPParser.cpp"
    break;

  case 820: // data_column_wise_header_members: data_column_wise_header_members ":"
#line 5950 "MCNPParser.bison"
 {
    (yylhs.value.token_indices) = (yystack_[1].value.token_indices);
    (yylhs.value.token_indices)->push_back((yystack_[0].value.token_index));
 }
#line 9703 "MCNPParser.cpp"
    break;

  case 821: // data_column_wise_header_members: data_column_wise_header_members "particle symbol"
#line 5955 "MCNPParser.bison"
 {
    (yylhs.value.token_indices) = (yystack_[1].value.token_indices);
    (yylhs.value.token_indices)->push_back((yystack_[0].value.token_index));
 }
#line 9712 "MCNPParser.cpp"
    break;

  case 822: // data_column_wise_row_members: VALUE_COMMA_LIST_TOKEN
#line 5961 "MCNPParser.bison"
  {
     (yylhs.value.token_indices) = new std::vector<size_t>();
     (yylhs.value.token_indices)->push_back((yystack_[0].value.token_index));
 }
#line 9721 "MCNPParser.cpp"
    break;

  case 823: // data_column_wise_row_members: data_column_wise_row_members VALUE_COMMA_LIST_TOKEN
#line 5966 "MCNPParser.bison"
 {
    (yylhs.value.token_indices) = (yystack_[1].value.token_indices);
    (yylhs.value.token_indices)->push_back((yystack_[0].value.token_index));
 }
#line 9730 "MCNPParser.cpp"
    break;

  case 824: // data_column_wise_table: data_column_wise_row_members "newline"
#line 5972 "MCNPParser.bison"
 {
    (yylhs.value.table_token_indices) = new std::vector<std::vector<size_t>*>();
    (yylhs.value.table_token_indices)->push_back((yystack_[1].value.token_indices));
    // ignore NEWLINE... only a used as a sentinel
 }
#line 9740 "MCNPParser.cpp"
    break;

  case 825: // data_column_wise_table: data_column_wise_table data_column_wise_row_members "newline"
#line 5978 "MCNPParser.bison"
 {
    (yylhs.value.table_token_indices) = (yystack_[2].value.table_token_indices);
    (yylhs.value.table_token_indices)->push_back((yystack_[1].value.token_indices));
 }
#line 9749 "MCNPParser.cpp"
    break;

  case 826: // data_column_wise_table: data_column_wise_table "newline"
#line 5983 "MCNPParser.bison"
 {
    (yylhs.value.table_token_indices) = (yystack_[1].value.table_token_indices);
 }
#line 9757 "MCNPParser.cpp"
    break;

  case 827: // data_column_wise_block: COLUMN_WISE_FLAG data_column_wise_header_members "newline" data_column_wise_table
#line 5988 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    // ignore newline as it is only a sentinel value

    // first construct the header
    // Header could be simple X Y Z or more complex particle-based Xn:p Yn:p, etc. or combination
    std::vector<std::vector<size_t>> cnodes(1);
    cnodes.front().push_back(decl); // the first column of data is the '#' flag which can optionally have cell ids, etc in text column 1-5
    auto htokens = (yystack_[2].value.token_indices);
    std::vector<std::string> column_headers(1, "rh"); // first column is the row header or 'rh'
    auto cindex = cnodes.size(); // start at the end
    for (size_t h = 0; h < htokens->size(); ++h, ++cindex)
    {
        // The following header logic accounts the for the following patterns
        // 1. STRING - simplest header
        // 2. STRING ID - header with an identifier
        // 3. STRING : PARTICLE_SYMBOL - particle-based header (delimited via colon ':')
        // 4. STRING ID : PARTICLE_SYMBOL - particle-based header with an identifier

        if (cindex == cnodes.size()) cnodes.resize(cindex+1);
        auto ttype = interpreter.token_type(htokens->at(h));

        // capture declarator token as a leaf node in the given column
        cnodes[cindex].push_back(interpreter.push_leaf(wasp::mcnpi::DECL, "decl", htokens->at(h)));
        std::string name = interpreter.token_data(htokens->at(h));
        std::transform(name.begin(), name.end(), name.begin(),
            [](unsigned char c){ return std::tolower(c); });
        column_headers.push_back(name);

        // If next token is a string we know it to be another header column, cycle
        if (htokens->size() > h+1 && interpreter.token_type(htokens->at(h+1)) == wasp::mcnpi::STRING) continue;

        // If the next token is id (n), need to capture as part of header
        // Xn
        if (htokens->size() > h+1 && interpreter.token_type(htokens->at(h+1)) == wasp::mcnpi::ID)
        {
            cnodes[cindex].push_back(interpreter.push_leaf(wasp::mcnpi::ID, "id", htokens->at(h+1)));
            ++h;
        }
        // If the next token is colon (:), need to capture as part of header
        // X:
        if (htokens->size() > h+1 && interpreter.token_type(htokens->at(h+1)) == wasp::mcnpi::COLON)
        {
            cnodes[cindex].push_back(interpreter.push_leaf(wasp::mcnpi::COLON, ":", htokens->at(h+1)));
            ++h;
        }
        // If the next token is PARTICLE_SYMBOL (ps), need to capture as part of header
        // Xps
        if (htokens->size() > h+1 && interpreter.token_type(htokens->at(h+1)) == wasp::mcnpi::PARTICLE_SYMBOL)
        {
            cnodes[cindex].push_back(interpreter.push_leaf(wasp::mcnpi::PARTICLE_SYMBOL, "pt", htokens->at(h+1)));
            ++h;
        }
    } // end of header tokens

    // Process colum data (captured via table rows)
    // TODO - no support for left-hand short columns...
    auto table_tokens = (yystack_[0].value.table_token_indices);
    for (size_t row_index = 0; row_index < table_tokens->size(); ++row_index)
    {
        auto row = table_tokens->at(row_index);
        // if column count is equal to column headers the row header has been specified, else offset by 1
        size_t col_offset = 0;
        if (row->size() != column_headers.size()) col_offset++;
        for (size_t column_index = 0; column_index <  row->size(); ++column_index)
        {
            wasp_ensure(column_index+col_offset < cnodes.size());
            cnodes[column_index+col_offset].push_back(interpreter.push_leaf(wasp::mcnpi::VALUE, "value", row->at(column_index)));
        }
    }
    std::vector<size_t> children;
    wasp_ensure(cnodes.size() == column_headers.size());
    for (size_t i = 0; i < column_headers.size(); ++i)
    {
        children.push_back(interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,column_headers[i].data()
                                ,cnodes[i]));
    }
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::TABLE
                                ,"table"
                                ,children);

    delete (yystack_[2].value.token_indices);
    // TODO delete token vectors

 }
#line 9848 "MCNPParser.cpp"
    break;

  case 828: // BFLD_SCALAR_KEY: FIELD
#line 6075 "MCNPParser.bison"
                   { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9854 "MCNPParser.cpp"
    break;

  case 829: // BFLD_SCALAR_KEY: MXDEFLC
#line 6075 "MCNPParser.bison"
                           { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9860 "MCNPParser.cpp"
    break;

  case 830: // BFLD_SCALAR_KEY: MAXSTEP
#line 6075 "MCNPParser.bison"
                                     { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9866 "MCNPParser.cpp"
    break;

  case 831: // BFLD_LIST_KEY: VEC
#line 6076 "MCNPParser.bison"
                 { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9872 "MCNPParser.cpp"
    break;

  case 832: // BFLD_LIST_KEY: AXS
#line 6076 "MCNPParser.bison"
                       { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9878 "MCNPParser.cpp"
    break;

  case 833: // BFLD_LIST_KEY: FFEDGES
#line 6076 "MCNPParser.bison"
                             { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9884 "MCNPParser.cpp"
    break;

  case 834: // BFLD_LIST_KEY: REFPNT
#line 6076 "MCNPParser.bison"
                                       { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 9890 "MCNPParser.cpp"
    break;

  case 835: // data_bfld_member: BFLD_LIST_KEY assign value_comma_list
#line 6079 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 9908 "MCNPParser.cpp"
    break;

  case 836: // data_bfld_member: BFLD_LIST_KEY value_comma_list
#line 6093 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
#line 9925 "MCNPParser.cpp"
    break;

  case 837: // data_bfld_member: BFLD_SCALAR_KEY assign value
#line 6106 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    std::string name = interpreter.token_data((yystack_[2].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, (yystack_[1].value.node_index), (yystack_[0].value.node_index)});
 }
#line 9939 "MCNPParser.cpp"
    break;

  case 838: // data_bfld_member: BFLD_SCALAR_KEY value
#line 6116 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    std::string name = interpreter.token_data((yystack_[1].value.token_index));
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, (yystack_[0].value.node_index)});
 }
#line 9953 "MCNPParser.cpp"
    break;

  case 839: // data_bfld_members: data_bfld_member
#line 6128 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 9962 "MCNPParser.cpp"
    break;

  case 840: // data_bfld_members: data_bfld_members data_bfld_member
#line 6133 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 9971 "MCNPParser.cpp"
    break;

  case 841: // data_bfld_card: "particle ray tracing (BFLD)" card_id "string" data_bfld_members
#line 6139 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[3].value.token_index));
    auto xs = interpreter.push_leaf(wasp::mcnpi::FLAG, "type", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), xs);
    m->insert(m->begin(), (yystack_[2].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"bfld"
                                ,*m);
    delete m;
 }
#line 9988 "MCNPParser.cpp"
    break;

  case 842: // material_zaid_pairs: material_zaid_entry
#line 6153 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 9997 "MCNPParser.cpp"
    break;

  case 843: // material_zaid_pairs: material_zaid_pairs material_zaid_entry
#line 6158 "MCNPParser.bison"
 {
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
 }
#line 10006 "MCNPParser.cpp"
    break;

  case 844: // data_awtab_card: "atomic weight card (AWTAB)" material_zaid_pairs
#line 6164 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"awtab"
                                ,*m);
    delete m;
 }
#line 10020 "MCNPParser.cpp"
    break;

  case 845: // data_cosy_card: "cosy" number_list
#line 6175 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"cosy"
                                ,*m);
    delete m;
 }
#line 10034 "MCNPParser.cpp"
    break;

  case 846: // data_cosyp_card: "transfer map card (COSPY))" number_list
#line 6186 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"cosyp"
                                ,*m);
    delete m;
 }
#line 10048 "MCNPParser.cpp"
    break;

  case 847: // data_pd_card: "pd" card_id number_list
#line 6197 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[2].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), (yystack_[1].value.node_index));
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"pd"
                                ,*m);
    delete m;
 }
#line 10063 "MCNPParser.cpp"
    break;

  case 848: // data_pd_card: "pd" number_list
#line 6208 "MCNPParser.bison"
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", (yystack_[1].value.token_index));
    auto m = (yystack_[0].value.node_indices);
    m->insert(m->begin(), decl);
    (yylhs.value.node_index) = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"pd"
                                ,*m);
    delete m;
}
#line 10077 "MCNPParser.cpp"
    break;

  case 849: // data_card: data_vol_card
#line 6218 "MCNPParser.bison"
             { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10083 "MCNPParser.cpp"
    break;

  case 850: // data_card: data_awtab_card
#line 6219 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10089 "MCNPParser.cpp"
    break;

  case 851: // data_card: data_pd_card
#line 6220 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10095 "MCNPParser.cpp"
    break;

  case 852: // data_card: data_cosy_card
#line 6221 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10101 "MCNPParser.cpp"
    break;

  case 853: // data_card: data_cosyp_card
#line 6222 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10107 "MCNPParser.cpp"
    break;

  case 854: // data_card: data_ptrac_card
#line 6223 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10113 "MCNPParser.cpp"
    break;

  case 855: // data_card: data_bfld_card
#line 6224 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10119 "MCNPParser.cpp"
    break;

  case 856: // data_card: data_fmult_card
#line 6225 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10125 "MCNPParser.cpp"
    break;

  case 857: // data_card: data_pwt_card
#line 6226 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10131 "MCNPParser.cpp"
    break;

  case 858: // data_card: data_kpert_card
#line 6227 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10137 "MCNPParser.cpp"
    break;

  case 859: // data_card: data_ksen_card
#line 6228 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10143 "MCNPParser.cpp"
    break;

  case 860: // data_card: data_mesh_card
#line 6229 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10149 "MCNPParser.cpp"
    break;

  case 861: // data_card: data_rdum_card
#line 6230 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10155 "MCNPParser.cpp"
    break;

  case 862: // data_card: data_idum_card
#line 6231 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10161 "MCNPParser.cpp"
    break;

  case 863: // data_card: data_ssw_card
#line 6232 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10167 "MCNPParser.cpp"
    break;

  case 864: // data_card: data_ssr_card
#line 6233 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10173 "MCNPParser.cpp"
    break;

  case 865: // data_card: data_void_card
#line 6234 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10179 "MCNPParser.cpp"
    break;

  case 866: // data_card: data_ctme_card
#line 6235 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10185 "MCNPParser.cpp"
    break;

  case 867: // data_card: data_drxs_card
#line 6236 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10191 "MCNPParser.cpp"
    break;

  case 868: // data_card: data_embed_card
#line 6237 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10197 "MCNPParser.cpp"
    break;

  case 869: // data_card: data_embee_card
#line 6238 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10203 "MCNPParser.cpp"
    break;

  case 870: // data_card: data_embeb_card
#line 6239 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10209 "MCNPParser.cpp"
    break;

  case 871: // data_card: data_embem_card
#line 6240 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10215 "MCNPParser.cpp"
    break;

  case 872: // data_card: data_embtb_card
#line 6241 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10221 "MCNPParser.cpp"
    break;

  case 873: // data_card: data_embtm_card
#line 6242 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10227 "MCNPParser.cpp"
    break;

  case 874: // data_card: data_embde_card
#line 6243 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10233 "MCNPParser.cpp"
    break;

  case 875: // data_card: data_embdf_card
#line 6244 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10239 "MCNPParser.cpp"
    break;

  case 876: // data_card: data_nonu_card
#line 6245 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10245 "MCNPParser.cpp"
    break;

  case 877: // data_card: data_notrn_card
#line 6246 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10251 "MCNPParser.cpp"
    break;

  case 878: // data_card: data_dxc_card
#line 6247 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10257 "MCNPParser.cpp"
    break;

  case 879: // data_card: data_unc_card
#line 6248 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10263 "MCNPParser.cpp"
    break;

  case 880: // data_card: data_pikmt_card
#line 6249 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10269 "MCNPParser.cpp"
    break;

  case 881: // data_card: data_lca_card
#line 6250 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10275 "MCNPParser.cpp"
    break;

  case 882: // data_card: data_lcb_card
#line 6251 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10281 "MCNPParser.cpp"
    break;

  case 883: // data_card: data_lcc_card
#line 6252 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10287 "MCNPParser.cpp"
    break;

  case 884: // data_card: data_lea_card
#line 6253 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10293 "MCNPParser.cpp"
    break;

  case 885: // data_card: data_leb_card
#line 6254 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10299 "MCNPParser.cpp"
    break;

  case 886: // data_card: data_tropt_card
#line 6255 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10305 "MCNPParser.cpp"
    break;

  case 887: // data_card: data_pert_card
#line 6256 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10311 "MCNPParser.cpp"
    break;

  case 888: // data_card: data_mphys_card
#line 6257 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10317 "MCNPParser.cpp"
    break;

  case 889: // data_card: data_mx_card
#line 6258 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10323 "MCNPParser.cpp"
    break;

  case 890: // data_card: data_tm_card
#line 6259 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10329 "MCNPParser.cpp"
    break;

  case 891: // data_card: data_thtme_card
#line 6260 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10335 "MCNPParser.cpp"
    break;

  case 892: // data_card: data_tmesh_card
#line 6261 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10341 "MCNPParser.cpp"
    break;

  case 893: // data_card: data_fmesh_card
#line 6262 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10347 "MCNPParser.cpp"
    break;

  case 894: // data_card: data_bbrem_card
#line 6263 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10353 "MCNPParser.cpp"
    break;

  case 895: // data_card: data_em_card
#line 6264 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10359 "MCNPParser.cpp"
    break;

  case 896: // data_card: data_fu_card
#line 6265 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10365 "MCNPParser.cpp"
    break;

  case 897: // data_card: data_ds_card
#line 6266 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10371 "MCNPParser.cpp"
    break;

  case 898: // data_card: data_sf_card
#line 6267 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10377 "MCNPParser.cpp"
    break;

  case 899: // data_card: data_ft_card
#line 6268 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10383 "MCNPParser.cpp"
    break;

  case 900: // data_card: data_cm_card
#line 6269 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10389 "MCNPParser.cpp"
    break;

  case 901: // data_card: data_dxt_card
#line 6270 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10395 "MCNPParser.cpp"
    break;

  case 902: // data_card: data_area_card
#line 6271 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10401 "MCNPParser.cpp"
    break;

  case 903: // data_card: data_transform_card
#line 6272 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10407 "MCNPParser.cpp"
    break;

  case 904: // data_card: data_universe_card
#line 6273 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10413 "MCNPParser.cpp"
    break;

  case 905: // data_card: data_lat_card
#line 6274 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10419 "MCNPParser.cpp"
    break;

  case 906: // data_card: data_fill_card
#line 6275 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10425 "MCNPParser.cpp"
    break;

  case 907: // data_card: data_uran_card
#line 6276 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10431 "MCNPParser.cpp"
    break;

  case 908: // data_card: data_material_card
#line 6277 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10437 "MCNPParser.cpp"
    break;

  case 909: // data_card: data_cutoff_card
#line 6278 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10443 "MCNPParser.cpp"
    break;

  case 910: // data_card: data_elpt_card
#line 6279 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10449 "MCNPParser.cpp"
    break;

  case 911: // data_card: data_activation_card
#line 6280 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10455 "MCNPParser.cpp"
    break;

  case 912: // data_card: data_fq_card
#line 6281 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10461 "MCNPParser.cpp"
    break;

  case 913: // data_card: data_fc_card
#line 6282 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10467 "MCNPParser.cpp"
    break;

  case 914: // data_card: data_ftally_card
#line 6283 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10473 "MCNPParser.cpp"
    break;

  case 915: // data_card: data_mode_card
#line 6284 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10479 "MCNPParser.cpp"
    break;

  case 916: // data_card: data_tally_time_card
#line 6285 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10485 "MCNPParser.cpp"
    break;

  case 917: // data_card: data_kcode_card
#line 6286 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10491 "MCNPParser.cpp"
    break;

  case 918: // data_card: data_nps_card
#line 6287 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10497 "MCNPParser.cpp"
    break;

  case 919: // data_card: data_print_card
#line 6288 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10503 "MCNPParser.cpp"
    break;

  case 920: // data_card: data_talnp_card
#line 6289 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10509 "MCNPParser.cpp"
    break;

  case 921: // data_card: data_prdmp_card
#line 6290 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10515 "MCNPParser.cpp"
    break;

  case 922: // data_card: data_mt_card
#line 6291 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10521 "MCNPParser.cpp"
    break;

  case 923: // data_card: data_imp_card
#line 6292 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10527 "MCNPParser.cpp"
    break;

  case 924: // data_card: data_ksrc_card
#line 6293 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10533 "MCNPParser.cpp"
    break;

  case 925: // data_card: data_tally_energy_card
#line 6294 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10539 "MCNPParser.cpp"
    break;

  case 926: // data_card: data_tally_multiplier_card
#line 6295 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10545 "MCNPParser.cpp"
    break;

  case 927: // data_card: data_sd_card
#line 6296 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10551 "MCNPParser.cpp"
    break;

  case 928: // data_card: data_sdef_card
#line 6297 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10557 "MCNPParser.cpp"
    break;

  case 929: // data_card: data_si_card
#line 6298 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10563 "MCNPParser.cpp"
    break;

  case 930: // data_card: data_sp_card
#line 6299 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10569 "MCNPParser.cpp"
    break;

  case 931: // data_card: data_sb_card
#line 6300 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10575 "MCNPParser.cpp"
    break;

  case 932: // data_card: data_sc_card
#line 6301 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10581 "MCNPParser.cpp"
    break;

  case 933: // data_card: data_totnu_card
#line 6302 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10587 "MCNPParser.cpp"
    break;

  case 934: // data_card: data_de_card
#line 6303 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10593 "MCNPParser.cpp"
    break;

  case 935: // data_card: data_df_card
#line 6304 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10599 "MCNPParser.cpp"
    break;

  case 936: // data_card: data_tmp_card
#line 6305 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10605 "MCNPParser.cpp"
    break;

  case 937: // data_card: data_fs_card
#line 6306 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10611 "MCNPParser.cpp"
    break;

  case 938: // data_card: data_tf_card
#line 6307 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10617 "MCNPParser.cpp"
    break;

  case 939: // data_card: data_lost_card
#line 6308 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10623 "MCNPParser.cpp"
    break;

  case 940: // data_card: data_hsrc_card
#line 6309 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10629 "MCNPParser.cpp"
    break;

  case 941: // data_card: data_xs_card
#line 6310 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10635 "MCNPParser.cpp"
    break;

  case 942: // data_card: data_phys_card
#line 6311 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10641 "MCNPParser.cpp"
    break;

  case 943: // data_card: data_kopts_card
#line 6312 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10647 "MCNPParser.cpp"
    break;

  case 944: // data_card: data_tally_cosine_card
#line 6313 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10653 "MCNPParser.cpp"
    break;

  case 945: // data_card: data_mgopt_card
#line 6314 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10659 "MCNPParser.cpp"
    break;

  case 946: // data_card: data_rand_card
#line 6315 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10665 "MCNPParser.cpp"
    break;

  case 947: // data_card: data_wwe_card
#line 6316 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10671 "MCNPParser.cpp"
    break;

  case 948: // data_card: data_wwg_card
#line 6317 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10677 "MCNPParser.cpp"
    break;

  case 949: // data_card: data_wwge_card
#line 6318 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10683 "MCNPParser.cpp"
    break;

  case 950: // data_card: data_wwgt_card
#line 6319 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10689 "MCNPParser.cpp"
    break;

  case 951: // data_card: data_wwn_card
#line 6320 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10695 "MCNPParser.cpp"
    break;

  case 952: // data_card: data_wwp_card
#line 6321 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10701 "MCNPParser.cpp"
    break;

  case 953: // data_card: data_wwt_card
#line 6322 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10707 "MCNPParser.cpp"
    break;

  case 954: // data_card: data_dbcn_card
#line 6323 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10713 "MCNPParser.cpp"
    break;

  case 955: // data_card: data_fcl_card
#line 6324 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10719 "MCNPParser.cpp"
    break;

  case 956: // data_card: data_var_card
#line 6325 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10725 "MCNPParser.cpp"
    break;

  case 957: // data_card: data_cf_card
#line 6326 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10731 "MCNPParser.cpp"
    break;

  case 958: // data_card: data_ext_card
#line 6327 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10737 "MCNPParser.cpp"
    break;

  case 959: // data_card: data_vect_card
#line 6328 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10743 "MCNPParser.cpp"
    break;

  case 960: // data_card: data_esplt_card
#line 6329 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10749 "MCNPParser.cpp"
    break;

  case 961: // data_card: data_tsplt_card
#line 6330 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10755 "MCNPParser.cpp"
    break;

  case 962: // data_card: data_dd_card
#line 6331 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10761 "MCNPParser.cpp"
    break;

  case 963: // data_card: data_column_wise_block
#line 6332 "MCNPParser.bison"
   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 10767 "MCNPParser.cpp"
    break;

  case 964: // data_block: data_card
#line 6336 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = new std::vector<size_t>();
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 10776 "MCNPParser.cpp"
    break;

  case 965: // data_block: data_block data_card
#line 6341 "MCNPParser.bison"
{
    (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
}
#line 10785 "MCNPParser.cpp"
    break;

  case 967: // start: message_block blank_line title_card cell_block blank_line surface_block blank_line data_block
#line 6349 "MCNPParser.bison"
        {
            std::vector<size_t> document_nodes;
            document_nodes.push_back((yystack_[7].value.node_index));
            document_nodes.push_back((yystack_[6].value.node_index));
            document_nodes.push_back((yystack_[5].value.node_index));
            document_nodes.insert(document_nodes.end(), (yystack_[4].value.node_indices)->begin(), (yystack_[4].value.node_indices)->end());
            delete (yystack_[4].value.node_indices);
            document_nodes.push_back((yystack_[3].value.node_index));
            document_nodes.insert(document_nodes.end(), (yystack_[2].value.node_indices)->begin(), (yystack_[2].value.node_indices)->end());
            delete (yystack_[2].value.node_indices);
            document_nodes.push_back((yystack_[1].value.node_index));
            document_nodes.insert(document_nodes.end(), (yystack_[0].value.node_indices)->begin(), (yystack_[0].value.node_indices)->end());
            delete (yystack_[0].value.node_indices);
            interpreter.push_staged_child(document_nodes);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
#line 10806 "MCNPParser.cpp"
    break;

  case 968: // start: message_block
#line 6366 "MCNPParser.bison"
        {
            std::vector<size_t> document_nodes;
            document_nodes.push_back((yystack_[0].value.node_index));
            interpreter.push_staged_child(document_nodes);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
#line 10817 "MCNPParser.cpp"
    break;

  case 969: // start: message_block blank_line title_card
#line 6373 "MCNPParser.bison"
        {
            std::vector<size_t> document_nodes;
            document_nodes.push_back((yystack_[2].value.node_index));
            document_nodes.push_back((yystack_[1].value.node_index));
            document_nodes.push_back((yystack_[0].value.node_index));
            interpreter.push_staged_child(document_nodes);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
#line 10830 "MCNPParser.cpp"
    break;

  case 970: // start: message_block blank_line title_card cell_block
#line 6382 "MCNPParser.bison"
        {
            std::vector<size_t> document_nodes;
            document_nodes.push_back((yystack_[3].value.node_index));
            document_nodes.push_back((yystack_[2].value.node_index));
            document_nodes.push_back((yystack_[1].value.node_index));
            document_nodes.insert(document_nodes.end(), (yystack_[0].value.node_indices)->begin(), (yystack_[0].value.node_indices)->end());
            delete (yystack_[0].value.node_indices);
            interpreter.push_staged_child(document_nodes);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
#line 10845 "MCNPParser.cpp"
    break;

  case 971: // start: message_block blank_line title_card cell_block blank_line surface_block
#line 6393 "MCNPParser.bison"
        {
            std::vector<size_t> document_nodes;
            document_nodes.push_back((yystack_[5].value.node_index));
            document_nodes.push_back((yystack_[4].value.node_index));
            document_nodes.push_back((yystack_[3].value.node_index));
            document_nodes.insert(document_nodes.end(), (yystack_[2].value.node_indices)->begin(), (yystack_[2].value.node_indices)->end());
            delete (yystack_[2].value.node_indices);
            document_nodes.push_back((yystack_[1].value.node_index));
            document_nodes.insert(document_nodes.end(), (yystack_[0].value.node_indices)->begin(), (yystack_[0].value.node_indices)->end());
            delete (yystack_[0].value.node_indices);
            interpreter.push_staged_child(document_nodes);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
#line 10863 "MCNPParser.cpp"
    break;

  case 972: // start: title_card cell_block blank_line surface_block blank_line data_block
#line 6407 "MCNPParser.bison"
        {
            std::vector<size_t> document_nodes;
            document_nodes.push_back((yystack_[5].value.node_index));
            document_nodes.insert(document_nodes.end(), (yystack_[4].value.node_indices)->begin(), (yystack_[4].value.node_indices)->end());
            delete (yystack_[4].value.node_indices);
            document_nodes.push_back((yystack_[3].value.node_index));
            document_nodes.insert(document_nodes.end(), (yystack_[2].value.node_indices)->begin(), (yystack_[2].value.node_indices)->end());
            delete (yystack_[2].value.node_indices);
            document_nodes.push_back((yystack_[1].value.node_index));
            document_nodes.insert(document_nodes.end(), (yystack_[0].value.node_indices)->begin(), (yystack_[0].value.node_indices)->end());
            delete (yystack_[0].value.node_indices);
            interpreter.push_staged_child(document_nodes);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
#line 10882 "MCNPParser.cpp"
    break;

  case 973: // start: title_card cell_block blank_line surface_block
#line 6422 "MCNPParser.bison"
        {
            std::vector<size_t> document_nodes;
            document_nodes.push_back((yystack_[3].value.node_index));
            document_nodes.insert(document_nodes.end(), (yystack_[2].value.node_indices)->begin(), (yystack_[2].value.node_indices)->end());
            delete (yystack_[2].value.node_indices);
            document_nodes.push_back((yystack_[1].value.node_index));
            document_nodes.insert(document_nodes.end(), (yystack_[0].value.node_indices)->begin(), (yystack_[0].value.node_indices)->end());
            delete (yystack_[0].value.node_indices);
            interpreter.push_staged_child(document_nodes);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
#line 10898 "MCNPParser.cpp"
    break;

  case 974: // start: title_card cell_block
#line 6434 "MCNPParser.bison"
        {
            std::vector<size_t> document_nodes;
            document_nodes.push_back((yystack_[1].value.node_index));
            document_nodes.insert(document_nodes.end(), (yystack_[0].value.node_indices)->begin(), (yystack_[0].value.node_indices)->end());
            delete (yystack_[0].value.node_indices);
            interpreter.push_staged_child(document_nodes);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
#line 10911 "MCNPParser.cpp"
    break;

  case 975: // start: title_card
#line 6443 "MCNPParser.bison"
        {
            std::vector<size_t> document_nodes;
            document_nodes.push_back((yystack_[0].value.node_index));
            interpreter.push_staged_child(document_nodes);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
#line 10922 "MCNPParser.cpp"
    break;

  case 976: // start: continue title_card data_block
#line 6450 "MCNPParser.bison"
        {
            std::vector<size_t> document_nodes;
            document_nodes.push_back((yystack_[2].value.node_index));
            document_nodes.push_back((yystack_[1].value.node_index));
            document_nodes.insert(document_nodes.end(), (yystack_[0].value.node_indices)->begin(), (yystack_[0].value.node_indices)->end());
            delete (yystack_[0].value.node_indices);
            interpreter.push_staged_child(document_nodes);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
#line 10936 "MCNPParser.cpp"
    break;

  case 977: // start: continue data_block
#line 6460 "MCNPParser.bison"
        {
            std::vector<size_t> document_nodes;
            document_nodes.push_back((yystack_[1].value.node_index));
            document_nodes.insert(document_nodes.end(), (yystack_[0].value.node_indices)->begin(), (yystack_[0].value.node_indices)->end());
            delete (yystack_[0].value.node_indices);
            interpreter.push_staged_child(document_nodes);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
#line 10949 "MCNPParser.cpp"
    break;

  case 978: // start: start blank_line
#line 6468 "MCNPParser.bison"
                          {interpreter.push_staged_child((yystack_[0].value.node_index)); if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}}
#line 10955 "MCNPParser.cpp"
    break;

  case 979: // start: start "anything block"
#line 6470 "MCNPParser.bison"
        {
            // This is an optional trailing block treated as a comment
            // that should consume the rest of the file
            // The ANYTHING_BLOCK token will only be triggered by the lexer after a
            // blank_line is encountered (following a data_block)
            auto a = interpreter.push_leaf(wasp::mcnpi::COMMENT, "anything", (yystack_[0].value.token_index));
            interpreter.push_staged_child(a);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
#line 10969 "MCNPParser.cpp"
    break;


#line 10973 "MCNPParser.cpp"

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
  MCNPParser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  MCNPParser::yytnamerr_ (const char *yystr)
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
  MCNPParser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  // MCNPParser::context.
  MCNPParser::context::context (const MCNPParser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  MCNPParser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
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
  MCNPParser::yy_syntax_error_arguments_ (const context& yyctx,
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
  MCNPParser::yysyntax_error_ (const context& yyctx) const
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


  const short MCNPParser::yypact_ninf_ = -1246;

  const signed char MCNPParser::yytable_ninf_ = -1;

  const short
  MCNPParser::yypact_[] =
  {
      92, -1246, -1246, -1246,  3293,    54,  3820,    21,    35, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246,   110,   110,   110,
   -1246, -1246, -1246, -1246,   110, -1246, -1246,   226, -1246, -1246,
   -1246,  2818,  3293, -1246,    55,  1151,  2018,   -50,  1076,   128,
    1658,  1076,    28,  1076,   256,   256,  1076,  1076,  1076,   128,
    1076,   128,  1076,   -50,   154,   193,   128,  2726,    26,   -50,
   -1246, -1246, -1246,   -50,   -50, -1246, -1246,   -50,   -50,   -50,
     206,   -50,   642,   642,  1076,   642,   -50,  1076,    27,   -50,
     -50,  2808,   128,   -50,    85, -1246,   -50,   -50,   -50,   256,
     -50,  1076,   642,   128,  1729,   -50,   232,   390,   128,   642,
     128,   128,    28,   128,   128,   642,   -50,   -50,   128,    -1,
     -50,    -2,   128,   128,   256,   128,   -50,   -50,   -50,   -50,
     -50,   -50,   -50,   642,   426,   -50,  1801,  1076,   -50,   262,
     108,   -50,   642,   642,   642,   642,   642,  1076,   -50,   -50,
     -50,   -50,   -50,   -50,   -50,   -50,  1076,  1076,    81,  1076,
    1076,   316,   629,     5,   -50,   -50,  1260,   268,   -50,   173,
    1076,  4190, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246,   -50, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,  4190,
   -1246,   -46, -1246,    60, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246,  2818,    21,  1467,  2083,   -50,   -50, -1246, -1246,  2083,
   -1246, -1246, -1246,   173, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,  1076, -1246,
   -1246,   206,  1076,  1076,  1076,   128,   206,  1076,  1076,  1076,
    1076,  1076,  1076,  1076,  1076,   206,  1076,   206,  1076,   821,
      81,   275, -1246,  1140,   206, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246,   377, -1246,  2726,
   -1246,   307,   308,   328,   128,  1899,   128,   309,   128, -1246,
   -1246,   206,  1334, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246,   642,   642,  1076,   642,   307,  1076,   386,  1908,  1696,
    1696, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246,  1747,  1747, -1246,  2808,   206,
     328, -1246,  2200,    77,   553,  1908,  1076,  1076,  1076,   642,
     206, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246,   377, -1246,  1729,  1076,   642, -1246, -1246,
   -1246, -1246,   377, -1246,   390,   206,   642,   206,   206,   128,
     206,   206,   206,   642,  1917,  2443,   206,   377,  1076,    81,
   -1246,    -2,   206,   206,  1076,  1076,   206,  1076,  1076,  1211,
     315,  1076,  1076,  1076,   642, -1246, -1246, -1246, -1246, -1246,
   -1246,   -50, -1246,   -50, -1246, -1246,   693,   128, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,  1886,
   -1246,  1801,  1076,   128, -1246, -1246, -1246, -1246, -1246, -1246,
     377, -1246,   108,   128,   642,   642,   642,   642,   642,  1076,
    2004,   128,  1908,  1076,  1076,  1076,  1076,  1076,  1076,  1076,
   -1246,  1076,  1076, -1246, -1246, -1246,    76,  1696,   316, -1246,
   -1246,  1575, -1246,  -111, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246,  1216, -1246,   629,   688,
   -1246, -1246, -1246, -1246, -1246, -1246,   377,  1216, -1246,   688,
     177,   326, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246,   377,  1216, -1246,  1260,
   -1246,   212,   330,   304,    71, -1246,   173,  1076,  4190,   128,
   -1246, -1246,   338,   464,   240, -1246,    60,  1076,  1076,  1129,
   -1246, -1246,  2528,  1076,   206,  1076,  1076,  1076,  1076,  1076,
    1076,  1076,    81,    81,    38, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246,   377,  1575, -1246, -1246,  1140,  1076, -1246,   226,
   -1246,   332, -1246,   308, -1246, -1246, -1246,   328,   206,  1696,
     206,   128,   625,   206,   128,   206, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246,  2518,   304, -1246,   386, -1246,   308,   386,
     206,  2618,  1696,  1320, -1246,  1696, -1246, -1246, -1246, -1246,
   -1246,   377,  1835, -1246, -1246, -1246, -1246,  1835, -1246,  1835,
   -1246,  1129,   328,  2412, -1246,  2259, -1246, -1246, -1246, -1246,
   -1246, -1246,   634,  1076,   642,  1076,   642,   206,  2655,  1076,
     642,   226, -1246, -1246,  2734,   642,   226, -1246, -1246,  1076,
    1076,  1076,   206,  1076,   883,  1076,  1076,  1076,  1076,  1076,
    1076,   226, -1246,  1076,    81, -1246,    81,    81,  1076,   642,
    1076,  1076, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
    1076, -1246,  1211,  1129,  2755,  1076,  1076,  1076,   875, -1246,
   -1246,   206,  1129, -1246,  1129, -1246,   206,   226, -1246, -1246,
     206, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246,   377,  1216, -1246,  2004,   206,   206,
    1076,  1076,  1076,  1076,  1076,  1076,   206,   332,  2267,  1696,
   -1246,  -111,  1076,  1076, -1246, -1246,  2001, -1246, -1246, -1246,
    2001, -1246,   688,   226, -1246,  2001,  2001, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246,   377,  1216, -1246,   177,   258,
     226, -1246,  2001,  2001, -1246, -1246, -1246,  2001, -1246, -1246,
     -33,    81, -1246,   304,    81, -1246, -1246,   206,   176, -1246,
   -1246, -1246,   123,   123, -1246,   123,   170,  2274, -1246,  3164,
   -1246,    36,   240,  1076,  1076, -1246,  1129, -1246,  1076,   206,
    1076,  1076,  1076,  1076,  1076,    81,    81, -1246,   203,   226,
   -1246,  1076,  1076, -1246, -1246,  1076,   332,   226, -1246,    81,
     614,  2419,  1076, -1246, -1246, -1246,  1696,   128,  1696,  1696,
      81,   206,    81,   352,    81, -1246, -1246,  1076,   354,  1943,
     356,  1696,  1696, -1246,   226, -1246, -1246,  1835, -1246,  1129,
     363,   226,   634,   654,  2259,  2259, -1246,  2435,  2259,  1076,
    1076,  1076,   362,   642, -1246,   366, -1246,  1076,  1076,  1076,
    1076,  1076,   642,  1076,  1076,  1076,  1076, -1246,    81,    81,
   -1246,    81,    81, -1246,    81,   642,  1076, -1246,  1129,   367,
    1076, -1246, -1246,   642, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246,   206, -1246,   363, -1246,  2599,  2311,  1129,
   -1246,  1129, -1246,    47,   226, -1246,  2001,  2001, -1246,   217,
    1076,   332, -1246, -1246,  2267,  1076,  2001, -1246, -1246,  2001,
     226, -1246,  2001,  2001, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246,   377,  1216, -1246,   258, -1246,  2001, -1246,
    1922,  1931, -1246, -1246, -1246, -1246, -1246, -1246, -1246,   377,
    1216, -1246,   -33, -1246,    81, -1246, -1246,  2419,  2146,  2274,
      91,   510, -1246,   123,   123,   107, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246,   314, -1246,   850, -1246, -1246,
   -1246, -1246, -1246, -1246,   862, -1246, -1246,    28, -1246,  2274,
   -1246,  3544,   642,    81,    81,    81,    81,   642,    81,    81,
      81,   642,   642,   642,    81,    81,    81, -1246, -1246, -1246,
     642,   642,   642,   642,   642, -1246, -1246, -1246,   642,   642,
     642,   642,   642,   642,   642,   642,   642,   642,   642,   642,
     642,   642, -1246,  4190, -1246,    36, -1246, -1246,  1076, -1246,
      81, -1246, -1246,  1076, -1246,    81, -1246,   143, -1246,   934,
   -1246,   614,  1076,  1696, -1246,    63,    81,    81,    81, -1246,
     335, -1246, -1246,  1076, -1246, -1246, -1246,  1943,  1899, -1246,
   -1246, -1246,   654,   634,  2259, -1246,   604,  1076, -1246, -1246,
    1076, -1246, -1246, -1246, -1246, -1246, -1246,   642,  2599,    81,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246,  1886, -1246,  2311,  1129, -1246, -1246, -1246,
   -1246, -1246, -1246,   377,  1216, -1246,   312, -1246,  2001, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,   377,
    1216, -1246,   -81,  1076, -1246, -1246,  2001,   226, -1246,  2001,
    2001, -1246, -1246, -1246, -1246,  1954,   226, -1246,  2001,  2001,
   -1246, -1246,   969,   377,   377, -1246, -1246,  2146, -1246, -1246,
   -1246,   123,   314,   850,   227, -1246,    81, -1246,   319,   322,
     369, -1246,   226, -1246,   363,   128,   206, -1246, -1246,   642,
   -1246, -1246, -1246, -1246,   642,    81,    81,    81,   642,   642,
     642, -1246, -1246, -1246,   642,   642,   642,   642,   642,   642,
     642,   642,   642,   642,   642,   642,   642,   642,   642,   642,
     642,   642,   642,  4190,  4190, -1246,    81, -1246, -1246, -1246,
   -1246, -1246,   614, -1246, -1246,    81,   346,    81, -1246, -1246,
   -1246, -1246,   654, -1246,  2599, -1246,  1129,  1129, -1246,   226,
   -1246,  2001,  2001, -1246,   226, -1246,  2001,  2001, -1246, -1246,
    2001, -1246, -1246,  2001, -1246,   226, -1246,   226, -1246, -1246,
     227, -1246,   322, -1246, -1246,    81,   381, -1246,   642, -1246,
     226,   206,   900, -1246, -1246, -1246,  4190,    81, -1246,    81,
   -1246, -1246, -1246,  1129, -1246,  2001, -1246,  2001, -1246, -1246,
   -1246, -1246,    81,   382,   871, -1246,   900,   226, -1246, -1246,
      81,   102,   327, -1246,   226, -1246, -1246,    81,    81, -1246,
     392, -1246,    81,    81,   396,    81,    81,   342,    81,    81,
     409,    81,    81,  1696, -1246,    81, -1246,  1696,    81,    81,
     133,   405, -1246, -1246
  };

  const short
  MCNPParser::yydefact_[] =
  {
     966,    18,    71,    69,    68,   968,     0,   975,     0,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    55,    56,    57,    58,    59,    60,    42,    44,     0,
      49,    50,    51,    52,     0,    53,    54,     0,    63,    48,
      61,    66,    65,    70,     0,     0,     0,     0,   381,     0,
       0,     0,     0,   665,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     328,   329,   330,     0,     0,   331,   332,     0,     0,     0,
       0,     0,     0,     0,   383,     0,     0,     0,     0,     0,
       0,   434,     0,     0,   443,   667,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   492,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   712,   714,     0,   717,
     719,   730,   750,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   849,   902,   903,   904,   905,   906,   907,   908,
     909,   910,   911,   912,   913,   932,     0,   914,   915,   916,
     917,   918,   919,   920,   921,   922,   923,   924,   925,   926,
     927,   928,   929,   930,   931,   933,   934,   935,   936,   937,
     938,   939,   940,   941,   942,   943,   944,   945,   946,   947,
     948,   949,   950,   951,   952,   953,   954,   955,   956,   957,
     958,   959,   960,   961,   962,   901,   900,   899,   898,   897,
     896,   895,   894,   892,   893,   860,   891,   890,   889,   888,
     886,   887,   881,   882,   883,   884,   885,   880,   878,   879,
     876,   877,   868,   869,   870,   871,   872,   873,   874,   875,
     865,   867,   866,   861,   862,   863,   864,   856,   858,   859,
     857,   854,   963,   855,   850,   852,   853,   851,   964,   977,
      72,     0,   145,   974,     1,   979,   978,    13,    43,    45,
      46,    47,     8,     7,     6,     9,    10,    14,    41,    62,
      64,    67,   969,     0,     0,     0,     0,   361,   481,     0,
     362,   397,    17,     0,    12,    11,   192,   194,   193,   195,
     196,    15,   223,   224,   227,   228,   226,   225,   380,   197,
     118,     0,   258,   256,   795,     0,     0,   666,     0,   848,
       0,   450,   263,   264,   265,     0,   845,     0,   259,     0,
       0,   268,    16,   293,     0,   297,   298,   299,   300,   301,
     302,   303,   304,   305,   306,   307,   308,     0,   314,   316,
     254,     0,   318,     0,     0,     0,     0,     0,     0,   119,
     323,   363,     0,   230,   231,   232,   234,   233,   235,   229,
     246,   378,   379,   382,   384,   386,   388,     0,     0,     0,
       0,   405,   422,   399,   400,   401,   402,   420,   403,   414,
     419,   404,   421,   411,   412,   413,   406,   407,   408,   409,
     410,   415,   416,   417,   418,     0,     0,   432,   435,     0,
       0,   444,     0,     0,     0,     0,   455,     0,   457,   458,
       0,   464,   461,   462,   463,   465,   466,   467,   468,   469,
     470,   471,   472,     0,   475,   477,     0,     0,   483,   484,
     485,   486,     0,   489,   491,     0,   494,     0,     0,     0,
       0,     0,     0,   501,     0,     0,     0,     0,     0,     0,
     508,   510,     0,     0,     0,   519,     0,     0,     0,     0,
       0,   549,     0,     0,   554,   555,   556,   557,   575,   576,
     577,     0,   581,     0,   582,   583,     0,     0,   617,   615,
     613,   614,   618,   619,   620,   621,   622,   623,   616,     0,
     626,   628,   629,     0,   632,   633,   634,   635,   636,   637,
       0,   640,   642,     0,   656,   657,   658,   659,   660,   661,
       0,     0,     0,     0,     0,     0,     0,     0,   713,   715,
     716,   718,   720,    78,   724,   723,     0,     0,   731,   199,
     727,     0,   728,   733,   744,   740,   742,   736,   743,   734,
     735,   737,   738,   739,   741,   747,     0,   748,   751,     0,
     753,   756,   752,   757,   754,   755,     0,     0,   762,   764,
       0,     0,   805,   804,   806,   801,   800,   799,   798,   797,
     796,   808,   807,   803,   802,   809,     0,     0,   814,   816,
     817,     0,     0,     0,     0,   842,   844,   846,   976,     0,
     965,    73,     0,     0,     0,   146,   970,     0,     0,     0,
     198,   321,     0,   257,     0,     0,   847,   449,     0,     0,
       0,   260,     0,     0,     0,   269,   272,   273,   271,   270,
     274,   275,     0,     0,   288,   290,   292,   295,     5,     0,
     310,   313,   315,   317,   255,     2,     3,   319,     0,     0,
       0,   333,   351,     0,     0,     0,   324,   364,   365,   366,
     367,   368,   369,   374,     0,   372,   377,   247,   385,   393,
       0,   389,     0,   394,   203,   398,   238,   237,   241,   240,
     239,     0,     0,   236,   248,   423,   430,     0,   426,   431,
     433,     0,   320,     6,   208,     0,   209,   212,   213,   211,
     210,   214,   437,   440,   438,     0,   441,     0,   453,   456,
       0,     0,   473,   476,   478,   482,     0,   487,   490,     0,
       0,     0,     0,     0,     0,     0,     0,   445,     0,   447,
       0,     0,   504,   505,     0,   509,     0,     0,   520,     0,
     522,   630,   541,   524,   525,   526,   527,   528,   529,   530,
     531,   532,   533,   534,   535,   536,   537,   538,   539,   540,
     543,   544,   546,     0,   550,   553,   547,     0,   579,   585,
     584,     0,     0,   250,   625,   627,     0,     0,   639,   641,
       0,   668,   679,   669,   670,   671,   672,   673,   674,   675,
     676,   677,   678,   680,     0,     0,   685,   687,     0,     0,
     705,   707,   708,   709,   710,   711,     0,   722,     0,     0,
     200,   732,     0,   726,   729,   243,     0,   242,   249,   252,
     746,   749,   765,     0,   761,     0,   759,   763,   768,   769,
     767,   772,   771,   766,   770,     0,     0,   777,   779,     0,
       0,   813,     0,   811,   815,   818,   820,     0,   821,   819,
       0,     0,   286,     0,     0,   282,   843,     0,     0,    76,
      74,    77,     0,     0,    80,     0,    85,   142,   153,     0,
     190,   973,     0,   261,   523,   244,   459,   309,   387,     0,
       0,   663,   296,   664,   262,     0,     0,   281,     0,     0,
     279,     0,   277,   289,   291,   294,   312,     0,     4,     0,
       0,     0,     0,   350,   325,   327,     0,   336,     0,     0,
       0,     0,     0,   376,     0,   371,   373,     0,   391,     0,
     396,     0,     0,   204,     0,   425,   428,   429,   427,   506,
       6,     0,   436,     0,     0,     0,   215,     0,     0,   439,
     442,     0,   452,   460,   474,   480,   488,   493,   495,   496,
       0,   498,   499,   500,   446,   448,   502,   503,     0,     0,
     512,   514,     0,   516,   518,   521,   542,   545,   548,   552,
     558,   559,   560,     0,   561,   562,   568,   569,   563,   564,
     565,   566,   567,     0,   570,     0,   573,   580,     0,   624,
     251,     0,   638,     0,     0,   684,     0,   682,   686,     0,
       0,   721,    79,   201,     0,   725,   745,   253,   760,   758,
       0,   776,     0,   774,   778,   785,   783,   782,   786,   780,
     787,   781,   784,     0,     0,   792,   794,   812,   810,   822,
       0,   827,   831,   832,   828,   829,   830,   833,   834,     0,
       0,   839,   841,   287,     0,   284,   283,     0,     0,   140,
      84,     0,    75,     0,    81,     0,   111,    86,    87,   112,
     113,    88,    89,    90,    91,     0,    92,     0,   116,    93,
      94,   117,   114,   115,     0,   127,   128,     0,   134,   138,
     143,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   147,   148,   149,
       0,     0,     0,     0,     0,   150,   151,   152,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   188,     0,   191,   971,   245,   322,   662,   266,
       0,   280,   278,   276,   311,     0,   344,   354,   335,   352,
     334,     0,     0,     0,   339,     0,     0,     0,     0,   346,
     359,   375,   370,   392,   390,   205,   395,     0,   207,   424,
     222,   216,     0,   218,     0,   219,     0,   454,   451,   479,
     497,   507,   511,   513,   515,   517,   551,   572,     0,     0,
     574,   589,   588,   604,   607,   586,   587,   590,   591,   592,
     593,   594,   595,   596,   597,   598,   599,   600,   601,   602,
     603,   605,   606,     0,   610,   612,   631,   643,   644,   646,
     647,   645,   648,     0,     0,   653,   655,   683,   681,   691,
     693,   688,   689,   690,   692,   696,   697,   694,   695,     0,
       0,   702,   704,   706,   202,   775,   773,     0,   791,     0,
     789,   793,   824,   823,   826,     0,     0,   838,     0,   836,
     840,   285,   348,     0,     0,   129,   136,   139,   144,   141,
      82,    83,     0,     0,     0,   102,     0,   108,     7,     0,
      95,    99,     0,   121,     0,     0,     0,   135,   189,   154,
     155,   156,   157,   158,   159,     0,     0,     0,   163,   164,
     165,   166,   167,   168,   170,   171,   172,   174,   175,   176,
     177,   178,   179,   183,   186,   180,   181,   182,   184,   185,
     187,   169,   173,   972,     0,   267,     0,   355,   345,   353,
     338,   337,     0,   326,   341,     0,   356,     0,   358,   347,
     206,   217,     0,   221,   578,   571,     0,   609,   611,     0,
     652,     0,   650,   654,     0,   701,     0,   699,   703,   790,
     788,   825,   837,   835,   349,     0,   131,     0,   133,   137,
       0,   109,     0,   101,   106,     0,     0,    98,     0,   120,
       0,     0,     0,   160,   161,   162,   967,     0,   340,     0,
     357,   343,   220,   608,   651,   649,   700,   698,   130,   132,
     107,   100,     0,     0,     0,   122,     0,     0,   125,   342,
       0,     0,     0,    96,     0,   126,   123,     0,     0,   103,
       0,   124,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   360,     0,    97,   110,     0,     0,
       0,     0,   104,   105
  };

  const short
  MCNPParser::yypgoto_[] =
  {
   -1246,  -610,   -28,  2329,  -379,   401,   131,   -47,  -164,   353,
      50, -1246, -1246,   375, -1246,   -32,   385, -1246, -1246, -1246,
      -6,    67, -1246, -1246, -1246, -1246, -1246, -1246,  1882,  -920,
    -622,  -473, -1246, -1241, -1246, -1245, -1246, -1246, -1246,   -15,
    1512, -1038,  -859, -1246, -1246,  -645, -1246,  -280,   117, -1246,
   -1246, -1246,  -676,  -894,  -465,  -443,  -397,  -383,  -267,  -236,
    3043,  -415,  -660,  -724,  -706,  1761,  1156,  -371,  -877,  -283,
     -61,  -527,   528,  -790,   543,  -289, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246,  -227,  -308, -1246, -1246,
   -1246, -1246, -1246,  -650,  -228,    65, -1246, -1246, -1246, -1246,
   -1246,  -451, -1246,  -718,  -730,  -716, -1246,  -683,  -694, -1144,
   -1246,  -698,    -7, -1246, -1246,  -639,    74, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246,   436, -1246, -1246,
   -1246,  -434,  -232,    53, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
      30, -1246, -1246,   -27, -1246, -1246,    24, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246,     9, -1246, -1246,  -486, -1246, -1246,  -488, -1246, -1246,
   -1246, -1246, -1246, -1246,  -285, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246, -1012,  -693, -1246, -1246,
       1, -1246, -1246, -1246,  -709, -1246, -1246, -1246,     2, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246,   -13, -1246, -1246, -1246,
   -1246,  -708, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246, -1246,  -288, -1246, -1246, -1246,
   -1246,  -720, -1246, -1246, -1246, -1246, -1246, -1246, -1246, -1246,
   -1246, -1246, -1246, -1246, -1246,  -165, -1246,  -553,    -4, -1246,
   -1246,   -35, -1246, -1246, -1246, -1246,  -571,   -17, -1246, -1246,
   -1246,  -306, -1246, -1246, -1246, -1246,  -483, -1246, -1246, -1246,
   -1246, -1246,   -45, -1246, -1246, -1246,  -478, -1246, -1246, -1246,
   -1246,  -494, -1246, -1246, -1246, -1246, -1246, -1246, -1246,  -296,
    -180, -1246
  };

  const short
  MCNPParser::yydefgoto_[] =
  {
       0,   686,   687,   971,   317,   341,   308,   734,   342,  1305,
     333,     4,    47,    48,    49,    50,    51,    52,     5,     6,
      54,     7,   301,   643,   902,  1093,   903,   904,   905,  1043,
    1094,   907,  1114,  1311,  1115,  1307,  1116,  1466,  1117,   975,
     651,  1118,  1296,  1119,  1297,  1120,  1298,   302,   303,  1160,
    1161,   909,  1162,   910,   911,   343,   344,   345,   346,   347,
     348,   578,   713,   741,   742,   349,   723,   857,   858,   916,
     411,   725,   859,   814,   860,   392,   182,   183,   184,   185,
     186,   187,   371,   188,   672,   673,   674,   635,   676,   189,
     190,   191,   387,   919,   681,   388,   389,   192,   193,   194,
     195,   652,   401,   948,  1364,   949,   196,   692,  1176,  1189,
    1177,  1190,   197,   198,   704,   705,   706,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   445,
     446,   728,   729,   447,   448,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   473,
     474,   475,   225,   226,   227,   482,   483,   484,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   500,   501,   241,  1000,  1001,   242,  1003,  1004,   243,
     244,   245,   246,   800,   801,   802,   247,   248,   249,   250,
     251,   252,   521,   522,  1024,  1025,  1026,  1027,   523,   524,
     525,   526,   253,  1243,  1244,  1245,   254,   539,   540,   541,
     255,   256,   257,   258,   259,   550,   551,   552,   260,  1253,
    1254,  1255,  1256,   261,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   834,   835,   836,   837,   272,  1269,
    1270,  1271,  1272,   273,   274,   275,   276,   277,   278,   279,
     280,   281,   282,   283,   284,   580,   581,   582,   583,   285,
     596,   597,   598,   286,   606,   607,   608,   609,   287,   875,
     876,   877,   878,   288,  1063,  1064,  1065,  1066,   289,   290,
     626,   627,   628,   629,   291,   631,  1070,  1071,   292,  1079,
    1080,  1081,  1082,   293,   636,   294,   295,   296,   297,   298,
     299,     8
  };

  const short
  MCNPParser::yytable_[] =
  {
     318,   638,   306,   640,   570,   715,   940,   595,   680,   736,
    1069,   726,   813,  1008,   599,  1220,  1259,  1164,   976,   319,
    1178,   906,  1029,   645,   977,   649,   978,   972,   328,   973,
     854,   937,   412,   641,   414,   304,   417,   390,   867,  1195,
     764,    53,    53,   350,   351,   678,  1369,   356,   327,   330,
    1295,   459,   959,  1201,   365,   737,   367,  1401,   486,  1404,
      53,   374,  1403,   917,   493,   675,    53,   956,  1407,   738,
     956,   334,   335,   181,   724,   724,   573,   938,   678,   334,
     335,  1317,   514,   678,   574,   334,   335,   449,   743,   334,
     335,   554,   555,   556,   557,   558,   399,     1,   460,   334,
     335,   300,   683,   485,   573,   487,   488,   490,   491,   492,
     334,   335,   355,   496,   358,   360,  1042,   502,   503,   307,
     506,   322,   938,   369,  1274,   847,   708,     2,   391,   393,
     642,   334,   335,   394,   395,   908,   573,   396,   397,   398,
     300,   402,  1471,   350,  1247,  1248,   415,  1042,   418,   419,
     420,   334,   335,   450,  1357,  1430,   452,   453,   454,   455,
     457,  1431,   848,   370,     2,   476,     3,  1072,   724,   309,
     310,  1300,   489,  1073,   332,   311,   494,   495,   334,   335,
     498,   633,   634,   573,   504,   739,   507,   508,   509,   510,
     511,   512,   513,  1283,  1069,   527,   927,  1302,   543,  1303,
     899,   553,   372,   336,   337,   338,   339,   340,   560,   561,
     562,   563,   564,   565,   566,   567,   740,   403,   404,   405,
     406,   407,  1369,   885,   610,   611,   399,   886,   632,   893,
     451,   887,   888,   917,   312,   313,   372,   314,   315,   941,
     573,  1246,  1249,   477,  1250,  1092,   639,   899,   976,   976,
     332,   332,   332,  1205,   977,   977,   978,   978,  1179,  1295,
     305,  1360,  1260,  1181,   334,   335,  1185,   497,  1202,  1203,
     499,  1164,  1206,   544,   868,   869,   813,  1370,   915,   630,
     906,  1090,  1371,  1091,   663,   813,   937,  1030,   966,   319,
     736,   867,   736,   575,   576,   968,   328,   644,   854,   736,
     680,  1197,  1198,  1261,  1262,  1263,  1264,  1265,  1266,  1267,
    1268,   678,   334,   335,  1259,   944,   327,   330,   390,   684,
     694,   678,   327,   372,   334,   335,   803,   573,   896,   573,
     312,  1308,   685,   314,   315,   774,   737,   879,   737,   908,
     654,   890,   640,   334,   335,   737,  1368,   898,   917,   408,
     738,   724,   738,  1088,   334,   335,   724,  1420,   724,   738,
     915,  1171,  1220,  1191,   600,  1194,   645,  1196,   934,  1033,
     678,   316,   870,  1208,   871,   647,   648,  1209,  1216,   688,
     690,   693,   573,   695,   678,   312,   313,  1039,   314,   315,
    1433,  1442,   744,   746,  1406,  1041,   336,   337,   338,   339,
     340,  1454,  1450,   710,  1292,  1457,  1251,  1252,  1283,  1247,
    1248,  1074,  1075,  1076,  1077,  1078,   755,  1460,  1463,  1042,
     601,   602,   732,   603,   604,   605,   752,   320,   373,  1089,
     945,  1363,   724,   595,  1044,   757,   889,   321,  1399,   646,
     747,   724,  1418,   724,  1299,  1318,  1087,  1165,   969,   933,
     772,   936,  1422,  1377,   682,  1055,   336,   337,   338,   339,
     340,   545,   546,   547,   548,   549,   739,   316,   739,   892,
     895,  1301,   334,   335,   762,   739,  1373,   573,   976,   976,
     332,  1030,   976,  1358,   977,   977,   978,   978,   977,  1366,
     978,   709,   331,   409,   409,   967,   409,   740,  1372,   740,
    1362,   730,  1030,   818,   813,   753,   740,  1249,   758,  1250,
     775,   574,   811,   409,  1443,  1213,  1215,  1007,   334,   335,
     409,  1449,   316,   573,  1042,  1374,   409,   810,   816,   736,
     736,   736,   736,   968,   736,   736,  1378,   872,   820,   819,
     955,   899,   900,   815,   409,  1166,   838,   839,  1383,  1038,
    1472,  1473,  1388,   409,   409,   409,   409,   409,  1174,   864,
    1260,   334,   335,   861,   745,   697,   698,   699,   700,   701,
     702,   807,  1054,   808,   851,   737,   737,   737,   737,   881,
     737,   737,   862,  1281,   884,  1092,  1423,   899,  1290,   738,
     738,   738,   738,  1285,   738,   738,   724,     0,  1166,   873,
     874,  1261,  1262,  1263,  1264,  1265,  1266,  1267,  1268,     0,
     998,     0,   312,   313,     0,   314,   315,   573,  1056,   350,
       0,     0,   334,   335,   897,   930,     0,   573,  1042,   478,
     479,   480,   481,   334,   335,     0,   943,   724,   573,     0,
     912,     0,   312,   313,     0,   314,   315,   573,   976,   350,
     334,   335,     0,     0,   977,     0,   978,     0,   724,     0,
     724,     0,   312,   313,     0,   314,   315,   573,  1042,   350,
       0,  1251,  1252,     0,   965,     0,   942,     0,     0,   951,
       0,  1057,     0,  1058,  1059,  1060,  1061,  1062,     0,   983,
       0,     0,     0,   403,   404,   405,   406,   407,   962,     0,
       0,     0,     0,   992,   984,   739,   739,   739,   739,   986,
     739,   739,     0,   584,     0,     0,   813,     0,  1005,  1030,
     575,   576,     0,     0,   997,     0,     0,  1083,     0,  1085,
    1086,   515,   516,   517,     0,     0,   740,   740,   740,   740,
       0,   740,   740,     0,   336,   337,   338,   339,   340,   316,
     518,   519,   520,     0,   336,   337,   338,   339,   340,   736,
     736,   736,     0,   736,     0,   336,   337,   338,   339,   340,
    1032,   662,     0,     0,   336,   337,   338,   339,   340,   316,
       0,     0,   403,   404,   405,   406,   407,  1035,     0,     0,
    1192,     0,     0,  1023,   336,   337,   338,   339,   340,   316,
       0,     0,     0,     0,     0,   737,   737,   737,     0,   737,
       0,     0,   409,   409,     0,   409,  1048,     0,     0,   738,
     738,   738,     0,   738,   585,   408,     0,     0,  1051,   334,
     335,     0,     0,  1067,  1211,   586,     0,     0,   660,     0,
       0,   587,   588,     0,     0,     0,   409,   409,     0,   813,
    1030,     0,     0,     0,   409,   409,   944,   678,   312,  1308,
     409,   314,   315,     0,     0,  1412,   944,     0,     0,   678,
     312,   313,   724,   314,   315,   724,     0,   944,   409,   334,
     335,     0,  1172,     0,     0,  1042,   944,   409,     0,     0,
     350,   334,   335,     0,   409,     0,  1030,     0,     0,   917,
       0,     0,     0,     0,     0,  1163,   944,   678,   312,   313,
       0,   314,   315,     0,   408,   409,   917,  1199,     0,     0,
    1291,     0,     0,     0,  1200,     0,     0,     0,     0,   736,
       0,     0,  1182,     0,     0,   739,   739,   739,     0,   739,
     409,     0,   334,   335,   962,  1359,     0,   573,     0,     0,
       0,     0,  1217,     0,     0,   409,   409,   409,   409,   409,
    1436,   336,   337,   338,   339,   340,   740,   740,   740,     0,
     740,   945,  1363,     0,     0,   737,     0,   334,   335,     0,
    1394,   945,   573,  1353,     0,     0,     0,  1257,     0,   738,
       0,     0,   945,     0,     0,   316,     0,   409,   515,   516,
     517,   945,     0,  1275,   809,   724,   724,   316,   409,     0,
       0,   403,   404,   405,   406,   407,  1278,   518,   519,   520,
       0,   945,     0,   403,   404,   405,   406,   407,   409,     0,
       0,     0,  1287,     0,   576,     0,   589,   590,   591,   592,
     593,   594,     0,     0,   901,   316,     0,   600,  1467,     0,
     409,     0,   724,     0,     0,  1375,     0,   640,     0,     0,
    1310,  1319,     0,   925,   926,     0,  1324,  1313,     0,     0,
    1328,  1329,  1330,     0,   336,   337,   338,   339,   340,  1334,
    1335,  1336,  1337,  1338,   334,   335,   332,  1339,  1340,  1341,
    1342,  1343,  1344,  1345,  1346,  1347,  1348,  1349,  1350,  1351,
    1352,     0,  1316,   601,   602,   739,   603,   604,   605,   336,
     337,   338,   339,   340,     0,     0,     0,     0,     0,     0,
     640,     0,     0,   409,     0,     0,     0,     0,   409,     0,
     409,     0,   409,     0,     0,     0,   740,   334,   335,     0,
     716,   717,     0,   408,   718,   409,     0,   409,   633,   634,
     866,   409,     0,     0,     0,   408,   409,     0,     0,  1354,
       0,     0,     0,     0,  1314,   409,     0,  1315,   323,   324,
     883,     0,     0,     0,  1416,     0,     0,   999,  1002,     0,
     409,     0,   962,   962,     0,     0,   944,  1011,  1012,  1013,
    1014,  1015,  1016,  1017,  1018,  1019,  1020,  1021,  1022,     0,
       0,     0,     0,     0,   409,     0,  1380,     0,     0,     0,
       0,     0,     0,   409,     0,   409,   336,   337,   338,   339,
     340,   944,  1385,   678,   334,   335,     0,   716,   717,     0,
    1389,   718,   855,     0,     0,     0,   409,     0,     0,  1392,
       0,     0,     0,     0,   782,     0,  1396,  1398,   410,   410,
       0,   410,     0,     0,     0,     0,  1310,   409,     0,     0,
       0,   409,  1310,     0,     0,  1409,   409,   409,   410,   403,
     404,   405,   406,   407,   719,   410,     0,   409,     0,     0,
       0,   410,     0,   409,   409,     0,     0,   720,   409,     0,
     664,   665,   666,   667,   668,   669,   670,   671,   325,   410,
    1411,   945,     0,   901,   901,     0,   901,   901,   410,   410,
     410,   410,   410,     0,     0,    80,    81,   409,    82,    83,
      84,    85,    86,    87,    88,    89,  1169,  1170,   334,   335,
       0,   960,  1424,   573,     0,   350,   945,  1426,     0,     0,
    1175,     0,   334,   335,     0,     0,     0,  1434,  1428,     0,
    1429,  1186,     0,  1188,     0,  1310,   403,   404,   405,   406,
     407,   719,     0,  1435,     0,  1438,     0,     0,   409,     0,
     409,     0,     0,     0,   720,     0,     0,     0,  1037,     0,
       0,     0,     0,     0,   409,     0,     0,   115,  1047,  1445,
    1446,     0,     0,   409,  1047,     0,     0,  1451,     0,  1046,
    1212,   408,   999,  1214,     0,  1002,   409,     0,  1049,   409,
       0,  1047,     0,     0,   409,     0,     0,     0,     0,  1053,
       0,     0,     0,     0,     0,  1068,     0,     0,   326,     0,
     409,     0,   409,     0,     0,     0,     0,   409,   409,     0,
       0,     0,     0,     0,     0,     0,   612,   409,     0,     0,
     409,     0,     0,   409,   409,   613,     0,     0,     0,     0,
     336,   337,   338,   339,   340,   409,     0,     0,     0,   409,
       0,   409,   409,     0,   336,   337,   338,   339,   340,     0,
       0,   409,     0,     0,   323,   324,     0,     0,   408,     0,
       0,   901,   901,     0,   901,   901,   783,   784,   785,   786,
     787,   788,   789,   790,   791,   792,   793,   794,   795,   796,
     797,   798,   799,   697,   698,   699,   700,   701,   702,     0,
       0,     0,     0,   409,  1320,  1321,  1322,  1323,   409,  1325,
    1326,  1327,   409,   409,   409,  1331,  1332,  1333,     0,     0,
       0,   409,   409,   409,   409,   409,     0,     0,     0,   409,
     409,   409,   409,   409,   409,   409,   409,   409,   409,   409,
     409,   409,   409,     0,     0,  1047,     0,   707,   707,     0,
     707,  1355,     0,     0,  1047,     0,  1356,  1047,  1175,  1258,
       0,  1047,   678,   334,   335,     0,     0,  1365,  1188,  1367,
       0,  1188,     0,     0,     0,  1276,  1047,     0,     0,     0,
       0,     0,   400,     0,     0,   614,     0,  1280,     0,   410,
     410,     0,     0,     0,     0,   707,     0,     0,   409,     0,
       0,     0,     0,  1289,     0,     0,     0,     0,     0,     0,
       0,    80,    81,   410,    82,    83,    84,    85,    86,    87,
      88,    89,   707,     0,   409,     0,     0,   409,     0,   707,
       0,     0,     0,     0,     0,   409,     0,     0,     0,   409,
       0,     0,     0,     0,     0,     0,   334,   335,     0,     0,
     707,   409,     0,     0,     0,     0,     0,   409,     0,     0,
     409,   409,     0,     0,     0,     0,   409,     0,     0,   409,
     409,   615,   616,   617,   618,   619,   620,   621,   622,   623,
     624,   625,   901,   115,   334,   335,     0,  1405,     0,   573,
     707,   707,   707,   707,   707,   336,   337,   338,   339,   340,
     409,     0,     0,     0,     0,   409,  1413,  1414,  1415,   409,
     409,   409,     0,     0,     0,   409,   409,   409,   409,   409,
     409,   409,   409,   409,   409,   409,   409,   409,   409,   409,
     409,   409,   409,   409,   678,   334,   335,  1417,   716,   717,
     461,     0,   718,     0,     0,     0,  1419,  1188,  1421,     0,
       0,     0,     0,     0,     0,     0,     0,   409,   409,     0,
       0,     0,   409,   409,     0,     0,  1047,   409,   409,     0,
       0,   409,     0,     0,   409,     0,     0,  1382,   336,   337,
     338,   339,   340,   352,  1047,     0,  1432,     0,  1047,   409,
       0,     0,     0,  1387,     0,     0,     0,  1047,  1439,     0,
    1440,     0,  1390,     0,   409,     0,   409,     0,   409,     0,
       0,  1393,     0,  1441,     0,   409,   336,   337,   338,   339,
     340,  1447,  1448,   334,   335,     0,   716,   717,  1452,  1453,
     718,     0,     0,  1455,  1456,     0,  1458,  1459,     0,  1461,
    1462,     0,  1464,  1465,     0,     0,  1468,     0,   655,  1469,
    1470,     0,     0,     0,     0,     0,     0,   658,     0,   659,
       0,     0,     0,     0,     0,     0,   677,   403,   404,   405,
     406,   407,   719,   678,   334,   335,     0,   716,   717,     0,
     707,   718,   707,     0,     0,   720,   410,   334,   335,     0,
    1047,   707,   573,   696,   350,  1047,   334,   335,  1047,     0,
     410,  1047,     0,   350,  1425,   334,   335,     0,   766,  1427,
     334,   335,   579,   716,   717,   410,     0,   718,   855,   334,
     335,  1282,   716,   717,     0,     0,   718,   855,     0,     0,
    1284,   334,   335,  1047,     0,  1047,   573,  1042,   350,     0,
       0,   731,   334,   335,     0,   716,   717,     0,   721,   718,
     855,     0,   750,  1391,     0,   403,   404,   405,   406,   407,
     719,     0,   462,   463,   464,   465,   466,   467,   468,   469,
     470,   471,   472,   720,     0,     0,     0,   759,     0,   760,
     761,   528,   763,     0,   765,     0,     0,   529,   770,   334,
     335,     0,   716,   717,   776,   777,   718,   855,   779,   408,
       0,     0,     0,     0,     0,     0,   403,   404,   405,   406,
     407,   719,     0,     0,     0,   329,    56,     0,     0,   336,
     337,   338,   339,   340,   720,     0,     0,     0,   336,   337,
     338,   339,   340,   577,     0,     0,   721,   336,   337,   338,
     339,   340,   403,   404,   405,   406,   407,   719,     0,     0,
       0,   403,   404,   405,   406,   407,   719,     0,     0,     0,
     720,     0,     0,   336,   337,   338,   339,   340,     0,   720,
       0,     0,     0,     0,   403,   404,   405,   406,   407,   719,
     329,   324,     0,     0,     0,     0,     0,   408,     0,   650,
       0,     0,   720,     0,   650,   650,     0,     0,   650,     0,
     650,     0,   650,   650,   650,   650,     0,   650,     0,   650,
     530,   531,   532,   533,   534,   535,   536,   537,     0,   707,
       0,   403,   404,   405,   406,   407,   719,     0,   707,   538,
       0,     0,     0,     0,     0,     0,   691,     0,   408,   720,
       0,   707,     0,  1095,     0,     0,   920,     0,     0,   410,
       0,     0,     0,     0,   650,     0,     0,   650,     0,     0,
     714,   579,    80,    81,     0,    82,    83,    84,    85,    86,
      87,    88,    89,     0,   408,     0,     0,     0,     0,     0,
     939,     0,     0,   408,     0,   950,     0,   952,   312,   313,
      99,   733,   315,   573,     0,     0,     0,   650,     0,   650,
       0,     0,   957,     0,     0,     0,   408,  1096,  1097,  1098,
    1099,  1100,  1101,  1102,  1103,  1104,  1105,  1106,  1107,  1108,
    1109,  1110,  1111,  1293,  1294,     0,     0,    80,    81,     0,
      82,    83,    84,    85,    86,    87,    88,    89,     0,   981,
       0,     0,     0,     0,     0,     0,   650,   312,   313,     0,
     970,   315,   573,   408,   990,   334,   335,   689,   410,     0,
     573,  1042,     0,   410,     0,     0,     0,   410,   410,   410,
       0,  1095,     0,     0,     0,     0,   410,   410,   410,   410,
     410,   712,   577,   650,   410,   410,   410,   410,   410,   410,
     410,   410,   410,   410,   410,   410,   410,   410,     0,     0,
     650,     0,     0,  1028,     0,     0,     0,     0,  1031,   650,
     650,     0,   650,   650,   735,     0,     0,     0,   579,   850,
     336,   337,   338,   339,   340,   316,     0,     0,     0,     0,
       0,  1040,     0,     0,     0,  1096,  1097,  1098,  1099,  1100,
    1101,  1102,  1103,  1104,  1105,  1106,  1107,  1108,  1109,  1110,
    1111,     0,     0,   707,   821,   822,   823,   824,   825,   826,
     827,   828,   829,   830,   831,   832,   833,     0,     0,     0,
       0,     0,     0,     0,  1112,     0,     0,     0,   650,   336,
     337,   338,   339,   340,   316,     0,     0,   336,   337,   338,
     339,   340,  1113,     0,   650,     0,     0,   650,   650,   678,
     312,   313,   650,   970,   315,   573,     0,   334,   335,     0,
       0,  1167,   573,     0,     0,   917,     0,     0,     0,     0,
       0,     0,     0,   312,   313,     0,   314,   315,   573,     0,
     691,   334,   335,   947,   768,     0,     0,     0,     0,   577,
     849,     0,     0,  1187,   650,     0,     0,     0,     0,     0,
       0,     0,   650,   714,   963,   707,   850,     0,     0,     0,
     707,     0,     0,     0,   707,   707,   707,     0,     0,     0,
     707,   707,   707,   707,   707,   707,   707,   707,   707,   707,
     707,   707,   707,   707,   707,   707,   707,   707,   707,   650,
     650,  1221,     0,     0,     0,   650,     0,  1222,     0,     0,
       0,     0,  1112,     0,  1223,     0,   334,   335,   650,   953,
     650,     0,     0,     0,   650,  1218,   334,   335,     0,   650,
    1113,   650,   650,     0,   917,     0,     0,     0,     0,     0,
       0,     0,   336,   337,   338,   339,   340,   316,     0,   336,
     337,   338,   339,   340,   410,   650,   650,   650,     0,     0,
       0,   689,     0,     0,   946,   336,   337,   338,   339,   340,
     316,     0,     0,   336,   337,   338,   339,   340,     0,     0,
     707,     0,     0,     0,   712,   961,     0,   849,     0,  1224,
       0,   650,   650,   650,   650,   650,   650,     0,     0,   850,
     579,     0,     0,     0,   650,   735,     0,   735,     0,     0,
       0,     0,     0,     0,   974,     0,   334,   335,     0,   958,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1225,  1226,  1227,  1228,  1229,  1230,  1231,  1232,  1233,  1234,
    1235,  1236,  1237,  1238,  1239,  1240,  1241,  1242,   336,   337,
     338,   339,   340,   334,   335,     0,   982,     0,   336,   337,
     338,   339,   340,     0,   650,   650,     0,     0,     0,   650,
       0,     0,   650,   650,   650,   650,     0,     0,     0,     0,
       0,     0,     0,   650,     0,     0,   650,     0,     0,     0,
       0,   947,   691,  1180,     0,     0,     0,   691,     0,  1184,
     691,     0,     0,     0,     0,     0,   679,     0,     0,     0,
     963,     0,   714,   714,     0,     0,     0,     0,     0,     0,
     849,   577,     0,     0,     0,     0,     0,     0,     0,     0,
     650,   650,   334,   335,     0,   985,     0,     0,   650,   650,
     650,     0,   650,     0,   650,   650,   650,   650,   336,   337,
     338,   339,   340,   334,   335,     0,  1009,   650,     0,     0,
       0,   650,     0,     0,   722,   727,   375,   376,   377,   378,
     379,   380,   381,   382,   383,   384,   385,   386,     0,     0,
       0,     0,     0,     0,     0,   336,   337,   338,   339,   340,
       0,     0,   751,     0,     0,   850,   650,     0,     0,     0,
       0,   756,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   946,   689,     0,     0,   771,     0,   689,     0,
    1183,   689,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   961,     0,   712,   712,     0,     0,     0,   691,     0,
       0,     0,     0,     0,   974,   974,   735,   735,     0,  1204,
     735,    31,    32,    33,    34,    35,    36,     0,   812,     0,
       0,     0,     0,     0,   336,   337,   338,   339,   340,   817,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
       0,     0,   421,     0,     0,   336,   337,   338,   339,   340,
       0,     0,     0,     0,     0,   846,     0,     0,     0,     0,
     852,  1011,  1012,  1013,  1014,  1015,  1016,  1017,  1018,  1019,
    1020,  1021,  1022,     0,     0,   856,   849,     0,     0,   650,
       0,     0,     0,     0,   650,   863,   865,     0,     0,     0,
     947,     0,   947,  1361,   691,     0,   947,     0,     0,     0,
       0,     0,     0,     0,   650,   880,   882,     0,   963,   963,
       0,     0,   891,   894,     0,     0,     0,     0,   650,   689,
       0,   650,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1306,     0,     0,
       0,     0,     0,   928,     0,     0,     0,     0,     0,     0,
       0,   929,   931,   422,   423,   424,   425,   426,   427,   428,
     429,   430,   431,     0,   432,   433,   434,   435,   436,   437,
     438,   439,   440,   441,   442,   443,   444,     0,     0,     0,
       0,     0,     0,   954,   650,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     964,     0,     0,   947,     0,     0,     0,     0,     0,     0,
       0,   946,     0,   946,     0,   689,     0,   946,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   961,
     961,     0,     0,     0,   974,   974,   735,     0,   974,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   353,   354,     0,   357,   359,   361,   362,
     363,   364,     0,   366,     0,   368,     0,     0,     0,     0,
       0,     0,     0,   947,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   413,     0,     0,
     416,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   456,     0,   458,     0,     0,     0,     0,     0,
       0,     0,     0,  1034,  1036,     0,     0,     0,     0,     0,
       0,     0,     0,  1121,   946,     0,     0,   505,     0,     0,
       0,     0,     0,     0,  1306,     0,  1306,     0,     0,     0,
     542,     0,  1408,     0,     0,     0,     0,     0,     0,     0,
     559,     0,     0,     0,  1050,  1052,     0,     0,     0,   568,
     569,     0,   571,   572,     0,     0,     0,     0,     0,     0,
       0,     0,  1084,   637,   579,     0,     0,     0,   850,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   946,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   974,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1122,  1123,  1124,  1125,  1126,  1127,
    1128,  1129,  1130,  1131,  1132,  1133,  1134,  1135,  1136,  1137,
    1138,  1139,  1306,  1140,  1141,  1142,  1143,  1144,  1145,  1146,
    1147,     0,  1148,  1149,  1150,  1151,  1152,  1153,  1154,  1155,
    1156,  1157,  1158,  1159,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,     0,     0,     0,   577,     0,     0,     0,   849,
       0,     0,     0,     0,  1219,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1277,  1279,     0,   653,     0,     0,     0,     0,
       0,   656,     0,   657,     0,     0,     0,     0,  1286,  1288,
       0,     0,   661,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1304,     0,  1309,     0,     0,     0,
       0,     0,     0,  1312,     0,   703,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   711,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   748,     0,
     749,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   754,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   767,   769,     0,
       0,   773,     0,     0,     0,     0,     0,   778,     0,     0,
     780,   781,     0,     0,   804,   805,   806,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1376,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1379,  1381,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1384,  1386,
       0,     0,     0,     0,     0,   840,   841,   842,   843,   844,
     845,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1395,  1397,   853,     0,     0,     0,     0,     0,
       0,  1400,  1402,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1410,  1122,  1123,  1124,  1125,  1126,  1127,
    1128,  1129,  1130,  1131,  1132,  1133,  1134,  1135,  1136,  1137,
    1138,  1139,     0,  1140,  1141,  1142,  1143,  1144,  1145,  1146,
    1147,     0,  1148,  1149,  1150,  1151,  1152,  1153,  1154,  1155,
    1156,  1157,  1158,  1159,     0,     0,     0,     0,     0,     0,
     913,   914,     0,     0,     0,   918,     0,     0,   921,     0,
       0,   922,   923,   924,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   932,     0,     0,     0,
     935,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1437,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1444,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   979,     0,   980,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   987,   988,   989,     0,   991,     0,   993,   994,
       0,   995,     0,   996,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    55,    56,     0,
       0,     0,     0,  1006,     0,     0,     0,     0,     0,     0,
    1010,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    57,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    58,     0,
       0,     0,     2,     0,     0,  1045,     0,     0,     0,     0,
       0,    59,    60,    61,     0,    62,    63,    64,    65,    66,
       0,    67,    68,    69,    70,     0,    71,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1168,     0,     0,    72,    73,    74,    75,
       0,     0,     0,     0,  1173,     0,     0,     0,     0,    76,
      77,    78,    79,     0,    80,    81,     0,    82,    83,    84,
      85,    86,    87,    88,    89,    90,     0,     0,    91,     0,
    1193,     0,     0,     0,     0,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1207,   102,     0,     0,     0,     0,
       0,     0,     0,  1210,     0,     0,     0,     0,     0,     0,
       0,     0,   103,   104,     0,     0,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,     0,
       0,     0,     0,     0,   118,   119,   120,   121,   122,   123,
     124,   125,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1273,   126,   127,   128,   129,     0,   130,
       0,   131,     0,   132,   133,   134,   135,   136,   137,     0,
       0,     0,     0,     0,   138,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   139,   140,   141,     0,     0,     0,   142,   143,
     144,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   145,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   146,     0,   147,
     148,   149,   150,     0,     0,     0,     0,     0,   151,     0,
       0,   152,   153,   154,   155,   156,   157,     0,     0,   158,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   159,     0,     0,     0,    55,    56,     0,
       0,     0,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,     0,     0,   172,     0,     0,     0,
       0,     0,     0,     0,   173,    57,     0,     0,     0,     0,
       0,   174,     0,     0,   175,     0,     0,     0,     0,     0,
     176,     0,     0,     0,     0,     0,     0,     0,    58,     0,
       0,     0,   177,   178,     0,     0,     0,     0,     0,   179,
     180,    59,    60,    61,     0,    62,    63,    64,    65,    66,
       0,    67,    68,    69,    70,     0,    71,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    72,    73,    74,    75,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    76,
      77,    78,    79,     0,    80,    81,     0,    82,    83,    84,
      85,    86,    87,    88,    89,    90,     0,     0,    91,     0,
       0,     0,     0,     0,     0,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   102,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   103,   104,     0,     0,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,     0,
       0,     0,     0,     0,   118,   119,   120,   121,   122,   123,
     124,   125,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   126,   127,   128,   129,     0,   130,
       0,   131,     0,   132,   133,   134,   135,   136,   137,     0,
       0,     0,     0,     0,   138,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   139,   140,   141,     0,     0,     0,   142,   143,
     144,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   145,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   146,     0,   147,
     148,   149,   150,     0,     0,     0,     0,     0,   151,     0,
       0,   152,   153,   154,   155,   156,   157,     0,     0,   158,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   159,     0,     0,     0,     0,     0,     0,
       0,     0,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,     0,     0,   172,     0,     0,     0,
       0,     0,     0,     0,   173,     0,     0,     0,     0,     0,
       0,   174,     0,     0,   175,     0,     0,     0,     0,     0,
     176,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   177,   178,     0,     0,     0,     0,     0,   179,
     180
  };

  const short
  MCNPParser::yycheck_[] =
  {
      47,   181,     8,   299,   168,   420,   689,   172,   387,   452,
     887,   445,   539,   803,     9,  1027,    97,   911,   742,    51,
     940,   643,   812,   303,   742,   333,   742,   733,    55,   735,
     583,   681,    93,    79,    95,     0,     9,    11,   609,   959,
     491,     6,     6,    15,    59,     7,  1190,    62,    55,    56,
    1088,   112,   712,   973,    69,   452,    71,  1302,   119,  1304,
       6,    76,  1303,    16,   125,   373,     6,   706,  1309,   452,
     709,     8,     9,     6,   445,   446,    13,   687,     7,     8,
       9,  1119,   143,     7,   195,     8,     9,   102,    11,     8,
       9,   152,   153,   154,   155,   156,    20,     5,   113,     8,
       9,    80,   391,   118,    13,   120,   121,   122,   123,   124,
       8,     9,    62,   128,    64,    65,    14,   132,   133,     9,
     135,    54,   732,    73,  1044,   576,   415,    72,    78,    79,
     176,     8,     9,    83,    84,    99,    13,    87,    88,    89,
      80,    91,     9,    15,    97,    98,    96,    14,    98,    99,
     100,     8,     9,   103,    11,  1400,   106,   107,   108,   109,
     110,  1402,   577,     9,    72,   115,    74,   200,   539,    38,
      39,  1091,   122,   206,   224,    44,   126,   127,     8,     9,
     130,     8,     9,    13,   134,   452,   136,   137,   138,   139,
     140,   141,   142,  1070,  1071,   145,   158,    90,   148,    92,
      77,   151,     9,   140,   141,   142,   143,   144,   158,   159,
     160,   161,   162,   163,   164,   165,   452,   140,   141,   142,
     143,   144,  1366,    11,   174,   175,    20,    15,   178,   158,
     145,    19,    20,    16,     8,     9,     9,    11,    12,   690,
      13,  1031,   195,    11,   197,    75,   196,    77,   972,   973,
     224,   224,   224,   977,   972,   973,   972,   973,   941,  1297,
     225,  1181,   343,   946,     8,     9,   949,   268,   974,   975,
     272,  1165,   978,    11,    97,    98,   803,  1197,   649,    11,
     902,   903,  1202,   905,     9,   812,   936,   814,   722,   321,
     733,   862,   735,   404,   405,   729,   323,   303,   851,   742,
     679,   961,   962,   384,   385,   386,   387,   388,   389,   390,
     391,     7,     8,     9,    97,   252,   323,   324,    11,    11,
      11,     7,   329,     9,     8,     9,    11,    13,   636,    13,
       8,     9,     4,    11,    12,   499,   733,    11,   735,    99,
     355,    11,   638,     8,     9,   742,    11,     9,    16,   272,
     733,   722,   735,   177,     8,     9,   727,    11,   729,   742,
     731,   158,  1374,    11,   359,    11,   646,    11,   676,   820,
       7,   145,   195,    11,   197,   325,   326,    11,    11,   394,
     395,   396,    13,   398,     7,     8,     9,   838,    11,    12,
       9,     9,   453,   454,    75,   846,   140,   141,   142,   143,
     144,     9,    75,   418,  1087,     9,   359,   360,  1285,    97,
      98,   444,   445,   446,   447,   448,   477,    75,     9,    14,
     415,   416,   450,   418,   419,   420,   473,    52,    75,   902,
     367,   368,   803,   598,   849,   482,   224,    52,  1297,   322,
     455,   812,  1362,   814,  1089,  1121,   897,   912,   731,   676,
     497,   679,  1372,  1243,   389,   197,   140,   141,   142,   143,
     144,   353,   354,   355,   356,   357,   733,   145,   735,   633,
     634,  1093,     8,     9,   489,   742,  1206,    13,  1202,  1203,
     224,  1008,  1206,  1177,  1202,  1203,  1202,  1203,  1206,  1187,
    1206,   417,    56,    92,    93,   727,    95,   733,  1204,   735,
    1183,   448,  1029,   550,  1031,   475,   742,   195,   484,   197,
     501,   195,   527,   112,  1434,  1001,  1004,   802,     8,     9,
     119,  1441,   145,    13,    14,  1218,   125,   526,   543,   972,
     973,   974,   975,   967,   977,   978,  1245,   360,   553,   552,
     704,    77,    78,   541,   143,   916,   561,   562,  1256,   837,
    1470,  1471,  1272,   152,   153,   154,   155,   156,   937,   606,
     343,     8,     9,   598,    11,   179,   180,   181,   182,   183,
     184,   521,   878,   523,   578,   972,   973,   974,   975,   626,
     977,   978,   599,  1066,   629,    75,  1376,    77,  1082,   972,
     973,   974,   975,  1071,   977,   978,   967,    -1,   969,   422,
     423,   384,   385,   386,   387,   388,   389,   390,   391,    -1,
     774,    -1,     8,     9,    -1,    11,    12,    13,   360,    15,
      -1,    -1,     8,     9,   639,   672,    -1,    13,    14,   239,
     240,   241,   242,     8,     9,    -1,    11,  1008,    13,    -1,
     646,    -1,     8,     9,    -1,    11,    12,    13,  1372,    15,
       8,     9,    -1,    -1,  1372,    -1,  1372,    -1,  1029,    -1,
    1031,    -1,     8,     9,    -1,    11,    12,    13,    14,    15,
      -1,   359,   360,    -1,   721,    -1,   691,    -1,    -1,   694,
      -1,   423,    -1,   425,   426,   427,   428,   429,    -1,   750,
      -1,    -1,    -1,   140,   141,   142,   143,   144,   713,    -1,
      -1,    -1,    -1,   764,   751,   972,   973,   974,   975,   756,
     977,   978,    -1,    84,    -1,    -1,  1243,    -1,   779,  1246,
     404,   405,    -1,    -1,   771,    -1,    -1,   891,    -1,   893,
     894,   305,   306,   307,    -1,    -1,   972,   973,   974,   975,
      -1,   977,   978,    -1,   140,   141,   142,   143,   144,   145,
     324,   325,   326,    -1,   140,   141,   142,   143,   144,  1202,
    1203,  1204,    -1,  1206,    -1,   140,   141,   142,   143,   144,
     817,   370,    -1,    -1,   140,   141,   142,   143,   144,   145,
      -1,    -1,   140,   141,   142,   143,   144,   834,    -1,    -1,
     954,    -1,    -1,   808,   140,   141,   142,   143,   144,   145,
      -1,    -1,    -1,    -1,    -1,  1202,  1203,  1204,    -1,  1206,
      -1,    -1,   411,   412,    -1,   414,   863,    -1,    -1,  1202,
    1203,  1204,    -1,  1206,   195,   272,    -1,    -1,   875,     8,
       9,    -1,    -1,   880,   998,   206,    -1,    -1,    17,    -1,
      -1,   212,   213,    -1,    -1,    -1,   445,   446,    -1,  1376,
    1377,    -1,    -1,    -1,   453,   454,   252,     7,     8,     9,
     459,    11,    12,    -1,    -1,  1316,   252,    -1,    -1,     7,
       8,     9,  1243,    11,    12,  1246,    -1,   252,   477,     8,
       9,    -1,   929,    -1,    -1,    14,   252,   486,    -1,    -1,
      15,     8,     9,    -1,   493,    -1,  1423,    -1,    -1,    16,
      -1,    -1,    -1,    -1,    -1,   911,   252,     7,     8,     9,
      -1,    11,    12,    -1,   272,   514,    16,   964,    -1,    -1,
    1084,    -1,    -1,    -1,   971,    -1,    -1,    -1,    -1,  1372,
      -1,    -1,   947,    -1,    -1,  1202,  1203,  1204,    -1,  1206,
     539,    -1,     8,     9,   959,    11,    -1,    13,    -1,    -1,
      -1,    -1,  1013,    -1,    -1,   554,   555,   556,   557,   558,
    1411,   140,   141,   142,   143,   144,  1202,  1203,  1204,    -1,
    1206,   367,   368,    -1,    -1,  1372,    -1,     8,     9,    -1,
      11,   367,    13,  1163,    -1,    -1,    -1,  1034,    -1,  1372,
      -1,    -1,   367,    -1,    -1,   145,    -1,   596,   305,   306,
     307,   367,    -1,  1050,   311,  1376,  1377,   145,   607,    -1,
      -1,   140,   141,   142,   143,   144,  1063,   324,   325,   326,
      -1,   367,    -1,   140,   141,   142,   143,   144,   627,    -1,
      -1,    -1,  1079,    -1,   405,    -1,   407,   408,   409,   410,
     411,   412,    -1,    -1,   643,   145,    -1,   359,  1463,    -1,
     649,    -1,  1423,    -1,    -1,  1219,    -1,  1353,    -1,    -1,
    1107,  1122,    -1,   662,   663,    -1,  1127,  1114,    -1,    -1,
    1131,  1132,  1133,    -1,   140,   141,   142,   143,   144,  1140,
    1141,  1142,  1143,  1144,     8,     9,   224,  1148,  1149,  1150,
    1151,  1152,  1153,  1154,  1155,  1156,  1157,  1158,  1159,  1160,
    1161,    -1,  1117,   415,   416,  1372,   418,   419,   420,   140,
     141,   142,   143,   144,    -1,    -1,    -1,    -1,    -1,    -1,
    1416,    -1,    -1,   722,    -1,    -1,    -1,    -1,   727,    -1,
     729,    -1,   731,    -1,    -1,    -1,  1372,     8,     9,    -1,
      11,    12,    -1,   272,    15,   744,    -1,   746,     8,     9,
     607,   750,    -1,    -1,    -1,   272,   755,    -1,    -1,  1165,
      -1,    -1,    -1,    -1,  1114,   764,    -1,  1117,    17,    18,
     627,    -1,    -1,    -1,  1354,    -1,    -1,   776,   777,    -1,
     779,    -1,  1197,  1198,    -1,    -1,   252,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,    -1,
      -1,    -1,    -1,    -1,   803,    -1,  1253,    -1,    -1,    -1,
      -1,    -1,    -1,   812,    -1,   814,   140,   141,   142,   143,
     144,   252,  1269,     7,     8,     9,    -1,    11,    12,    -1,
    1277,    15,    16,    -1,    -1,    -1,   835,    -1,    -1,  1286,
      -1,    -1,    -1,    -1,    33,    -1,  1293,  1294,    92,    93,
      -1,    95,    -1,    -1,    -1,    -1,  1303,   856,    -1,    -1,
      -1,   860,  1309,    -1,    -1,  1312,   865,   866,   112,   140,
     141,   142,   143,   144,   145,   119,    -1,   876,    -1,    -1,
      -1,   125,    -1,   882,   883,    -1,    -1,   158,   887,    -1,
     150,   151,   152,   153,   154,   155,   156,   157,   147,   143,
    1315,   367,    -1,   902,   903,    -1,   905,   906,   152,   153,
     154,   155,   156,    -1,    -1,   164,   165,   916,   167,   168,
     169,   170,   171,   172,   173,   174,   925,   926,     8,     9,
      -1,    11,  1379,    13,    -1,    15,   367,  1384,    -1,    -1,
     939,    -1,     8,     9,    -1,    -1,    -1,  1408,  1395,    -1,
    1397,   950,    -1,   952,    -1,  1402,   140,   141,   142,   143,
     144,   145,    -1,  1410,    -1,  1412,    -1,    -1,   967,    -1,
     969,    -1,    -1,    -1,   158,    -1,    -1,    -1,   835,    -1,
      -1,    -1,    -1,    -1,   983,    -1,    -1,   236,   860,  1436,
    1437,    -1,    -1,   992,   866,    -1,    -1,  1444,    -1,   856,
     999,   272,  1001,  1002,    -1,  1004,  1005,    -1,   865,  1008,
      -1,   883,    -1,    -1,  1013,    -1,    -1,    -1,    -1,   876,
      -1,    -1,    -1,    -1,    -1,   882,    -1,    -1,   277,    -1,
    1029,    -1,  1031,    -1,    -1,    -1,    -1,  1036,  1037,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   186,  1046,    -1,    -1,
    1049,    -1,    -1,  1052,  1053,   195,    -1,    -1,    -1,    -1,
     140,   141,   142,   143,   144,  1064,    -1,    -1,    -1,  1068,
      -1,  1070,  1071,    -1,   140,   141,   142,   143,   144,    -1,
      -1,  1080,    -1,    -1,    17,    18,    -1,    -1,   272,    -1,
      -1,  1090,  1091,    -1,  1093,  1094,   285,   286,   287,   288,
     289,   290,   291,   292,   293,   294,   295,   296,   297,   298,
     299,   300,   301,   179,   180,   181,   182,   183,   184,    -1,
      -1,    -1,    -1,  1122,  1123,  1124,  1125,  1126,  1127,  1128,
    1129,  1130,  1131,  1132,  1133,  1134,  1135,  1136,    -1,    -1,
      -1,  1140,  1141,  1142,  1143,  1144,    -1,    -1,    -1,  1148,
    1149,  1150,  1151,  1152,  1153,  1154,  1155,  1156,  1157,  1158,
    1159,  1160,  1161,    -1,    -1,  1037,    -1,   411,   412,    -1,
     414,  1170,    -1,    -1,  1046,    -1,  1175,  1049,  1177,  1036,
      -1,  1053,     7,     8,     9,    -1,    -1,  1186,  1187,  1188,
      -1,  1190,    -1,    -1,    -1,  1052,  1068,    -1,    -1,    -1,
      -1,    -1,    90,    -1,    -1,   345,    -1,  1064,    -1,   453,
     454,    -1,    -1,    -1,    -1,   459,    -1,    -1,  1217,    -1,
      -1,    -1,    -1,  1080,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   164,   165,   477,   167,   168,   169,   170,   171,   172,
     173,   174,   486,    -1,  1243,    -1,    -1,  1246,    -1,   493,
      -1,    -1,    -1,    -1,    -1,  1254,    -1,    -1,    -1,  1258,
      -1,    -1,    -1,    -1,    -1,    -1,     8,     9,    -1,    -1,
     514,  1270,    -1,    -1,    -1,    -1,    -1,  1276,    -1,    -1,
    1279,  1280,    -1,    -1,    -1,    -1,  1285,    -1,    -1,  1288,
    1289,   431,   432,   433,   434,   435,   436,   437,   438,   439,
     440,   441,  1301,   236,     8,     9,    -1,  1306,    -1,    13,
     554,   555,   556,   557,   558,   140,   141,   142,   143,   144,
    1319,    -1,    -1,    -1,    -1,  1324,  1325,  1326,  1327,  1328,
    1329,  1330,    -1,    -1,    -1,  1334,  1335,  1336,  1337,  1338,
    1339,  1340,  1341,  1342,  1343,  1344,  1345,  1346,  1347,  1348,
    1349,  1350,  1351,  1352,     7,     8,     9,  1356,    11,    12,
      31,    -1,    15,    -1,    -1,    -1,  1365,  1366,  1367,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1376,  1377,    -1,
      -1,    -1,  1381,  1382,    -1,    -1,  1258,  1386,  1387,    -1,
      -1,  1390,    -1,    -1,  1393,    -1,    -1,  1254,   140,   141,
     142,   143,   144,   145,  1276,    -1,  1405,    -1,  1280,  1408,
      -1,    -1,    -1,  1270,    -1,    -1,    -1,  1289,  1417,    -1,
    1419,    -1,  1279,    -1,  1423,    -1,  1425,    -1,  1427,    -1,
      -1,  1288,    -1,  1432,    -1,  1434,   140,   141,   142,   143,
     144,  1440,  1441,     8,     9,    -1,    11,    12,  1447,  1448,
      15,    -1,    -1,  1452,  1453,    -1,  1455,  1456,    -1,  1458,
    1459,    -1,  1461,  1462,    -1,    -1,  1465,    -1,   356,  1468,
    1469,    -1,    -1,    -1,    -1,    -1,    -1,   365,    -1,   367,
      -1,    -1,    -1,    -1,    -1,    -1,   374,   140,   141,   142,
     143,   144,   145,     7,     8,     9,    -1,    11,    12,    -1,
     744,    15,   746,    -1,    -1,   158,   750,     8,     9,    -1,
    1382,   755,    13,   401,    15,  1387,     8,     9,  1390,    -1,
     764,  1393,    -1,    15,  1381,     8,     9,    -1,    11,  1386,
       8,     9,   171,    11,    12,   779,    -1,    15,    16,     8,
       9,    19,    11,    12,    -1,    -1,    15,    16,    -1,    -1,
      19,     8,     9,  1425,    -1,  1427,    13,    14,    15,    -1,
      -1,   449,     8,     9,    -1,    11,    12,    -1,   221,    15,
      16,    -1,   460,    19,    -1,   140,   141,   142,   143,   144,
     145,    -1,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   158,    -1,    -1,    -1,   485,    -1,   487,
     488,   200,   490,    -1,   492,    -1,    -1,   206,   496,     8,
       9,    -1,    11,    12,   502,   503,    15,    16,   506,   272,
      -1,    -1,    -1,    -1,    -1,    -1,   140,   141,   142,   143,
     144,   145,    -1,    -1,    -1,    17,    18,    -1,    -1,   140,
     141,   142,   143,   144,   158,    -1,    -1,    -1,   140,   141,
     142,   143,   144,   171,    -1,    -1,   221,   140,   141,   142,
     143,   144,   140,   141,   142,   143,   144,   145,    -1,    -1,
      -1,   140,   141,   142,   143,   144,   145,    -1,    -1,    -1,
     158,    -1,    -1,   140,   141,   142,   143,   144,    -1,   158,
      -1,    -1,    -1,    -1,   140,   141,   142,   143,   144,   145,
      17,    18,    -1,    -1,    -1,    -1,    -1,   272,    -1,   348,
      -1,    -1,   158,    -1,   353,   354,    -1,    -1,   357,    -1,
     359,    -1,   361,   362,   363,   364,    -1,   366,    -1,   368,
     329,   330,   331,   332,   333,   334,   335,   336,    -1,   983,
      -1,   140,   141,   142,   143,   144,   145,    -1,   992,   348,
      -1,    -1,    -1,    -1,    -1,    -1,   395,    -1,   272,   158,
      -1,  1005,    -1,    17,    -1,    -1,   654,    -1,    -1,  1013,
      -1,    -1,    -1,    -1,   413,    -1,    -1,   416,    -1,    -1,
     419,   420,   164,   165,    -1,   167,   168,   169,   170,   171,
     172,   173,   174,    -1,   272,    -1,    -1,    -1,    -1,    -1,
     688,    -1,    -1,   272,    -1,   693,    -1,   695,     8,     9,
     192,    11,    12,    13,    -1,    -1,    -1,   456,    -1,   458,
      -1,    -1,   710,    -1,    -1,    -1,   272,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    -1,    -1,   164,   165,    -1,
     167,   168,   169,   170,   171,   172,   173,   174,    -1,   747,
      -1,    -1,    -1,    -1,    -1,    -1,   505,     8,     9,    -1,
      11,    12,    13,   272,   762,     8,     9,   395,  1122,    -1,
      13,    14,    -1,  1127,    -1,    -1,    -1,  1131,  1132,  1133,
      -1,    17,    -1,    -1,    -1,    -1,  1140,  1141,  1142,  1143,
    1144,   419,   420,   542,  1148,  1149,  1150,  1151,  1152,  1153,
    1154,  1155,  1156,  1157,  1158,  1159,  1160,  1161,    -1,    -1,
     559,    -1,    -1,   811,    -1,    -1,    -1,    -1,   816,   568,
     569,    -1,   571,   572,   452,    -1,    -1,    -1,   577,   578,
     140,   141,   142,   143,   144,   145,    -1,    -1,    -1,    -1,
      -1,   839,    -1,    -1,    -1,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    -1,    -1,  1217,   370,   371,   372,   373,   374,   375,
     376,   377,   378,   379,   380,   381,   382,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   248,    -1,    -1,    -1,   637,   140,
     141,   142,   143,   144,   145,    -1,    -1,   140,   141,   142,
     143,   144,   266,    -1,   653,    -1,    -1,   656,   657,     7,
       8,     9,   661,    11,    12,    13,    -1,     8,     9,    -1,
      -1,   919,    13,    -1,    -1,    16,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     8,     9,    -1,    11,    12,    13,    -1,
     689,     8,     9,   692,    11,    -1,    -1,    -1,    -1,   577,
     578,    -1,    -1,   951,   703,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   711,   712,   713,  1319,   715,    -1,    -1,    -1,
    1324,    -1,    -1,    -1,  1328,  1329,  1330,    -1,    -1,    -1,
    1334,  1335,  1336,  1337,  1338,  1339,  1340,  1341,  1342,  1343,
    1344,  1345,  1346,  1347,  1348,  1349,  1350,  1351,  1352,   748,
     749,   200,    -1,    -1,    -1,   754,    -1,   206,    -1,    -1,
      -1,    -1,   248,    -1,   213,    -1,     8,     9,   767,    11,
     769,    -1,    -1,    -1,   773,  1023,     8,     9,    -1,   778,
     266,   780,   781,    -1,    16,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   140,   141,   142,   143,   144,   145,    -1,   140,
     141,   142,   143,   144,  1408,   804,   805,   806,    -1,    -1,
      -1,   689,    -1,    -1,   692,   140,   141,   142,   143,   144,
     145,    -1,    -1,   140,   141,   142,   143,   144,    -1,    -1,
    1434,    -1,    -1,    -1,   712,   713,    -1,   715,    -1,   288,
      -1,   840,   841,   842,   843,   844,   845,    -1,    -1,   848,
     849,    -1,    -1,    -1,   853,   733,    -1,   735,    -1,    -1,
      -1,    -1,    -1,    -1,   742,    -1,     8,     9,    -1,    11,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     339,   340,   341,   342,   343,   344,   345,   346,   140,   141,
     142,   143,   144,     8,     9,    -1,    11,    -1,   140,   141,
     142,   143,   144,    -1,   913,   914,    -1,    -1,    -1,   918,
      -1,    -1,   921,   922,   923,   924,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   932,    -1,    -1,   935,    -1,    -1,    -1,
      -1,   940,   941,   942,    -1,    -1,    -1,   946,    -1,   948,
     949,    -1,    -1,    -1,    -1,    -1,   387,    -1,    -1,    -1,
     959,    -1,   961,   962,    -1,    -1,    -1,    -1,    -1,    -1,
     848,   849,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     979,   980,     8,     9,    -1,    11,    -1,    -1,   987,   988,
     989,    -1,   991,    -1,   993,   994,   995,   996,   140,   141,
     142,   143,   144,     8,     9,    -1,    11,  1006,    -1,    -1,
      -1,  1010,    -1,    -1,   445,   446,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   140,   141,   142,   143,   144,
      -1,    -1,   473,    -1,    -1,  1044,  1045,    -1,    -1,    -1,
      -1,   482,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   940,   941,    -1,    -1,   497,    -1,   946,    -1,
     948,   949,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   959,    -1,   961,   962,    -1,    -1,    -1,  1087,    -1,
      -1,    -1,    -1,    -1,   972,   973,   974,   975,    -1,   977,
     978,    43,    44,    45,    46,    47,    48,    -1,   539,    -1,
      -1,    -1,    -1,    -1,   140,   141,   142,   143,   144,   550,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      -1,    -1,    84,    -1,    -1,   140,   141,   142,   143,   144,
      -1,    -1,    -1,    -1,    -1,   576,    -1,    -1,    -1,    -1,
     581,   312,   313,   314,   315,   316,   317,   318,   319,   320,
     321,   322,   323,    -1,    -1,   596,  1044,    -1,    -1,  1168,
      -1,    -1,    -1,    -1,  1173,   606,   607,    -1,    -1,    -1,
    1179,    -1,  1181,  1182,  1183,    -1,  1185,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1193,   626,   627,    -1,  1197,  1198,
      -1,    -1,   633,   634,    -1,    -1,    -1,    -1,  1207,  1087,
      -1,  1210,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1105,    -1,    -1,
      -1,    -1,    -1,   664,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   672,   673,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,    -1,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,    -1,    -1,    -1,
      -1,    -1,    -1,   704,  1273,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     721,    -1,    -1,  1292,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1179,    -1,  1181,    -1,  1183,    -1,  1185,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1197,
    1198,    -1,    -1,    -1,  1202,  1203,  1204,    -1,  1206,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    60,    61,    -1,    63,    64,    65,    66,
      67,    68,    -1,    70,    -1,    72,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1362,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,    -1,
      97,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   109,    -1,   111,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   834,   835,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     9,  1292,    -1,    -1,   134,    -1,    -1,
      -1,    -1,    -1,    -1,  1302,    -1,  1304,    -1,    -1,    -1,
     147,    -1,  1310,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     157,    -1,    -1,    -1,   875,   876,    -1,    -1,    -1,   166,
     167,    -1,   169,   170,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   893,   180,  1463,    -1,    -1,    -1,  1467,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1362,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1372,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,  1400,   119,   120,   121,   122,   123,   124,   125,
     126,    -1,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    -1,    -1,    -1,  1463,    -1,    -1,    -1,  1467,
      -1,    -1,    -1,    -1,  1025,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1063,  1064,    -1,   352,    -1,    -1,    -1,    -1,
      -1,   358,    -1,   360,    -1,    -1,    -1,    -1,  1079,  1080,
      -1,    -1,   369,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,  1105,    -1,  1107,    -1,    -1,    -1,
      -1,    -1,    -1,  1114,    -1,   402,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   418,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   455,    -1,
     457,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   476,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   494,   495,    -1,
      -1,   498,    -1,    -1,    -1,    -1,    -1,   504,    -1,    -1,
     507,   508,    -1,    -1,   511,   512,   513,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1243,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1253,  1254,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1269,  1270,
      -1,    -1,    -1,    -1,    -1,   562,   563,   564,   565,   566,
     567,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,  1293,  1294,   581,    -1,    -1,    -1,    -1,    -1,
      -1,  1302,  1303,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1314,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,    -1,   119,   120,   121,   122,   123,   124,   125,
     126,    -1,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,    -1,    -1,    -1,    -1,    -1,    -1,
     647,   648,    -1,    -1,    -1,   652,    -1,    -1,   655,    -1,
      -1,   658,   659,   660,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   673,    -1,    -1,    -1,
     677,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1412,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1436,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   743,    -1,   745,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   759,   760,   761,    -1,   763,    -1,   765,   766,
      -1,   768,    -1,   770,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    17,    18,    -1,
      -1,    -1,    -1,   800,    -1,    -1,    -1,    -1,    -1,    -1,
     807,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,    -1,
      -1,    -1,    72,    -1,    -1,   852,    -1,    -1,    -1,    -1,
      -1,    81,    82,    83,    -1,    85,    86,    87,    88,    89,
      -1,    91,    92,    93,    94,    -1,    96,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   920,    -1,    -1,   146,   147,   148,   149,
      -1,    -1,    -1,    -1,   931,    -1,    -1,    -1,    -1,   159,
     160,   161,   162,    -1,   164,   165,    -1,   167,   168,   169,
     170,   171,   172,   173,   174,   175,    -1,    -1,   178,    -1,
     957,    -1,    -1,    -1,    -1,   185,   186,   187,   188,   189,
     190,   191,   192,   193,   194,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   981,   205,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   990,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   222,   223,    -1,    -1,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,    -1,
      -1,    -1,    -1,    -1,   244,   245,   246,   247,   248,   249,
     250,   251,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,  1040,   264,   265,   266,   267,    -1,   269,
      -1,   271,    -1,   273,   274,   275,   276,   277,   278,    -1,
      -1,    -1,    -1,    -1,   284,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   302,   303,   304,    -1,    -1,    -1,   308,   309,
     310,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   328,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   347,    -1,   349,
     350,   351,   352,    -1,    -1,    -1,    -1,    -1,   358,    -1,
      -1,   361,   362,   363,   364,   365,   366,    -1,    -1,   369,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   383,    -1,    -1,    -1,    17,    18,    -1,
      -1,    -1,   392,   393,   394,   395,   396,   397,   398,   399,
     400,   401,   402,   403,    -1,    -1,   406,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   414,    45,    -1,    -1,    -1,    -1,
      -1,   421,    -1,    -1,   424,    -1,    -1,    -1,    -1,    -1,
     430,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,    -1,
      -1,    -1,   442,   443,    -1,    -1,    -1,    -1,    -1,   449,
     450,    81,    82,    83,    -1,    85,    86,    87,    88,    89,
      -1,    91,    92,    93,    94,    -1,    96,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   146,   147,   148,   149,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   159,
     160,   161,   162,    -1,   164,   165,    -1,   167,   168,   169,
     170,   171,   172,   173,   174,   175,    -1,    -1,   178,    -1,
      -1,    -1,    -1,    -1,    -1,   185,   186,   187,   188,   189,
     190,   191,   192,   193,   194,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   205,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   222,   223,    -1,    -1,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,    -1,
      -1,    -1,    -1,    -1,   244,   245,   246,   247,   248,   249,
     250,   251,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   264,   265,   266,   267,    -1,   269,
      -1,   271,    -1,   273,   274,   275,   276,   277,   278,    -1,
      -1,    -1,    -1,    -1,   284,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   302,   303,   304,    -1,    -1,    -1,   308,   309,
     310,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   328,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   347,    -1,   349,
     350,   351,   352,    -1,    -1,    -1,    -1,    -1,   358,    -1,
      -1,   361,   362,   363,   364,   365,   366,    -1,    -1,   369,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   383,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   392,   393,   394,   395,   396,   397,   398,   399,
     400,   401,   402,   403,    -1,    -1,   406,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   414,    -1,    -1,    -1,    -1,    -1,
      -1,   421,    -1,    -1,   424,    -1,    -1,    -1,    -1,    -1,
     430,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   442,   443,    -1,    -1,    -1,    -1,    -1,   449,
     450
  };

  const short
  MCNPParser::yystos_[] =
  {
       0,     5,    72,    74,   462,   469,   470,   472,   752,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,   463,   464,   465,
     466,   467,   468,     6,   471,    17,    18,    45,    68,    81,
      82,    83,    85,    86,    87,    88,    89,    91,    92,    93,
      94,    96,   146,   147,   148,   149,   159,   160,   161,   162,
     164,   165,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   178,   185,   186,   187,   188,   189,   190,   191,   192,
     193,   194,   205,   222,   223,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   244,   245,
     246,   247,   248,   249,   250,   251,   264,   265,   266,   267,
     269,   271,   273,   274,   275,   276,   277,   278,   284,   302,
     303,   304,   308,   309,   310,   328,   347,   349,   350,   351,
     352,   358,   361,   362,   363,   364,   365,   366,   369,   383,
     392,   393,   394,   395,   396,   397,   398,   399,   400,   401,
     402,   403,   406,   414,   421,   424,   430,   442,   443,   449,
     450,   472,   527,   528,   529,   530,   531,   532,   534,   540,
     541,   542,   548,   549,   550,   551,   557,   563,   564,   568,
     569,   570,   571,   572,   573,   574,   575,   576,   577,   578,
     579,   586,   587,   588,   589,   590,   591,   592,   593,   594,
     595,   596,   597,   598,   599,   603,   604,   605,   609,   610,
     611,   612,   613,   614,   615,   616,   617,   618,   619,   620,
     621,   624,   627,   630,   631,   632,   633,   637,   638,   639,
     640,   641,   642,   653,   657,   661,   662,   663,   664,   665,
     669,   674,   675,   676,   677,   678,   679,   680,   681,   682,
     683,   684,   689,   694,   695,   696,   697,   698,   699,   700,
     701,   702,   703,   704,   705,   710,   714,   719,   724,   729,
     730,   735,   739,   744,   746,   747,   748,   749,   750,   751,
      80,   473,   498,   499,     0,   225,   471,     9,   457,   457,
     457,   457,     8,     9,    11,    12,   145,   455,   458,   466,
     464,   467,   472,    17,    18,   147,   277,   563,   604,    17,
     563,   578,   224,   461,     8,     9,   140,   141,   142,   143,
     144,   456,   459,   506,   507,   508,   509,   510,   511,   516,
      15,   490,   145,   511,   511,   461,   490,   511,   461,   511,
     461,   511,   511,   511,   511,   490,   511,   490,   511,   461,
       9,   533,     9,   460,   490,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,   543,   546,   547,
      11,   461,   526,   461,   461,   461,   461,   461,   461,    20,
     491,   553,   461,   140,   141,   142,   143,   144,   272,   456,
     517,   521,   521,   511,   521,   461,   511,     9,   461,   461,
     461,    84,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   580,   581,   584,   585,   490,
     461,   145,   461,   461,   461,   461,   511,   461,   511,   521,
     490,    31,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   600,   601,   602,   461,    11,   239,   240,
     241,   242,   606,   607,   608,   490,   521,   490,   490,   461,
     490,   490,   490,   521,   461,   461,   490,   268,   461,   272,
     622,   623,   490,   490,   461,   511,   490,   461,   461,   461,
     461,   461,   461,   461,   521,   305,   306,   307,   324,   325,
     326,   643,   644,   649,   650,   651,   652,   461,   200,   206,
     329,   330,   331,   332,   333,   334,   335,   336,   348,   658,
     659,   660,   511,   461,    11,   353,   354,   355,   356,   357,
     666,   667,   668,   461,   521,   521,   521,   521,   521,   511,
     461,   461,   461,   461,   461,   461,   461,   461,   511,   511,
     459,   511,   511,    13,   195,   404,   405,   479,   512,   516,
     706,   707,   708,   709,    84,   195,   206,   212,   213,   407,
     408,   409,   410,   411,   412,   706,   711,   712,   713,     9,
     359,   415,   416,   418,   419,   420,   715,   716,   717,   718,
     461,   461,   186,   195,   345,   431,   432,   433,   434,   435,
     436,   437,   438,   439,   440,   441,   731,   732,   733,   734,
      11,   736,   461,     8,     9,   538,   745,   511,   751,   461,
     750,    79,   176,   474,   471,   498,   499,   461,   461,   538,
     516,   491,   552,   511,   490,   491,   511,   511,   491,   491,
      17,   511,   456,     9,   150,   151,   152,   153,   154,   155,
     156,   157,   535,   536,   537,   538,   539,   491,     7,   454,
     455,   545,   546,   526,    11,     4,   452,   453,   490,   479,
     490,   516,   558,   490,    11,   490,   491,   179,   180,   181,
     182,   183,   184,   511,   565,   566,   567,   517,   526,   567,
     490,   511,   479,   513,   516,   512,    11,    12,    15,   145,
     158,   221,   454,   517,   518,   522,   582,   454,   582,   583,
     584,   491,   453,    11,   458,   479,   506,   507,   508,   509,
     510,   514,   515,    11,   521,    11,   521,   490,   511,   511,
     491,   454,   458,   601,   511,   521,   454,   458,   607,   491,
     491,   491,   490,   491,   552,   491,    11,   511,    11,   511,
     491,   454,   458,   511,   459,   622,   491,   491,   511,   491,
     511,   511,    33,   285,   286,   287,   288,   289,   290,   291,
     292,   293,   294,   295,   296,   297,   298,   299,   300,   301,
     634,   635,   636,    11,   511,   511,   511,   461,   461,   311,
     651,   490,   454,   522,   524,   659,   490,   454,   458,   667,
     490,   370,   371,   372,   373,   374,   375,   376,   377,   378,
     379,   380,   381,   382,   685,   686,   687,   688,   490,   490,
     511,   511,   511,   511,   511,   511,   454,   552,   512,   479,
     516,   709,   454,   511,   708,    16,   454,   518,   519,   523,
     525,   712,   718,   454,   458,   454,   525,   717,    97,    98,
     195,   197,   360,   422,   423,   720,   721,   722,   723,    11,
     454,   458,   454,   525,   733,    11,    15,    19,    20,   224,
      11,   454,   459,   158,   454,   459,   538,   490,     9,    77,
      78,   456,   475,   477,   478,   479,   481,   482,    99,   502,
     504,   505,   471,   511,   511,   518,   520,    16,   511,   544,
     491,   511,   511,   511,   511,   456,   456,   158,   454,   454,
     458,   454,   511,   537,   538,   511,   545,   544,   452,   491,
     558,   552,   490,    11,   252,   367,   479,   516,   554,   556,
     491,   490,   491,    11,   454,   459,   566,   491,    11,   513,
      11,   479,   490,   516,   454,   458,   582,   583,   582,   520,
      11,   454,   515,   515,   479,   490,   514,   554,   556,   511,
     511,   491,    11,   521,   458,    11,   458,   511,   511,   511,
     491,   511,   521,   511,   511,   511,   511,   458,   459,   456,
     625,   626,   456,   628,   629,   521,   511,   635,   524,    11,
     511,   312,   313,   314,   315,   316,   317,   318,   319,   320,
     321,   322,   323,   490,   645,   646,   647,   648,   491,   524,
     522,   491,   458,   552,   454,   458,   454,   525,   687,   552,
     491,   552,    14,   480,   512,   511,   525,   523,   458,   525,
     454,   458,   454,   525,   722,   197,   360,   423,   425,   426,
     427,   428,   429,   725,   726,   727,   728,   458,   525,   519,
     737,   738,   200,   206,   444,   445,   446,   447,   448,   740,
     741,   742,   743,   459,   454,   459,   459,   552,   177,   482,
     481,   481,    75,   476,   481,    17,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,   248,   266,   483,   485,   487,   489,   492,   494,
     496,     9,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     119,   120,   121,   122,   123,   124,   125,   126,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     500,   501,   503,   471,   504,   505,   518,   491,   511,   456,
     456,   158,   458,   511,   455,   456,   559,   561,   480,   558,
     516,   558,   490,   479,   516,   558,   456,   491,   456,   560,
     562,    11,   459,   511,    11,   480,    11,   513,   513,   458,
     458,   480,   515,   515,   479,   514,   515,   511,    11,    11,
     511,   459,   456,   625,   456,   628,    11,   521,   491,   454,
     647,   200,   206,   213,   288,   329,   330,   331,   332,   333,
     334,   335,   336,   337,   338,   339,   340,   341,   342,   343,
     344,   345,   346,   654,   655,   656,   524,    97,    98,   195,
     197,   359,   360,   670,   671,   672,   673,   458,   525,    97,
     343,   384,   385,   386,   387,   388,   389,   390,   391,   690,
     691,   692,   693,   511,   480,   458,   525,   454,   458,   454,
     525,   727,    19,   519,    19,   737,   454,   458,   454,   525,
     742,   459,   558,    97,    98,   492,   493,   495,   497,   496,
     480,   481,    90,    92,   454,   460,   479,   486,     9,   454,
     458,   484,   454,   458,   461,   461,   490,   492,   503,   521,
     456,   456,   456,   456,   521,   456,   456,   456,   521,   521,
     521,   456,   456,   456,   521,   521,   521,   521,   521,   521,
     521,   521,   521,   521,   521,   521,   521,   521,   521,   521,
     521,   521,   521,   751,   471,   456,   456,    11,   559,    11,
     480,   516,   558,   368,   555,   456,   562,   456,    11,   560,
     480,   480,   515,   555,   648,   459,   454,   524,   655,   454,
     458,   454,   525,   672,   454,   458,   454,   525,   692,   458,
     525,    19,   458,   525,    11,   454,   458,   454,   458,   493,
     454,   486,   454,   484,   486,   456,    75,   484,   479,   458,
     454,   490,   552,   456,   456,   456,   751,   456,   480,   456,
      11,   456,   480,   524,   458,   525,   458,   525,   458,   458,
     486,   484,   456,     9,   521,   458,   552,   454,   458,   456,
     456,   456,     9,   480,   454,   458,   458,   456,   456,   480,
      75,   458,   456,   456,     9,   456,   456,     9,   456,   456,
      75,   456,   456,     9,   456,   456,   488,   512,   456,   456,
     456,     9,   480,   480
  };

  const short
  MCNPParser::yyr1_[] =
  {
       0,   451,   452,   453,   453,   454,   455,   455,   455,   455,
     455,   456,   456,   457,   458,   459,   460,   461,   462,   463,
     463,   463,   463,   463,   463,   463,   463,   463,   463,   463,
     463,   463,   463,   463,   463,   463,   463,   463,   463,   463,
     463,   464,   465,   465,   465,   465,   465,   465,   466,   466,
     466,   466,   466,   466,   466,   466,   466,   466,   466,   466,
     466,   467,   467,   468,   468,   469,   469,   469,   469,   470,
     471,   472,   473,   474,   475,   476,   477,   478,   479,   480,
     481,   481,   481,   481,   481,   482,   483,   483,   483,   483,
     483,   483,   483,   483,   483,   484,   484,   484,   485,   485,
     485,   485,   486,   486,   486,   486,   487,   487,   487,   487,
     488,   489,   489,   489,   489,   489,   489,   489,   490,   491,
     492,   492,   492,   492,   492,   492,   492,   492,   492,   493,
     493,   493,   493,   493,   494,   494,   495,   495,   496,   497,
     498,   498,   498,   498,   498,   499,   499,   500,   500,   500,
     501,   501,   501,   502,   503,   503,   503,   503,   503,   503,
     503,   503,   503,   503,   503,   503,   503,   503,   503,   503,
     503,   503,   503,   503,   503,   503,   503,   503,   503,   503,
     503,   503,   503,   503,   503,   503,   503,   503,   504,   504,
     505,   505,   506,   507,   508,   509,   510,   511,   511,   512,
     512,   512,   512,   513,   513,   513,   513,   513,   514,   514,
     514,   514,   514,   514,   515,   515,   515,   515,   515,   515,
     515,   515,   515,   516,   516,   516,   516,   516,   516,   517,
     517,   517,   517,   517,   517,   517,   518,   518,   518,   518,
     518,   518,   519,   519,   520,   520,   521,   521,   522,   523,
     524,   524,   525,   525,   526,   526,   527,   527,   527,   528,
     529,   529,   529,   530,   531,   532,   533,   533,   534,   535,
     535,   535,   535,   535,   536,   536,   537,   537,   537,   537,
     537,   537,   538,   538,   538,   538,   538,   538,   539,   539,
     539,   539,   540,   540,   541,   541,   542,   543,   543,   543,
     543,   543,   543,   543,   543,   543,   543,   543,   543,   544,
     545,   545,   546,   546,   547,   547,   548,   549,   549,   550,
     551,   552,   552,   553,   553,   554,   555,   556,   557,   557,
     557,   557,   557,   558,   558,   558,   558,   558,   558,   558,
     558,   558,   559,   560,   561,   561,   562,   562,   563,   563,
     563,   563,   563,   563,   563,   563,   563,   563,   563,   563,
     563,   563,   563,   564,   565,   565,   565,   565,   565,   565,
     566,   566,   567,   567,   568,   568,   568,   568,   569,   570,
     571,   571,   572,   572,   573,   574,   574,   575,   576,   577,
     577,   577,   577,   577,   578,   578,   578,   578,   579,   580,
     580,   580,   580,   580,   580,   580,   580,   580,   580,   580,
     580,   580,   580,   580,   580,   581,   581,   581,   581,   581,
     581,   581,   581,   582,   582,   582,   583,   583,   584,   584,
     584,   584,   585,   585,   586,   586,   587,   587,   588,   588,
     588,   589,   589,   590,   590,   591,   591,   592,   592,   593,
     593,   594,   594,   594,   594,   594,   595,   596,   597,   598,
     599,   600,   600,   600,   600,   600,   600,   600,   600,   600,
     600,   600,   600,   601,   601,   602,   602,   603,   604,   604,
     604,   604,   605,   606,   606,   606,   606,   607,   607,   608,
     608,   609,   609,   610,   611,   612,   613,   614,   614,   615,
     616,   617,   618,   619,   619,   620,   621,   622,   623,   623,
     624,   625,   626,   626,   627,   628,   629,   629,   630,   631,
     631,   632,   633,   633,   634,   634,   634,   634,   634,   634,
     634,   634,   634,   634,   634,   634,   634,   634,   634,   634,
     634,   634,   635,   635,   636,   636,   637,   638,   639,   640,
     640,   640,   640,   641,   642,   643,   643,   643,   644,   645,
     645,   645,   645,   645,   645,   645,   645,   645,   646,   646,
     647,   647,   647,   648,   648,   649,   649,   649,   650,   650,
     650,   651,   651,   652,   652,   653,   654,   654,   654,   654,
     654,   654,   654,   654,   654,   654,   654,   654,   654,   654,
     654,   654,   654,   654,   654,   654,   654,   654,   655,   655,
     656,   656,   657,   658,   658,   658,   658,   658,   658,   658,
     658,   658,   658,   658,   659,   659,   660,   660,   661,   662,
     663,   664,   665,   666,   666,   666,   666,   666,   667,   667,
     668,   668,   669,   670,   670,   670,   671,   671,   671,   672,
     672,   672,   672,   673,   673,   674,   675,   676,   677,   678,
     679,   680,   681,   681,   682,   683,   683,   684,   685,   685,
     685,   685,   685,   685,   685,   685,   685,   685,   685,   686,
     686,   687,   687,   687,   687,   688,   688,   689,   690,   690,
     690,   690,   690,   690,   690,   690,   690,   691,   692,   692,
     692,   692,   693,   693,   694,   695,   695,   696,   697,   698,
     699,   700,   701,   701,   702,   702,   703,   704,   704,   705,
     705,   706,   706,   707,   707,   708,   708,   708,   709,   709,
     710,   710,   710,   710,   711,   711,   711,   711,   711,   711,
     711,   711,   711,   711,   711,   712,   712,   712,   713,   713,
     714,   714,   715,   715,   715,   715,   716,   716,   717,   717,
     717,   717,   718,   718,   719,   719,   720,   721,   721,   721,
     721,   721,   721,   722,   722,   722,   722,   723,   723,   724,
     725,   725,   726,   726,   726,   726,   726,   726,   727,   727,
     727,   727,   728,   728,   729,   730,   731,   731,   731,   731,
     731,   731,   732,   732,   732,   732,   732,   732,   732,   732,
     733,   733,   733,   733,   734,   734,   735,   736,   736,   736,
     736,   736,   737,   737,   738,   738,   738,   739,   740,   740,
     740,   741,   741,   741,   741,   742,   742,   742,   742,   743,
     743,   744,   745,   745,   746,   747,   748,   749,   749,   750,
     750,   750,   750,   750,   750,   750,   750,   750,   750,   750,
     750,   750,   750,   750,   750,   750,   750,   750,   750,   750,
     750,   750,   750,   750,   750,   750,   750,   750,   750,   750,
     750,   750,   750,   750,   750,   750,   750,   750,   750,   750,
     750,   750,   750,   750,   750,   750,   750,   750,   750,   750,
     750,   750,   750,   750,   750,   750,   750,   750,   750,   750,
     750,   750,   750,   750,   750,   750,   750,   750,   750,   750,
     750,   750,   750,   750,   750,   750,   750,   750,   750,   750,
     750,   750,   750,   750,   750,   750,   750,   750,   750,   750,
     750,   750,   750,   750,   750,   750,   750,   750,   750,   750,
     750,   750,   750,   750,   750,   750,   750,   750,   750,   750,
     750,   750,   750,   750,   751,   751,   752,   752,   752,   752,
     752,   752,   752,   752,   752,   752,   752,   752,   752,   752
  };

  const signed char
  MCNPParser::yyr2_[] =
  {
       0,     2,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     1,     2,     1,     2,     2,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     2,     2,     2,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     3,     3,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     4,    10,     3,     2,
       4,     3,     1,     5,    14,    15,     3,     4,     2,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     2,     4,     5,     6,     4,     5,     1,     1,     1,
       3,     2,     3,     2,     1,     2,     1,     2,     1,     1,
       4,     5,     3,     4,     5,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     2,     2,     2,     2,
       3,     3,     3,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     3,
       1,     2,     1,     1,     1,     1,     1,     1,     2,     1,
       2,     3,     4,     1,     2,     3,     4,     3,     1,     1,
       1,     1,     1,     1,     1,     2,     3,     4,     3,     3,
       5,     4,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     2,     1,     1,
       1,     2,     1,     2,     1,     2,     2,     3,     2,     2,
       3,     4,     4,     2,     2,     2,     4,     5,     2,     1,
       1,     1,     1,     1,     1,     1,     3,     2,     3,     2,
       3,     2,     2,     3,     3,     4,     2,     3,     1,     2,
       1,     2,     3,     2,     4,     3,     4,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     2,     1,     2,     2,     3,     2,     3,
       3,     1,     3,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     2,     4,     4,     3,
       5,     4,     4,     3,     1,     2,     1,     2,     5,     6,
       4,     3,     5,     6,     5,     6,     6,     7,     6,     5,
      14,     2,     2,     2,     1,     1,     1,     1,     1,     1,
       3,     2,     1,     2,     3,     5,     4,     3,     2,     2,
       2,     1,     2,     1,     2,     3,     2,     4,     2,     3,
       5,     4,     5,     3,     3,     5,     4,     2,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     2,     1,     2,     3,     3,
       2,     2,     1,     2,     1,     2,     4,     3,     3,     4,
       3,     3,     4,     1,     2,     3,     4,     3,     4,     3,
       2,     5,     4,     3,     5,     2,     3,     2,     2,     4,
       4,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     3,     1,     2,     2,     3,     5,
       4,     2,     3,     1,     1,     1,     1,     2,     3,     1,
       2,     2,     1,     4,     2,     4,     4,     5,     4,     4,
       4,     2,     4,     4,     3,     3,     4,     4,     1,     2,
       2,     2,     1,     2,     4,     2,     1,     2,     4,     2,
       3,     4,     3,     4,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     1,     2,     3,     3,     4,     2,
       3,     5,     4,     3,     2,     1,     1,     1,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     2,     1,     2,     1,     1,     1,     5,     2,
       3,     1,     1,     1,     2,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     2,
       1,     2,     5,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     2,     1,     2,     2,     2,
       3,     5,     2,     1,     1,     1,     1,     1,     3,     2,
       1,     2,     2,     1,     1,     1,     1,     1,     1,     3,
       2,     3,     2,     1,     2,     5,     2,     2,     2,     2,
       2,     2,     5,     4,     4,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     2,     3,     2,     1,     2,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     2,
       3,     2,     1,     2,     5,     3,     5,     3,     3,     3,
       3,     3,     1,     2,     1,     2,     2,     1,     2,     1,
       2,     3,     2,     1,     1,     3,     2,     1,     1,     2,
       1,     2,     3,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     2,     1,     1,     2,
       1,     2,     1,     1,     1,     1,     1,     1,     3,     2,
       3,     2,     1,     2,     2,     3,     1,     1,     1,     1,
       1,     1,     1,     3,     2,     3,     2,     1,     2,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     2,
       3,     2,     1,     2,     4,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     2,     3,     2,     1,     2,     2,     1,     2,     2,
       2,     2,     1,     2,     2,     3,     2,     4,     1,     1,
       1,     1,     1,     1,     1,     3,     2,     3,     2,     1,
       2,     4,     1,     2,     2,     2,     2,     3,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     0,     8,     1,     3,
       4,     6,     6,     4,     2,     1,     3,     2,     2,     2
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const MCNPParser::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "\"end of line\"",
  "\"comment\"", "\"message card\"", "\"blank line delimiter\"", "\"=\"",
  "\"double precision number\"", "\"integer number\"", "\"quoted string\"",
  "\"string\"", "\"value\"", "\"(\"", "\")\"", "\":\"", "\",\"", "\"*\"",
  "\"+\"", "\"newline\"", "\"particle symbol\"", "\"datapath\"", "\"inp\"",
  "\"outp\"", "\"runtpe\"", "\"wwinp\"", "\"wwout\"", "\"wwone\"",
  "\"partinp\"", "\"linkin\"", "\"linkout\"",
  "\"format of senstitivity profile output file (KSENTAL)\"", "\"histp\"",
  "\"com\"", "\"comout\"", "\"plotm\"", "\"mctal\"", "\"meshtal\"",
  "\"mdata\"", "\"name\"", "\"srctp\"", "\"wssa\"", "\"rssa\"",
  "\"input\"", "\"plot_geom\"", "\"crosssection\"", "\"transport\"",
  "\"plot_data\"", "\"partisn_input\"", "\"ziaaa.abx\"", "\"fission\"",
  "\"nonfiss\"", "\"delayed neutron (DN)\"", "\"delayed gamma (DG)\"",
  "\"discrete delayed-gamma line threshold (THRESH)\"",
  "\"delayed neutron per interaction (DNBIAS)\"", "\"nap\"",
  "\"delayed neutron energy bias (DNEB)\"",
  "\"delayed gamma energy bias (DPEB)\"",
  "\"delayed gamma energy cutoff (PECUT)\"",
  "\"spontaneos half-life cutoff (HLCUT)\"",
  "\"correlated or uncorrelated flag (SAMPLE)\"", "\"c\"", "\"cn\"",
  "\"dbug\"", "\"devtest\"", "\"notek\"", "\"fatal\"", "\"print\"",
  "\"tasks\"", "\"balance\"", "\"eol\"", "\"title\"", "WORD",
  "\"continue\"", "\"union\"", "\"intersection\"", "\"#\"", "\"density\"",
  "\"material_id\"", "\"cell_id\"", "\"imp\"", "\"vol\"", "\"pwt\"",
  "\"ext\"", "\"dxc\"", "\"nonu\"", "\"pd\"", "\"tmp\"", "\"universe\"",
  "\"cell transformation (TRCL)\"", "\"lat\"", "\"fill\"", "\"elpt\"",
  "\"cosy\"", "\"bfcl\"", "\"unc\"", "\"mat\"", "\"rho\"",
  "\"surface_id\"", "\"p\"", "\"px\"", "\"py\"", "\"pz\"", "\"so\"",
  "\"sph\"", "\"sx\"", "\"sy\"", "\"sz\"", "\"c/x\"", "\"c/y\"", "\"c/z\"",
  "\"cx\"", "\"cy\"", "\"cz\"", "\"k/x\"", "\"k/y\"", "\"k/z\"",
  "\"x, y, or z cone (K/X K/Y K/Z)\"", "\"kx\"", "\"ky\"", "\"kz\"",
  "\"sq\"", "\"gq\"", "\"tx\"", "\"ty\"", "\"tz\"",
  "\"x, y, or z torus (TX TY TZ)\"", "\"x\"", "\"y\"", "\"z\"", "\"box\"",
  "\"hex\"", "\"wed\"", "\"rpp\"", "\"rcc\"", "\"rec\"", "\"trc\"",
  "\"ell\"", "\"arb\"", "\"array jump (nJ)\"", "\"array repeat (nR)\"",
  "\"array multiply (xM)\"", "\"array logarithmic interpolation (nLOG)\"",
  "\"array linear interpoloation (nI)\"", "\"no\"", "\"area\"",
  "\"transformation (TRn)\"", "\"random universe (URAN)\"",
  "\"material id (Mn)\"", "\"(N|P|PN|E|H|A|S|T|D)LIB\"", "GAS", "ESTEP",
  "HSTEP", "COND", "REFI", "REFC", "REFS",
  "\"library id (.?[0-9]{2}[tcdmgpuyehporsa])\"",
  "\"time energy and weight cutoff card (CUT)\"",
  "\"activation options (ACT)\"", "\"print hierarchy (FQn)\"",
  "\"tally comment card (FCn)\"", "\"base tally card (Fn)\"",
  "\"integrated current surface tally card (F1)\"",
  "\"flux averaged surface tally card (F2)\"",
  "\"unsupported tally needed for internal indexing\"",
  "\"flux averaged cell tally card (F4)\"",
  "\"flux point or ring tally card (F5)\"", "\"F6 cell tally card (F6)\"",
  "\"fission energy deposition cell tally card (F7)\"",
  "\"F8 tally card (F8)\"", "\"image tally card (FI[CPR]n)\"",
  "\"ring detector tally card (F5[XYZ])\"",
  "\"ring detector tally card (F[XYZ]5)\"", "\"problem type (MODE)\"",
  "\"like\"", "\"but\"", "\"tally time (Tn)\"",
  "\"ref starting time (CBEG)\"", "\"frequency of cycling (CFRQ)\"",
  "\"dead time interval (COFI)\"", "\"alive time interval (CONI)\"",
  "\"number of subdivisions ot use within alive time (CSUB)\"",
  "\"ref ending time (CEND)\"", "\"criticality source card (KCODE)\"",
  "\"history cutoff (NPS)\"", "\"negate printing of tallies (TALNP)\"",
  "\"print and dump cycle (PRDMP)\"",
  "\"s(a,b) thermal neutron scattering (MTm)\"",
  "\"criticality source points (KSRC)\"", "\"tally energy card (En)\"",
  "\"tally multiplier card (FMn)\"", "\"segment divisor card (SDn)\"",
  "\"general source (SDEF)\"", "CEL", "\"surface number (SUR)\"", "ERG",
  "\"time (TME)\"",
  "\"cosine angle between VEC and particle direction (DIR)\"", "VEC",
  "\"sign of surface normal (NRM)\"", "\"source particle type (PAR)\"",
  "\"reference point for position sampling in vector notation (POS)\"",
  "\"radial distance from the position from POS or AXS (RAD)\"",
  "EXT_CARD", "AXS", "\"x-coordinate of position (X)\"",
  "\"y-coordinate of position (Y)\"", "\"z-coordinate of position (Z)\"",
  "\"cookie-cutter cell number (CCC)\"", "\"Area of surface (ARA)\"",
  "WGT", "TR",
  "\"rejection efficiency critera for position sampling (EFF)\"",
  "\"date to use for comic-ray and background sources (DAT)\"",
  "\"location of cosmic particle source (LOC)\"",
  "\"Beam emittence parameters (BEM)\"",
  "\"Beam aperature parameters (BAP)\"",
  "\"CEL | SUR | ERG | TME | DIR | NRM | RAD | EXT | CCC | ARA | WGT | TR | EFF\"",
  "\"DAT | LOC | BEM | BAP | POS | PAR | VEC | AXS\"", "SDEF_PREFIXED_VAR",
  "\"source comment card (SCn)\"", "\"total fission card (TOTNU)\"",
  "\"integer identifier\"", "\"anything block\"",
  "\"direct-only neutron-particle point detector card (NOTRN)\"",
  "\"source information for distribution for n (SIn)\"",
  "\"source probability for distribution for n (SPn)\"",
  "\"source bias for distribution for n (SBn)\"",
  "\"tally segment card (FSn)\"", "\"tally fluctation card (TFn)\"",
  "\"lost particle card (LOST)\"", "\"shannon entropy card (HSRC)\"",
  "\"particle physics options card (PHYS)\"",
  "\"criticality calculation options card (KOPTS)\"",
  "\"tally cosine card (Cn)\"",
  "\"multigroup adjoint transport option card (MGOPT)\"",
  "\"random number generation card (RAND)\"",
  "\"random number generator type (GEN)\"",
  "\"random number generator seed (SEED)\"",
  "\"number of random numbers between source particles (STRIDE)\"", "HIST",
  "\"GEN or SEED or STRIDE or HIST\"",
  "\"weight-window energy or time intervals (WWE)\"",
  "\"weight-window generation card (WWG)\"",
  "\"weight-window generation energy or time bounds (WWGE)\"",
  "\"weight-window generation time bounds (WWGT)\"",
  "\"cell-based lower weight-window bounds (WWNi)\"",
  "\"weight-window parameter card (WWP)\"",
  "\"weight-window time intervals (WWT)\"",
  "\"debug information card (DBCN)\"", "\"<\"",
  "\"number of cycles in every outer iteration (BLOCKSIZE=n)\"",
  "\"calculate point-kinetic parameters (KINETICS=YES/NO)\"",
  "\"calculate detailed precursor information (PRECURSOR=YES/NO)\"",
  "\"turn on FMAT (FMAT=YES/NO)\"", "\"FMATSKIP=n\"", "\"FMATSPACE=s\"",
  "\"FMATACCEL=YES/NO\"", "\"FMATREDUCE=YES/NO\"", "FMATNX", "FMATNY",
  "FMATNZ", "\"dose energy card (DEn)\"", "\"dose function card (DFn)\"",
  "\"forced-collision card (FCL)\"",
  "\"variance reduction control card (VAR)\"",
  "\"russion roulette key (RR)\"", "\"cell-flagging card (CFn)\"",
  "\"exponential transform card (EFT)\"", "\"vector input card (VECT)\"",
  "\"Vn\"", "\"energy splitting and roulette card (ESPLT)\"",
  "\"time splitting and roulette card (TSPLT)\"",
  "\"detector diagnostics card (DD)\"", "\"dxtran card (DXT)\"",
  "\"cosine multiplier card (CMn)\"", "\"time multiplier card (TMn)\"",
  "\"string or number\"", "\"double or integer\"",
  "\"FISSION | NONFISS | DN | DG | THRESH | DNBIAS | NAP | DNEB | DPEB | PECUT | HLCUT | SAMPLE\"",
  "\"F1 | F2 | F4 | F7\"", "\"CBEG | CFRQ | COFI | CONI | CSUB | CEND\"",
  "\"special treatment tallies card (FT)\"", "FRV", "GEB", "TMC", "INC",
  "ICD", "SCX", "SCD", "ELC", "PTT", "PHL", "CAP", "RES", "TAG", "LET",
  "ROC", "PDS", "FFT", "\"dependent source distribution card (DS)\"",
  "\"tallyx card (FU)\"", "\"energy multiplier card (EM)\"", "CORA",
  "CORB", "CORC", "\"surface-flagging card (SF)\"",
  "\"bremsstrahlung biasing card (BBREM)\"",
  "\"superimposed mesh tally card (TMESH)\"",
  "\"mesh tally terminator (ENDMD)\"", "TRAKS", "FLUX", "DOSE", "POPUL",
  "PEDEP", "MFACT", "TRANS", "TOTAL", "DEDX", "RECOL", "TLEST", "EDLCT",
  "\"cylindrical mesh (CMESH)\"", "\"rectilinear mesh (RMESH)\"",
  "\"spherical mesh (SMESH)\"", "\"R/C/SMESH\"",
  "\"superimposed fmesh mesh tally card (FMESH)\"", "GEOM", "ORIGIN",
  "IMESH", "IINTS", "JMESH", "JINTS", "KMESH", "KINTS", "EMESH", "EINTS",
  "ENORM", "FMESH_TMESH", "TINTS", "TNORM", "FACTOR", "MCNP_OUT", "TYPE",
  "KCLEAR", "\"superimposed mesh variable card (MESH)\"", "REF",
  "\"thermal times card (THTME)\"",
  "\"material nuclide substitution card (MX)\"",
  "\"model physics control card (MPHYS)\"",
  "\"transport options card (TROPT)\"", "MCSCAT", "ELOSS", "NREACT",
  "NESCAT", "GENXS", "\"tally perturbation card (PERT)\"", "METHOD", "RXN",
  "LCA", "LCB", "LCC", "LEA", "LEB",
  "\"photon production bias card (PIKMT)\"", "\"[\"", "\"]\"",
  "EMBED_CARD", "BACKGROUND", "MATCELL", "MESHGEO", "MGEOIN", "MEEOUT",
  "MEEIN", "CALC_VOLS", "DEBUG", "FILETYPE", "GMVFILE", "LENGTH",
  "MCNPUMFILE", "OVERLAP", "EMBEE", "EMBED", "ENERGY", "TIME", "ATOM",
  "COMMENT_FLAG", "LIST", "MTYPE", "ERRORS", "EMBEB", "EMBEM", "EMBTB",
  "EMBTM", "EMBDE", "EMBDF", "MCNP_VOID", "DRXS", "CTME", "RDUM", "IDUM",
  "SSW", "SYM", "PTY", "SSR", "OLD", "NEW", "PSC", "POA", "COL", "BCW",
  "COMMENT_KEY", "\"multiplicity constant card (FMULT)\"", "SFNU", "WIDTH",
  "SYFIELD", "WATT", "DATA", "SHIFT",
  "\"reactivity perturbation card (KPERT)\"", "LINEAR", "ISO",
  "\"keff sensitivity coefficients card (KSEN)\"", "EIN", "LEGENDRE",
  "COS", "CONSTRAIN", "MT", "\"particle track output (PTRAC)\"", "BUFFER",
  "FILE", "MAX", "MEPH", "WRITE", "COINC", "EVENT", "FILTER", "SURFACE",
  "TALLY", "VALUE_KEY", "COLUMN_WISE_FLAG",
  "\"particle ray tracing (BFLD)\"", "FIELD", "MXDEFLC", "MAXSTEP",
  "FFEDGES", "REFPNT", "\"atomic weight card (AWTAB)\"",
  "\"transfer map card (COSPY))\"", "$accept", "comment", "comment_list",
  "assign", "PRIMITIVE", "NUMBER", "int_value", "value", "number", "id",
  "card_id", "message_card", "execution_filename_key", "filename_value",
  "other_option_optional_int", "execution_other_option",
  "execution_other_options", "execution_filenames", "message_block",
  "continue", "blank_line", "title_card", "cell_id", "material_id",
  "density", "geom_op_union", "geom_op_complement", "geom_id", "lparen",
  "rparen", "cell_geom_expression", "cell_geom", "cell_param_key",
  "cell_fill_data", "cell_fill", "cell_transform_data",
  "cell_transformation", "cell_fill_list", "cell_param_particle_based_key",
  "colon", "particle_symbol", "acell_param", "like_but_cell_param",
  "cell_params", "like_but_cell_params", "cell_param",
  "like_but_cell_param_object", "cell_card", "cell_block", "PARCONE",
  "TORUS", "surface_id", "surface_geom", "surface_card", "surface_block",
  "array_jump", "array_multiply", "array_repeat", "array_log_interp",
  "array_lin_interp", "number_list", "number_paren_list",
  "number_paren_colon_list", "value_or_sc", "value_paren_colon_list",
  "number_or_sc", "NUMBER_LIST_TOKEN", "VALUE_LIST_TOKEN",
  "VALUE_COMMA_LIST_TOKEN", "value_token_list", "number_token_list",
  "value_list_item", "value_comma_list_item", "value_list",
  "value_comma_list", "string_list", "data_vol_card", "data_area_card",
  "data_transform_card", "data_universe_card", "data_lat_card",
  "data_fill_card", "uran_members", "data_uran_card",
  "MATERIAL_SCALAR_KEY", "MATERIAL_LIST_KEY", "amaterial_option",
  "material_zaid_entry", "material_options", "data_material_card",
  "data_cutoff_card", "data_elpt_card", "ACTIVATION_OPTION_KEY", "comma",
  "comma_sep_list", "activation_option", "activation_options",
  "data_activation_card", "data_fq_card", "data_fc_card", "data_sc_card",
  "particle_symbol_list", "mode_particle_symbol_list", "lt", "rbracket",
  "lbracket", "SURFACE_OR_CELL_TALLY", "tally_surface_list",
  "f5_tally_member", "f5a_tally_member", "f5_tally_members",
  "f5a_tally_members", "data_ftally_card", "data_mode_card",
  "TALLY_TIME_OPTION_KEY", "tally_time_option", "tally_time_options",
  "data_tally_time_card", "data_kcode_card", "data_nps_card",
  "data_print_card", "data_talnp_card", "data_prdmp_card", "data_mt_card",
  "data_imp_card", "data_ksrc_card", "data_tally_energy_card",
  "data_tally_multiplier_card", "data_sd_card", "SDEF_SCALAR_VALUE_KEY",
  "SDEF_LIST_VALUE_KEY", "sdef_value", "sdef_value_list",
  "data_sdef_member", "data_sdef_members", "data_sdef_card",
  "data_si_card", "data_sp_card", "data_sb_card", "data_totnu_card",
  "data_de_card", "data_df_card", "data_tmp_card", "data_fs_card",
  "data_tf_card", "data_lost_card", "data_hsrc_card", "data_xs_card",
  "data_phys_card", "KOPTS_OPTION", "data_kopts_option",
  "data_kopts_options", "data_kopts_card", "data_tally_cosine_card",
  "data_mgopt_card", "RAND_OPTION", "data_rand_card_option",
  "data_rand_card_options", "data_rand_card", "data_wwe_card",
  "data_wwg_card", "data_wwge_card", "data_wwgt_card", "data_wwn_card",
  "data_wwp_card", "data_wwt_card", "data_dbcn_card", "data_fcl_card",
  "data_var_card", "data_cf_card", "data_ext_card", "data_vect_member",
  "data_vect_members", "data_vect_card", "data_esplt_pair",
  "data_esplt_pairs", "data_esplt_card", "data_tsplt_pair",
  "data_tsplt_pairs", "data_tsplt_card", "data_dd_card", "data_dxt_card",
  "data_cm_card", "FT_MEMBER_KEY", "data_ft_member", "data_ft_members",
  "data_ft_card", "data_sf_card", "data_ds_card", "data_fu_card",
  "data_em_card", "data_bbrem_card", "CORX", "tmesh_corx_card",
  "XMESH_FLAG_KEYWORD", "XMESH_VALUE_KEYWORD", "tmesh_xmesh_member",
  "tmesh_xmesh_members", "XMESH", "tmesh_xmesh_card", "data_tmesh_member",
  "data_tmesh_members", "data_tmesh_card", "FMESH_KEY",
  "data_fmesh_member", "data_fmesh_members", "data_fmesh_card", "MESH_KEY",
  "data_mesh_member", "data_mesh_members", "data_mesh_card",
  "data_thtme_card", "data_tm_card", "data_mx_card", "data_mphys_card",
  "TROPT_KEY", "data_tropt_member", "data_tropt_members",
  "data_tropt_card", "PERT_SCALAR_KEY", "PERT_LIST_KEY",
  "data_pert_member", "data_pert_members", "data_pert_card",
  "data_lca_card", "data_lcb_card", "data_lcc_card", "data_lea_card",
  "data_leb_card", "data_pikmt_card", "data_dxc_card", "data_unc_card",
  "data_nonu_card", "data_notrn_card", "EMBED_SCALAR_KEY",
  "EMBED_LIST_KEY", "data_embed_member", "data_embed_members",
  "data_embed_card", "EMBEE_SCALAR_KEY", "EMBEE_LIST_KEY",
  "data_embee_member", "data_embee_members", "data_embee_card",
  "data_embeb_card", "data_embem_card", "data_embtb_card",
  "data_embtm_card", "data_embde_card", "data_embdf_card",
  "data_void_card", "data_drxs_card", "data_ctme_card", "data_rdum_card",
  "data_idum_card", "pty", "SSW_KEY", "data_ssw_member",
  "data_ssw_members", "data_ssw_card", "SSR_KEY", "data_ssr_member",
  "data_ssr_members", "data_ssr_card", "FMULT_SCALAR_KEY",
  "FMULT_LIST_KEY", "data_fmult_member", "data_fmult_members",
  "data_fmult_card", "KPERT_SCALAR_KEY", "KPERT_LIST_KEY",
  "data_kpert_member", "data_kpert_members", "data_kpert_card",
  "KSEN_SCALAR_KEY", "KSEN_LIST_KEY", "data_ksen_member",
  "data_ksen_members", "data_ksen_card", "data_pwt_card",
  "PTRAC_SCALAR_KEY", "PTRAC_LIST_KEY", "data_ptrac_member",
  "data_ptrac_members", "data_ptrac_card",
  "data_column_wise_header_members", "data_column_wise_row_members",
  "data_column_wise_table", "data_column_wise_block", "BFLD_SCALAR_KEY",
  "BFLD_LIST_KEY", "data_bfld_member", "data_bfld_members",
  "data_bfld_card", "material_zaid_pairs", "data_awtab_card",
  "data_cosy_card", "data_cosyp_card", "data_pd_card", "data_card",
  "data_block", "start", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
  MCNPParser::yyrline_[] =
  {
       0,   731,   731,   734,   739,   746,   749,   749,   749,   749,
     749,   750,   750,   753,   756,   758,   760,   762,   764,   766,
     767,   768,   769,   770,   771,   772,   773,   774,   775,   776,
     777,   778,   779,   780,   781,   782,   783,   784,   785,   786,
     787,   790,   799,   800,   801,   802,   803,   804,   807,   808,
     809,   810,   811,   812,   813,   814,   815,   816,   817,   818,
     819,   822,   827,   834,   839,   846,   854,   862,   872,   879,
     884,   890,   895,   900,   905,   910,   915,   920,   926,   931,
     940,   945,   951,   957,   964,   972,   982,   983,   984,   985,
     986,   987,   988,   989,   990,   994,   999,  1020,  1043,  1053,
    1062,  1074,  1087,  1091,  1102,  1142,  1186,  1197,  1210,  1220,
    1233,  1237,  1238,  1240,  1242,  1243,  1244,  1245,  1249,  1252,
    1255,  1262,  1270,  1277,  1290,  1304,  1316,  1329,  1330,  1332,
    1333,  1340,  1346,  1352,  1360,  1365,  1372,  1377,  1383,  1391,
    1399,  1405,  1411,  1417,  1423,  1437,  1442,  1448,  1448,  1448,
    1450,  1450,  1450,  1452,  1456,  1479,  1487,  1495,  1503,  1511,
    1525,  1534,  1543,  1552,  1566,  1580,  1594,  1602,  1610,  1618,
    1638,  1649,  1660,  1671,  1691,  1705,  1719,  1733,  1747,  1761,
    1775,  1789,  1803,  1825,  1839,  1853,  1867,  1882,  1905,  1911,
    1929,  1934,  1941,  1943,  1946,  1951,  1956,  1962,  1967,  1975,
    1980,  1985,  1991,  2000,  2005,  2010,  2016,  2024,  2032,  2032,
    2032,  2032,  2032,  2032,  2034,  2039,  2044,  2050,  2058,  2065,
    2071,  2080,  2088,  2099,  2099,  2099,  2099,  2099,  2099,  2101,
    2101,  2101,  2101,  2102,  2102,  2102,  2104,  2104,  2104,  2104,
    2104,  2104,  2105,  2105,  2107,  2112,  2119,  2124,  2130,  2135,
    2140,  2145,  2151,  2156,  2162,  2168,  2176,  2184,  2194,  2204,
    2215,  2224,  2235,  2247,  2257,  2266,  2275,  2284,  2295,  2304,
    2304,  2304,  2304,  2304,  2305,  2305,  2307,  2320,  2332,  2342,
    2352,  2360,  2369,  2376,  2383,  2391,  2399,  2406,  2414,  2419,
    2424,  2429,  2436,  2446,  2454,  2465,  2473,  2485,  2485,  2485,
    2485,  2485,  2485,  2485,  2486,  2486,  2486,  2486,  2486,  2488,
    2490,  2496,  2505,  2514,  2524,  2528,  2535,  2545,  2554,  2563,
    2573,  2583,  2589,  2596,  2600,  2605,  2610,  2615,  2620,  2620,
    2620,  2620,  2620,  2622,  2627,  2634,  2640,  2645,  2652,  2660,
    2666,  2675,  2685,  2697,  2708,  2713,  2719,  2724,  2731,  2750,
    2768,  2779,  2789,  2803,  2817,  2831,  2845,  2862,  2882,  2900,
    2915,  2943,  2950,  2958,  2968,  2968,  2968,  2968,  2968,  2968,
    2969,  2976,  2984,  2989,  2995,  3006,  3035,  3057,  3069,  3085,
    3100,  3110,  3118,  3128,  3136,  3152,  3163,  3171,  3185,  3197,
    3208,  3237,  3259,  3274,  3288,  3299,  3328,  3350,  3358,  3371,
    3371,  3371,  3371,  3372,  3372,  3372,  3372,  3372,  3373,  3373,
    3373,  3373,  3373,  3373,  3373,  3375,  3375,  3375,  3375,  3375,
    3375,  3375,  3375,  3377,  3378,  3389,  3400,  3405,  3413,  3423,
    3437,  3447,  3461,  3466,  3472,  3479,  3492,  3505,  3517,  3537,
    3550,  3559,  3579,  3593,  3600,  3610,  3619,  3631,  3640,  3652,
    3661,  3670,  3695,  3713,  3722,  3733,  3742,  3752,  3761,  3777,
    3808,  3859,  3859,  3859,  3859,  3859,  3860,  3860,  3860,  3860,
    3860,  3861,  3861,  3863,  3871,  3880,  3885,  3892,  3902,  3913,
    3942,  3964,  3972,  3986,  3986,  3986,  3986,  3988,  3996,  4005,
    4010,  4017,  4025,  4031,  4043,  4059,  4071,  4083,  4097,  4111,
    4129,  4141,  4155,  4167,  4176,  4186,  4197,  4210,  4218,  4223,
    4229,  4241,  4250,  4255,  4261,  4274,  4283,  4288,  4294,  4307,
    4317,  4329,  4373,  4384,  4398,  4398,  4398,  4398,  4398,  4398,
    4398,  4398,  4398,  4398,  4398,  4398,  4398,  4398,  4398,  4399,
    4399,  4399,  4400,  4413,  4424,  4429,  4435,  4447,  4459,  4473,
    4480,  4491,  4520,  4543,  4555,  4570,  4570,  4570,  4571,  4586,
    4586,  4586,  4586,  4586,  4586,  4586,  4586,  4586,  4588,  4588,
    4590,  4597,  4607,  4622,  4627,  4633,  4633,  4633,  4635,  4651,
    4661,  4676,  4676,  4678,  4683,  4689,  4702,  4702,  4702,  4702,
    4702,  4702,  4702,  4702,  4703,  4703,  4703,  4703,  4703,  4703,
    4703,  4703,  4704,  4704,  4704,  4704,  4704,  4704,  4706,  4720,
    4734,  4739,  4745,  4759,  4759,  4759,  4759,  4759,  4759,  4759,
    4759,  4759,  4760,  4760,  4762,  4776,  4790,  4795,  4801,  4812,
    4823,  4835,  4849,  4858,  4858,  4858,  4858,  4858,  4860,  4871,
    4883,  4888,  4894,  4905,  4905,  4905,  4906,  4906,  4906,  4908,
    4922,  4935,  4945,  4957,  4962,  4968,  4984,  4997,  5010,  5023,
    5036,  5053,  5064,  5076,  5088,  5100,  5107,  5117,  5125,  5125,
    5125,  5125,  5125,  5125,  5125,  5126,  5126,  5126,  5126,  5127,
    5127,  5129,  5143,  5156,  5166,  5178,  5183,  5189,  5201,  5201,
    5201,  5201,  5201,  5201,  5201,  5201,  5201,  5202,  5204,  5218,
    5231,  5241,  5253,  5258,  5264,  5280,  5291,  5305,  5317,  5329,
    5341,  5353,  5365,  5372,  5383,  5390,  5401,  5409,  5416,  5427,
    5434,  5445,  5456,  5467,  5467,  5469,  5483,  5496,  5497,  5502,
    5508,  5515,  5525,  5536,  5547,  5547,  5547,  5547,  5547,  5547,
    5547,  5547,  5547,  5547,  5547,  5550,  5564,  5577,  5580,  5585,
    5592,  5599,  5611,  5611,  5611,  5611,  5612,  5612,  5614,  5628,
    5641,  5651,  5663,  5668,  5674,  5684,  5697,  5698,  5698,  5698,
    5698,  5698,  5698,  5700,  5714,  5727,  5737,  5749,  5754,  5760,
    5772,  5772,  5773,  5773,  5773,  5773,  5773,  5773,  5775,  5789,
    5802,  5812,  5824,  5829,  5835,  5849,  5860,  5860,  5860,  5860,
    5860,  5860,  5861,  5861,  5861,  5861,  5861,  5861,  5861,  5861,
    5863,  5877,  5890,  5900,  5912,  5917,  5923,  5934,  5939,  5944,
    5949,  5954,  5960,  5965,  5971,  5977,  5982,  5987,  6075,  6075,
    6075,  6076,  6076,  6076,  6076,  6078,  6092,  6105,  6115,  6127,
    6132,  6138,  6152,  6157,  6163,  6174,  6185,  6196,  6207,  6218,
    6219,  6220,  6221,  6222,  6223,  6224,  6225,  6226,  6227,  6228,
    6229,  6230,  6231,  6232,  6233,  6234,  6235,  6236,  6237,  6238,
    6239,  6240,  6241,  6242,  6243,  6244,  6245,  6246,  6247,  6248,
    6249,  6250,  6251,  6252,  6253,  6254,  6255,  6256,  6257,  6258,
    6259,  6260,  6261,  6262,  6263,  6264,  6265,  6266,  6267,  6268,
    6269,  6270,  6271,  6272,  6273,  6274,  6275,  6276,  6277,  6278,
    6279,  6280,  6281,  6282,  6283,  6284,  6285,  6286,  6287,  6288,
    6289,  6290,  6291,  6292,  6293,  6294,  6295,  6296,  6297,  6298,
    6299,  6300,  6301,  6302,  6303,  6304,  6305,  6306,  6307,  6308,
    6309,  6310,  6311,  6312,  6313,  6314,  6315,  6316,  6317,  6318,
    6319,  6320,  6321,  6322,  6323,  6324,  6325,  6326,  6327,  6328,
    6329,  6330,  6331,  6332,  6335,  6340,  6347,  6348,  6365,  6372,
    6381,  6392,  6406,  6421,  6433,  6442,  6449,  6459,  6468,  6469
  };

  void
  MCNPParser::yy_stack_print_ () const
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
  MCNPParser::yy_reduce_print_ (int yyrule) const
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

  MCNPParser::symbol_kind_type
  MCNPParser::yytranslate_ (int t) YY_NOEXCEPT
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const short
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
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450
    };
    // Last valid token kind.
    const int code_max = 705;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return static_cast <symbol_kind_type> (translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

#line 35 "MCNPParser.bison"
} // wasp
#line 13490 "MCNPParser.cpp"

#line 6484 "MCNPParser.bison"
 /*** Additional Code ***/

void wasp::MCNPParser::error(const MCNPParser::location_type& l,
                           const std::string& m)
{
    interpreter.error_stream()<<l<<": "<<m<<std::endl;
}

void assign_names(wasp::AbstractInterpreter& interpreter,
                const std::vector<std::string>& names,
                const std::vector<wasp::mcnpi::NODE>& types,
                const std::vector<size_t>* tokens,
                std::vector<size_t>& results)
{
    wasp_require(names.size() == types.size());
    // for each token index determine type/index and assign name
    for (size_t namei = 0,  tokeni = 0; tokeni <  tokens->size();  ++tokeni)
    {
        auto token_index = tokens->at(tokeni);
        auto token_type = interpreter.token_type(token_index);

        // JUMP token increments the name index
        if (token_type == wasp::mcnpi::ARRAY_JUMP)
        {
            int jump = 1;
            std::string token_data = interpreter.token_data(token_index);
            // Jump shortcut can be just 'j' which produces an exception... guard
            if (token_data.size() > 1) jump = std::stoi(token_data);
            if (namei + jump < names.size()) namei += jump;
            else std::cerr<<"***Warning: line "
                          << interpreter.token_line(token_index)
                           << " contains jump statement (" << interpreter.token_data(token_index) << ") that exceeds available context size of " << names.size()
                           << std::endl;
            // capture the jump token as a child node
            auto leaf_node_index = interpreter.push_leaf(wasp::mcnpi::ARRAY_JUMP, "jump", token_index);
            results.push_back(leaf_node_index);
        }
        else
        {
            // capture the token as a named and typed child
            auto leaf_node_index = interpreter.push_leaf(types[namei], namei < names.size() ? names[namei].data() : "unknown", token_index);
            results.push_back(leaf_node_index);
            ++namei;
        }
    }
    // Need to have captured all tokens... but
    // results could be prepopulated with previously identified children indices
    wasp_ensure(results.size() >= tokens->size());
}
