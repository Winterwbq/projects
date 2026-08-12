%top{
/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

}
%{ /*** C/C++ Declarations ***/

#include <string>
#include <sstream>
#include "MCNPLexer.h"
#include "MCNPInterpreter.h"

/* import the parser's token type into a local typedef */
typedef wasp::MCNPParser::token token;
typedef wasp::MCNPParser::token_type token_type;

/* By default yylex returns int, we use token_type. Unfortunately yyterminate
 * by default returns 0, which is not of token_type. */
#define yyterminate() return token::END

%}

 /*** Flex Declarations and Options ***/

 /* enable c++ scanner class generation */
%option c++

 /* change the name of the scanner class. results in "MCNPFlexLexer" */
%option prefix="MCNP"
%option outfile="MCNPLexer.cpp"
 /*%option yyclass="wasp::MCNPLexerImpl"*/

 /* enable scanner to generate debug output. disable this for release
 * versions. */
 /*%option debug*/

 /* no support for include files is planned */
%option yywrap nounput

 /* MCNP doesn't care about case */
%option caseless

 /* enables the use of start condition stacks */
%option stack
/*%x exclusive*/
/*%s inclusive*/
%x start_mcnp_state
%x message_block_state
%x message_block_filename_value_state
%x title_card_state
%s cell_card_block_state
%s cell_card_state
%s cell_density_state
%s cell_geom_state
%s cell_id_state
%s particle_designator_state
%s xyz_state
%s surface_card_block_state
%s surface_card_state
%s data_card_block_state
%x column_forced_comment_state
%s x_comment_card_state
%s comment_flag_state
%x exclusive_comment_state
%s data_mode_card_state
%s cell_card_like_but_state
%x card_id_state
%s abx_lib_id_state
%x anything_block_state
%x single_value_state
%x single_token_state
%s column_wise_header_state
%s column_wise_state
%x continue_card_state

BLANK_LINE_DELIMITER ^[ \t]*\r?\n
FRAC \-?[0-9]+([eE]\+?[0-9]+)?
WHOLE [\+\-]?[0-9]+([eE]\+?[0-9]+)?
INTEGER {WHOLE}
EXPONENT [eE]{WHOLE}
NON_WHITESPACE [^ \t\n\r]+
LINE_CONTINUATION &\ *\r?\n

DOUBLE {WHOLE}?\.{FRAC}?{EXPONENT}?|{WHOLE}\.({FRAC}?{EXPONENT}?)?|{WHOLE}\.?{EXPONENT}
FORTRAN_FLOAT ([\+\-]?[0-9]*((\.[0-9]+(D?[\+\-]?[0-9]+)?)))|([\+\-]?[0-9]+((D?[\-\+][0-9]+)))
NUMBER {DOUBLE}|{INTEGER}
STR [A-Za-z0-9_\./\\]*
STRING [A-Za-z_/\\]({STR}(-{STR})?)*
UNICODE [^\x00-\x7F]+
MESSAGE_CARD message:

DOUBLE_QUOTED_STRING \"([^\"\n])*\"
SINGLE_QUOTED_STRING \'([^\'\n])*\'
QSTRING {DOUBLE_QUOTED_STRING}|{SINGLE_QUOTED_STRING}
 // Comments are:
 // 1. Trailing comment where line ends with dollar '$'
 // 2. Line-wise where 'C' starts in one of the first 5 columns with at least a single whitespace following
COMMENT \$([^\n]|{UNICODE})*
COMMENT_CARD ^\ {0,4}C\ ([^\n]|{UNICODE})*
COLUMN_FORCED_COMMENT ([^\n]|{UNICODE})+
FORCED_FLAGGED_COMMENT [^=\ ]+([^\n]|{UNICODE})*
FORCED_COMMENT ([^\n]|{UNICODE})+

ARRAY_JUMP {INTEGER}?J
ARRAY_REPEAT {INTEGER}?R
ARRAY_MULTIPLY {NUMBER}M
ARRAY_LOG_INTERP {INTEGER}?I?LOG
ARRAY_LIN_INTERP {INTEGER}?I

ASSIGN =
NEWLINE \n

 /* Execution line input file name keys, execution_option(s) and other_option(s) */
 /* file name keys */
DATAPATH datapath=
INP i(n(p)?)?=
OUTP o(u(t(p)?)?)?=
RUNTPE r(u(n(t(p(e)?)?)?)?)?=
WWINP wwi(n(p)?)?=
WWOUT wwou(t)?=
WWONE wwon(e)?=
PARTINP pa(r(t(i(n(p)?)?)?)?)?=
LINKIN linki(n)?=
LINKOUT linko(u(t)?)?=
KSENTAL k(s(e(n(t(e(l)?)?)?)?)?)?=
HISTP h(i(s(t(p)?)?)?)?=
COM com=
COMOUT como(u(t)?)?=
PLOTM pl(o(t(m)?)?)?=
MCTAL mc(t(a(l)?)?)?=
MESHTAL me(s(h(t(a(l)?)?)?)?)?=
MDATA md(a(t(a)?)?)?=
NAME n(a(m(e)?)?)?=
SRCTP s(r(c(t(p)?)?)?)?=
WSSA w(s(s(a)?)?)?=
RSSA r(s(s(a)?)?)?=


 /* execution options */
INPUT i
PLOT_GEOM p
XS x
TRANSPORT r
PLOT_DATA z
PARTISN_INPUT m

 /* other options for execution line */
CONTINUE_RUN c
CONTINUE_RUN_TPE cn
DBUG dbug\.?
DEVTEST dev-test\.?
NOTEK notek\.?
FATAL fatal\.?
PRINT print\.?
TASKS tasks
BALANCE balance\.?
MCNPEOL eol\.?
RECORD_IDENTIFIER ^\ {0,4}{INTEGER}
SURFACE_RECORD_IDENTIFIER ^\ {0,4}[*]?{INTEGER}\+?
MATERIAL_CARD ^\ {0,4}M
PARTICLE_DESIGNATOR [nqpe|quvfyo!<>g/z~CW@DTSAHL+\-Xk%^b_*?#]

 /* Indicates the problem is a continuation problem... only requires data block */
CONTINUE continue

 /* The following paragraph suffices to track locations accurately. Each time
 * yylex is invoked, the begin position is moved onto the end position.
 * If the token starts after column 128 it (remainder of the line) is a comment
 * and we put back the text and retry for comment. Make sure the trailing newline
 * doesn't also trigger/push the forced_comment_state.
 * NOTE: prior column limit was 80
 *      this is not backward/forward compatible so this is an area where inputs will need to be update
std::cout <<"Matched '"<<yytext<<"' at " << *yylloc << " in state " << YY_START <<  " and BOL " << YY_AT_BOL() << std::endl; \
 */
%{
#define YY_USER_ACTION  { \
  if (yylloc->begin.column > 128 && YY_START != column_forced_comment_state && yytext[0] != '\n') \
  { \
    yy_push_state(column_forced_comment_state); \
    *yylloc -= yyleng; \
    yyless(0); \
    yylloc->step(); \
    YY_BREAK; \
  } \
  yylloc->columns(yyleng); file_offset+=yyleng; \
  }
%}

%% /*** Regular Expressions Part ***/
 /* code to place at the beginning of yylex() */
%{
    // reset location
    yylloc->step();
%}
 /*** BEGIN EXAMPLE - Change the wasp lexer rules below ***/

<start_mcnp_state>{CONTINUE} {
    capture_token(yylval,wasp::mcnpi::CONTINUE_PROBLEM);
    yy_push_state(continue_card_state);
    return token::CONTINUE;
}

<continue_card_state>{NON_WHITESPACE}[^\n]+ {
    // content trailing the 'CONTINUE' is deemed a TITLE
    capture_token(yylval,wasp::mcnpi::TITLE_CARD);
    return token::TITLE_CARD;
}

<continue_card_state>{NEWLINE} {
    yy_pop_state();
    // We are done with continue card... moving on to data card
    yy_push_state(data_card_block_state);
    yylloc->lines(yyleng); yylloc->step();
    interpreter.push_line_offset(file_offset-yyleng);
}
<start_mcnp_state>{MESSAGE_CARD} {
    capture_token(yylval,wasp::mcnpi::MESSAGE_CARD);
    yy_push_state(message_block_state);
    return token::MESSAGE_CARD;
}
<start_mcnp_state>{NON_WHITESPACE} {
    // Not Message or Continue, must be title...
    // make sure all tokens on this line are captured as the title
    yy_push_state(title_card_state);
    *yylloc -= yyleng;
    yyless(0);
    YY_BREAK;
}
<title_card_state>[^\n]+ {
    capture_token(yylval,wasp::mcnpi::TITLE_CARD);
    return token::TITLE_CARD;
}

<title_card_state>{NEWLINE} {
    yy_pop_state(); // pops title_card ... afterwhich the cell card block is processed
    yy_push_state(cell_card_block_state);
    yylloc->lines(yyleng); yylloc->step();
    interpreter.push_line_offset(file_offset-yyleng);
}
<comment_flag_state>{ASSIGN} {
    capture_token(yylval,wasp::mcnpi::ASSIGN);
    return token::ASSIGN;
}
<comment_flag_state>{FORCED_FLAGGED_COMMENT} {
    yy_pop_state();
    capture_token(yylval,wasp::mcnpi::VALUE);
    return token::VALUE;
}
<exclusive_comment_state>{FORCED_COMMENT} {
    int non_ws = 1;
    // if this field is beyond 'FCn'/'SCn'
    // it is the first encounter
    if (yylloc->begin.column > 3) goto forced_comment_capture;

    for (int i = 0; i < yyleng; ++i, ++non_ws)
    {
        if (yytext[i] != ' ' || i > 5)
        {
            break;
        }
    }
    // Beyond 5th column indicates continuation
    if (non_ws > 5)
    {
        forced_comment_capture:
        capture_token(yylval,wasp::mcnpi::COMMENT);
        return token::COMMENT;
    }
    // not comment, pop comment and tally/source state and return text
    yy_pop_state();
    yy_pop_state();
    *yylloc-= yyleng; // undo column increment
    yyless(0);
    // Reset the beginning of line so patterns that start at BOL properly match
    yy_set_bol(1);
    YY_BREAK;
}
<column_wise_state>^\ {0,4}[^ \-0-9c]+[^\n]* {
    // A non-digit/non-comment character in first 5 columns is a data record and
    // and signals the end of the column-wise data
    // Put the data back and conclude
    *yylloc -= yyleng;
    yyless(0);
    yylloc->step();
    yy_pop_state();
    // Reset the beginning of line so patterns that start at BOL properly match
    yy_set_bol(1);
    YY_BREAK;
}

