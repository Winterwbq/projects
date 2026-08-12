// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton interface for Bison LALR(1) parsers in C++

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


/**
 ** \file MCNPParser.hpp
 ** Define the wasp::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

#ifndef YY_YY_MCNPPARSER_HPP_INCLUDED
# define YY_YY_MCNPPARSER_HPP_INCLUDED
// "%code requires" blocks.
#line 11 "MCNPParser.bison"

#include <cctype>
#include <memory>
#include "waspcore/utils.h"
#include "waspcore/decl.h"
#include "waspcore/wasp_bug.h"

#line 57 "MCNPParser.hpp"


# include <cstdlib> // std::abort
# include <iostream>
# include <stdexcept>
# include <string>
# include <vector>

#if defined __cplusplus
# define YY_CPLUSPLUS __cplusplus
#else
# define YY_CPLUSPLUS 199711L
#endif

// Support move semantics when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_MOVE           std::move
# define YY_MOVE_OR_COPY   move
# define YY_MOVE_REF(Type) Type&&
# define YY_RVREF(Type)    Type&&
# define YY_COPY(Type)     Type
#else
# define YY_MOVE
# define YY_MOVE_OR_COPY   copy
# define YY_MOVE_REF(Type) Type&
# define YY_RVREF(Type)    const Type&
# define YY_COPY(Type)     const Type&
#endif

// Support noexcept when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_NOEXCEPT noexcept
# define YY_NOTHROW
#else
# define YY_NOEXCEPT
# define YY_NOTHROW throw ()
#endif

// Support constexpr when possible.
#if 201703 <= YY_CPLUSPLUS
# define YY_CONSTEXPR constexpr
#else
# define YY_CONSTEXPR
#endif
# include "../waspcore/location.hh"


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

#line 35 "MCNPParser.bison"
namespace wasp {
#line 193 "MCNPParser.hpp"




  /// A Bison parser.
  class WASP_PUBLIC MCNPParser
  {
  public:
#ifdef YYSTYPE
# ifdef __GNUC__
#  pragma GCC message "bison: do not #define YYSTYPE in C++, use %define api.value.type"
# endif
    typedef YYSTYPE value_type;
#else
    /// Symbol semantic values.
    union value_type
    {
#line 67 "MCNPParser.bison"

        std::size_t token_index;
        std::size_t node_index;
        std::size_t stage_index;
        std::vector<size_t>* node_indices;
        std::vector<size_t>* token_indices;
        std::vector<std::vector<size_t>*>* table_token_indices;

#line 220 "MCNPParser.hpp"

    };
#endif
    /// Backward compatibility (Bison 3.8).
    typedef value_type semantic_type;

    /// Symbol locations.
    typedef location location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const location_type& l, const std::string& m)
        : std::runtime_error (m)
        , location (l)
      {}

      syntax_error (const syntax_error& s)
        : std::runtime_error (s.what ())
        , location (s.location)
      {}

      ~syntax_error () YY_NOEXCEPT YY_NOTHROW;

      location_type location;
    };

    /// Token kinds.
    struct token
    {
      enum token_kind_type
      {
        YYEMPTY = -2,
    END = 0,                       // "end of file"
    YYerror = 256,                 // error
    YYUNDEF = 257,                 // "invalid token"
    EOL = 258,                     // "end of line"
    COMMENT = 259,                 // "comment"
    MESSAGE_CARD = 260,            // "message card"
    BLANK_LINE = 261,              // "blank line delimiter"
    ASSIGN = 262,                  // "="
    DOUBLE = 263,                  // "double precision number"
    INTEGER = 264,                 // "integer number"
    QSTRING = 265,                 // "quoted string"
    STRING = 266,                  // "string"
    VALUE = 267,                   // "value"
    LPAREN = 268,                  // "("
    RPAREN = 269,                  // ")"
    COLON = 270,                   // ":"
    COMMA = 271,                   // ","
    ASTERISK = 272,                // "*"
    PLUS = 273,                    // "+"
    NEWLINE = 274,                 // "newline"
    PARTICLE_SYMBOL = 275,         // "particle symbol"
    DATAPATH = 276,                // "datapath"
    INP = 277,                     // "inp"
    OUTP = 278,                    // "outp"
    RUNTPE = 279,                  // "runtpe"
    WWINP = 280,                   // "wwinp"
    WWOUT = 281,                   // "wwout"
    WWONE = 282,                   // "wwone"
    PARTINP = 283,                 // "partinp"
    LINKIN = 284,                  // "linkin"
    LINKOUT = 285,                 // "linkout"
    KSENTAL = 286,                 // "format of senstitivity profile output file (KSENTAL)"
    HISTP = 287,                   // "histp"
    COM = 288,                     // "com"
    COMOUT = 289,                  // "comout"
    PLOTM = 290,                   // "plotm"
    MCTAL = 291,                   // "mctal"
    MESHTAL = 292,                 // "meshtal"
    MDATA = 293,                   // "mdata"
    NAME = 294,                    // "name"
    SRCTP = 295,                   // "srctp"
    WSSA = 296,                    // "wssa"
    RSSA = 297,                    // "rssa"
    INPUT = 298,                   // "input"
    PLOT_GEOM = 299,               // "plot_geom"
    XS = 300,                      // "crosssection"
    TRANSPORT = 301,               // "transport"
    PLOT_DATA = 302,               // "plot_data"
    PARTISN_INPUT = 303,           // "partisn_input"
    ZAID = 304,                    // "ziaaa.abx"
    FISSION = 305,                 // "fission"
    NONFISS = 306,                 // "nonfiss"
    DN = 307,                      // "delayed neutron (DN)"
    DG = 308,                      // "delayed gamma (DG)"
    THRESH = 309,                  // "discrete delayed-gamma line threshold (THRESH)"
    DNBIAS = 310,                  // "delayed neutron per interaction (DNBIAS)"
    NAP = 311,                     // "nap"
    DNEB = 312,                    // "delayed neutron energy bias (DNEB)"
    DPEB = 313,                    // "delayed gamma energy bias (DPEB)"
    PECUT = 314,                   // "delayed gamma energy cutoff (PECUT)"
    HLCUT = 315,                   // "spontaneos half-life cutoff (HLCUT)"
    SAMPLE = 316,                  // "correlated or uncorrelated flag (SAMPLE)"
    CONTINUE_RUN = 317,            // "c"
    CONTINUE_RUN_TPE = 318,        // "cn"
    DBUG = 319,                    // "dbug"
    DEVTEST = 320,                 // "devtest"
    NOTEK = 321,                   // "notek"
    FATAL = 322,                   // "fatal"
    PRINT = 323,                   // "print"
    TASKS = 324,                   // "tasks"
    BALANCE = 325,                 // "balance"
    MCNPEOL = 326,                 // "eol"
    TITLE_CARD = 327,              // "title"
    WORD = 328,                    // WORD
    CONTINUE = 329,                // "continue"
    UNION = 330,                   // "union"
    INTERSECTION = 331,            // "intersection"
    COMPLEMENT = 332,              // "#"
    DENSITY = 333,                 // "density"
    MATERIAL_ID = 334,             // "material_id"
    CELL_ID = 335,                 // "cell_id"
    IMP = 336,                     // "imp"
    VOL = 337,                     // "vol"
    PWT = 338,                     // "pwt"
    EXT = 339,                     // "ext"
    DXC = 340,                     // "dxc"
    NONU = 341,                    // "nonu"
    PD = 342,                      // "pd"
    TMP = 343,                     // "tmp"
    UNIVERSE = 344,                // "universe"
    TRCL = 345,                    // "cell transformation (TRCL)"
    LAT = 346,                     // "lat"
    FILL = 347,                    // "fill"
    ELPT = 348,                    // "elpt"
    COSY = 349,                    // "cosy"
    BFLCL = 350,                   // "bfcl"
    UNC = 351,                     // "unc"
    MAT = 352,                     // "mat"
    RHO = 353,                     // "rho"
    SURFACE_ID = 354,              // "surface_id"
    PLANE = 355,                   // "p"
    XPLANE = 356,                  // "px"
    YPLANE = 357,                  // "py"
    ZPLANE = 358,                  // "pz"
    ORIGIN_SPHERE = 359,           // "so"
    SPHERE = 360,                  // "sph"
    XSPHERE = 361,                 // "sx"
    YSPHERE = 362,                 // "sy"
    ZSPHERE = 363,                 // "sz"
    XPARCYLINDER = 364,            // "c/x"
    YPARCYLINDER = 365,            // "c/y"
    ZPARCYLINDER = 366,            // "c/z"
    XCYLINDER = 367,               // "cx"
    YCYLINDER = 368,               // "cy"
    ZCYLINDER = 369,               // "cz"
    XPARCONE = 370,                // "k/x"
    YPARCONE = 371,                // "k/y"
    ZPARCONE = 372,                // "k/z"
    XCONE = 374,                   // "kx"
    YCONE = 375,                   // "ky"
    ZCONE = 376,                   // "kz"
    SQUADRIC = 377,                // "sq"
    GQUADRIC = 378,                // "gq"
    XTORUS = 379,                  // "tx"
    YTORUS = 380,                  // "ty"
    ZTORUS = 381,                  // "tz"
    XPOINTS = 383,                 // "x"
    YPOINTS = 384,                 // "y"
    ZPOINTS = 385,                 // "z"
    BOX = 386,                     // "box"
    HEX = 387,                     // "hex"
    WEDGE = 388,                   // "wed"
    RECTANGULAR_PPIPED = 389,      // "rpp"
    RIGHT_CIRCULAR_CYLINDER = 390, // "rcc"
    RIGHT_ELLIPTICAL_CYLINDER = 391, // "rec"
    TRUNC_RIGHT_ANGLE_CONE = 392,  // "trc"
    ELLIPSOID = 393,               // "ell"
    ARBITRARY_POLYHEDRON = 394,    // "arb"
    ARRAY_JUMP = 395,              // "array jump (nJ)"
    ARRAY_REPEAT = 396,            // "array repeat (nR)"
    ARRAY_MULTIPLY = 397,          // "array multiply (xM)"
    ARRAY_LOG_INTERP = 398,        // "array logarithmic interpolation (nLOG)"
    ARRAY_LIN_INTERP = 399,        // "array linear interpoloation (nI)"
    NO = 400,                      // "no"
    AREA = 401,                    // "area"
    TRANSFORMATION = 402,          // "transformation (TRn)"
    URAN = 403,                    // "random universe (URAN)"
    MATERIAL_KEY = 404,            // "material id (Mn)"
    MATERIAL_LIB = 405,            // "(N|P|PN|E|H|A|S|T|D)LIB"
    GAS = 406,                     // GAS
    ESTEP = 407,                   // ESTEP
    HSTEP = 408,                   // HSTEP
    COND = 409,                    // COND
    REFI = 410,                    // REFI
    REFC = 411,                    // REFC
    REFS = 412,                    // REFS
    LIB_ID = 413,                  // "library id (.?[0-9]{2}[tcdmgpuyehporsa])"
    CUTOFF = 414,                  // "time energy and weight cutoff card (CUT)"
    ACTIVATION_OPTIONS = 415,      // "activation options (ACT)"
    FQ = 416,                      // "print hierarchy (FQn)"
    FC = 417,                      // "tally comment card (FCn)"
    F_TALLY = 418,                 // "base tally card (Fn)"
    F1_TALLY = 419,                // "integrated current surface tally card (F1)"
    F2_TALLY = 420,                // "flux averaged surface tally card (F2)"
    F3_TALLY = 421,                // "unsupported tally needed for internal indexing"
    F4_TALLY = 422,                // "flux averaged cell tally card (F4)"
    F5_TALLY = 423,                // "flux point or ring tally card (F5)"
    F6_TALLY = 424,                // "F6 cell tally card (F6)"
    F7_TALLY = 425,                // "fission energy deposition cell tally card (F7)"
    F8_TALLY = 426,                // "F8 tally card (F8)"
    FI_TALLY = 427,                // "image tally card (FI[CPR]n)"
    F5A_TALLY = 428,               // "ring detector tally card (F5[XYZ])"
    FA5_TALLY = 429,               // "ring detector tally card (F[XYZ]5)"
    MODE = 430,                    // "problem type (MODE)"
    LIKE = 431,                    // "like"
    BUT = 432,                     // "but"
    TALLY_TIME = 433,              // "tally time (Tn)"
    CBEG = 434,                    // "ref starting time (CBEG)"
    CFRQ = 435,                    // "frequency of cycling (CFRQ)"
    COFI = 436,                    // "dead time interval (COFI)"
    CONI = 437,                    // "alive time interval (CONI)"
    CSUB = 438,                    // "number of subdivisions ot use within alive time (CSUB)"
    CEND = 439,                    // "ref ending time (CEND)"
    KCODE = 440,                   // "criticality source card (KCODE)"
    NPS = 441,                     // "history cutoff (NPS)"
    TALNP = 442,                   // "negate printing of tallies (TALNP)"
    PRDMP = 443,                   // "print and dump cycle (PRDMP)"
    MT_CARD = 444,                 // "s(a,b) thermal neutron scattering (MTm)"
    KSRC = 445,                    // "criticality source points (KSRC)"
    TALLY_ENERGY = 446,            // "tally energy card (En)"
    TALLY_MULTIPLIER = 447,        // "tally multiplier card (FMn)"
    SEGMENT_DIVISOR = 448,         // "segment divisor card (SDn)"
    GENERAL_SOURCE = 449,          // "general source (SDEF)"
    CEL = 450,                     // CEL
    SDEF_SUR = 451,                // "surface number (SUR)"
    ERG = 452,                     // ERG
    SDEF_TME = 453,                // "time (TME)"
    SDEF_DIR = 454,                // "cosine angle between VEC and particle direction (DIR)"
    VEC = 455,                     // VEC
    SDEF_NRM = 456,                // "sign of surface normal (NRM)"
    SDEF_PAR = 457,                // "source particle type (PAR)"
    SDEF_POS = 458,                // "reference point for position sampling in vector notation (POS)"
    SDEF_RAD = 459,                // "radial distance from the position from POS or AXS (RAD)"
    EXT_CARD = 460,                // EXT_CARD
    AXS = 461,                     // AXS
    SDEF_X = 462,                  // "x-coordinate of position (X)"
    SDEF_Y = 463,                  // "y-coordinate of position (Y)"
    SDEF_Z = 464,                  // "z-coordinate of position (Z)"
    SDEF_CCC = 465,                // "cookie-cutter cell number (CCC)"
    SDEF_ARA = 466,                // "Area of surface (ARA)"
    WGT = 467,                     // WGT
    TR = 468,                      // TR
    SDEF_EFF = 469,                // "rejection efficiency critera for position sampling (EFF)"
    SDEF_DAT = 470,                // "date to use for comic-ray and background sources (DAT)"
    SDEF_LOC = 471,                // "location of cosmic particle source (LOC)"
    SDEF_BEM = 472,                // "Beam emittence parameters (BEM)"
    SDEF_BAP = 473,                // "Beam aperature parameters (BAP)"
    SDEF_PREFIXED_VAR = 476,       // SDEF_PREFIXED_VAR
    SC = 477,                      // "source comment card (SCn)"
    TOTNU = 478,                   // "total fission card (TOTNU)"
    ID = 479,                      // "integer identifier"
    ANYTHING_BLOCK = 480,          // "anything block"
    NOTRN = 481,                   // "direct-only neutron-particle point detector card (NOTRN)"
    SOURCE_INFO = 482,             // "source information for distribution for n (SIn)"
    SOURCE_PROB = 483,             // "source probability for distribution for n (SPn)"
    SOURCE_BIAS = 484,             // "source bias for distribution for n (SBn)"
    TALLY_SEGMENT = 485,           // "tally segment card (FSn)"
    TALLY_FLUCTUATION = 486,       // "tally fluctation card (TFn)"
    LOST = 487,                    // "lost particle card (LOST)"
    HSRC = 488,                    // "shannon entropy card (HSRC)"
    PHYS = 489,                    // "particle physics options card (PHYS)"
    KOPTS = 490,                   // "criticality calculation options card (KOPTS)"
    TALLY_COSINE = 491,            // "tally cosine card (Cn)"
    MGOPT = 492,                   // "multigroup adjoint transport option card (MGOPT)"
    RAND = 493,                    // "random number generation card (RAND)"
    GEN = 494,                     // "random number generator type (GEN)"
    SEED = 495,                    // "random number generator seed (SEED)"
    STRIDE = 496,                  // "number of random numbers between source particles (STRIDE)"
    HIST = 497,                    // HIST
    WWE = 499,                     // "weight-window energy or time intervals (WWE)"
    WWG = 500,                     // "weight-window generation card (WWG)"
    WWGE = 501,                    // "weight-window generation energy or time bounds (WWGE)"
    WWGT = 502,                    // "weight-window generation time bounds (WWGT)"
    WWN = 503,                     // "cell-based lower weight-window bounds (WWNi)"
    WWP = 504,                     // "weight-window parameter card (WWP)"
    WWT = 505,                     // "weight-window time intervals (WWT)"
    DBCN = 506,                    // "debug information card (DBCN)"
    LT = 507,                      // "<"
    BLOCKSIZE = 508,               // "number of cycles in every outer iteration (BLOCKSIZE=n)"
    KINETICS = 509,                // "calculate point-kinetic parameters (KINETICS=YES/NO)"
    PRECURSOR = 510,               // "calculate detailed precursor information (PRECURSOR=YES/NO)"
    FMAT = 511,                    // "turn on FMAT (FMAT=YES/NO)"
    FMATSKIP = 512,                // "FMATSKIP=n"
    FMATSPACE = 513,               // "FMATSPACE=s"
    FMATACCEL = 514,               // "FMATACCEL=YES/NO"
    FMATREDUCE = 515,              // "FMATREDUCE=YES/NO"
    FMATNX = 516,                  // FMATNX
    FMATNY = 517,                  // FMATNY
    FMATNZ = 518,                  // FMATNZ
    DOSE_ENERGY = 519,             // "dose energy card (DEn)"
    DOSE_FUNCTION = 520,           // "dose function card (DFn)"
    FCL = 521,                     // "forced-collision card (FCL)"
    VAR = 522,                     // "variance reduction control card (VAR)"
    VAR_RR = 523,                  // "russion roulette key (RR)"
    CELL_FLAGGING = 524,           // "cell-flagging card (CFn)"
    EFT = 525,                     // "exponential transform card (EFT)"
    VECT = 526,                    // "vector input card (VECT)"
    VN = 527,                      // "Vn"
    ESPLT = 528,                   // "energy splitting and roulette card (ESPLT)"
    TSPLT = 529,                   // "time splitting and roulette card (TSPLT)"
    DD = 530,                      // "detector diagnostics card (DD)"
    DXT = 531,                     // "dxtran card (DXT)"
    CM = 532,                      // "cosine multiplier card (CMn)"
    TM = 533,                      // "time multiplier card (TMn)"
    FT = 539,                      // "special treatment tallies card (FT)"
    FRV = 540,                     // FRV
    GEB = 541,                     // GEB
    TMC = 542,                     // TMC
    INC = 543,                     // INC
    ICD = 544,                     // ICD
    SCX = 545,                     // SCX
    SCD = 546,                     // SCD
    ELC = 547,                     // ELC
    PTT = 548,                     // PTT
    PHL = 549,                     // PHL
    CAP = 550,                     // CAP
    RES = 551,                     // RES
    TAG = 552,                     // TAG
    LET = 553,                     // LET
    ROC = 554,                     // ROC
    PDS = 555,                     // PDS
    FFT = 556,                     // FFT
    DS = 557,                      // "dependent source distribution card (DS)"
    FU = 558,                      // "tallyx card (FU)"
    EM = 559,                      // "energy multiplier card (EM)"
    CORA = 560,                    // CORA
    CORB = 561,                    // CORB
    CORC = 562,                    // CORC
    SF = 563,                      // "surface-flagging card (SF)"
    BBREM = 564,                   // "bremsstrahlung biasing card (BBREM)"
    TMESH = 565,                   // "superimposed mesh tally card (TMESH)"
    ENDMD = 566,                   // "mesh tally terminator (ENDMD)"
    TRAKS = 567,                   // TRAKS
    FLUX = 568,                    // FLUX
    DOSE = 569,                    // DOSE
    POPUL = 570,                   // POPUL
    PEDEP = 571,                   // PEDEP
    MFACT = 572,                   // MFACT
    TRANS = 573,                   // TRANS
    TOTAL = 574,                   // TOTAL
    DEDX = 575,                    // DEDX
    RECOL = 576,                   // RECOL
    TLEST = 577,                   // TLEST
    EDLCT = 578,                   // EDLCT
    CMESH = 579,                   // "cylindrical mesh (CMESH)"
    RMESH = 580,                   // "rectilinear mesh (RMESH)"
    SMESH = 581,                   // "spherical mesh (SMESH)"
    FMESH = 583,                   // "superimposed fmesh mesh tally card (FMESH)"
    GEOM = 584,                    // GEOM
    ORIGIN = 585,                  // ORIGIN
    IMESH = 586,                   // IMESH
    IINTS = 587,                   // IINTS
    JMESH = 588,                   // JMESH
    JINTS = 589,                   // JINTS
    KMESH = 590,                   // KMESH
    KINTS = 591,                   // KINTS
    EMESH = 592,                   // EMESH
    EINTS = 593,                   // EINTS
    ENORM = 594,                   // ENORM
    FMESH_TMESH = 595,             // FMESH_TMESH
    TINTS = 596,                   // TINTS
    TNORM = 597,                   // TNORM
    FACTOR = 598,                  // FACTOR
    MCNP_OUT = 599,                // MCNP_OUT
    TYPE = 600,                    // TYPE
    KCLEAR = 601,                  // KCLEAR
    MESH = 602,                    // "superimposed mesh variable card (MESH)"
    REF = 603,                     // REF
    THTME = 604,                   // "thermal times card (THTME)"
    MX = 605,                      // "material nuclide substitution card (MX)"
    MPHYS = 606,                   // "model physics control card (MPHYS)"
    TROPT = 607,                   // "transport options card (TROPT)"
    MCSCAT = 608,                  // MCSCAT
    ELOSS = 609,                   // ELOSS
    NREACT = 610,                  // NREACT
    NESCAT = 611,                  // NESCAT
    GENXS = 612,                   // GENXS
    PERT = 613,                    // "tally perturbation card (PERT)"
    METHOD = 614,                  // METHOD
    RXN = 615,                     // RXN
    LCA = 616,                     // LCA
    LCB = 617,                     // LCB
    LCC = 618,                     // LCC
    LEA = 619,                     // LEA
    LEB = 620,                     // LEB
    PIKMT = 621,                   // "photon production bias card (PIKMT)"
    LBRACKET = 622,                // "["
    RBRACKET = 623,                // "]"
    EMBED_CARD = 624,              // EMBED_CARD
    BACKGROUND = 625,              // BACKGROUND
    MATCELL = 626,                 // MATCELL
    MESHGEO = 627,                 // MESHGEO
    MGEOIN = 628,                  // MGEOIN
    MEEOUT = 629,                  // MEEOUT
    MEEIN = 630,                   // MEEIN
    CALC_VOLS = 631,               // CALC_VOLS
    DEBUG = 632,                   // DEBUG
    FILETYPE = 633,                // FILETYPE
    GMVFILE = 634,                 // GMVFILE
    LENGTH = 635,                  // LENGTH
    MCNPUMFILE = 636,              // MCNPUMFILE
    OVERLAP = 637,                 // OVERLAP
    EMBEE = 638,                   // EMBEE
    EMBED = 639,                   // EMBED
    ENERGY = 640,                  // ENERGY
    TIME = 641,                    // TIME
    ATOM = 642,                    // ATOM
    COMMENT_FLAG = 643,            // COMMENT_FLAG
    LIST = 644,                    // LIST
    MTYPE = 645,                   // MTYPE
    ERRORS = 646,                  // ERRORS
    EMBEB = 647,                   // EMBEB
    EMBEM = 648,                   // EMBEM
    EMBTB = 649,                   // EMBTB
    EMBTM = 650,                   // EMBTM
    EMBDE = 651,                   // EMBDE
    EMBDF = 652,                   // EMBDF
    MCNP_VOID = 653,               // MCNP_VOID
    DRXS = 654,                    // DRXS
    CTME = 655,                    // CTME
    RDUM = 656,                    // RDUM
    IDUM = 657,                    // IDUM
    SSW = 658,                     // SSW
    SYM = 659,                     // SYM
    PTY = 660,                     // PTY
    SSR = 661,                     // SSR
    OLD = 662,                     // OLD
    NEW = 663,                     // NEW
    PSC = 664,                     // PSC
    POA = 665,                     // POA
    COL = 666,                     // COL
    BCW = 667,                     // BCW
    COMMENT_KEY = 668,             // COMMENT_KEY
    FMULT = 669,                   // "multiplicity constant card (FMULT)"
    SFNU = 670,                    // SFNU
    WIDTH = 671,                   // WIDTH
    SYFIELD = 672,                 // SYFIELD
    WATT = 673,                    // WATT
    DATA = 674,                    // DATA
    SHIFT = 675,                   // SHIFT
    KPERT = 676,                   // "reactivity perturbation card (KPERT)"
    LINEAR = 677,                  // LINEAR
    ISO = 678,                     // ISO
    KSEN = 679,                    // "keff sensitivity coefficients card (KSEN)"
    EIN = 680,                     // EIN
    LEGENDRE = 681,                // LEGENDRE
    COS = 682,                     // COS
    CONSTRAIN = 683,               // CONSTRAIN
    MT = 684,                      // MT
    PTRAC = 685,                   // "particle track output (PTRAC)"
    BUFFER = 686,                  // BUFFER
    FILE = 687,                    // FILE
    MAX = 688,                     // MAX
    MEPH = 689,                    // MEPH
    WRITE = 690,                   // WRITE
    COINC = 691,                   // COINC
    EVENT = 692,                   // EVENT
    FILTER = 693,                  // FILTER
    SURFACE = 694,                 // SURFACE
    TALLY = 695,                   // TALLY
    VALUE_KEY = 696,               // VALUE_KEY
    COLUMN_WISE_FLAG = 697,        // COLUMN_WISE_FLAG
    BFLD = 698,                    // "particle ray tracing (BFLD)"
    FIELD = 699,                   // FIELD
    MXDEFLC = 700,                 // MXDEFLC
    MAXSTEP = 701,                 // MAXSTEP
    FFEDGES = 702,                 // FFEDGES
    REFPNT = 703,                  // REFPNT
    AWTAB = 704,                   // "atomic weight card (AWTAB)"
    COSYP = 705                    // "transfer map card (COSPY))"
      };
      /// Backward compatibility alias (Bison 3.6).
      typedef token_kind_type yytokentype;
    };

    /// Token kind, as returned by yylex.
    typedef token::token_kind_type token_kind_type;

    /// Backward compatibility alias (Bison 3.6).
    typedef token_kind_type token_type;

    /// Symbol kinds.
    struct symbol_kind
    {
      enum symbol_kind_type
      {
        YYNTOKENS = 451, ///< Number of tokens.
        S_YYEMPTY = -2,
        S_YYEOF = 0,                             // "end of file"
        S_YYerror = 1,                           // error
        S_YYUNDEF = 2,                           // "invalid token"
        S_EOL = 3,                               // "end of line"
        S_COMMENT = 4,                           // "comment"
        S_MESSAGE_CARD = 5,                      // "message card"
        S_BLANK_LINE = 6,                        // "blank line delimiter"
        S_ASSIGN = 7,                            // "="
        S_DOUBLE = 8,                            // "double precision number"
        S_INTEGER = 9,                           // "integer number"
        S_QSTRING = 10,                          // "quoted string"
        S_STRING = 11,                           // "string"
        S_VALUE = 12,                            // "value"
        S_LPAREN = 13,                           // "("
        S_RPAREN = 14,                           // ")"
        S_COLON = 15,                            // ":"
        S_COMMA = 16,                            // ","
        S_ASTERISK = 17,                         // "*"
        S_PLUS = 18,                             // "+"
        S_NEWLINE = 19,                          // "newline"
        S_PARTICLE_SYMBOL = 20,                  // "particle symbol"
        S_DATAPATH = 21,                         // "datapath"
        S_INP = 22,                              // "inp"
        S_OUTP = 23,                             // "outp"
        S_RUNTPE = 24,                           // "runtpe"
        S_WWINP = 25,                            // "wwinp"
        S_WWOUT = 26,                            // "wwout"
        S_WWONE = 27,                            // "wwone"
        S_PARTINP = 28,                          // "partinp"
        S_LINKIN = 29,                           // "linkin"
        S_LINKOUT = 30,                          // "linkout"
        S_KSENTAL = 31,                          // "format of senstitivity profile output file (KSENTAL)"
        S_HISTP = 32,                            // "histp"
        S_COM = 33,                              // "com"
        S_COMOUT = 34,                           // "comout"
        S_PLOTM = 35,                            // "plotm"
        S_MCTAL = 36,                            // "mctal"
        S_MESHTAL = 37,                          // "meshtal"
        S_MDATA = 38,                            // "mdata"
        S_NAME = 39,                             // "name"
        S_SRCTP = 40,                            // "srctp"
        S_WSSA = 41,                             // "wssa"
        S_RSSA = 42,                             // "rssa"
        S_INPUT = 43,                            // "input"
        S_PLOT_GEOM = 44,                        // "plot_geom"
        S_XS = 45,                               // "crosssection"
        S_TRANSPORT = 46,                        // "transport"
        S_PLOT_DATA = 47,                        // "plot_data"
        S_PARTISN_INPUT = 48,                    // "partisn_input"
        S_ZAID = 49,                             // "ziaaa.abx"
        S_FISSION = 50,                          // "fission"
        S_NONFISS = 51,                          // "nonfiss"
        S_DN = 52,                               // "delayed neutron (DN)"
        S_DG = 53,                               // "delayed gamma (DG)"
        S_THRESH = 54,                           // "discrete delayed-gamma line threshold (THRESH)"
        S_DNBIAS = 55,                           // "delayed neutron per interaction (DNBIAS)"
        S_NAP = 56,                              // "nap"
        S_DNEB = 57,                             // "delayed neutron energy bias (DNEB)"
        S_DPEB = 58,                             // "delayed gamma energy bias (DPEB)"
        S_PECUT = 59,                            // "delayed gamma energy cutoff (PECUT)"
        S_HLCUT = 60,                            // "spontaneos half-life cutoff (HLCUT)"
        S_SAMPLE = 61,                           // "correlated or uncorrelated flag (SAMPLE)"
        S_CONTINUE_RUN = 62,                     // "c"
        S_CONTINUE_RUN_TPE = 63,                 // "cn"
        S_DBUG = 64,                             // "dbug"
        S_DEVTEST = 65,                          // "devtest"
        S_NOTEK = 66,                            // "notek"
        S_FATAL = 67,                            // "fatal"
        S_PRINT = 68,                            // "print"
        S_TASKS = 69,                            // "tasks"
        S_BALANCE = 70,                          // "balance"
        S_MCNPEOL = 71,                          // "eol"
        S_TITLE_CARD = 72,                       // "title"
        S_WORD = 73,                             // WORD
        S_CONTINUE = 74,                         // "continue"
        S_UNION = 75,                            // "union"
        S_INTERSECTION = 76,                     // "intersection"
        S_COMPLEMENT = 77,                       // "#"
        S_DENSITY = 78,                          // "density"
        S_MATERIAL_ID = 79,                      // "material_id"
        S_CELL_ID = 80,                          // "cell_id"
        S_IMP = 81,                              // "imp"
        S_VOL = 82,                              // "vol"
        S_PWT = 83,                              // "pwt"
        S_EXT = 84,                              // "ext"
        S_DXC = 85,                              // "dxc"
        S_NONU = 86,                             // "nonu"
        S_PD = 87,                               // "pd"
        S_TMP = 88,                              // "tmp"
        S_UNIVERSE = 89,                         // "universe"
        S_TRCL = 90,                             // "cell transformation (TRCL)"
        S_LAT = 91,                              // "lat"
        S_FILL = 92,                             // "fill"
        S_ELPT = 93,                             // "elpt"
        S_COSY = 94,                             // "cosy"
        S_BFLCL = 95,                            // "bfcl"
        S_UNC = 96,                              // "unc"
        S_MAT = 97,                              // "mat"
        S_RHO = 98,                              // "rho"
        S_SURFACE_ID = 99,                       // "surface_id"
        S_PLANE = 100,                           // "p"
        S_XPLANE = 101,                          // "px"
        S_YPLANE = 102,                          // "py"
        S_ZPLANE = 103,                          // "pz"
        S_ORIGIN_SPHERE = 104,                   // "so"
        S_SPHERE = 105,                          // "sph"
        S_XSPHERE = 106,                         // "sx"
        S_YSPHERE = 107,                         // "sy"
        S_ZSPHERE = 108,                         // "sz"
        S_XPARCYLINDER = 109,                    // "c/x"
        S_YPARCYLINDER = 110,                    // "c/y"
        S_ZPARCYLINDER = 111,                    // "c/z"
        S_XCYLINDER = 112,                       // "cx"
        S_YCYLINDER = 113,                       // "cy"
        S_ZCYLINDER = 114,                       // "cz"
        S_XPARCONE = 115,                        // "k/x"
        S_YPARCONE = 116,                        // "k/y"
        S_ZPARCONE = 117,                        // "k/z"
        S_118_x_y_or_z_cone_K_X_K_Y_K_Z_ = 118,  // "x, y, or z cone (K/X K/Y K/Z)"
        S_XCONE = 119,                           // "kx"
        S_YCONE = 120,                           // "ky"
        S_ZCONE = 121,                           // "kz"
        S_SQUADRIC = 122,                        // "sq"
        S_GQUADRIC = 123,                        // "gq"
        S_XTORUS = 124,                          // "tx"
        S_YTORUS = 125,                          // "ty"
        S_ZTORUS = 126,                          // "tz"
        S_127_x_y_or_z_torus_TX_TY_TZ_ = 127,    // "x, y, or z torus (TX TY TZ)"
        S_XPOINTS = 128,                         // "x"
        S_YPOINTS = 129,                         // "y"
        S_ZPOINTS = 130,                         // "z"
        S_BOX = 131,                             // "box"
        S_HEX = 132,                             // "hex"
        S_WEDGE = 133,                           // "wed"
        S_RECTANGULAR_PPIPED = 134,              // "rpp"
        S_RIGHT_CIRCULAR_CYLINDER = 135,         // "rcc"
        S_RIGHT_ELLIPTICAL_CYLINDER = 136,       // "rec"
        S_TRUNC_RIGHT_ANGLE_CONE = 137,          // "trc"
        S_ELLIPSOID = 138,                       // "ell"
        S_ARBITRARY_POLYHEDRON = 139,            // "arb"
        S_ARRAY_JUMP = 140,                      // "array jump (nJ)"
        S_ARRAY_REPEAT = 141,                    // "array repeat (nR)"
        S_ARRAY_MULTIPLY = 142,                  // "array multiply (xM)"
        S_ARRAY_LOG_INTERP = 143,                // "array logarithmic interpolation (nLOG)"
        S_ARRAY_LIN_INTERP = 144,                // "array linear interpoloation (nI)"
        S_NO = 145,                              // "no"
        S_AREA = 146,                            // "area"
        S_TRANSFORMATION = 147,                  // "transformation (TRn)"
        S_URAN = 148,                            // "random universe (URAN)"
        S_MATERIAL_KEY = 149,                    // "material id (Mn)"
        S_MATERIAL_LIB = 150,                    // "(N|P|PN|E|H|A|S|T|D)LIB"
        S_GAS = 151,                             // GAS
        S_ESTEP = 152,                           // ESTEP
        S_HSTEP = 153,                           // HSTEP
        S_COND = 154,                            // COND
        S_REFI = 155,                            // REFI
        S_REFC = 156,                            // REFC
        S_REFS = 157,                            // REFS
        S_LIB_ID = 158,                          // "library id (.?[0-9]{2}[tcdmgpuyehporsa])"
        S_CUTOFF = 159,                          // "time energy and weight cutoff card (CUT)"
        S_ACTIVATION_OPTIONS = 160,              // "activation options (ACT)"
        S_FQ = 161,                              // "print hierarchy (FQn)"
        S_FC = 162,                              // "tally comment card (FCn)"
        S_F_TALLY = 163,                         // "base tally card (Fn)"
        S_F1_TALLY = 164,                        // "integrated current surface tally card (F1)"
        S_F2_TALLY = 165,                        // "flux averaged surface tally card (F2)"
        S_F3_TALLY = 166,                        // "unsupported tally needed for internal indexing"
        S_F4_TALLY = 167,                        // "flux averaged cell tally card (F4)"
        S_F5_TALLY = 168,                        // "flux point or ring tally card (F5)"
        S_F6_TALLY = 169,                        // "F6 cell tally card (F6)"
        S_F7_TALLY = 170,                        // "fission energy deposition cell tally card (F7)"
        S_F8_TALLY = 171,                        // "F8 tally card (F8)"
        S_FI_TALLY = 172,                        // "image tally card (FI[CPR]n)"
        S_F5A_TALLY = 173,                       // "ring detector tally card (F5[XYZ])"
        S_FA5_TALLY = 174,                       // "ring detector tally card (F[XYZ]5)"
        S_MODE = 175,                            // "problem type (MODE)"
        S_LIKE = 176,                            // "like"
        S_BUT = 177,                             // "but"
        S_TALLY_TIME = 178,                      // "tally time (Tn)"
        S_CBEG = 179,                            // "ref starting time (CBEG)"
        S_CFRQ = 180,                            // "frequency of cycling (CFRQ)"
        S_COFI = 181,                            // "dead time interval (COFI)"
        S_CONI = 182,                            // "alive time interval (CONI)"
        S_CSUB = 183,                            // "number of subdivisions ot use within alive time (CSUB)"
        S_CEND = 184,                            // "ref ending time (CEND)"
        S_KCODE = 185,                           // "criticality source card (KCODE)"
        S_NPS = 186,                             // "history cutoff (NPS)"
        S_TALNP = 187,                           // "negate printing of tallies (TALNP)"
        S_PRDMP = 188,                           // "print and dump cycle (PRDMP)"
        S_MT_CARD = 189,                         // "s(a,b) thermal neutron scattering (MTm)"
        S_KSRC = 190,                            // "criticality source points (KSRC)"
        S_TALLY_ENERGY = 191,                    // "tally energy card (En)"
        S_TALLY_MULTIPLIER = 192,                // "tally multiplier card (FMn)"
        S_SEGMENT_DIVISOR = 193,                 // "segment divisor card (SDn)"
        S_GENERAL_SOURCE = 194,                  // "general source (SDEF)"
        S_CEL = 195,                             // CEL
        S_SDEF_SUR = 196,                        // "surface number (SUR)"
        S_ERG = 197,                             // ERG
        S_SDEF_TME = 198,                        // "time (TME)"
        S_SDEF_DIR = 199,                        // "cosine angle between VEC and particle direction (DIR)"
        S_VEC = 200,                             // VEC
        S_SDEF_NRM = 201,                        // "sign of surface normal (NRM)"
        S_SDEF_PAR = 202,                        // "source particle type (PAR)"
        S_SDEF_POS = 203,                        // "reference point for position sampling in vector notation (POS)"
        S_SDEF_RAD = 204,                        // "radial distance from the position from POS or AXS (RAD)"
        S_EXT_CARD = 205,                        // EXT_CARD
        S_AXS = 206,                             // AXS
        S_SDEF_X = 207,                          // "x-coordinate of position (X)"
        S_SDEF_Y = 208,                          // "y-coordinate of position (Y)"
        S_SDEF_Z = 209,                          // "z-coordinate of position (Z)"
        S_SDEF_CCC = 210,                        // "cookie-cutter cell number (CCC)"
        S_SDEF_ARA = 211,                        // "Area of surface (ARA)"
        S_WGT = 212,                             // WGT
        S_TR = 213,                              // TR
        S_SDEF_EFF = 214,                        // "rejection efficiency critera for position sampling (EFF)"
        S_SDEF_DAT = 215,                        // "date to use for comic-ray and background sources (DAT)"
        S_SDEF_LOC = 216,                        // "location of cosmic particle source (LOC)"
        S_SDEF_BEM = 217,                        // "Beam emittence parameters (BEM)"
        S_SDEF_BAP = 218,                        // "Beam aperature parameters (BAP)"
        S_219_CEL_SUR_ERG_TME_DIR_NRM_RAD_EXT_CCC_ARA_WGT_TR_EFF_ = 219, // "CEL | SUR | ERG | TME | DIR | NRM | RAD | EXT | CCC | ARA | WGT | TR | EFF"
        S_220_DAT_LOC_BEM_BAP_POS_PAR_VEC_AXS_ = 220, // "DAT | LOC | BEM | BAP | POS | PAR | VEC | AXS"
        S_SDEF_PREFIXED_VAR = 221,               // SDEF_PREFIXED_VAR
        S_SC = 222,                              // "source comment card (SCn)"
        S_TOTNU = 223,                           // "total fission card (TOTNU)"
        S_ID = 224,                              // "integer identifier"
        S_ANYTHING_BLOCK = 225,                  // "anything block"
        S_NOTRN = 226,                           // "direct-only neutron-particle point detector card (NOTRN)"
        S_SOURCE_INFO = 227,                     // "source information for distribution for n (SIn)"
        S_SOURCE_PROB = 228,                     // "source probability for distribution for n (SPn)"
        S_SOURCE_BIAS = 229,                     // "source bias for distribution for n (SBn)"
        S_TALLY_SEGMENT = 230,                   // "tally segment card (FSn)"
        S_TALLY_FLUCTUATION = 231,               // "tally fluctation card (TFn)"
        S_LOST = 232,                            // "lost particle card (LOST)"
        S_HSRC = 233,                            // "shannon entropy card (HSRC)"
        S_PHYS = 234,                            // "particle physics options card (PHYS)"
        S_KOPTS = 235,                           // "criticality calculation options card (KOPTS)"
        S_TALLY_COSINE = 236,                    // "tally cosine card (Cn)"
        S_MGOPT = 237,                           // "multigroup adjoint transport option card (MGOPT)"
        S_RAND = 238,                            // "random number generation card (RAND)"
        S_GEN = 239,                             // "random number generator type (GEN)"
        S_SEED = 240,                            // "random number generator seed (SEED)"
        S_STRIDE = 241,                          // "number of random numbers between source particles (STRIDE)"
        S_HIST = 242,                            // HIST
        S_243_GEN_or_SEED_or_STRIDE_or_HIST_ = 243, // "GEN or SEED or STRIDE or HIST"
        S_WWE = 244,                             // "weight-window energy or time intervals (WWE)"
        S_WWG = 245,                             // "weight-window generation card (WWG)"
        S_WWGE = 246,                            // "weight-window generation energy or time bounds (WWGE)"
        S_WWGT = 247,                            // "weight-window generation time bounds (WWGT)"
        S_WWN = 248,                             // "cell-based lower weight-window bounds (WWNi)"
        S_WWP = 249,                             // "weight-window parameter card (WWP)"
        S_WWT = 250,                             // "weight-window time intervals (WWT)"
        S_DBCN = 251,                            // "debug information card (DBCN)"
        S_LT = 252,                              // "<"
        S_BLOCKSIZE = 253,                       // "number of cycles in every outer iteration (BLOCKSIZE=n)"
        S_KINETICS = 254,                        // "calculate point-kinetic parameters (KINETICS=YES/NO)"
        S_PRECURSOR = 255,                       // "calculate detailed precursor information (PRECURSOR=YES/NO)"
        S_FMAT = 256,                            // "turn on FMAT (FMAT=YES/NO)"
        S_FMATSKIP = 257,                        // "FMATSKIP=n"
        S_FMATSPACE = 258,                       // "FMATSPACE=s"
        S_FMATACCEL = 259,                       // "FMATACCEL=YES/NO"
        S_FMATREDUCE = 260,                      // "FMATREDUCE=YES/NO"
        S_FMATNX = 261,                          // FMATNX
        S_FMATNY = 262,                          // FMATNY
        S_FMATNZ = 263,                          // FMATNZ
        S_DOSE_ENERGY = 264,                     // "dose energy card (DEn)"
        S_DOSE_FUNCTION = 265,                   // "dose function card (DFn)"
        S_FCL = 266,                             // "forced-collision card (FCL)"
        S_VAR = 267,                             // "variance reduction control card (VAR)"
        S_VAR_RR = 268,                          // "russion roulette key (RR)"
        S_CELL_FLAGGING = 269,                   // "cell-flagging card (CFn)"
        S_EFT = 270,                             // "exponential transform card (EFT)"
        S_VECT = 271,                            // "vector input card (VECT)"
        S_VN = 272,                              // "Vn"
        S_ESPLT = 273,                           // "energy splitting and roulette card (ESPLT)"
        S_TSPLT = 274,                           // "time splitting and roulette card (TSPLT)"
        S_DD = 275,                              // "detector diagnostics card (DD)"
        S_DXT = 276,                             // "dxtran card (DXT)"
        S_CM = 277,                              // "cosine multiplier card (CMn)"
        S_TM = 278,                              // "time multiplier card (TMn)"
        S_279_string_or_number_ = 279,           // "string or number"
        S_280_double_or_integer_ = 280,          // "double or integer"
        S_281_FISSION_NONFISS_DN_DG_THRESH_DNBIAS_NAP_DNEB_DPEB_PECUT_HLCUT_SAMPLE_ = 281, // "FISSION | NONFISS | DN | DG | THRESH | DNBIAS | NAP | DNEB | DPEB | PECUT | HLCUT | SAMPLE"
        S_282_F1_F2_F4_F7_ = 282,                // "F1 | F2 | F4 | F7"
        S_283_CBEG_CFRQ_COFI_CONI_CSUB_CEND_ = 283, // "CBEG | CFRQ | COFI | CONI | CSUB | CEND"
        S_FT = 284,                              // "special treatment tallies card (FT)"
        S_FRV = 285,                             // FRV
        S_GEB = 286,                             // GEB
        S_TMC = 287,                             // TMC
        S_INC = 288,                             // INC
        S_ICD = 289,                             // ICD
        S_SCX = 290,                             // SCX
        S_SCD = 291,                             // SCD
        S_ELC = 292,                             // ELC
        S_PTT = 293,                             // PTT
        S_PHL = 294,                             // PHL
        S_CAP = 295,                             // CAP
        S_RES = 296,                             // RES
        S_TAG = 297,                             // TAG
        S_LET = 298,                             // LET
        S_ROC = 299,                             // ROC
        S_PDS = 300,                             // PDS
        S_FFT = 301,                             // FFT
        S_DS = 302,                              // "dependent source distribution card (DS)"
        S_FU = 303,                              // "tallyx card (FU)"
        S_EM = 304,                              // "energy multiplier card (EM)"
        S_CORA = 305,                            // CORA
        S_CORB = 306,                            // CORB
        S_CORC = 307,                            // CORC
        S_SF = 308,                              // "surface-flagging card (SF)"
        S_BBREM = 309,                           // "bremsstrahlung biasing card (BBREM)"
        S_TMESH = 310,                           // "superimposed mesh tally card (TMESH)"
        S_ENDMD = 311,                           // "mesh tally terminator (ENDMD)"
        S_TRAKS = 312,                           // TRAKS
        S_FLUX = 313,                            // FLUX
        S_DOSE = 314,                            // DOSE
        S_POPUL = 315,                           // POPUL
        S_PEDEP = 316,                           // PEDEP
        S_MFACT = 317,                           // MFACT
        S_TRANS = 318,                           // TRANS
        S_TOTAL = 319,                           // TOTAL
        S_DEDX = 320,                            // DEDX
        S_RECOL = 321,                           // RECOL
        S_TLEST = 322,                           // TLEST
        S_EDLCT = 323,                           // EDLCT
        S_CMESH = 324,                           // "cylindrical mesh (CMESH)"
        S_RMESH = 325,                           // "rectilinear mesh (RMESH)"
        S_SMESH = 326,                           // "spherical mesh (SMESH)"
        S_327_R_C_SMESH_ = 327,                  // "R/C/SMESH"
        S_FMESH = 328,                           // "superimposed fmesh mesh tally card (FMESH)"
        S_GEOM = 329,                            // GEOM
        S_ORIGIN = 330,                          // ORIGIN
        S_IMESH = 331,                           // IMESH
        S_IINTS = 332,                           // IINTS
        S_JMESH = 333,                           // JMESH
        S_JINTS = 334,                           // JINTS
        S_KMESH = 335,                           // KMESH
        S_KINTS = 336,                           // KINTS
        S_EMESH = 337,                           // EMESH
        S_EINTS = 338,                           // EINTS
        S_ENORM = 339,                           // ENORM
        S_FMESH_TMESH = 340,                     // FMESH_TMESH
        S_TINTS = 341,                           // TINTS
        S_TNORM = 342,                           // TNORM
        S_FACTOR = 343,                          // FACTOR
        S_MCNP_OUT = 344,                        // MCNP_OUT
        S_TYPE = 345,                            // TYPE
        S_KCLEAR = 346,                          // KCLEAR
        S_MESH = 347,                            // "superimposed mesh variable card (MESH)"
        S_REF = 348,                             // REF
        S_THTME = 349,                           // "thermal times card (THTME)"
        S_MX = 350,                              // "material nuclide substitution card (MX)"
        S_MPHYS = 351,                           // "model physics control card (MPHYS)"
        S_TROPT = 352,                           // "transport options card (TROPT)"
        S_MCSCAT = 353,                          // MCSCAT
        S_ELOSS = 354,                           // ELOSS
        S_NREACT = 355,                          // NREACT
        S_NESCAT = 356,                          // NESCAT
        S_GENXS = 357,                           // GENXS
        S_PERT = 358,                            // "tally perturbation card (PERT)"
        S_METHOD = 359,                          // METHOD
        S_RXN = 360,                             // RXN
        S_LCA = 361,                             // LCA
        S_LCB = 362,                             // LCB
        S_LCC = 363,                             // LCC
        S_LEA = 364,                             // LEA
        S_LEB = 365,                             // LEB
        S_PIKMT = 366,                           // "photon production bias card (PIKMT)"
        S_LBRACKET = 367,                        // "["
        S_RBRACKET = 368,                        // "]"
        S_EMBED_CARD = 369,                      // EMBED_CARD
        S_BACKGROUND = 370,                      // BACKGROUND
        S_MATCELL = 371,                         // MATCELL
        S_MESHGEO = 372,                         // MESHGEO
        S_MGEOIN = 373,                          // MGEOIN
        S_MEEOUT = 374,                          // MEEOUT
        S_MEEIN = 375,                           // MEEIN
        S_CALC_VOLS = 376,                       // CALC_VOLS
        S_DEBUG = 377,                           // DEBUG
        S_FILETYPE = 378,                        // FILETYPE
        S_GMVFILE = 379,                         // GMVFILE
        S_LENGTH = 380,                          // LENGTH
        S_MCNPUMFILE = 381,                      // MCNPUMFILE
        S_OVERLAP = 382,                         // OVERLAP
        S_EMBEE = 383,                           // EMBEE
        S_EMBED = 384,                           // EMBED
        S_ENERGY = 385,                          // ENERGY
        S_TIME = 386,                            // TIME
        S_ATOM = 387,                            // ATOM
        S_COMMENT_FLAG = 388,                    // COMMENT_FLAG
        S_LIST = 389,                            // LIST
        S_MTYPE = 390,                           // MTYPE
        S_ERRORS = 391,                          // ERRORS
        S_EMBEB = 392,                           // EMBEB
        S_EMBEM = 393,                           // EMBEM
        S_EMBTB = 394,                           // EMBTB
        S_EMBTM = 395,                           // EMBTM
        S_EMBDE = 396,                           // EMBDE
        S_EMBDF = 397,                           // EMBDF
        S_MCNP_VOID = 398,                       // MCNP_VOID
        S_DRXS = 399,                            // DRXS
        S_CTME = 400,                            // CTME
        S_RDUM = 401,                            // RDUM
        S_IDUM = 402,                            // IDUM
        S_SSW = 403,                             // SSW
        S_SYM = 404,                             // SYM
        S_PTY = 405,                             // PTY
        S_SSR = 406,                             // SSR
        S_OLD = 407,                             // OLD
        S_NEW = 408,                             // NEW
        S_PSC = 409,                             // PSC
        S_POA = 410,                             // POA
        S_COL = 411,                             // COL
        S_BCW = 412,                             // BCW
        S_COMMENT_KEY = 413,                     // COMMENT_KEY
        S_FMULT = 414,                           // "multiplicity constant card (FMULT)"
        S_SFNU = 415,                            // SFNU
        S_WIDTH = 416,                           // WIDTH
        S_SYFIELD = 417,                         // SYFIELD
        S_WATT = 418,                            // WATT
        S_DATA = 419,                            // DATA
        S_SHIFT = 420,                           // SHIFT
        S_KPERT = 421,                           // "reactivity perturbation card (KPERT)"
        S_LINEAR = 422,                          // LINEAR
        S_ISO = 423,                             // ISO
        S_KSEN = 424,                            // "keff sensitivity coefficients card (KSEN)"
        S_EIN = 425,                             // EIN
        S_LEGENDRE = 426,                        // LEGENDRE
        S_COS = 427,                             // COS
        S_CONSTRAIN = 428,                       // CONSTRAIN
        S_MT = 429,                              // MT
        S_PTRAC = 430,                           // "particle track output (PTRAC)"
        S_BUFFER = 431,                          // BUFFER
        S_FILE = 432,                            // FILE
        S_MAX = 433,                             // MAX
        S_MEPH = 434,                            // MEPH
        S_WRITE = 435,                           // WRITE
        S_COINC = 436,                           // COINC
        S_EVENT = 437,                           // EVENT
        S_FILTER = 438,                          // FILTER
        S_SURFACE = 439,                         // SURFACE
        S_TALLY = 440,                           // TALLY
        S_VALUE_KEY = 441,                       // VALUE_KEY
        S_COLUMN_WISE_FLAG = 442,                // COLUMN_WISE_FLAG
        S_BFLD = 443,                            // "particle ray tracing (BFLD)"
        S_FIELD = 444,                           // FIELD
        S_MXDEFLC = 445,                         // MXDEFLC
        S_MAXSTEP = 446,                         // MAXSTEP
        S_FFEDGES = 447,                         // FFEDGES
        S_REFPNT = 448,                          // REFPNT
        S_AWTAB = 449,                           // "atomic weight card (AWTAB)"
        S_COSYP = 450,                           // "transfer map card (COSPY))"
        S_YYACCEPT = 451,                        // $accept
        S_comment = 452,                         // comment
        S_comment_list = 453,                    // comment_list
        S_assign = 454,                          // assign
        S_PRIMITIVE = 455,                       // PRIMITIVE
        S_NUMBER = 456,                          // NUMBER
        S_int_value = 457,                       // int_value
        S_value = 458,                           // value
        S_number = 459,                          // number
        S_id = 460,                              // id
        S_card_id = 461,                         // card_id
        S_message_card = 462,                    // message_card
        S_execution_filename_key = 463,          // execution_filename_key
        S_filename_value = 464,                  // filename_value
        S_other_option_optional_int = 465,       // other_option_optional_int
        S_execution_other_option = 466,          // execution_other_option
        S_execution_other_options = 467,         // execution_other_options
        S_execution_filenames = 468,             // execution_filenames
        S_message_block = 469,                   // message_block
        S_continue = 470,                        // continue
        S_blank_line = 471,                      // blank_line
        S_title_card = 472,                      // title_card
        S_cell_id = 473,                         // cell_id
        S_material_id = 474,                     // material_id
        S_density = 475,                         // density
        S_geom_op_union = 476,                   // geom_op_union
        S_geom_op_complement = 477,              // geom_op_complement
        S_geom_id = 478,                         // geom_id
        S_lparen = 479,                          // lparen
        S_rparen = 480,                          // rparen
        S_cell_geom_expression = 481,            // cell_geom_expression
        S_cell_geom = 482,                       // cell_geom
        S_cell_param_key = 483,                  // cell_param_key
        S_cell_fill_data = 484,                  // cell_fill_data
        S_cell_fill = 485,                       // cell_fill
        S_cell_transform_data = 486,             // cell_transform_data
        S_cell_transformation = 487,             // cell_transformation
        S_cell_fill_list = 488,                  // cell_fill_list
        S_cell_param_particle_based_key = 489,   // cell_param_particle_based_key
        S_colon = 490,                           // colon
        S_particle_symbol = 491,                 // particle_symbol
        S_acell_param = 492,                     // acell_param
        S_like_but_cell_param = 493,             // like_but_cell_param
        S_cell_params = 494,                     // cell_params
        S_like_but_cell_params = 495,            // like_but_cell_params
        S_cell_param = 496,                      // cell_param
        S_like_but_cell_param_object = 497,      // like_but_cell_param_object
        S_cell_card = 498,                       // cell_card
        S_cell_block = 499,                      // cell_block
        S_PARCONE = 500,                         // PARCONE
        S_TORUS = 501,                           // TORUS
        S_surface_id = 502,                      // surface_id
        S_surface_geom = 503,                    // surface_geom
        S_surface_card = 504,                    // surface_card
        S_surface_block = 505,                   // surface_block
        S_array_jump = 506,                      // array_jump
        S_array_multiply = 507,                  // array_multiply
        S_array_repeat = 508,                    // array_repeat
        S_array_log_interp = 509,                // array_log_interp
        S_array_lin_interp = 510,                // array_lin_interp
        S_number_list = 511,                     // number_list
        S_number_paren_list = 512,               // number_paren_list
        S_number_paren_colon_list = 513,         // number_paren_colon_list
        S_value_or_sc = 514,                     // value_or_sc
        S_value_paren_colon_list = 515,          // value_paren_colon_list
        S_number_or_sc = 516,                    // number_or_sc
        S_NUMBER_LIST_TOKEN = 517,               // NUMBER_LIST_TOKEN
        S_VALUE_LIST_TOKEN = 518,                // VALUE_LIST_TOKEN
        S_VALUE_COMMA_LIST_TOKEN = 519,          // VALUE_COMMA_LIST_TOKEN
        S_value_token_list = 520,                // value_token_list
        S_number_token_list = 521,               // number_token_list
        S_value_list_item = 522,                 // value_list_item
        S_value_comma_list_item = 523,           // value_comma_list_item
        S_value_list = 524,                      // value_list
        S_value_comma_list = 525,                // value_comma_list
        S_string_list = 526,                     // string_list
        S_data_vol_card = 527,                   // data_vol_card
        S_data_area_card = 528,                  // data_area_card
        S_data_transform_card = 529,             // data_transform_card
        S_data_universe_card = 530,              // data_universe_card
        S_data_lat_card = 531,                   // data_lat_card
        S_data_fill_card = 532,                  // data_fill_card
        S_uran_members = 533,                    // uran_members
        S_data_uran_card = 534,                  // data_uran_card
        S_MATERIAL_SCALAR_KEY = 535,             // MATERIAL_SCALAR_KEY
        S_MATERIAL_LIST_KEY = 536,               // MATERIAL_LIST_KEY
        S_amaterial_option = 537,                // amaterial_option
        S_material_zaid_entry = 538,             // material_zaid_entry
        S_material_options = 539,                // material_options
        S_data_material_card = 540,              // data_material_card
        S_data_cutoff_card = 541,                // data_cutoff_card
        S_data_elpt_card = 542,                  // data_elpt_card
        S_ACTIVATION_OPTION_KEY = 543,           // ACTIVATION_OPTION_KEY
        S_comma = 544,                           // comma
        S_comma_sep_list = 545,                  // comma_sep_list
        S_activation_option = 546,               // activation_option
        S_activation_options = 547,              // activation_options
        S_data_activation_card = 548,            // data_activation_card
        S_data_fq_card = 549,                    // data_fq_card
        S_data_fc_card = 550,                    // data_fc_card
        S_data_sc_card = 551,                    // data_sc_card
        S_particle_symbol_list = 552,            // particle_symbol_list
        S_mode_particle_symbol_list = 553,       // mode_particle_symbol_list
        S_lt = 554,                              // lt
        S_rbracket = 555,                        // rbracket
        S_lbracket = 556,                        // lbracket
        S_SURFACE_OR_CELL_TALLY = 557,           // SURFACE_OR_CELL_TALLY
        S_tally_surface_list = 558,              // tally_surface_list
        S_f5_tally_member = 559,                 // f5_tally_member
        S_f5a_tally_member = 560,                // f5a_tally_member
        S_f5_tally_members = 561,                // f5_tally_members
        S_f5a_tally_members = 562,               // f5a_tally_members
        S_data_ftally_card = 563,                // data_ftally_card
        S_data_mode_card = 564,                  // data_mode_card
        S_TALLY_TIME_OPTION_KEY = 565,           // TALLY_TIME_OPTION_KEY
        S_tally_time_option = 566,               // tally_time_option
        S_tally_time_options = 567,              // tally_time_options
        S_data_tally_time_card = 568,            // data_tally_time_card
        S_data_kcode_card = 569,                 // data_kcode_card
        S_data_nps_card = 570,                   // data_nps_card
        S_data_print_card = 571,                 // data_print_card
        S_data_talnp_card = 572,                 // data_talnp_card
        S_data_prdmp_card = 573,                 // data_prdmp_card
        S_data_mt_card = 574,                    // data_mt_card
        S_data_imp_card = 575,                   // data_imp_card
        S_data_ksrc_card = 576,                  // data_ksrc_card
        S_data_tally_energy_card = 577,          // data_tally_energy_card
        S_data_tally_multiplier_card = 578,      // data_tally_multiplier_card
        S_data_sd_card = 579,                    // data_sd_card
        S_SDEF_SCALAR_VALUE_KEY = 580,           // SDEF_SCALAR_VALUE_KEY
        S_SDEF_LIST_VALUE_KEY = 581,             // SDEF_LIST_VALUE_KEY
        S_sdef_value = 582,                      // sdef_value
        S_sdef_value_list = 583,                 // sdef_value_list
        S_data_sdef_member = 584,                // data_sdef_member
        S_data_sdef_members = 585,               // data_sdef_members
        S_data_sdef_card = 586,                  // data_sdef_card
        S_data_si_card = 587,                    // data_si_card
        S_data_sp_card = 588,                    // data_sp_card
        S_data_sb_card = 589,                    // data_sb_card
        S_data_totnu_card = 590,                 // data_totnu_card
        S_data_de_card = 591,                    // data_de_card
        S_data_df_card = 592,                    // data_df_card
        S_data_tmp_card = 593,                   // data_tmp_card
        S_data_fs_card = 594,                    // data_fs_card
        S_data_tf_card = 595,                    // data_tf_card
        S_data_lost_card = 596,                  // data_lost_card
        S_data_hsrc_card = 597,                  // data_hsrc_card
        S_data_xs_card = 598,                    // data_xs_card
        S_data_phys_card = 599,                  // data_phys_card
        S_KOPTS_OPTION = 600,                    // KOPTS_OPTION
        S_data_kopts_option = 601,               // data_kopts_option
        S_data_kopts_options = 602,              // data_kopts_options
        S_data_kopts_card = 603,                 // data_kopts_card
        S_data_tally_cosine_card = 604,          // data_tally_cosine_card
        S_data_mgopt_card = 605,                 // data_mgopt_card
        S_RAND_OPTION = 606,                     // RAND_OPTION
        S_data_rand_card_option = 607,           // data_rand_card_option
        S_data_rand_card_options = 608,          // data_rand_card_options
        S_data_rand_card = 609,                  // data_rand_card
        S_data_wwe_card = 610,                   // data_wwe_card
        S_data_wwg_card = 611,                   // data_wwg_card
        S_data_wwge_card = 612,                  // data_wwge_card
        S_data_wwgt_card = 613,                  // data_wwgt_card
        S_data_wwn_card = 614,                   // data_wwn_card
        S_data_wwp_card = 615,                   // data_wwp_card
        S_data_wwt_card = 616,                   // data_wwt_card
        S_data_dbcn_card = 617,                  // data_dbcn_card
        S_data_fcl_card = 618,                   // data_fcl_card
        S_data_var_card = 619,                   // data_var_card
        S_data_cf_card = 620,                    // data_cf_card
        S_data_ext_card = 621,                   // data_ext_card
        S_data_vect_member = 622,                // data_vect_member
        S_data_vect_members = 623,               // data_vect_members
        S_data_vect_card = 624,                  // data_vect_card
        S_data_esplt_pair = 625,                 // data_esplt_pair
        S_data_esplt_pairs = 626,                // data_esplt_pairs
        S_data_esplt_card = 627,                 // data_esplt_card
        S_data_tsplt_pair = 628,                 // data_tsplt_pair
        S_data_tsplt_pairs = 629,                // data_tsplt_pairs
        S_data_tsplt_card = 630,                 // data_tsplt_card
        S_data_dd_card = 631,                    // data_dd_card
        S_data_dxt_card = 632,                   // data_dxt_card
        S_data_cm_card = 633,                    // data_cm_card
        S_FT_MEMBER_KEY = 634,                   // FT_MEMBER_KEY
        S_data_ft_member = 635,                  // data_ft_member
        S_data_ft_members = 636,                 // data_ft_members
        S_data_ft_card = 637,                    // data_ft_card
        S_data_sf_card = 638,                    // data_sf_card
        S_data_ds_card = 639,                    // data_ds_card
        S_data_fu_card = 640,                    // data_fu_card
        S_data_em_card = 641,                    // data_em_card
        S_data_bbrem_card = 642,                 // data_bbrem_card
        S_CORX = 643,                            // CORX
        S_tmesh_corx_card = 644,                 // tmesh_corx_card
        S_XMESH_FLAG_KEYWORD = 645,              // XMESH_FLAG_KEYWORD
        S_XMESH_VALUE_KEYWORD = 646,             // XMESH_VALUE_KEYWORD
        S_tmesh_xmesh_member = 647,              // tmesh_xmesh_member
        S_tmesh_xmesh_members = 648,             // tmesh_xmesh_members
        S_XMESH = 649,                           // XMESH
        S_tmesh_xmesh_card = 650,                // tmesh_xmesh_card
        S_data_tmesh_member = 651,               // data_tmesh_member
        S_data_tmesh_members = 652,              // data_tmesh_members
        S_data_tmesh_card = 653,                 // data_tmesh_card
        S_FMESH_KEY = 654,                       // FMESH_KEY
        S_data_fmesh_member = 655,               // data_fmesh_member
        S_data_fmesh_members = 656,              // data_fmesh_members
        S_data_fmesh_card = 657,                 // data_fmesh_card
        S_MESH_KEY = 658,                        // MESH_KEY
        S_data_mesh_member = 659,                // data_mesh_member
        S_data_mesh_members = 660,               // data_mesh_members
        S_data_mesh_card = 661,                  // data_mesh_card
        S_data_thtme_card = 662,                 // data_thtme_card
        S_data_tm_card = 663,                    // data_tm_card
        S_data_mx_card = 664,                    // data_mx_card
        S_data_mphys_card = 665,                 // data_mphys_card
        S_TROPT_KEY = 666,                       // TROPT_KEY
        S_data_tropt_member = 667,               // data_tropt_member
        S_data_tropt_members = 668,              // data_tropt_members
        S_data_tropt_card = 669,                 // data_tropt_card
        S_PERT_SCALAR_KEY = 670,                 // PERT_SCALAR_KEY
        S_PERT_LIST_KEY = 671,                   // PERT_LIST_KEY
        S_data_pert_member = 672,                // data_pert_member
        S_data_pert_members = 673,               // data_pert_members
        S_data_pert_card = 674,                  // data_pert_card
        S_data_lca_card = 675,                   // data_lca_card
        S_data_lcb_card = 676,                   // data_lcb_card
        S_data_lcc_card = 677,                   // data_lcc_card
        S_data_lea_card = 678,                   // data_lea_card
        S_data_leb_card = 679,                   // data_leb_card
        S_data_pikmt_card = 680,                 // data_pikmt_card
        S_data_dxc_card = 681,                   // data_dxc_card
        S_data_unc_card = 682,                   // data_unc_card
        S_data_nonu_card = 683,                  // data_nonu_card
        S_data_notrn_card = 684,                 // data_notrn_card
        S_EMBED_SCALAR_KEY = 685,                // EMBED_SCALAR_KEY
        S_EMBED_LIST_KEY = 686,                  // EMBED_LIST_KEY
        S_data_embed_member = 687,               // data_embed_member
        S_data_embed_members = 688,              // data_embed_members
        S_data_embed_card = 689,                 // data_embed_card
        S_EMBEE_SCALAR_KEY = 690,                // EMBEE_SCALAR_KEY
        S_EMBEE_LIST_KEY = 691,                  // EMBEE_LIST_KEY
        S_data_embee_member = 692,               // data_embee_member
        S_data_embee_members = 693,              // data_embee_members
        S_data_embee_card = 694,                 // data_embee_card
        S_data_embeb_card = 695,                 // data_embeb_card
        S_data_embem_card = 696,                 // data_embem_card
        S_data_embtb_card = 697,                 // data_embtb_card
        S_data_embtm_card = 698,                 // data_embtm_card
        S_data_embde_card = 699,                 // data_embde_card
        S_data_embdf_card = 700,                 // data_embdf_card
        S_data_void_card = 701,                  // data_void_card
        S_data_drxs_card = 702,                  // data_drxs_card
        S_data_ctme_card = 703,                  // data_ctme_card
        S_data_rdum_card = 704,                  // data_rdum_card
        S_data_idum_card = 705,                  // data_idum_card
        S_pty = 706,                             // pty
        S_SSW_KEY = 707,                         // SSW_KEY
        S_data_ssw_member = 708,                 // data_ssw_member
        S_data_ssw_members = 709,                // data_ssw_members
        S_data_ssw_card = 710,                   // data_ssw_card
        S_SSR_KEY = 711,                         // SSR_KEY
        S_data_ssr_member = 712,                 // data_ssr_member
        S_data_ssr_members = 713,                // data_ssr_members
        S_data_ssr_card = 714,                   // data_ssr_card
        S_FMULT_SCALAR_KEY = 715,                // FMULT_SCALAR_KEY
        S_FMULT_LIST_KEY = 716,                  // FMULT_LIST_KEY
        S_data_fmult_member = 717,               // data_fmult_member
        S_data_fmult_members = 718,              // data_fmult_members
        S_data_fmult_card = 719,                 // data_fmult_card
        S_KPERT_SCALAR_KEY = 720,                // KPERT_SCALAR_KEY
        S_KPERT_LIST_KEY = 721,                  // KPERT_LIST_KEY
        S_data_kpert_member = 722,               // data_kpert_member
        S_data_kpert_members = 723,              // data_kpert_members
        S_data_kpert_card = 724,                 // data_kpert_card
        S_KSEN_SCALAR_KEY = 725,                 // KSEN_SCALAR_KEY
        S_KSEN_LIST_KEY = 726,                   // KSEN_LIST_KEY
        S_data_ksen_member = 727,                // data_ksen_member
        S_data_ksen_members = 728,               // data_ksen_members
        S_data_ksen_card = 729,                  // data_ksen_card
        S_data_pwt_card = 730,                   // data_pwt_card
        S_PTRAC_SCALAR_KEY = 731,                // PTRAC_SCALAR_KEY
        S_PTRAC_LIST_KEY = 732,                  // PTRAC_LIST_KEY
        S_data_ptrac_member = 733,               // data_ptrac_member
        S_data_ptrac_members = 734,              // data_ptrac_members
        S_data_ptrac_card = 735,                 // data_ptrac_card
        S_data_column_wise_header_members = 736, // data_column_wise_header_members
        S_data_column_wise_row_members = 737,    // data_column_wise_row_members
        S_data_column_wise_table = 738,          // data_column_wise_table
        S_data_column_wise_block = 739,          // data_column_wise_block
        S_BFLD_SCALAR_KEY = 740,                 // BFLD_SCALAR_KEY
        S_BFLD_LIST_KEY = 741,                   // BFLD_LIST_KEY
        S_data_bfld_member = 742,                // data_bfld_member
        S_data_bfld_members = 743,               // data_bfld_members
        S_data_bfld_card = 744,                  // data_bfld_card
        S_material_zaid_pairs = 745,             // material_zaid_pairs
        S_data_awtab_card = 746,                 // data_awtab_card
        S_data_cosy_card = 747,                  // data_cosy_card
        S_data_cosyp_card = 748,                 // data_cosyp_card
        S_data_pd_card = 749,                    // data_pd_card
        S_data_card = 750,                       // data_card
        S_data_block = 751,                      // data_block
        S_start = 752                            // start
      };
    };

    /// (Internal) symbol kind.
    typedef symbol_kind::symbol_kind_type symbol_kind_type;

    /// The number of tokens.
    static const symbol_kind_type YYNTOKENS = symbol_kind::YYNTOKENS;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol kind
    /// via kind ().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol () YY_NOEXCEPT
        : value ()
        , location ()
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      basic_symbol (basic_symbol&& that)
        : Base (std::move (that))
        , value (std::move (that.value))
        , location (std::move (that.location))
      {}
#endif

      /// Copy constructor.
      basic_symbol (const basic_symbol& that);
      /// Constructor for valueless symbols.
      basic_symbol (typename Base::kind_type t,
                    YY_MOVE_REF (location_type) l);

      /// Constructor for symbols with semantic value.
      basic_symbol (typename Base::kind_type t,
                    YY_RVREF (value_type) v,
                    YY_RVREF (location_type) l);

      /// Destroy the symbol.
      ~basic_symbol ()
      {
        clear ();
      }



      /// Destroy contents, and record that is empty.
      void clear () YY_NOEXCEPT
      {
        Base::clear ();
      }

      /// The user-facing name of this symbol.
      std::string name () const YY_NOEXCEPT
      {
        return MCNPParser::symbol_name (this->kind ());
      }

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// Whether empty.
      bool empty () const YY_NOEXCEPT;

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      value_type value;

      /// The location.
      location_type location;

    private:
#if YY_CPLUSPLUS < 201103L
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& that);
#endif
    };

    /// Type access provider for token (enum) based symbols.
    struct by_kind
    {
      /// The symbol kind as needed by the constructor.
      typedef token_kind_type kind_type;

      /// Default constructor.
      by_kind () YY_NOEXCEPT;

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      by_kind (by_kind&& that) YY_NOEXCEPT;
#endif

      /// Copy constructor.
      by_kind (const by_kind& that) YY_NOEXCEPT;

      /// Constructor from (external) token numbers.
      by_kind (kind_type t) YY_NOEXCEPT;



      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_kind& that);

      /// The (internal) type number (corresponding to \a type).
      /// \a empty when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// The symbol kind.
      /// \a S_YYEMPTY when empty.
      symbol_kind_type kind_;
    };

    /// Backward compatibility for a private implementation detail (Bison 3.6).
    typedef by_kind by_type;

    /// "External" symbols: returned by the scanner.
    struct symbol_type : basic_symbol<by_kind>
    {};

    /// Build a parser object.
    MCNPParser (class AbstractInterpreter& interpreter_yyarg, std::istream &input_stream_yyarg, std::shared_ptr<class MCNPLexerImpl> lexer_yyarg);
    virtual ~MCNPParser ();

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    MCNPParser (const MCNPParser&) = delete;
    /// Non copyable.
    MCNPParser& operator= (const MCNPParser&) = delete;
#endif

    /// Parse.  An alias for parse ().
    /// \returns  0 iff parsing succeeded.
    int operator() ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

    /// The user-facing name of the symbol whose (internal) number is
    /// YYSYMBOL.  No bounds checking.
    static std::string symbol_name (symbol_kind_type yysymbol);



    class context
    {
    public:
      context (const MCNPParser& yyparser, const symbol_type& yyla);
      const symbol_type& lookahead () const YY_NOEXCEPT { return yyla_; }
      symbol_kind_type token () const YY_NOEXCEPT { return yyla_.kind (); }
      const location_type& location () const YY_NOEXCEPT { return yyla_.location; }

      /// Put in YYARG at most YYARGN of the expected tokens, and return the
      /// number of tokens stored in YYARG.  If YYARG is null, return the
      /// number of expected tokens (guaranteed to be less than YYNTOKENS).
      int expected_tokens (symbol_kind_type yyarg[], int yyargn) const;

    private:
      const MCNPParser& yyparser_;
      const symbol_type& yyla_;
    };

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    MCNPParser (const MCNPParser&);
    /// Non copyable.
    MCNPParser& operator= (const MCNPParser&);
#endif


    /// Stored state numbers (used for stacks).
    typedef short state_type;

    /// The arguments of the error message.
    int yy_syntax_error_arguments_ (const context& yyctx,
                                    symbol_kind_type yyarg[], int yyargn) const;

    /// Generate an error message.
    /// \param yyctx     the context in which the error occurred.
    virtual std::string yysyntax_error_ (const context& yyctx) const;
    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    static state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT;

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT;

    static const short yypact_ninf_;
    static const signed char yytable_ninf_;

    /// Convert a scanner token kind \a t to a symbol kind.
    /// In theory \a t should be a token_kind_type, but character literals
    /// are valid, yet not members of the token_kind_type enum.
    static symbol_kind_type yytranslate_ (int t) YY_NOEXCEPT;

    /// Convert the symbol name \a n to a form suitable for a diagnostic.
    static std::string yytnamerr_ (const char *yystr);

    /// For a symbol, its name in clear.
    static const char* const yytname_[];


    // Tables.
    // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
    // STATE-NUM.
    static const short yypact_[];

    // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
    // Performed when YYTABLE does not specify something else to do.  Zero
    // means the default is an error.
    static const short yydefact_[];

    // YYPGOTO[NTERM-NUM].
    static const short yypgoto_[];

    // YYDEFGOTO[NTERM-NUM].
    static const short yydefgoto_[];

    // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
    // positive, shift that token.  If negative, reduce the rule whose
    // number is the opposite.  If YYTABLE_NINF, syntax error.
    static const short yytable_[];

    static const short yycheck_[];

    // YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
    // state STATE-NUM.
    static const short yystos_[];

    // YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.
    static const short yyr1_[];

    // YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.
    static const signed char yyr2_[];


#if YYDEBUG
    // YYRLINE[YYN] -- Source line where rule number YYN was defined.
    static const short yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r) const;
    /// Print the state stack on the debug stream.
    virtual void yy_stack_print_ () const;

    /// Debugging level.
    int yydebug_;
    /// Debug stream.
    std::ostream* yycdebug_;

    /// \brief Display a symbol kind, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state () YY_NOEXCEPT;

      /// The symbol kind as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s) YY_NOEXCEPT;

      /// Copy constructor.
      by_state (const by_state& that) YY_NOEXCEPT;

      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_state& that);

      /// The symbol kind (corresponding to \a state).
      /// \a symbol_kind::S_YYEMPTY when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// The state number used to denote an empty symbol.
      /// We use the initial state, as it does not have a value.
      enum { empty_state = 0 };

      /// The state.
      /// \a empty when empty.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type ();
      /// Move or copy construction.
      stack_symbol_type (YY_RVREF (stack_symbol_type) that);
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) sym);
#if YY_CPLUSPLUS < 201103L
      /// Assignment, needed by push_back by some old implementations.
      /// Moves the contents of that.
      stack_symbol_type& operator= (stack_symbol_type& that);

      /// Assignment, needed by push_back by other implementations.
      /// Needed by some other old implementations.
      stack_symbol_type& operator= (const stack_symbol_type& that);
#endif
    };

    /// A stack with random access from its top.
    template <typename T, typename S = std::vector<T> >
    class stack
    {
    public:
      // Hide our reversed order.
      typedef typename S::iterator iterator;
      typedef typename S::const_iterator const_iterator;
      typedef typename S::size_type size_type;
      typedef typename std::ptrdiff_t index_type;

      stack (size_type n = 200) YY_NOEXCEPT
        : seq_ (n)
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Non copyable.
      stack (const stack&) = delete;
      /// Non copyable.
      stack& operator= (const stack&) = delete;
#endif

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      const T&
      operator[] (index_type i) const
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      T&
      operator[] (index_type i)
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Steal the contents of \a t.
      ///
      /// Close to move-semantics.
      void
      push (YY_MOVE_REF (T) t)
      {
        seq_.push_back (T ());
        operator[] (0).move (t);
      }

      /// Pop elements from the stack.
      void
      pop (std::ptrdiff_t n = 1) YY_NOEXCEPT
      {
        for (; 0 < n; --n)
          seq_.pop_back ();
      }

      /// Pop all elements from the stack.
      void
      clear () YY_NOEXCEPT
      {
        seq_.clear ();
      }

      /// Number of elements on the stack.
      index_type
      size () const YY_NOEXCEPT
      {
        return index_type (seq_.size ());
      }

      /// Iterator on top of the stack (going downwards).
      const_iterator
      begin () const YY_NOEXCEPT
      {
        return seq_.begin ();
      }

      /// Bottom of the stack.
      const_iterator
      end () const YY_NOEXCEPT
      {
        return seq_.end ();
      }

      /// Present a slice of the top of a stack.
      class slice
      {
      public:
        slice (const stack& stack, index_type range) YY_NOEXCEPT
          : stack_ (stack)
          , range_ (range)
        {}

        const T&
        operator[] (index_type i) const
        {
          return stack_[range_ - i];
        }

      private:
        const stack& stack_;
        index_type range_;
      };

    private:
#if YY_CPLUSPLUS < 201103L
      /// Non copyable.
      stack (const stack&);
      /// Non copyable.
      stack& operator= (const stack&);
#endif
      /// The wrapped container.
      S seq_;
    };


    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;

    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param sym  the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a sym.value is stolen.
    void yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym);

    /// Pop \a n symbols from the stack.
    void yypop_ (int n = 1) YY_NOEXCEPT;

    /// Constants.
    enum
    {
      yylast_ = 4640,     ///< Last index in yytable_.
      yynnts_ = 302,  ///< Number of nonterminal symbols.
      yyfinal_ = 304 ///< Termination state number.
    };


    // User arguments.
    class AbstractInterpreter& interpreter;
    std::istream &input_stream;
    std::shared_ptr<class MCNPLexerImpl> lexer;

  };


#line 35 "MCNPParser.bison"
} // wasp
#line 1992 "MCNPParser.hpp"




#endif // !YY_YY_MCNPPARSER_HPP_INCLUDED