<message_block_state>{DATAPATH}      {yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::DATAPATH;}
<message_block_state>{INP}           {yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::INP;}
<message_block_state>{OUTP}          {yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::OUTP;}
<message_block_state>{RUNTPE}        {yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::RUNTPE;}
<message_block_state>{WWINP}         {yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::WWINP;}
<message_block_state>{WWOUT}         {yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::WWOUT;}
<message_block_state>{WWONE}         {yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::WWONE;}
<message_block_state>{PARTINP}       {yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::PARTINP;}
<message_block_state>{LINKIN}        {yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::LINKIN;}
<message_block_state>{LINKOUT}       {yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::LINKOUT;}
<message_block_state>{KSENTAL}       {yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::KSENTAL;}
<message_block_state>{HISTP}         {yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::HISTP;}
<message_block_state>{COM}           {yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::COM;}
<message_block_state>{COMOUT}        {yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::COMOUT;}
<message_block_state>{PLOTM}         {yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::PLOTM;}
<message_block_state>{MCTAL}         {yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::MCTAL;}
<message_block_state>{MESHTAL}       {yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::MESHTAL;}
<message_block_state>{MDATA}         {yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::MDATA;}
<message_block_state>{NAME}          {yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::NAME;}
<message_block_state>{SRCTP}         {yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::SRCTP;}
<message_block_state>{WSSA}          {yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::WSSA;}
<message_block_state>{RSSA}          {yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::RSSA;}
<message_block_state>{INPUT}         {capture_token(yylval,wasp::mcnpi::KEY); return token::INPUT;}
<message_block_state>{PLOT_GEOM}     {capture_token(yylval,wasp::mcnpi::KEY); return token::PLOT_GEOM;}
<message_block_state>{XS}            {capture_token(yylval,wasp::mcnpi::KEY); return token::XS;}
<message_block_state>{TRANSPORT}     {capture_token(yylval,wasp::mcnpi::KEY); return token::TRANSPORT;}
<message_block_state>{PLOT_DATA}     {capture_token(yylval,wasp::mcnpi::KEY); return token::PLOT_DATA;}
<message_block_state>{PARTISN_INPUT} {capture_token(yylval,wasp::mcnpi::KEY); return token::PARTISN_INPUT;}

<message_block_state,cell_geom_state,surface_card_block_state,surface_card_state,data_card_block_state,column_wise_state,column_wise_header_state>{LINE_CONTINUATION} {
    // consumes the '&' and newline
    // Capture only the '&' not the newline...
    // capture_token("&",wasp::mcnpi::LINE_CONTINUATION);
    std::size_t offset = file_offset - yyleng;
    yylval->token_index = interpreter.token_count();
    interpreter.push_token("&", wasp::mcnpi::LINE_CONTINUATION, offset);
    yylloc->lines(1); yylloc->step();
    interpreter.push_line_offset(file_offset-1);
    interpreter.push_hidden_leaf(wasp::mcnpi::LINE_CONTINUATION,"LC", yylval->token_index);

    // Line continuation in column-wise header terminates the header
    // and we need newlines during table processing
    if (YY_START == column_wise_state)
    {
        return token::NEWLINE;
    }
    else if(YY_START == column_wise_header_state)
    {
        yy_pop_state();
        yy_push_state(column_wise_state);
        return token::NEWLINE;
    }
    YY_BREAK;
}

<message_block_filename_value_state>{INTEGER} {
    capture_token(yylval,wasp::mcnpi::VALUE);
    yy_pop_state();  // pop exclusive state that disabmiguates the remaining flag-based execution line options
    return token::INTEGER;
}
<message_block_filename_value_state>{NON_WHITESPACE} {
    capture_token(yylval,wasp::mcnpi::VALUE);
    yy_pop_state();  // pop exclusive state that disabmiguates the remaining flag-based execution line options
    return token::VALUE;
}

<message_block_state>{CONTINUE_RUN} {capture_token(yylval,wasp::mcnpi::KEY); return token::CONTINUE_RUN;}
<message_block_state>{CONTINUE_RUN}\ +{INTEGER} {yyless(1); yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::CONTINUE_RUN;}
<message_block_state>{CONTINUE_RUN_TPE} {capture_token(yylval,wasp::mcnpi::KEY); return token::CONTINUE_RUN_TPE;}
<message_block_state>{CONTINUE_RUN_TPE}\ +{INTEGER} {yyless(2); yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::CONTINUE_RUN_TPE;}
<message_block_state>{DBUG} {yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::DBUG;}
<message_block_state>{DEVTEST} {capture_token(yylval,wasp::mcnpi::KEY); return token::DEVTEST;}
<message_block_state>{NOTEK} {capture_token(yylval,wasp::mcnpi::KEY); return token::NOTEK;}
<message_block_state>{FATAL} {capture_token(yylval,wasp::mcnpi::KEY); return token::FATAL;}
<message_block_state>{PRINT} {capture_token(yylval,wasp::mcnpi::KEY); return token::PRINT;}
<message_block_state>{TASKS} {yy_push_state(message_block_filename_value_state); capture_token(yylval,wasp::mcnpi::KEY); return token::TASKS;}
<message_block_state>{BALANCE} {capture_token(yylval,wasp::mcnpi::KEY); return token::BALANCE;}
<message_block_state>{MCNPEOL} {capture_token(yylval,wasp::mcnpi::KEY); return token::MCNPEOL;}


<message_block_state>{BLANK_LINE_DELIMITER} {
   capture_token(yylval,wasp::mcnpi::BLANK_LINE);
   yy_pop_state(/*message_block_state*/); // exit message block state
   yylloc->lines(yyleng); yylloc->step();
   interpreter.push_line_offset(file_offset-yyleng);
   return token::BLANK_LINE;
}

<message_block_state>{NEWLINE} {
   yylloc->lines(yyleng); yylloc->step();
   interpreter.push_line_offset(file_offset-yyleng);
}


<cell_card_block_state>{RECORD_IDENTIFIER} {
    capture_token(yylval,wasp::mcnpi::ID);
    yy_push_state(cell_card_state);
    return token::CELL_ID;
}

<cell_geom_state>{RECORD_IDENTIFIER} {
    auto li =  interpreter.token_count(); // should always be > 0 per MCNP format
    if (li > 0 && interpreter.token_type(li-1) == wasp::mcnpi::LINE_CONTINUATION)
    {
        // not a record id but just a integer
        capture_token(yylval,wasp::mcnpi::INTEGER);
        return token::INTEGER;
    }
// If a cell record identifier is encountered (integer in first five colomns) a new record
// is being input... pop geom state back to cell_card_state so as to continue material id processing
    yy_pop_state(); // leave cell_geom_state and return to cell_card_state for expected material token
    capture_token(yylval,wasp::mcnpi::ID);
    return token::CELL_ID;
}

<cell_card_state>{INTEGER} {
    capture_token(yylval,wasp::mcnpi::MATERIAL_ID);
    int material_id = std::stoi(yytext);
    if (material_id != 0) // non-void has required density
    {
        yy_push_state(cell_density_state);
    }
    else
    {
        yy_push_state(cell_geom_state);
    }
    return token::MATERIAL_ID;
}

<cell_geom_state>{INTEGER} {
    capture_token(yylval,wasp::mcnpi::VALUE);
    return token::INTEGER;
}


<cell_card_state>LIKE {
    yy_push_state(cell_geom_state);
    yy_push_state(cell_card_like_but_state);
    capture_token(yylval,wasp::mcnpi::FLAG);
    return token::LIKE;
}

<cell_card_like_but_state>BUT {
    capture_token(yylval,wasp::mcnpi::FLAG);
    yy_pop_state(); // return to cell_geom_state
    return token::BUT;
}
<cell_geom_state,data_card_block_state>MAT {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::MAT;
}

<cell_geom_state,data_card_block_state>RHO {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::RHO;
}
<cell_geom_state,data_card_block_state>VOL {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::VOL;
}
<cell_geom_state,data_card_block_state>PWT {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::PWT;
}
<cell_geom_state,data_card_block_state>NONU {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::NONU;
}
<cell_geom_state,data_card_block_state>PD{INTEGER} {
    yyless(2); // put back {INTEGER}
    capture_token(yylval,wasp::mcnpi::DECL);
    yy_push_state(card_id_state);
    return token::PD;
}
<cell_geom_state,data_card_block_state>PD {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::PD;
}
<cell_geom_state,cell_card_block_state,data_card_block_state>TMP{INTEGER} {
    yyless(3); // put back {INTEGER}
    capture_token(yylval,wasp::mcnpi::DECL);
    yy_push_state(card_id_state);
    return token::TMP;
}
<cell_geom_state,cell_card_block_state,data_card_block_state>TMP {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::TMP;
}
<cell_geom_state>U {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::UNIVERSE;
}
<cell_geom_state>TRCL {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::TRCL;
}
<cell_geom_state,data_card_block_state>\* {
    capture_token(yylval,wasp::mcnpi::FLAG);
    return token::ASTERISK;
}
<cell_geom_state>LAT {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::LAT;
}
<cell_geom_state>FILL {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::FILL;
}
<cell_geom_state,data_card_block_state>COSY {capture_token(yylval,wasp::mcnpi::DECL);return token::COSY;}

<cell_geom_state>BFLCL {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::BFLCL;
}
<cell_geom_state>UNC {
    capture_token(yylval,wasp::mcnpi::DECL);
    yy_push_state(particle_designator_state);
    return token::UNC;
}
<cell_geom_state,data_card_block_state>IMP {
    capture_token(yylval,wasp::mcnpi::DECL);
    yy_push_state(particle_designator_state);
    return token::IMP;
}
<cell_geom_state>EXT {
    capture_token(yylval,wasp::mcnpi::DECL);
    yy_push_state(particle_designator_state);
    return token::EXT;
}
<cell_geom_state,data_card_block_state>DXC{INTEGER} {
    yyless(3); // put back INTEGER
    capture_token(yylval,wasp::mcnpi::DECL);
    yy_push_state(particle_designator_state);
    yy_push_state(card_id_state);
    return token::DXC;
}
<data_card_block_state>DXC: {
    yyless(3); // put back COLON
    capture_token(yylval,wasp::mcnpi::DECL);
    yy_push_state(particle_designator_state);
    return token::DXC;
}
<cell_geom_state>DXC {
    capture_token(yylval,wasp::mcnpi::DECL);
    yy_push_state(particle_designator_state);
    return token::DXC;
}
<cell_geom_state>WWN{INTEGER} {
    yyless(3); // put back INTEGER
    capture_token(yylval,wasp::mcnpi::DECL);
    yy_push_state(particle_designator_state);
    yy_push_state(card_id_state);
    return token::WWN;
}
<cell_geom_state>WWN {
    capture_token(yylval,wasp::mcnpi::DECL);
    yy_push_state(particle_designator_state);
    return token::WWN;
}
<cell_geom_state>FCL {
    capture_token(yylval,wasp::mcnpi::DECL);
    yy_push_state(particle_designator_state);
    return token::FCL;
}
<cell_geom_state,data_card_block_state>ELPT {
    capture_token(yylval,wasp::mcnpi::DECL);
    yy_push_state(particle_designator_state);
    return token::ELPT;
}

<data_card_block_state>PHYS {
    capture_token(yylval,wasp::mcnpi::DECL);
    yy_push_state(particle_designator_state);
    return token::PHYS;
}

<particle_designator_state,data_card_block_state>: {
    capture_token(yylval,wasp::mcnpi::COLON);
    return token::COLON;
}
<xyz_state>X|Y|Z {
    // This state is intended to disabmiguate F5A tally
    // E.g., F5X:X confuses the parser because the A (i.e. X,Y, or Z) matches particle_designator_state
    // Parser rule expects a string to be returned
    capture_token(yylval,wasp::mcnpi::STRING);
    yy_pop_state(); // return to particle_designator_state_state
    return token::STRING;
}
<particle_designator_state>{PARTICLE_DESIGNATOR} {
    yy_pop_state();
    capture_token(yylval,wasp::mcnpi::PARTICLE_SYMBOL);
    return token::PARTICLE_SYMBOL;
}
<particle_designator_state>{PARTICLE_DESIGNATOR}, {
    yyless(yyleng-1); // put back comma
    // when a comma follows we do not pop the state because we expect additional designators
    capture_token(yylval,wasp::mcnpi::PARTICLE_SYMBOL);
    return token::PARTICLE_SYMBOL;
}
<cell_geom_state># {
    capture_token(yylval,wasp::mcnpi::COMPLEMENT);
    return token::COMPLEMENT;
}

<cell_geom_state>: {
    capture_token(yylval,wasp::mcnpi::UNION);
    return token::UNION;
}

<cell_geom_state,data_card_block_state>\( {
    capture_token(yylval,wasp::mcnpi::LPAREN);
    return token::LPAREN;
}
<cell_geom_state,data_card_block_state>\) {
    capture_token(yylval,wasp::mcnpi::RPAREN);
    return token::RPAREN;
}
<cell_density_state>{INTEGER}|{DOUBLE} {
    capture_token(yylval,wasp::mcnpi::DENSITY);
    yy_pop_state(); // return to cell_card_state
    yy_push_state(cell_geom_state);
    return token::DENSITY;
}

<cell_density_state>{FORTRAN_FLOAT} {
    capture_upconverted_fortran_float_token(yylval,wasp::mcnpi::DENSITY);
    yy_pop_state(); // return to cell_card_state
    yy_push_state(cell_geom_state);
    return token::DENSITY;
}

<cell_geom_state>{BLANK_LINE_DELIMITER} {
    capture_token(yylval,wasp::mcnpi::BLANK_LINE);
    yy_pop_state(/*cell_geom_state*/); // exit cell_geom_state
    yy_push_state(surface_card_block_state); // surface card follows cell_card
    yylloc->lines(yyleng); yylloc->step();
    return token::BLANK_LINE;
}

<cell_geom_state>{NEWLINE} {
   yylloc->lines(yyleng); yylloc->step();
   interpreter.push_line_offset(file_offset-yyleng);
}

<cell_card_state>{NEWLINE} {
   yy_pop_state(/*cell_card_state*/); // exit cell_card_state
   yylloc->lines(yyleng); yylloc->step();
   interpreter.push_line_offset(file_offset-yyleng);
}

<surface_card_block_state>{SURFACE_RECORD_IDENTIFIER} {
    capture_token(yylval,wasp::mcnpi::ID);
    yy_push_state(surface_card_state);
    return token::SURFACE_ID;
}
<surface_card_state>{SURFACE_RECORD_IDENTIFIER} {
    capture_token(yylval,wasp::mcnpi::ID);
    return token::SURFACE_ID;
}

<surface_card_state>P {
    capture_token(yylval,wasp::mcnpi::PLANE);
    return token::PLANE;
}
<surface_card_state>PX {
    capture_token(yylval,wasp::mcnpi::XPLANE);
    return token::XPLANE;
}
<surface_card_state>PY {
    capture_token(yylval,wasp::mcnpi::YPLANE);
    return token::YPLANE;
}
<surface_card_state>PZ {
    capture_token(yylval,wasp::mcnpi::ZPLANE);
    return token::ZPLANE;
}
<surface_card_state>SO {
    capture_token(yylval,wasp::mcnpi::ORIGIN_SPHERE);
    return token::ORIGIN_SPHERE;
}
<surface_card_state>S|SPH {
    capture_token(yylval,wasp::mcnpi::SPHERE);
    return token::SPHERE;
}

<surface_card_state>SX {
    capture_token(yylval,wasp::mcnpi::XSPHERE);
    return token::XSPHERE;
}
<surface_card_state>SY {
    capture_token(yylval,wasp::mcnpi::YSPHERE);
    return token::YSPHERE;
}
<surface_card_state>SZ {
    capture_token(yylval,wasp::mcnpi::ZSPHERE);
    return token::ZSPHERE;
}
<surface_card_state>C\/X {
    capture_token(yylval,wasp::mcnpi::XPARCYLINDER);
    return token::XPARCYLINDER;
}
<surface_card_state>C\/Y {
    capture_token(yylval,wasp::mcnpi::YPARCYLINDER);
    return token::YPARCYLINDER;
}
<surface_card_state>C\/Z {
    capture_token(yylval,wasp::mcnpi::ZPARCYLINDER);
    return token::ZPARCYLINDER;
}

<surface_card_state>CX {
    capture_token(yylval,wasp::mcnpi::XCYLINDER);
    return token::XCYLINDER;
}
<surface_card_state>CY {
    capture_token(yylval,wasp::mcnpi::YCYLINDER);
    return token::YCYLINDER;
}
<surface_card_state>CZ {
    capture_token(yylval,wasp::mcnpi::ZCYLINDER);
    return token::ZCYLINDER;
}

<surface_card_state>K\/X {
    capture_token(yylval,wasp::mcnpi::XPARCONE);
    return token::XPARCONE;
}
<surface_card_state>K\/Y {
    capture_token(yylval,wasp::mcnpi::YPARCONE);
    return token::YPARCONE;
}
<surface_card_state>K\/Z {
    capture_token(yylval,wasp::mcnpi::ZPARCONE);
    return token::ZPARCONE;
}

<surface_card_state>KX {
    capture_token(yylval,wasp::mcnpi::XCONE);
    return token::XCONE;
}
<surface_card_state>KY {
    capture_token(yylval,wasp::mcnpi::YCONE);
    return token::YCONE;
}
<surface_card_state>KZ {
    capture_token(yylval,wasp::mcnpi::ZCONE);
    return token::ZCONE;
}

<surface_card_state>SQ {
    capture_token(yylval,wasp::mcnpi::SQUADRIC);
    return token::SQUADRIC;
}
<surface_card_state>GQ {
    capture_token(yylval,wasp::mcnpi::GQUADRIC);
    return token::GQUADRIC;
}

<surface_card_state>TX {
    capture_token(yylval,wasp::mcnpi::XTORUS);
    return token::XTORUS;
}
<surface_card_state>TY {
    capture_token(yylval,wasp::mcnpi::YTORUS);
    return token::YTORUS;
}
<surface_card_state>TZ {
    capture_token(yylval,wasp::mcnpi::ZTORUS);
    return token::ZTORUS;
}

<surface_card_state>BOX {
    capture_token(yylval,wasp::mcnpi::BOX);
    return token::BOX;
}

<surface_card_state>X {
    capture_token(yylval,wasp::mcnpi::XPOINTS);
    return token::XPOINTS;
}

<surface_card_state>Y {
    capture_token(yylval,wasp::mcnpi::YPOINTS);
    return token::YPOINTS;
}

<surface_card_state>Z {
    capture_token(yylval,wasp::mcnpi::ZPOINTS);
    return token::ZPOINTS;
}
<surface_card_state>RPP {
    capture_token(yylval,wasp::mcnpi::RECTANGULAR_PPIPED);
    return token::RECTANGULAR_PPIPED;
}
<surface_card_state>RCC {
    capture_token(yylval,wasp::mcnpi::RIGHT_CIRCULAR_CYLINDER);
    return token::RIGHT_CIRCULAR_CYLINDER;
}

<surface_card_state>HEX|RHP {
    capture_token(yylval,wasp::mcnpi::HEX);
    return token::HEX;
}
<surface_card_state>REC {
    capture_token(yylval,wasp::mcnpi::RIGHT_ELLIPTICAL_CYLINDER);
    return token::RIGHT_ELLIPTICAL_CYLINDER;
}

<surface_card_state>TRC {
    capture_token(yylval,wasp::mcnpi::TRUNC_RIGHT_ANGLE_CONE);
    return token::TRUNC_RIGHT_ANGLE_CONE;
}

<surface_card_state>ELL {
    capture_token(yylval,wasp::mcnpi::ELLIPSOID);
    return token::ELLIPSOID;
}

<surface_card_state>WED {
    capture_token(yylval,wasp::mcnpi::WEDGE);
    return token::WEDGE;
}

<surface_card_state>ARB {
    capture_token(yylval,wasp::mcnpi::ARBITRARY_POLYHEDRON);
    return token::ARBITRARY_POLYHEDRON;
}

<surface_card_state>{BLANK_LINE_DELIMITER} {
    capture_token(yylval,wasp::mcnpi::BLANK_LINE);
    yy_pop_state(); // exit surface_card_state
    yy_pop_state(); // exit surface_card_block_state
    yy_push_state(data_card_block_state); // data card follows cell_card
    yylloc->lines(1); yylloc->step();
    return token::BLANK_LINE;
}

<data_card_block_state>{MATERIAL_CARD}{INTEGER} {
    char * ptr = strchr(yytext, 'm');
    if (ptr == nullptr)
    {
        ptr = strchr(yytext, 'M');
    }
    if (ptr){
        // return integer for separate tokenization
        yyless(ptr - yytext+1);
    }

    capture_token(yylval,wasp::mcnpi::DECL);
    return token::MATERIAL_KEY;
}

<data_card_block_state>GAS   {capture_token(yylval, wasp::mcnpi::DECL); return token::GAS ;}
<data_card_block_state>ESTEP {capture_token(yylval, wasp::mcnpi::DECL); return token::ESTEP;}
<data_card_block_state>HSTEP {capture_token(yylval, wasp::mcnpi::DECL); return token::HSTEP;}
<data_card_block_state>REFI  {capture_token(yylval, wasp::mcnpi::DECL); return token::REFI ;}
<data_card_block_state>REFS  {capture_token(yylval, wasp::mcnpi::DECL); return token::REFS ;}
<data_card_block_state>REFC  {capture_token(yylval, wasp::mcnpi::DECL); return token::REFC ;}
<data_card_block_state>COND  {capture_token(yylval, wasp::mcnpi::DECL); return token::COND ;}
<data_card_block_state>[0-9]{4,}\.[0-9]{2}[tcdmgpuyehporsa] {
    // return libid for separate tokenization
    char * ptr = strrchr(yytext, '.');
    if (ptr){
        // return lib id for separate tokenization
        yyless(ptr - yytext);
    }
    capture_token(yylval,wasp::mcnpi::INTEGER);
    yy_push_state(abx_lib_id_state);
    return token::INTEGER;
}
<data_card_block_state>{INTEGER}\. {
    capture_token(yylval,wasp::mcnpi::INTEGER);
    return token::INTEGER;
}

<abx_lib_id_state>\.?[0-9]{2}[tcdmgpuyehporsa]? {
    capture_token(yylval,wasp::mcnpi::LIB_ID);
    yy_pop_state();
    return token::LIB_ID;
}

<data_card_block_state>(N|P|PN|E|H|A|S|T|D)LIB {
    capture_token(yylval,wasp::mcnpi::MATERIAL_LIB);
    yy_push_state(abx_lib_id_state); // expect abx {LIB_ID}
    return token::MATERIAL_LIB;
}

<data_card_block_state>CUT: {
    yyless(3); // put back ':'
    capture_token(yylval,wasp::mcnpi::CUTOFF);
    yy_push_state(particle_designator_state);
    return token::CUTOFF;
}

<data_card_block_state>ACT {
    capture_token(yylval,wasp::mcnpi::ACTIVATION_OPTIONS);
    return token::ACTIVATION_OPTIONS;
}

<data_card_block_state>FISSION {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::FISSION;
}
<data_card_block_state>NONFISS {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::NONFISS;
}
<data_card_block_state>DN {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::DN;
}
<data_card_block_state>DG {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::DG;
}
<data_card_block_state>THRESH {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::THRESH;
}
<data_card_block_state>DNBIAS {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::DNBIAS;
}
<data_card_block_state>NAP {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::NAP;
}
<data_card_block_state>DNEB {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::DNEB;
}
<data_card_block_state>DPEB {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::DPEB;
}

<data_card_block_state>PECUT {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::PECUT;
}
<data_card_block_state>HLCUT {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::HLCUT;
}
<data_card_block_state>SAMPLE {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::SAMPLE;
}

<data_card_block_state>fq[0-9]+ {
    yyless(2);
    capture_token(yylval,wasp::mcnpi::DECL);
    yy_push_state(card_id_state);
    return token::FQ;
}
<data_card_block_state>fq {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::FQ;
}
<data_card_block_state>fc[0-9]+ {
    yyless(2);
    capture_token(yylval,wasp::mcnpi::DECL);
    yy_push_state(x_comment_card_state);
    return token::FC;
}
<x_comment_card_state>{INTEGER} {
    capture_token(yylval,wasp::mcnpi::ID);
    yy_push_state(exclusive_comment_state);
    return token::ID;
}

<data_card_block_state>sc[0-9]+ {
    yyless(2);
    capture_token(yylval,wasp::mcnpi::DECL);
    yy_push_state(x_comment_card_state);
    return token::SC;
}
<exclusive_comment_state>{NEWLINE} {
    yylloc->lines(yyleng); yylloc->step();
    interpreter.push_line_offset(file_offset-yyleng);
}

<data_card_block_state>f[0-9]+: {
    std::string t = std::string(yytext).substr(1);
    int number = std::stoi(t); // convert [0-9][xyz] to integer
    int type = number % 10;
    // put back everything except the 'f'
    yyless(1);
    capture_token(yylval,wasp::mcnpi::DECL);
    yy_push_state(particle_designator_state);
    yy_push_state(card_id_state);
    // Requires the Fn_tally cards to be contiguous enums
    return token::token_kind_type(token::F_TALLY+type);
}
<data_card_block_state>\< {
    capture_token(yylval,wasp::mcnpi::LT);
    return token::LT;
}
<data_card_block_state>\[ {
    capture_token(yylval,wasp::mcnpi::LBRACKET);
    return token::LBRACKET;
}
<data_card_block_state>\] {
    capture_token(yylval,wasp::mcnpi::RBRACKET);
    return token::RBRACKET;
}
<data_card_block_state>f{INTEGER}?5[xyz]: {
    // put back everything except the 'f'
    yyless(1);
    capture_token(yylval,wasp::mcnpi::DECL);
    yy_push_state(particle_designator_state);
    // because XYZ is ambiguous with particle designators
    // we need a state to consume the XYZ axis prior to
    // entering the particle designator state
    yy_push_state(xyz_state);
    yy_push_state(card_id_state);
    return token::F5A_TALLY;
}
<data_card_block_state>f[xyz]{INTEGER}?5: {
    // This appears to be a legacy syntax
    // put back everything except the 'f?'
    yyless(2);
    capture_token(yylval,wasp::mcnpi::DECL);
    yy_push_state(particle_designator_state);
    yy_push_state(card_id_state);
    return token::FA5_TALLY;
}

<data_card_block_state>fi[cpr][0-9]+: {
    // put back everything but fi
    yyless(3);
    capture_token(yylval,wasp::mcnpi::DECL);
    yy_push_state(particle_designator_state);
    yy_push_state(card_id_state);
    return token::FI_TALLY;
}
<data_card_block_state>f{INTEGER}?6 {
    yyless(1);
    capture_token(yylval,wasp::mcnpi::DECL);
    yy_push_state(card_id_state);
    return token::F6_TALLY;
}

<data_card_block_state>\+f {
    yyless(1);
    // Tally flags
    capture_token(yylval,wasp::mcnpi::FLAG);
    return token::PLUS;
}
<cell_geom_state,data_card_block_state>\*f {
    yyless(1);
    // Tally flags
    capture_token(yylval,wasp::mcnpi::FLAG);
    return token::ASTERISK;
}

<data_card_block_state>mode {
    capture_token(yylval,wasp::mcnpi::DECL);
    yy_push_state(data_mode_card_state);
    return token::MODE;
}
<data_mode_card_state>\-?{PARTICLE_DESIGNATOR}|# {
    // '-' indicates an anti-particle
    capture_token(yylval,wasp::mcnpi::PARTICLE_SYMBOL);
    return token::PARTICLE_SYMBOL;
}

<data_mode_card_state>{NEWLINE} {
    // terminate the mode state upon newline
    yy_pop_state();
    yylloc->lines(yyleng); yylloc->step();
    interpreter.push_line_offset(file_offset-yyleng);
}

<data_card_block_state>T{INTEGER} {
    yyless(1); // put back INTEGER
    capture_token(yylval,wasp::mcnpi::DECL);
    yy_push_state(card_id_state);
    return token::TALLY_TIME;
}

<data_card_block_state>CBEG {capture_token(yylval,wasp::mcnpi::DECL); return token::CBEG;}
<data_card_block_state>CFRQ {capture_token(yylval,wasp::mcnpi::DECL); return token::CFRQ;}
<data_card_block_state>COFI {capture_token(yylval,wasp::mcnpi::DECL); return token::COFI;}
<data_card_block_state>CONI {capture_token(yylval,wasp::mcnpi::DECL); return token::CONI;}
<data_card_block_state>CSUB {capture_token(yylval,wasp::mcnpi::DECL); return token::CSUB;}
<data_card_block_state>CEND {capture_token(yylval,wasp::mcnpi::DECL); return token::CEND;}

<data_card_block_state>KCODE {capture_token(yylval,wasp::mcnpi::DECL); return token::KCODE;}
<data_card_block_state>NPS {capture_token(yylval,wasp::mcnpi::DECL); return token::NPS;}
<data_card_block_state>PRINT {capture_token(yylval,wasp::mcnpi::DECL); return token::PRINT;}
<data_card_block_state>TALNP {capture_token(yylval,wasp::mcnpi::DECL); return token::TALNP;}
<data_card_block_state>PRDMP {capture_token(yylval,wasp::mcnpi::DECL); return token::PRDMP;}
<data_card_block_state>MT{INTEGER} {yyless(2);yy_push_state(card_id_state);capture_token(yylval,wasp::mcnpi::DECL); return token::MT_CARD;}
<data_card_block_state>KSRC {capture_token(yylval,wasp::mcnpi::DECL); return token::KSRC;}
<data_card_block_state>E{INTEGER} {yyless(1);yy_push_state(card_id_state);capture_token(yylval,wasp::mcnpi::DECL); return token::TALLY_ENERGY;}
<data_card_block_state>E{INTEGER}: {yyless(1);yy_push_state(particle_designator_state); yy_push_state(card_id_state);capture_token(yylval,wasp::mcnpi::DECL); return token::TALLY_ENERGY;}
<data_card_block_state>FM{INTEGER} {yyless(2);yy_push_state(card_id_state);capture_token(yylval,wasp::mcnpi::DECL); return token::TALLY_MULTIPLIER;}
<data_card_block_state>SD{INTEGER} {yyless(2);yy_push_state(card_id_state);capture_token(yylval,wasp::mcnpi::DECL); return token::SEGMENT_DIVISOR;}
<data_card_block_state>SDEF {capture_token(yylval,wasp::mcnpi::DECL); return token::GENERAL_SOURCE;}
<data_card_block_state>CELL? {capture_token(yylval,wasp::mcnpi::DECL); return token::CEL;}
<data_card_block_state>SUR   {capture_token(yylval,wasp::mcnpi::DECL); return token::SDEF_SUR;}
<data_card_block_state>ERG   {capture_token(yylval,wasp::mcnpi::DECL); return token::ERG;}
<data_card_block_state>TME   {capture_token(yylval,wasp::mcnpi::DECL); return token::SDEF_TME;}
<data_card_block_state>DIR   {capture_token(yylval,wasp::mcnpi::DECL); return token::SDEF_DIR;}
<data_card_block_state>VEC   {capture_token(yylval,wasp::mcnpi::DECL); return token::VEC;}
<data_card_block_state>NRM   {capture_token(yylval,wasp::mcnpi::DECL); return token::SDEF_NRM;}
<data_card_block_state>POS   {capture_token(yylval,wasp::mcnpi::DECL); return token::SDEF_POS;}
<data_card_block_state>PAR   {capture_token(yylval,wasp::mcnpi::DECL); yy_push_state(single_token_state); return token::SDEF_PAR;}
<data_card_block_state>RAD   {capture_token(yylval,wasp::mcnpi::DECL); return token::SDEF_RAD;}
<data_card_block_state>EXT   {capture_token(yylval,wasp::mcnpi::DECL); return token::EXT;}
<data_card_block_state>AXS   {capture_token(yylval,wasp::mcnpi::DECL); return token::AXS;}
<data_card_block_state>X     {capture_token(yylval,wasp::mcnpi::DECL); return token::SDEF_X;}
<data_card_block_state>Y     {capture_token(yylval,wasp::mcnpi::DECL); return token::SDEF_Y;}
<data_card_block_state>Z     {capture_token(yylval,wasp::mcnpi::DECL); return token::SDEF_Z;}
<data_card_block_state>CCC   {capture_token(yylval,wasp::mcnpi::DECL); return token::SDEF_CCC;}
<data_card_block_state>ARA   {capture_token(yylval,wasp::mcnpi::DECL); return token::SDEF_ARA;}
<data_card_block_state>WGT   {capture_token(yylval,wasp::mcnpi::DECL); return token::WGT;}
<data_card_block_state>TR    {capture_token(yylval,wasp::mcnpi::DECL); return token::TR ;}
<data_card_block_state>EFF   {capture_token(yylval,wasp::mcnpi::DECL); return token::SDEF_EFF;}
<data_card_block_state>DAT   {capture_token(yylval,wasp::mcnpi::DECL); return token::SDEF_DAT;}
<data_card_block_state>LOC   {capture_token(yylval,wasp::mcnpi::DECL); return token::SDEF_LOC;}
<data_card_block_state>BEM   {capture_token(yylval,wasp::mcnpi::DECL); return token::SDEF_BEM;}
<data_card_block_state>BAP   {capture_token(yylval,wasp::mcnpi::DECL); return token::SDEF_BAP;}
<data_card_block_state>F(CELL?|SUR|ERG|TME|DIR|VEC|NRM|POS|PAR|RAD|EXT|AXS|X|Y|Z|CCC|ARA|WGT|TR|EFF|DAT|LOC|BEM|BAP) {
    capture_token(yylval,wasp::mcnpi::VALUE); return token::SDEF_PREFIXED_VAR;
}

<data_card_block_state>SI{INTEGER} {yyless(2);yy_push_state(card_id_state);capture_token(yylval,wasp::mcnpi::DECL); return token::SOURCE_INFO;}
<data_card_block_state>SP{INTEGER} {yyless(2);yy_push_state(card_id_state);capture_token(yylval,wasp::mcnpi::DECL); return token::SOURCE_PROB;}
<data_card_block_state>SB{INTEGER} {yyless(2);yy_push_state(card_id_state);capture_token(yylval,wasp::mcnpi::DECL); return token::SOURCE_BIAS;}

<data_card_block_state>DE{INTEGER} {yyless(2);yy_push_state(card_id_state);capture_token(yylval,wasp::mcnpi::DECL); return token::DOSE_ENERGY;}
<data_card_block_state>DF{INTEGER} {yyless(2);yy_push_state(card_id_state);capture_token(yylval,wasp::mcnpi::DECL); return token::DOSE_FUNCTION;}
<data_card_block_state>FS{INTEGER} {yyless(2);yy_push_state(card_id_state);capture_token(yylval,wasp::mcnpi::DECL); return token::TALLY_SEGMENT;}
<data_card_block_state>FS{INTEGER}: {yyless(2);yy_push_state(particle_designator_state);yy_push_state(card_id_state);capture_token(yylval,wasp::mcnpi::DECL); return token::TALLY_SEGMENT;}
<data_card_block_state>FS {capture_token(yylval,wasp::mcnpi::DECL); return token::TALLY_SEGMENT;}
<data_card_block_state>TF{INTEGER} {yyless(2);yy_push_state(card_id_state);capture_token(yylval,wasp::mcnpi::DECL); return token::TALLY_FLUCTUATION;}
<data_card_block_state>LOST {capture_token(yylval,wasp::mcnpi::DECL); return token::LOST;}
<data_card_block_state>HSRC {capture_token(yylval,wasp::mcnpi::DECL); return token::HSRC;}
<data_card_block_state>XS{INTEGER} {yyless(2);yy_push_state(card_id_state);capture_token(yylval,wasp::mcnpi::DECL); return token::XS;}

<data_card_block_state>KOPTS {capture_token(yylval,wasp::mcnpi::DECL); return token::KOPTS;}
<data_card_block_state>BLOCKSIZE {capture_token(yylval,wasp::mcnpi::DECL); return token::BLOCKSIZE;}
<data_card_block_state>KINETICS   {capture_token(yylval,wasp::mcnpi::DECL); return token::KINETICS;}
<data_card_block_state>PRECURSOR {capture_token(yylval,wasp::mcnpi::DECL); return token::PRECURSOR;}
<data_card_block_state>KSENTAL   {capture_token(yylval,wasp::mcnpi::DECL); return token::KSENTAL;}
<data_card_block_state>FMAT      {capture_token(yylval,wasp::mcnpi::DECL); return token::FMAT;}
<data_card_block_state>FMATSKIP  {capture_token(yylval,wasp::mcnpi::DECL); return token::FMATSKIP;}
<data_card_block_state>FMATSPACE {capture_token(yylval,wasp::mcnpi::DECL); return token::FMATSPACE;}
<data_card_block_state>FMATACCEL {capture_token(yylval,wasp::mcnpi::DECL); return token::FMATACCEL;}
<data_card_block_state>FMATREDUCE {capture_token(yylval,wasp::mcnpi::DECL); return token::FMATREDUCE;}
<data_card_block_state>FMATNX {capture_token(yylval,wasp::mcnpi::DECL); return token::FMATNX;}
<data_card_block_state>FMATNY {capture_token(yylval,wasp::mcnpi::DECL); return token::FMATNY;}
<data_card_block_state>FMATNZ {capture_token(yylval,wasp::mcnpi::DECL); return token::FMATNZ;}

<data_card_block_state>C{INTEGER} {yyless(1);yy_push_state(card_id_state);capture_token(yylval,wasp::mcnpi::DECL); return token::TALLY_COSINE;}
<data_card_block_state>MGOPT {capture_token(yylval,wasp::mcnpi::DECL); return token::MGOPT;}
<data_card_block_state>RAND {capture_token(yylval,wasp::mcnpi::DECL); return token::RAND;}
<data_card_block_state>GEN {capture_token(yylval,wasp::mcnpi::DECL); return token::GEN;}
<data_card_block_state>SEED {capture_token(yylval,wasp::mcnpi::DECL); return token::SEED;}
<data_card_block_state>STRIDE {capture_token(yylval,wasp::mcnpi::DECL); return token::STRIDE;}
<data_card_block_state>HIST {capture_token(yylval,wasp::mcnpi::DECL); return token::HIST;}

<data_card_block_state>WWE {capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(particle_designator_state);return token::WWE;}
<data_card_block_state>WWG {capture_token(yylval,wasp::mcnpi::DECL);return token::WWG;}
<data_card_block_state>WWGE {capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(particle_designator_state);return token::WWGE;}
<data_card_block_state>WWGT {capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(particle_designator_state);return token::WWGT;}
<data_card_block_state>WWN{INTEGER} {yyless(3);capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(particle_designator_state);yy_push_state(card_id_state);return token::WWN;}
<data_card_block_state>WWN: {yyless(3);capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(particle_designator_state);return token::WWN;}
<data_card_block_state>WWP {capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(particle_designator_state);return token::WWP;}
<data_card_block_state>WWT {capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(particle_designator_state);return token::WWT;}

<data_card_block_state>FCL {capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(particle_designator_state);return token::FCL;}
<data_card_block_state>EXT/: {capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(particle_designator_state);return token::EXT_CARD;}
<data_card_block_state>ESPLT {capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(particle_designator_state);return token::ESPLT;}
<data_card_block_state>TSPLT {capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(particle_designator_state);return token::TSPLT;}

<data_card_block_state>VECT {capture_token(yylval,wasp::mcnpi::DECL);return token::VECT;}
<data_card_block_state>V{NUMBER} {capture_token(yylval,wasp::mcnpi::DECL);return token::VN;}

<data_card_block_state>DBCN {capture_token(yylval,wasp::mcnpi::DECL); return token::DBCN;}
<data_card_block_state>VAR {capture_token(yylval,wasp::mcnpi::DECL); return token::VAR;}
<data_card_block_state>RR {capture_token(yylval,wasp::mcnpi::DECL); return token::VAR_RR;}
<data_card_block_state>DD {capture_token(yylval,wasp::mcnpi::DECL); return token::DD;}
<data_card_block_state>DD{INTEGER} {yyless(2);capture_token(yylval,wasp::mcnpi::DECL); yy_push_state(card_id_state);return token::DD;}
<data_card_block_state>DXT {capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(particle_designator_state);return token::DXT;}
<data_card_block_state>CM{INTEGER} {yyless(2);capture_token(yylval,wasp::mcnpi::DECL); yy_push_state(card_id_state);return token::CM;}
<data_card_block_state>DS{INTEGER} {yyless(2);yy_push_state(card_id_state);capture_token(yylval,wasp::mcnpi::DECL); return token::DS;}
<data_card_block_state>FU{INTEGER} {yyless(2);yy_push_state(card_id_state);capture_token(yylval,wasp::mcnpi::DECL); return token::FU;}

<data_card_block_state>CF{INTEGER} {yyless(2);capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(card_id_state);return token::CELL_FLAGGING;}
<data_card_block_state>FT{INTEGER} {yyless(2);capture_token(yylval,wasp::mcnpi::DECL); yy_push_state(card_id_state);return token::FT;}
<data_card_block_state>EM{INTEGER} {yyless(2);capture_token(yylval,wasp::mcnpi::DECL); yy_push_state(card_id_state);return token::EM;}

<data_card_block_state>FRV {capture_token(yylval,wasp::mcnpi::DECL); return token::FRV;}
<data_card_block_state>GEB {capture_token(yylval,wasp::mcnpi::DECL); return token::GEB;}
<data_card_block_state>TMC {capture_token(yylval,wasp::mcnpi::DECL); return token::TMC;}
<data_card_block_state>INC {capture_token(yylval,wasp::mcnpi::DECL); return token::INC;}
<data_card_block_state>ICD {capture_token(yylval,wasp::mcnpi::DECL); return token::ICD;}
<data_card_block_state>SCX {capture_token(yylval,wasp::mcnpi::DECL); return token::SCX;}
<data_card_block_state>SCD {capture_token(yylval,wasp::mcnpi::DECL); return token::SCD;}
<data_card_block_state>ELC {capture_token(yylval,wasp::mcnpi::DECL); return token::ELC;}
<data_card_block_state>PTT {capture_token(yylval,wasp::mcnpi::DECL); return token::PTT;}
<data_card_block_state>PHL {capture_token(yylval,wasp::mcnpi::DECL); return token::PHL;}
<data_card_block_state>CAP {capture_token(yylval,wasp::mcnpi::DECL); return token::CAP;}
<data_card_block_state>RES {capture_token(yylval,wasp::mcnpi::DECL); return token::RES;}
<data_card_block_state>TAG {capture_token(yylval,wasp::mcnpi::DECL); return token::TAG;}
<data_card_block_state>LET {capture_token(yylval,wasp::mcnpi::DECL); return token::LET;}
<data_card_block_state>ROC {capture_token(yylval,wasp::mcnpi::DECL); return token::ROC;}
<data_card_block_state>PDS {capture_token(yylval,wasp::mcnpi::DECL); return token::PDS;}
<data_card_block_state>FFT {capture_token(yylval,wasp::mcnpi::DECL); return token::FFT;}
<data_card_block_state>COM {capture_token(yylval,wasp::mcnpi::DECL); return token::COM;}

<data_card_block_state>BBREM {capture_token(yylval,wasp::mcnpi::DECL); return token::BBREM;}

<data_card_block_state>CORA{INTEGER} {yyless(4);capture_token(yylval,wasp::mcnpi::DECL); yy_push_state(card_id_state);return token::CORA;}
<data_card_block_state>CORB{INTEGER} {yyless(4);capture_token(yylval,wasp::mcnpi::DECL); yy_push_state(card_id_state);return token::CORB;}
<data_card_block_state>CORC{INTEGER} {yyless(4);capture_token(yylval,wasp::mcnpi::DECL); yy_push_state(card_id_state);return token::CORC;}

<data_card_block_state>TMESH {
    capture_token(yylval,wasp::mcnpi::DECL);
    // check if it is ourside the card range, if so it belows to FMESH
    if (yylloc->begin.column > 5 ) return token::FMESH_TMESH;
    return token::TMESH;
}
<data_card_block_state>ENDMD {capture_token(yylval,wasp::mcnpi::DECL);return token::ENDMD;}
<data_card_block_state>CMESH{INTEGER}: {yyless(5);capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(particle_designator_state);yy_push_state(card_id_state);return token::CMESH;}
<data_card_block_state>RMESH{INTEGER}: {yyless(5);capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(particle_designator_state);yy_push_state(card_id_state);return token::RMESH;}
<data_card_block_state>SMESH{INTEGER}: {yyless(5);capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(particle_designator_state);yy_push_state(card_id_state);return token::SMESH;}
<data_card_block_state>CMESH{INTEGER} {yyless(5);capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(card_id_state);return token::CMESH;}
<data_card_block_state>RMESH{INTEGER} {yyless(5);capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(card_id_state);return token::RMESH;}
<data_card_block_state>SMESH{INTEGER} {yyless(5);capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(card_id_state);return token::SMESH;}

<data_card_block_state>FMESH{INTEGER}: {yyless(5);capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(particle_designator_state);yy_push_state(card_id_state);return token::FMESH;}

<data_card_block_state>GEOM    {capture_token(yylval,wasp::mcnpi::DECL); yy_push_state(single_value_state); return token::GEOM  ;}
<data_card_block_state>ORIGIN  {capture_token(yylval,wasp::mcnpi::DECL);return token::ORIGIN;}
<data_card_block_state>IMESH   {capture_token(yylval,wasp::mcnpi::DECL);return token::IMESH ;}
<data_card_block_state>IINTS   {capture_token(yylval,wasp::mcnpi::DECL);return token::IINTS ;}
<data_card_block_state>JMESH   {capture_token(yylval,wasp::mcnpi::DECL);return token::JMESH ;}
<data_card_block_state>JINTS   {capture_token(yylval,wasp::mcnpi::DECL);return token::JINTS ;}
<data_card_block_state>KMESH   {capture_token(yylval,wasp::mcnpi::DECL);return token::KMESH ;}
<data_card_block_state>KINTS   {capture_token(yylval,wasp::mcnpi::DECL);return token::KINTS ;}
<data_card_block_state>EMESH   {capture_token(yylval,wasp::mcnpi::DECL);return token::EMESH ;}
<data_card_block_state>EINTS   {capture_token(yylval,wasp::mcnpi::DECL);return token::EINTS ;}
<data_card_block_state>ENORM   {capture_token(yylval,wasp::mcnpi::DECL);return token::ENORM ;}
<data_card_block_state>TINTS   {capture_token(yylval,wasp::mcnpi::DECL);return token::TINTS ;}
<data_card_block_state>TNORM   {capture_token(yylval,wasp::mcnpi::DECL);return token::TNORM ;}
<data_card_block_state>FACTOR  {capture_token(yylval,wasp::mcnpi::DECL);return token::FACTOR;}
<data_card_block_state>OUT     {capture_token(yylval,wasp::mcnpi::DECL); yy_push_state(single_value_state); return token::MCNP_OUT;}
<data_card_block_state>TYPE    {capture_token(yylval,wasp::mcnpi::DECL); yy_push_state(single_value_state); return token::TYPE;}
<data_card_block_state>KCLEAR  {capture_token(yylval,wasp::mcnpi::DECL);return token::KCLEAR;}

<data_card_block_state>MESH {capture_token(yylval,wasp::mcnpi::DECL);return token::MESH;}
<data_card_block_state>REF {capture_token(yylval,wasp::mcnpi::DECL);return token::REF;}

<data_card_block_state>TRAKS {capture_token(yylval, wasp::mcnpi::DECL); return token::TRAKS;}
<data_card_block_state>FLUX  {capture_token(yylval, wasp::mcnpi::DECL); return token::FLUX ;}
<data_card_block_state>DOSE  {capture_token(yylval, wasp::mcnpi::DECL); return token::DOSE ;}
<data_card_block_state>POPUL {capture_token(yylval, wasp::mcnpi::DECL); return token::POPUL;}
<data_card_block_state>PEDEP {capture_token(yylval, wasp::mcnpi::DECL); return token::PEDEP;}
<data_card_block_state>MFACT {capture_token(yylval, wasp::mcnpi::DECL); return token::MFACT;}
<data_card_block_state>TRANS {capture_token(yylval, wasp::mcnpi::DECL); return token::TRANS;}
<data_card_block_state>TOTAL {capture_token(yylval, wasp::mcnpi::DECL); return token::TOTAL;}
<data_card_block_state>DE\/DX {capture_token(yylval, wasp::mcnpi::DECL); return token::DEDX;}
<data_card_block_state>RECOL {capture_token(yylval, wasp::mcnpi::DECL); return token::RECOL;}
<data_card_block_state>TLEST {capture_token(yylval, wasp::mcnpi::DECL); return token::TLEST;}
<data_card_block_state>EDLCT {capture_token(yylval, wasp::mcnpi::DECL); return token::EDLCT;}

<data_card_block_state>SF{INTEGER} {yyless(2);capture_token(yylval,wasp::mcnpi::DECL); yy_push_state(card_id_state);return token::SF;}
<data_card_block_state>TM{INTEGER} {yyless(2);capture_token(yylval,wasp::mcnpi::DECL); yy_push_state(card_id_state);return token::TM;}
<data_card_block_state>THTME {capture_token(yylval, wasp::mcnpi::DECL); return token::THTME;}

<data_card_block_state>MX{INTEGER} {yyless(2);capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(particle_designator_state);yy_push_state(card_id_state);return token::MX;}
<data_card_block_state>MPHYS {capture_token(yylval, wasp::mcnpi::DECL); return token::MPHYS;}
<data_card_block_state>TROPT {capture_token(yylval, wasp::mcnpi::DECL); return token::TROPT;}
<data_card_block_state>MCSCAT {capture_token(yylval, wasp::mcnpi::DECL); return token::MCSCAT;}
<data_card_block_state>ELOSS  {capture_token(yylval, wasp::mcnpi::DECL); return token::ELOSS ;}
<data_card_block_state>NREACT {capture_token(yylval, wasp::mcnpi::DECL); return token::NREACT;}
<data_card_block_state>NESCAT {capture_token(yylval, wasp::mcnpi::DECL); return token::NESCAT;}
<data_card_block_state>GENXS  {capture_token(yylval, wasp::mcnpi::DECL); return token::GENXS ;}

<data_card_block_state>PERT{INTEGER} {yyless(4);capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(particle_designator_state);yy_push_state(card_id_state);return token::PERT;}
<data_card_block_state>METHOD {capture_token(yylval, wasp::mcnpi::DECL); return token::METHOD;}
<data_card_block_state>RXN {capture_token(yylval, wasp::mcnpi::DECL); return token::RXN;}

<data_card_block_state>LCA {capture_token(yylval, wasp::mcnpi::DECL); return token::LCA;}
<data_card_block_state>LCB {capture_token(yylval, wasp::mcnpi::DECL); return token::LCB;}
<data_card_block_state>LCC {capture_token(yylval, wasp::mcnpi::DECL); return token::LCC;}
<data_card_block_state>LEA {capture_token(yylval, wasp::mcnpi::DECL); return token::LEA;}
<data_card_block_state>LEB {capture_token(yylval, wasp::mcnpi::DECL); return token::LEB;}
<data_card_block_state>PIKMT {capture_token(yylval, wasp::mcnpi::DECL); return token::PIKMT;}
<data_card_block_state>NOTRN {capture_token(yylval, wasp::mcnpi::DECL); return token::NOTRN;}

<data_card_block_state>UNC: {yyless(3);capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(particle_designator_state);return token::UNC;}

<data_card_block_state>EMBED{INTEGER} {yyless(5); capture_token(yylval, wasp::mcnpi::DECL); yy_push_state(card_id_state); return token::EMBED_CARD;}
<data_card_block_state>BACKGROUND {capture_token(yylval, wasp::mcnpi::DECL); return token::BACKGROUND;}
<data_card_block_state>MATCELL    {capture_token(yylval, wasp::mcnpi::DECL); return token::MATCELL   ;}
<data_card_block_state>MESHGEO    {capture_token(yylval, wasp::mcnpi::DECL); return token::MESHGEO   ;}
<data_card_block_state>MGEOIN     {capture_token(yylval, wasp::mcnpi::DECL); return token::MGEOIN    ;}
<data_card_block_state>MEEOUT     {capture_token(yylval, wasp::mcnpi::DECL); return token::MEEOUT    ;}
<data_card_block_state>MEEIN      {capture_token(yylval, wasp::mcnpi::DECL); return token::MEEIN     ;}
<data_card_block_state>CALC_VOLS  {capture_token(yylval, wasp::mcnpi::DECL); return token::CALC_VOLS ;}
<data_card_block_state>DEBUG      {capture_token(yylval, wasp::mcnpi::DECL); return token::DEBUG     ;}
<data_card_block_state>FILETYPE   {capture_token(yylval, wasp::mcnpi::DECL); return token::FILETYPE  ;}
<data_card_block_state>GMVFILE    {capture_token(yylval, wasp::mcnpi::DECL); return token::GMVFILE   ;}
<data_card_block_state>LENGTH     {capture_token(yylval, wasp::mcnpi::DECL); return token::LENGTH    ;}
<data_card_block_state>MCNPUMFILE {capture_token(yylval, wasp::mcnpi::DECL); return token::MCNPUMFILE;}
<data_card_block_state>OVERLAP    {capture_token(yylval, wasp::mcnpi::DECL); return token::OVERLAP   ;}

<data_card_block_state>EMBED {capture_token(yylval, wasp::mcnpi::DECL); return token::EMBED;}
<data_card_block_state>EMBEE{INTEGER} {yyless(5);capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(particle_designator_state); yy_push_state(card_id_state); return token::EMBEE;}
<data_card_block_state>ENERGY {capture_token(yylval, wasp::mcnpi::DECL); return token::ENERGY;}
<data_card_block_state>TIME   {capture_token(yylval, wasp::mcnpi::DECL); return token::TIME  ;}
<data_card_block_state>ATOM   {capture_token(yylval, wasp::mcnpi::DECL); return token::ATOM  ;}
<data_card_block_state>COMMENT   {capture_token(yylval, wasp::mcnpi::DECL); yy_push_state(comment_flag_state); return token::COMMENT_FLAG;}
<data_card_block_state>LIST   {capture_token(yylval, wasp::mcnpi::DECL); return token::LIST  ;}
<data_card_block_state>MTYPE  {capture_token(yylval, wasp::mcnpi::DECL); return token::MTYPE ;}
<data_card_block_state>ERRORS  {capture_token(yylval, wasp::mcnpi::DECL); return token::ERRORS;}

<data_card_block_state>EMBEB{INTEGER} {yyless(5);capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(card_id_state); return token::EMBEB;}
<data_card_block_state>EMBEB{INTEGER}: {yyless(5);capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(particle_designator_state);yy_push_state(card_id_state); return token::EMBEB;}
<data_card_block_state>EMBEM{INTEGER} {yyless(5);capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(card_id_state); return token::EMBEM;}
<data_card_block_state>EMBTB{INTEGER} {yyless(5);capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(card_id_state); return token::EMBTB;}
<data_card_block_state>EMBTM{INTEGER} {yyless(5);capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(card_id_state); return token::EMBTM;}
<data_card_block_state>EMBDE{INTEGER} {yyless(5);capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(card_id_state); return token::EMBDE;}
<data_card_block_state>EMBDF{INTEGER} {yyless(5);capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(card_id_state); return token::EMBDF;}

<data_card_block_state>VOID  {capture_token(yylval, wasp::mcnpi::DECL); return token::MCNP_VOID ;}
<data_card_block_state>DRXS  {capture_token(yylval, wasp::mcnpi::DECL); return token::DRXS;}
<data_card_block_state>CTME  {capture_token(yylval, wasp::mcnpi::DECL); return token::CTME;}
<data_card_block_state>RDUM  {capture_token(yylval, wasp::mcnpi::DECL); return token::RDUM;}
<data_card_block_state>IDUM  {capture_token(yylval, wasp::mcnpi::DECL); return token::IDUM;}
<data_card_block_state>SSW  {capture_token(yylval, wasp::mcnpi::DECL); return token::SSW;}
<data_card_block_state>SYM  {capture_token(yylval, wasp::mcnpi::DECL); return token::SYM;}
<data_card_block_state>PTY  {capture_token(yylval, wasp::mcnpi::DECL); return token::PTY;}
<data_card_block_state>SSR  {capture_token(yylval, wasp::mcnpi::DECL); return token::SSR;}
<data_card_block_state>OLD  {capture_token(yylval, wasp::mcnpi::DECL); return token::OLD;}
<data_card_block_state>NEW  {capture_token(yylval, wasp::mcnpi::DECL); return token::NEW;}
<data_card_block_state>PSC  {capture_token(yylval, wasp::mcnpi::DECL); return token::PSC;}
<data_card_block_state>POA  {capture_token(yylval, wasp::mcnpi::DECL); return token::POA;}
<data_card_block_state>COL  {capture_token(yylval, wasp::mcnpi::DECL); return token::COL;}
<data_card_block_state>BCW  {capture_token(yylval, wasp::mcnpi::DECL); return token::BCW;}

<data_card_block_state>FMULT  {capture_token(yylval, wasp::mcnpi::DECL); return token::FMULT;}
<data_card_block_state>SFNU    {capture_token(yylval, wasp::mcnpi::DECL); return token::SFNU   ;}
<data_card_block_state>WIDTH   {capture_token(yylval, wasp::mcnpi::DECL); return token::WIDTH  ;}
<data_card_block_state>SYFIELD {capture_token(yylval, wasp::mcnpi::DECL); return token::SYFIELD;}
<data_card_block_state>WATT    {capture_token(yylval, wasp::mcnpi::DECL); return token::WATT   ;}
<data_card_block_state>DATA    {capture_token(yylval, wasp::mcnpi::DECL); return token::DATA   ;}
<data_card_block_state>SHIFT   {capture_token(yylval, wasp::mcnpi::DECL); return token::SHIFT  ;}

<data_card_block_state>KPERT{INTEGER} {yyless(5);capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(card_id_state); return token::KPERT;}
<data_card_block_state>LINEAR   {capture_token(yylval, wasp::mcnpi::DECL); return token::LINEAR;}
<data_card_block_state>ISO   {capture_token(yylval, wasp::mcnpi::DECL); return token::ISO;}

<data_card_block_state>KSEN{INTEGER} {yyless(4);capture_token(yylval,wasp::mcnpi::DECL);yy_push_state(card_id_state); return token::KSEN;}
<data_card_block_state>EIN       {capture_token(yylval, wasp::mcnpi::DECL); return token::EIN      ;}
<data_card_block_state>LEGENDRE  {capture_token(yylval, wasp::mcnpi::DECL); return token::LEGENDRE ;}
<data_card_block_state>COS       {capture_token(yylval, wasp::mcnpi::DECL); return token::COS      ;}
<data_card_block_state>CONSTRAIN {capture_token(yylval, wasp::mcnpi::DECL); return token::CONSTRAIN;}
<data_card_block_state>MT        {capture_token(yylval, wasp::mcnpi::DECL); return token::MT;}

<data_card_block_state>PTRAC        {capture_token(yylval, wasp::mcnpi::DECL); return token::PTRAC;}
<data_card_block_state>BUFFER       {capture_token(yylval, wasp::mcnpi::DECL); return token::BUFFER;}
<data_card_block_state>FILE    {capture_token(yylval, wasp::mcnpi::DECL); yy_push_state(single_token_state); return token::FILE   ;}
<data_card_block_state>MAX     {capture_token(yylval, wasp::mcnpi::DECL); return token::MAX    ;}
<data_card_block_state>MEPH    {capture_token(yylval, wasp::mcnpi::DECL); return token::MEPH   ;}
<data_card_block_state>WRITE   {capture_token(yylval, wasp::mcnpi::DECL); yy_push_state(single_token_state); return token::WRITE  ;}
<data_card_block_state>COINC   {capture_token(yylval, wasp::mcnpi::DECL); return token::COINC  ;}
<data_card_block_state>EVENT   {capture_token(yylval, wasp::mcnpi::DECL); yy_push_state(single_token_state); return token::EVENT  ;}
<data_card_block_state>FILTER  {capture_token(yylval, wasp::mcnpi::DECL); yy_push_state(single_token_state); return token::FILTER ;}
<data_card_block_state>SURFACE {capture_token(yylval, wasp::mcnpi::DECL); return token::SURFACE;}
<data_card_block_state>TALLY   {capture_token(yylval, wasp::mcnpi::DECL); return token::TALLY  ;}
<data_card_block_state>VALUE   {capture_token(yylval, wasp::mcnpi::DECL); return token::VALUE_KEY;}

<data_card_block_state>BFLD{INTEGER}   {yyless(4);capture_token(yylval, wasp::mcnpi::DECL); yy_push_state(card_id_state); return token::BFLD;}
<data_card_block_state>FIELD    {capture_token(yylval, wasp::mcnpi::DECL); return token::FIELD  ;}
<data_card_block_state>MXDEFLC  {capture_token(yylval, wasp::mcnpi::DECL); return token::MXDEFLC;}
<data_card_block_state>MAXSTEP  {capture_token(yylval, wasp::mcnpi::DECL); return token::MAXSTEP;}
<data_card_block_state>FFEDGES  {capture_token(yylval, wasp::mcnpi::DECL); return token::FFEDGES;}
<data_card_block_state>REFPNT   {capture_token(yylval, wasp::mcnpi::DECL); return token::REFPNT ;}

<data_card_block_state>AWTAB  {capture_token(yylval,wasp::mcnpi::DECL);return token::AWTAB;}
<data_card_block_state>COSYP  {capture_token(yylval,wasp::mcnpi::DECL);return token::COSYP;}

^\ {0,4}#\  {
    capture_token(yylval,wasp::mcnpi::FLAG);
    yy_push_state(column_wise_header_state);
    return token::COLUMN_WISE_FLAG;
}
<column_wise_header_state>[a-z]+{INTEGER}: {
    std::string txt = yytext;
    auto index = txt.find_first_of("0123456789");
    wasp_ensure(index != std::string::npos);
    yyless(index); // put back INTEGER
    yy_push_state(particle_designator_state);
    yy_push_state(card_id_state);
    capture_token(yylval,wasp::mcnpi::STRING);
    return token::STRING;
}
<column_wise_header_state>[a-z]+{INTEGER} {
    std::string txt = yytext;
    auto index = txt.find_first_of("0123456789");
    wasp_ensure(index != std::string::npos);
    yyless(index); // put back INTEGER
    yy_push_state(card_id_state);
    capture_token(yylval,wasp::mcnpi::STRING);
    return token::STRING;
}
<column_wise_header_state>[a-z]+ {
    capture_token(yylval,wasp::mcnpi::STRING);
    return token::STRING;
}

<column_wise_state>{PARTICLE_DESIGNATOR} {capture_token(yylval,wasp::mcnpi::STRING);return token::STRING;
}
<column_wise_header_state>{NEWLINE} {
   yylloc->lines(yyleng); yylloc->step();
   interpreter.push_line_offset(file_offset-yyleng);
   yy_pop_state();
   yy_push_state(column_wise_state);
   return token::NEWLINE;
}

<column_wise_state>{ARRAY_JUMP} {
    capture_token(yylval,wasp::mcnpi::ARRAY_JUMP);
    return token::ARRAY_JUMP;
}

<column_wise_state>{ARRAY_REPEAT} {
    capture_token(yylval,wasp::mcnpi::ARRAY_REPEAT);
    return token::ARRAY_REPEAT;
}

<column_wise_state>{ARRAY_MULTIPLY} {
    capture_token(yylval,wasp::mcnpi::ARRAY_MULTIPLY);
    return token::ARRAY_MULTIPLY;
}

<column_wise_state>{ARRAY_LIN_INTERP} {
    capture_token(yylval,wasp::mcnpi::ARRAY_LIN_INTERP);
    return token::ARRAY_LIN_INTERP;
}

<column_wise_state>{ARRAY_LOG_INTERP} {
    capture_token(yylval,wasp::mcnpi::ARRAY_LOG_INTERP);
    return token::ARRAY_LOG_INTERP;
}

<card_id_state>{INTEGER} {
    capture_token(yylval,wasp::mcnpi::ID);
    yy_pop_state();
    return token::ID;
}
<data_card_block_state>TOTNU {capture_token(yylval,wasp::mcnpi::DECL);return token::TOTNU;}

<start_mcnp_state>{BLANK_LINE_DELIMITER} {
    capture_token(yylval,wasp::mcnpi::BLANK_LINE);
    yylloc->lines(1); yylloc->step();
    return token::BLANK_LINE;
}
<data_card_block_state>{BLANK_LINE_DELIMITER} {
    capture_token(yylval,wasp::mcnpi::BLANK_LINE);
    yy_pop_state(/*data_card_block_state*/); // exit data_card_block_state
    yy_push_state(anything_block_state); // anything card follows data
    yylloc->lines(1); yylloc->step();
    return token::BLANK_LINE;
}
<anything_block_state>(.*{NEWLINE})* {
    // intended to match until EOF
    capture_token(yylval, wasp::mcnpi::COMMENT);
    return token::ANYTHING_BLOCK;
}
<start_mcnp_state>{NEWLINE} {
   yylloc->lines(yyleng); yylloc->step();
   interpreter.push_line_offset(file_offset-yyleng);
}

<*>{ASSIGN} {
    capture_token(yylval,wasp::mcnpi::ASSIGN);
    return token::ASSIGN;
}
<single_token_state>[^\ =,\n]+/, {
    // a trailing comma indicates the single value is a part of a list so we dont pop the state
    capture_token(yylval,wasp::mcnpi::VALUE);
    return token::VALUE;
}
<single_token_state>[^\ =,\n\t\r]+ {
    yy_pop_state();
    capture_token(yylval,wasp::mcnpi::VALUE);
    return token::VALUE;
}
<*>{DOUBLE} {
    capture_token(yylval,wasp::mcnpi::REAL);
    return token::DOUBLE;
}
<*>{FORTRAN_FLOAT} {
    capture_upconverted_fortran_float_token(yylval,wasp::mcnpi::REAL);
    return token::DOUBLE;
}
<*>{INTEGER} {
    capture_token(yylval,wasp::mcnpi::INTEGER);
    return token::INTEGER;
}

<*>, {
    capture_token(yylval,wasp::mcnpi::DATA_COMMA);
    return token::COMMA;
}
<column_wise_state>{NEWLINE} {
   yylloc->lines(yyleng); yylloc->step();
   interpreter.push_line_offset(file_offset-yyleng);
   return token::NEWLINE;
}

 /* gobble up white-spaces and unsupported unicode */
<*>([ \t\r]|{UNICODE})+ {
    yylloc->step();
}

 /* gobble up end-of-lines */
{NEWLINE} {
    yylloc->lines(yyleng); yylloc->step();
    interpreter.push_line_offset(file_offset-yyleng);
}

<*>{QSTRING} {
    capture_token(yylval,wasp::mcnpi::QUOTED_STRING);
    return token::QSTRING;
}

<*>{COMMENT} {
    // First line of the file is a title
    if (interpreter.token_count() == 0)
    {
        capture_token(yylval,wasp::mcnpi::TITLE_CARD);
        yy_push_state(cell_card_block_state);
        return token::TITLE_CARD;
    }
    capture_token(yylval,wasp::mcnpi::COMMENT);
    interpreter.push_hidden_leaf(wasp::mcnpi::COMMENT,"comment", yylval->token_index);
    yylloc->step();
}
<*>{COMMENT_CARD} {
    // First line of the file is a title
    if (interpreter.token_count() == 0)
    {
        capture_token(yylval,wasp::mcnpi::TITLE_CARD);
        yy_push_state(cell_card_block_state);
        return token::TITLE_CARD;
    }
    capture_token(yylval,wasp::mcnpi::COMMENT);
    interpreter.push_hidden_leaf(wasp::mcnpi::COMMENT,"comment", yylval->token_index);
    yylloc->step();
}

URAN {
    capture_token(yylval,wasp::mcnpi::DECL);
    return token::URAN;
}

NO {
    capture_token(yylval,wasp::mcnpi::NO);
    return token::NO;
}

{ARRAY_JUMP} {
    capture_token(yylval,wasp::mcnpi::ARRAY_JUMP);
    return token::ARRAY_JUMP;
}

{ARRAY_REPEAT} {
    capture_token(yylval,wasp::mcnpi::ARRAY_REPEAT);
    return token::ARRAY_REPEAT;
}

{ARRAY_MULTIPLY} {
    capture_token(yylval,wasp::mcnpi::ARRAY_MULTIPLY);
    return token::ARRAY_MULTIPLY;
}

{ARRAY_LIN_INTERP} {
    capture_token(yylval,wasp::mcnpi::ARRAY_LIN_INTERP);
    return token::ARRAY_LIN_INTERP;
}

{ARRAY_LOG_INTERP} {
    capture_token(yylval,wasp::mcnpi::ARRAY_LOG_INTERP);
    return token::ARRAY_LOG_INTERP;
}
TR{INTEGER} {
    yyless(2);
    capture_token(yylval,wasp::mcnpi::TRANSFORMATION);
    yy_push_state(card_id_state);
    return token::TRANSFORMATION;
}
\*TR{INTEGER} {
    yyless(1);
    capture_token(yylval,wasp::mcnpi::FLAG);
    return token::ASTERISK;
}
AREA {
    capture_token(yylval,wasp::mcnpi::AREA);
    return token::AREA;
}
^\ {0,4}C/[\r\n] {
    capture_token(yylval,wasp::mcnpi::COMMENT);
    interpreter.push_hidden_leaf(wasp::mcnpi::COMMENT,"comment", yylval->token_index);
    yylloc->step();
}
{STRING} {
    capture_token(yylval,wasp::mcnpi::STRING);
    return token::STRING;
}
<single_value_state>[^\ =\n]+ {
    yy_pop_state();
    capture_token(yylval,wasp::mcnpi::VALUE);
    return token::VALUE;
}
<column_forced_comment_state>{COLUMN_FORCED_COMMENT} {
    yy_pop_state();
    capture_token(yylval,wasp::mcnpi::COMMENT);
    interpreter.push_hidden_leaf(wasp::mcnpi::COMMENT,"comment", yylval->token_index);
    yylloc->step();
}


 /* pass all other characters up to bison */
. {
    return static_cast<token_type>(*yytext);
}

 /*** END EXAMPLE - Change the wasp lexer rules above ***/

%% /*** Additional Code ***/

namespace wasp {

MCNPLexerImpl::MCNPLexerImpl(
                AbstractInterpreter & interpreter,
                std::istream* in,
                std::ostream* out)
    : MCNPFlexLexer(in, out)
    , interpreter(interpreter)
    , file_offset(0)
{
    yy_push_state(start_mcnp_state);
}

MCNPLexerImpl::~MCNPLexerImpl()
{
}

void MCNPLexerImpl::set_debug(bool b)
{
    yy_flex_debug = b;
}
void MCNPLexerImpl::rewind()
{
    yyin.seekg(-yyleng,std::ios_base::cur);
    yyless(0);
}
void MCNPLexerImpl::capture_token(
        wasp::MCNPParser::semantic_type* yylval,
        wasp::mcnpi::NODE type)
{
    std::size_t offset = file_offset - yyleng;
    yylval->token_index = interpreter.token_count();
    interpreter.push_token(yytext, type, offset);
}

void MCNPLexerImpl::capture_upconverted_fortran_float_token(
        wasp::MCNPParser::semantic_type* yylval,
        wasp::mcnpi::NODE type)
{
    std::string legacy  = std::string(yytext);
    size_t position = legacy.find_first_of("Dd");

    // if we have found a 'D', replace it with an 'E'
    if (position != std::string::npos)
    {
        legacy.replace(position, 1, "e");
    }
    // no 'D' exists, assume it is the exponentless version
    // i.e., 1.6-4
    else
    {
        position = legacy.find_last_of("-+");
        // need to find the exponent 'dash' and not be the minus/negative
        // at the beginning of the string (position=0)
        if (position != std::string::npos && position != 0)
        {
            legacy.insert(position, 1, 'e');
        }
    }

    std::size_t offset = file_offset - yyleng;
    yylval->token_index = interpreter.token_count();
    interpreter.push_token(legacy.data(), type, offset);
}
}

/* This implementation of MCNPFlexLexer::yylex() is required to fill the
 * vtable of the class MCNPFlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the MCNPLexerImpl class instead. */

#ifdef yylex
#undef yylex
#endif

int MCNPFlexLexer::yylex()
{
    std::cerr << "in MCNPFlexLexer::yylex() !" << std::endl;
    return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int MCNPFlexLexer::yywrap()
{
    return 1;
}
