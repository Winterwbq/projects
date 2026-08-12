%{ /*** C/C++ Declarations ***/

#include <stdio.h>
#include <string>
#include <vector>

%}

/*** yacc/bison Declarations ***/

%code requires{
#include <cctype>
#include <memory>
#include "waspcore/utils.h"
#include "waspcore/decl.h"
#include "waspcore/wasp_bug.h"
}

%output "MCNPParser.cpp"
/* add debug output code to generated parser. disable this for release
 * versions. */
%debug

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
%define api.parser.class {MCNPParser}

/* keep track of the current position within the input */
%locations
%initial-action
{
    // initialize the initial location object
    @$.begin.filename = @$.end.filename = &interpreter.stream_name();
    @$.begin.line = @$.end.line = interpreter.start_line();
    @$.begin.column = @$.end.column = interpreter.start_column();
    lexer = std::make_shared<MCNPLexerImpl>(interpreter,&input_stream);

    // lexer->set_debug(true);
    // set_debug_level(true);
};

/* The interpreter is passed by reference to the parser and to the MCNPLexer. This
 * provides a simple but effective pure interface, not relying on global
 * variables. */
%parse-param { class AbstractInterpreter& interpreter }
             {std::istream &input_stream}
             {std::shared_ptr<class MCNPLexerImpl> lexer}

/* verbose error messages */
%define parse.error verbose

 /*** BEGIN EXAMPLE - Change the wasp grammar's tokens below ***/

%union {
        std::size_t token_index;
        std::size_t node_index;
        std::size_t stage_index;
        std::vector<size_t>* node_indices;
        std::vector<size_t>* token_indices;
        std::vector<std::vector<size_t>*>* table_token_indices;
}

%token                  END          0  "end of file"
%token                  EOL             "end of line"

%token <token_index>    COMMENT           "comment"
%token <token_index>    MESSAGE_CARD      "message card"
%token <token_index>    BLANK_LINE      "blank line delimiter"
%token <token_index>    ASSIGN           "="
%token <token_index>    DOUBLE           "double precision number"
%token <token_index>    INTEGER          "integer number"
%token <token_index>    QSTRING          "quoted string"
%token <token_index>    STRING           "string"
%token <token_index>    VALUE            "value"
%token <token_index>    LPAREN  "("
%token <token_index>    RPAREN  ")"
%token <token_index>    COLON   ":"
%token <token_index>    COMMA   ","
%token <token_index>    ASTERISK   "*"
%token <token_index>    PLUS   "+"
%token <token_index>    NEWLINE   "newline"

%token <token_index>    PARTICLE_SYMBOL "particle symbol"
%token <token_index>    DATAPATH "datapath"
%token <token_index>    INP "inp"
%token <token_index>    OUTP "outp"
%token <token_index>    RUNTPE "runtpe"
%token <token_index>    WWINP "wwinp"
%token <token_index>    WWOUT "wwout"
%token <token_index>    WWONE "wwone"
%token <token_index>    PARTINP "partinp"
%token <token_index>    LINKIN "linkin"
%token <token_index>    LINKOUT "linkout"
%token <token_index>    KSENTAL "format of senstitivity profile output file (KSENTAL)"
%token <token_index>    HISTP "histp"
%token <token_index>    COM "com"
%token <token_index>    COMOUT "comout"
%token <token_index>    PLOTM "plotm"
%token <token_index>    MCTAL "mctal"
%token <token_index>    MESHTAL "meshtal"
%token <token_index>    MDATA "mdata"
%token <token_index>    NAME "name"
%token <token_index>    SRCTP "srctp"
%token <token_index>    WSSA "wssa"
%token <token_index>    RSSA "rssa"
%token <token_index>    INPUT "input"
%token <token_index>    PLOT_GEOM "plot_geom"
%token <token_index>    XS "crosssection"
%token <token_index>    TRANSPORT "transport"
%token <token_index>    PLOT_DATA "plot_data"
%token <token_index>    PARTISN_INPUT "partisn_input"
%token <token_index>    ZAID "ziaaa.abx"
%token <token_index>    FISSION "fission"
%token <token_index>    NONFISS "nonfiss"
%token <token_index>    DN "delayed neutron (DN)"
%token <token_index>    DG "delayed gamma (DG)"
%token <token_index>    THRESH "discrete delayed-gamma line threshold (THRESH)"
%token <token_index>    DNBIAS "delayed neutron per interaction (DNBIAS)"
%token <token_index>    NAP "nap"
%token <token_index>    DNEB "delayed neutron energy bias (DNEB)"
%token <token_index>    DPEB "delayed gamma energy bias (DPEB)"
%token <token_index>    PECUT "delayed gamma energy cutoff (PECUT)"
%token <token_index>    HLCUT "spontaneos half-life cutoff (HLCUT)"
%token <token_index>    SAMPLE "correlated or uncorrelated flag (SAMPLE)"

%token <token_index>    CONTINUE_RUN "c"
%token <token_index>    CONTINUE_RUN_TPE "cn"
%token <token_index>    DBUG "dbug"
%token <token_index>    DEVTEST "devtest"
%token <token_index>    NOTEK "notek"
%token <token_index>    FATAL "fatal"
%token <token_index>    PRINT "print"
%token <token_index>    TASKS "tasks"
%token <token_index>    BALANCE "balance"
%token <token_index>    MCNPEOL "eol"
%token <token_index>    TITLE_CARD "title"
%token <token_index>    WORD
%token <token_index>    CONTINUE "continue"
%token <token_index>    UNION "union"
%token <token_index>    INTERSECTION "intersection"
%token <token_index>    COMPLEMENT "#"
%token <token_index>    DENSITY "density"
%token <token_index>    MATERIAL_ID "material_id"
%token <token_index>    CELL_ID "cell_id"
%token <token_index>    IMP "imp"
%token <token_index>    VOL "vol"
%token <token_index>    PWT "pwt"
%token <token_index>    EXT "ext"
%token <token_index>    DXC "dxc"
%token <token_index>    NONU "nonu"
%token <token_index>    PD "pd"
%token <token_index>    TMP "tmp"
%token <token_index>    UNIVERSE "universe"
%token <token_index>    TRCL "cell transformation (TRCL)"
%token <token_index>    LAT "lat"
%token <token_index>    FILL "fill"
%token <token_index>    ELPT "elpt"
%token <token_index>    COSY "cosy"
%token <token_index>    BFLCL "bfcl"
%token <token_index>    UNC "unc"
%token <token_index>    MAT "mat"
%token <token_index>    RHO "rho"
%token <token_index>    SURFACE_ID "surface_id"
%token <token_index>    PLANE "p"
%token <token_index>    XPLANE "px"
%token <token_index>    YPLANE "py"
%token <token_index>    ZPLANE "pz"
%token <token_index>    ORIGIN_SPHERE "so"
%token <token_index>    SPHERE "sph"
%token <token_index>    XSPHERE "sx"
%token <token_index>    YSPHERE "sy"
%token <token_index>    ZSPHERE "sz"
%token <token_index>    XPARCYLINDER "c/x"
%token <token_index>    YPARCYLINDER "c/y"
%token <token_index>    ZPARCYLINDER "c/z"
%token <token_index>    XCYLINDER "cx"
%token <token_index>    YCYLINDER "cy"
%token <token_index>    ZCYLINDER "cz"
%token <token_index>    XPARCONE "k/x"
%token <token_index>    YPARCONE "k/y"
%token <token_index>    ZPARCONE "k/z"
%type <token_index>     PARCONE "x, y, or z cone (K/X K/Y K/Z)"
%token <token_index>    XCONE "kx"
%token <token_index>    YCONE "ky"
%token <token_index>    ZCONE "kz"
%token <token_index>    SQUADRIC "sq"
%token <token_index>    GQUADRIC "gq"
%token <token_index>    XTORUS "tx"
%token <token_index>    YTORUS "ty"
%token <token_index>    ZTORUS "tz"
%type <token_index>     TORUS "x, y, or z torus (TX TY TZ)"
%token <token_index>    XPOINTS "x"
%token <token_index>    YPOINTS "y"
%token <token_index>    ZPOINTS "z"
%token <token_index>    BOX "box"
%token <token_index>    HEX "hex"
%token <token_index>    WEDGE "wed"
%token <token_index>    RECTANGULAR_PPIPED "rpp"
%token <token_index>    RIGHT_CIRCULAR_CYLINDER "rcc"
%token <token_index>    RIGHT_ELLIPTICAL_CYLINDER "rec"
%token <token_index>    TRUNC_RIGHT_ANGLE_CONE  "trc"
%token <token_index>    ELLIPSOID "ell"
%token <token_index>    ARBITRARY_POLYHEDRON "arb"
%token <token_index>    ARRAY_JUMP "array jump (nJ)"
%token <token_index>    ARRAY_REPEAT "array repeat (nR)"
%token <token_index>    ARRAY_MULTIPLY "array multiply (xM)"
%token <token_index>    ARRAY_LOG_INTERP "array logarithmic interpolation (nLOG)"
%token <token_index>    ARRAY_LIN_INTERP "array linear interpoloation (nI)"
%token <token_index>    NO "no"
%token <token_index>    AREA "area"
%token <token_index>    TRANSFORMATION "transformation (TRn)"
%token <token_index>    URAN "random universe (URAN)"
%token <token_index>    MATERIAL_KEY "material id (Mn)"
%token <token_index>    MATERIAL_LIB "(N|P|PN|E|H|A|S|T|D)LIB"
%token <token_index>    GAS
%token <token_index>    ESTEP
%token <token_index>    HSTEP
%token <token_index>    COND
%token <token_index>    REFI
%token <token_index>    REFC
%token <token_index>    REFS
%type <token_index>     MATERIAL_SCALAR_KEY
%type <token_index>     MATERIAL_LIST_KEY
%token <token_index>    LIB_ID "library id (.?[0-9]{2}[tcdmgpuyehporsa])"
%token <token_index>    CUTOFF           "time energy and weight cutoff card (CUT)"
%token <token_index>    ACTIVATION_OPTIONS  "activation options (ACT)"
%token <token_index>    FQ  "print hierarchy (FQn)"
%token <token_index>    FC  "tally comment card (FCn)"
%token <token_index>    F_TALLY  "base tally card (Fn)"
%token <token_index>    F1_TALLY  "integrated current surface tally card (F1)"
%token <token_index>    F2_TALLY  "flux averaged surface tally card (F2)"
%token <token_index>    F3_TALLY  "unsupported tally needed for internal indexing"
%token <token_index>    F4_TALLY  "flux averaged cell tally card (F4)"
%token <token_index>    F5_TALLY  "flux point or ring tally card (F5)"
%token <token_index>    F6_TALLY  "F6 cell tally card (F6)"
%token <token_index>    F7_TALLY  "fission energy deposition cell tally card (F7)"
%token <token_index>    F8_TALLY  "F8 tally card (F8)"
%token <token_index>    FI_TALLY  "image tally card (FI[CPR]n)"
%token <token_index>    F5A_TALLY  "ring detector tally card (F5[XYZ])"
%token <token_index>    FA5_TALLY  "ring detector tally card (F[XYZ]5)"
%token <token_index>    MODE "problem type (MODE)"
%token <token_index>    LIKE "like"
%token <token_index>    BUT "but"
%token <token_index>    TALLY_TIME "tally time (Tn)"
%token <token_index>    CBEG "ref starting time (CBEG)"
%token <token_index>    CFRQ "frequency of cycling (CFRQ)"
%token <token_index>    COFI "dead time interval (COFI)"
%token <token_index>    CONI "alive time interval (CONI)"
%token <token_index>    CSUB "number of subdivisions ot use within alive time (CSUB)"
%token <token_index>    CEND "ref ending time (CEND)"
%token <token_index>    KCODE "criticality source card (KCODE)"
%token <token_index>    NPS "history cutoff (NPS)"
%token <token_index>    TALNP "negate printing of tallies (TALNP)"
%token <token_index>    PRDMP "print and dump cycle (PRDMP)"
%token <token_index>    MT_CARD "s(a,b) thermal neutron scattering (MTm)"
%token <token_index>    KSRC "criticality source points (KSRC)"
%token <token_index>    TALLY_ENERGY "tally energy card (En)"
%token <token_index>    TALLY_MULTIPLIER "tally multiplier card (FMn)"
%token <token_index>    SEGMENT_DIVISOR "segment divisor card (SDn)"
%token <token_index>    GENERAL_SOURCE "general source (SDEF)"
%token <token_index>    CEL
%token <token_index>    SDEF_SUR "surface number (SUR)"
%token <token_index>    ERG
%token <token_index>    SDEF_TME "time (TME)"
%token <token_index>    SDEF_DIR "cosine angle between VEC and particle direction (DIR)"
%token <token_index>    VEC
%token <token_index>    SDEF_NRM "sign of surface normal (NRM)"
%token <token_index>    SDEF_PAR "source particle type (PAR)"
%token <token_index>    SDEF_POS "reference point for position sampling in vector notation (POS)"
%token <token_index>    SDEF_RAD "radial distance from the position from POS or AXS (RAD)"
%token <token_index>    EXT_CARD
%token <token_index>    AXS
%token <token_index>    SDEF_X   "x-coordinate of position (X)"
%token <token_index>    SDEF_Y   "y-coordinate of position (Y)"
%token <token_index>    SDEF_Z   "z-coordinate of position (Z)"
%token <token_index>    SDEF_CCC "cookie-cutter cell number (CCC)"
%token <token_index>    SDEF_ARA "Area of surface (ARA)"
%token <token_index>    WGT
%token <token_index>    TR
%token <token_index>    SDEF_EFF "rejection efficiency critera for position sampling (EFF)"
%token <token_index>    SDEF_DAT "date to use for comic-ray and background sources (DAT)"
%token <token_index>    SDEF_LOC "location of cosmic particle source (LOC)"
%token <token_index>    SDEF_BEM "Beam emittence parameters (BEM)"
%token <token_index>    SDEF_BAP "Beam aperature parameters (BAP)"
%type <token_index>     SDEF_SCALAR_VALUE_KEY "CEL | SUR | ERG | TME | DIR | NRM | RAD | EXT | CCC | ARA | WGT | TR | EFF"
%type <token_index>     SDEF_LIST_VALUE_KEY "DAT | LOC | BEM | BAP | POS | PAR | VEC | AXS"
%token <token_index>    SDEF_PREFIXED_VAR
%token <token_index>    SC  "source comment card (SCn)"
%token <token_index>    TOTNU  "total fission card (TOTNU)"
%token <token_index>    ID  "integer identifier"
%token <token_index>    ANYTHING_BLOCK  "anything block"
%token <token_index>    NOTRN  "direct-only neutron-particle point detector card (NOTRN)"

%token <token_index>    SOURCE_INFO "source information for distribution for n (SIn)"
%token <token_index>    SOURCE_PROB "source probability for distribution for n (SPn)"
%token <token_index>    SOURCE_BIAS "source bias for distribution for n (SBn)"
%token <token_index>    TALLY_SEGMENT "tally segment card (FSn)"
%token <token_index>    TALLY_FLUCTUATION "tally fluctation card (TFn)"
%token <token_index>    LOST "lost particle card (LOST)"
%token <token_index>    HSRC "shannon entropy card (HSRC)"
%token <token_index>    PHYS "particle physics options card (PHYS)"
%token <token_index>    KOPTS "criticality calculation options card (KOPTS)"
%token <token_index>    TALLY_COSINE "tally cosine card (Cn)"
%token <token_index>    MGOPT "multigroup adjoint transport option card (MGOPT)"
%token <token_index>    RAND "random number generation card (RAND)"
%token <token_index>    GEN "random number generator type (GEN)"
%token <token_index>    SEED "random number generator seed (SEED)"
%token <token_index>    STRIDE "number of random numbers between source particles (STRIDE)"
%token <token_index>    HIST
%type <token_index>     RAND_OPTION "GEN or SEED or STRIDE or HIST"
%token <token_index>    WWE "weight-window energy or time intervals (WWE)"
%token <token_index>    WWG "weight-window generation card (WWG)"
%token <token_index>    WWGE "weight-window generation energy or time bounds (WWGE)"
%token <token_index>    WWGT "weight-window generation time bounds (WWGT)"
%token <token_index>    WWN "cell-based lower weight-window bounds (WWNi)"
%token <token_index>    WWP "weight-window parameter card (WWP)"
%token <token_index>    WWT "weight-window time intervals (WWT)"
%token <token_index>    DBCN "debug information card (DBCN)"
%token <token_index>    LT "<"

%token <token_index>    BLOCKSIZE  "number of cycles in every outer iteration (BLOCKSIZE=n)"
%token <token_index>    KINETICS   "calculate point-kinetic parameters (KINETICS=YES/NO)"
%token <token_index>    PRECURSOR  "calculate detailed precursor information (PRECURSOR=YES/NO)"

%token <token_index>    FMAT       "turn on FMAT (FMAT=YES/NO)"
%token <token_index>    FMATSKIP   "FMATSKIP=n"
%token <token_index>    FMATSPACE  "FMATSPACE=s"
%token <token_index>    FMATACCEL  "FMATACCEL=YES/NO"
%token <token_index>    FMATREDUCE "FMATREDUCE=YES/NO"
%token <token_index>    FMATNX
%token <token_index>    FMATNY
%token <token_index>    FMATNZ
%type <token_index>     KOPTS_OPTION

%token <token_index>    DOSE_ENERGY "dose energy card (DEn)"
%token <token_index>    DOSE_FUNCTION "dose function card (DFn)"
%token <token_index>    FCL "forced-collision card (FCL)"
%token <token_index>    VAR "variance reduction control card (VAR)"
%token <token_index>    VAR_RR "russion roulette key (RR)"

%token <token_index>    CELL_FLAGGING "cell-flagging card (CFn)"
%token <token_index>    EFT "exponential transform card (EFT)"
%token <token_index>    VECT "vector input card (VECT)"
%token <token_index>    VN "Vn"
%token <token_index>    ESPLT "energy splitting and roulette card (ESPLT)"
%token <token_index>    TSPLT "time splitting and roulette card (TSPLT)"
%token <token_index>    DD "detector diagnostics card (DD)"
%token <token_index>    DXT "dxtran card (DXT)"
%token <token_index>    CM "cosine multiplier card (CMn)"
%token <token_index>    TM "time multiplier card (TMn)"

%type <token_index>    PRIMITIVE        "string or number"
%type <token_index>    NUMBER           "double or integer"
%type <token_index>    ACTIVATION_OPTION_KEY     "FISSION | NONFISS | DN | DG | THRESH | DNBIAS | NAP | DNEB | DPEB | PECUT | HLCUT | SAMPLE"
%type <token_index>    SURFACE_OR_CELL_TALLY  "F1 | F2 | F4 | F7"
%type <token_index>    TALLY_TIME_OPTION_KEY "CBEG | CFRQ | COFI | CONI | CSUB | CEND"
%token <token_index>    FT "special treatment tallies card (FT)"
%token <token_index>    FRV
%token <token_index>    GEB
%token <token_index>    TMC
%token <token_index>    INC
%token <token_index>    ICD
%token <token_index>    SCX
%token <token_index>    SCD
%token <token_index>    ELC
%token <token_index>    PTT
%token <token_index>    PHL
%token <token_index>    CAP
%token <token_index>    RES
%token <token_index>    TAG
%token <token_index>    LET
%token <token_index>    ROC
%token <token_index>    PDS
%token <token_index>    FFT
%type <token_index>    FT_MEMBER_KEY
%token <token_index>    DS "dependent source distribution card (DS)"
%token <token_index>    FU "tallyx card (FU)"
%token <token_index>    EM "energy multiplier card (EM)"
%token <token_index>    CORA
%token <token_index>    CORB
%token <token_index>    CORC
%type <token_index>     CORX
%token <token_index>    SF "surface-flagging card (SF)"
%token <token_index>    BBREM "bremsstrahlung biasing card (BBREM)"
%token <token_index>    TMESH "superimposed mesh tally card (TMESH)"
%token <token_index>    ENDMD "mesh tally terminator (ENDMD)"
%token <token_index>    TRAKS
%token <token_index>    FLUX
%token <token_index>    DOSE
%token <token_index>    POPUL
%token <token_index>    PEDEP
%token <token_index>    MFACT
%token <token_index>    TRANS
%token <token_index>    TOTAL
%token <token_index>    DEDX
%token <token_index>    RECOL
%token <token_index>    TLEST
%token <token_index>    EDLCT
%token <token_index>    CMESH "cylindrical mesh (CMESH)"
%token <token_index>    RMESH "rectilinear mesh (RMESH)"
%token <token_index>    SMESH "spherical mesh (SMESH)"
%type <token_index>     XMESH "R/C/SMESH"
%type <token_index>     XMESH_FLAG_KEYWORD
%type <token_index>     XMESH_VALUE_KEYWORD
%token <token_index>    FMESH "superimposed fmesh mesh tally card (FMESH)"
%token <token_index>    GEOM
%token <token_index>    ORIGIN
%token <token_index>    IMESH
%token <token_index>    IINTS
%token <token_index>    JMESH
%token <token_index>    JINTS
%token <token_index>    KMESH
%token <token_index>    KINTS
%token <token_index>    EMESH
%token <token_index>    EINTS
%token <token_index>    ENORM
%token <token_index>    FMESH_TMESH
%token <token_index>    TINTS
%token <token_index>    TNORM
%token <token_index>    FACTOR
%token <token_index>    MCNP_OUT
%token <token_index>    TYPE
%token <token_index>    KCLEAR
%type <token_index>     FMESH_KEY

%token <token_index>    MESH "superimposed mesh variable card (MESH)"
%token <token_index>    REF
%type <token_index>     MESH_KEY

%token <token_index>    THTME "thermal times card (THTME)"
%token <token_index>    MX "material nuclide substitution card (MX)"
%token <token_index>    MPHYS "model physics control card (MPHYS)"
%token <token_index>    TROPT "transport options card (TROPT)"
%token <token_index>    MCSCAT
%token <token_index>    ELOSS
%token <token_index>    NREACT
%token <token_index>    NESCAT
%token <token_index>    GENXS
%type <token_index>     TROPT_KEY
%token <token_index>    PERT "tally perturbation card (PERT)"
%token <token_index>    METHOD
%token <token_index>    RXN
%type <token_index>    PERT_SCALAR_KEY
%type <token_index>    PERT_LIST_KEY
%token <token_index>    LCA
%token <token_index>    LCB
%token <token_index>    LCC
%token <token_index>    LEA
%token <token_index>    LEB
%token <token_index>    PIKMT "photon production bias card (PIKMT)"
%token <token_index>    LBRACKET "["
%token <token_index>    RBRACKET "]"
%token <token_index>    EMBED_CARD
%token <token_index>    BACKGROUND
%token <token_index>    MATCELL
%token <token_index>    MESHGEO
%token <token_index>    MGEOIN
%token <token_index>    MEEOUT
%token <token_index>    MEEIN
%token <token_index>    CALC_VOLS
%token <token_index>    DEBUG
%token <token_index>    FILETYPE
%token <token_index>    GMVFILE
%token <token_index>    LENGTH
%token <token_index>    MCNPUMFILE
%token <token_index>    OVERLAP

%type <token_index>     EMBED_SCALAR_KEY
%type <token_index>     EMBED_LIST_KEY

%token <token_index>    EMBEE
%token <token_index>    EMBED
%token <token_index>    ENERGY
%token <token_index>    TIME
%token <token_index>    ATOM
%token <token_index>    COMMENT_FLAG
%token <token_index>    LIST
%token <token_index>    MTYPE
%token <token_index>    ERRORS
%type <token_index>     EMBEE_SCALAR_KEY
%type <token_index>     EMBEE_LIST_KEY
%token <token_index>    EMBEB
%token <token_index>    EMBEM
%token <token_index>    EMBTB
%token <token_index>    EMBTM
%token <token_index>    EMBDE
%token <token_index>    EMBDF
%token <token_index>    MCNP_VOID
%token <token_index>    DRXS
%token <token_index>    CTME
%token <token_index>    RDUM
%token <token_index>    IDUM
%token <token_index>    SSW
%token <token_index>    SYM
%token <token_index>    PTY
%token <token_index>    SSR
%token <token_index>    OLD
%token <token_index>    NEW
%token <token_index>    PSC
%token <token_index>    POA
%token <token_index>    COL
%token <token_index>    BCW
%token <token_index>    COMMENT_KEY

%type <token_index>    SSW_KEY
%type <token_index>    SSR_KEY
%token <token_index>   FMULT "multiplicity constant card (FMULT)"
%token <token_index>   SFNU
%token <token_index>   WIDTH
%token <token_index>   SYFIELD
%token <token_index>   WATT
%token <token_index>   DATA
%token <token_index>   SHIFT
%type <token_index>    FMULT_SCALAR_KEY
%type <token_index>    FMULT_LIST_KEY

%token <token_index>   KPERT "reactivity perturbation card (KPERT)"
%token <token_index>   LINEAR
%token <token_index>   ISO
%type <token_index>    KPERT_SCALAR_KEY
%type <token_index>    KPERT_LIST_KEY


%token <token_index>   KSEN "keff sensitivity coefficients card (KSEN)"
%token <token_index>   EIN
%token <token_index>   LEGENDRE
%token <token_index>   COS
%token <token_index>   CONSTRAIN
%token <token_index>   MT
%type <token_index>    KSEN_SCALAR_KEY
%type <token_index>    KSEN_LIST_KEY

%token <token_index>   PTRAC  "particle track output (PTRAC)"
%token <token_index>   BUFFER
%token <token_index>   FILE
%token <token_index>   MAX
%token <token_index>   MEPH
%token <token_index>   WRITE
%token <token_index>   COINC
%token <token_index>   EVENT
%token <token_index>   FILTER
%token <token_index>   SURFACE
%token <token_index>   TALLY
%token <token_index>   VALUE_KEY
%type <token_index>    PTRAC_SCALAR_KEY
%type <token_index>    PTRAC_LIST_KEY

%type <token_index>    NUMBER_LIST_TOKEN
%type <token_index>    VALUE_LIST_TOKEN
%type <token_index>    VALUE_COMMA_LIST_TOKEN
%token <token_index>   COLUMN_WISE_FLAG

%token <token_index>    BFLD "particle ray tracing (BFLD)"
%token <token_index>    FIELD
%token <token_index>    MXDEFLC
%token <token_index>    MAXSTEP
%token <token_index>    FFEDGES
%token <token_index>    REFPNT
%type <token_index>    BFLD_SCALAR_KEY
%type <token_index>    BFLD_LIST_KEY

%token <token_index>   AWTAB "atomic weight card (AWTAB)"
%token <token_index>   COSYP "transfer map card (COSPY))"

%type <node_index> colon cell_param_particle_based_key particle_symbol
%type <node_index>  assign value int_value continue
%type <node_index> comment execution_filename_key title_card cell_geom
%type <node_index> message_block message_card execution_other_option other_option_optional_int
%type <node_index> acell_param cell_param like_but_cell_param pty

//%type <node_index> execution_option
%type <node_indices>  execution_filenames  execution_other_options cell_geom_expression
%type <node_indices>  cell_params cell_fill_list like_but_cell_params
%type <node_indices> cell_fill_data number_list uran_members   string_list
%type <node_indices> material_options
%type <node_index> amaterial_option material_zaid_entry
//%type <node_indices> execution_options
//%type <node_index> key_value
%type <node_index> filename_value

%type <node_index> blank_line

%type <node_index> cell_id material_id density geom_op_union geom_op_complement geom_id lparen rparen
%type <node_index> cell_param_key cell_card cell_fill tmesh_corx_card
%type <node_index> surface_card surface_id surface_geom lt lbracket rbracket

%type <node_index> data_card array_jump array_repeat array_multiply
%type <node_index> array_lin_interp array_log_interp number number_or_sc value_or_sc data_material_card
%type <node_index> data_activation_card activation_option data_fu_card data_pwt_card

%type <node_index> data_vol_card data_area_card data_transform_card data_universe_card
%type <node_index> data_lat_card data_fill_card data_uran_card data_cutoff_card data_elpt_card
%type <node_index> data_fq_card id data_fc_card data_ftally_card comma data_mode_card
%type <node_index> f5_tally_member f5a_tally_member  data_kcode_card data_nps_card data_print_card
%type <node_index> data_talnp_card data_prdmp_card data_mt_card
%type <node_index> cell_transformation data_tally_time_card tally_time_option
%type <node_index> data_imp_card data_ksrc_card data_tally_energy_card data_tally_multiplier_card
%type <node_index> data_sd_card data_sdef_card data_sdef_member data_si_card data_sp_card data_sb_card data_sc_card
%type <node_index> data_totnu_card like_but_cell_param_object
%type <node_index> data_de_card data_df_card data_tmp_card card_id
%type <node_index> data_fs_card data_tf_card data_lost_card data_hsrc_card data_xs_card data_phys_card
%type <node_index> data_kopts_card data_kopts_option data_tally_cosine_card data_mgopt_card
%type <node_index> data_rand_card  data_rand_card_option
%type <node_index> data_wwe_card data_wwg_card data_wwge_card data_wwgt_card data_wwn_card data_wwp_card data_wwt_card
%type <node_index> data_dbcn_card data_fcl_card data_var_card data_cf_card data_ext_card
%type <node_index> data_vect_card data_vect_member data_esplt_card data_esplt_pair data_tsplt_card data_tsplt_pair
%type <node_index> data_dd_card data_dxt_card data_cm_card data_ft_card data_ft_member
%type <node_index> data_sf_card sdef_value value_list_item value_comma_list_item data_ds_card data_em_card data_bbrem_card
%type <node_index> data_tmesh_card data_tmesh_member data_lca_card data_lcb_card data_lcc_card data_lea_card data_leb_card
%type <node_index> data_pikmt_card data_awtab_card data_cosyp_card

%type <node_indices> activation_options comma_sep_list particle_symbol_list tally_surface_list
%type <node_indices> f5_tally_members f5a_tally_members  mode_particle_symbol_list data_vect_members data_ft_members
%type <node_indices> cell_transform_data tally_time_options comment_list number_paren_list number_paren_colon_list value_paren_colon_list
%type <node_indices> data_sdef_members cell_block surface_block data_block data_kopts_options data_rand_card_options
%type <node_indices> data_esplt_pairs data_tsplt_pairs value_list sdef_value_list data_tmesh_members value_comma_list

%type <token_indices> number_token_list value_token_list

%destructor { delete $$; } execution_filenames execution_other_options cell_geom_expression cell_params cell_fill_list
%destructor { delete $$; } cell_fill_data number_list uran_members  material_options string_list data_ft_members
%destructor { delete $$; } activation_options comma_sep_list particle_symbol_list tally_surface_list data_tmesh_members
%destructor { delete $$; } f5_tally_members f5a_tally_members mode_particle_symbol_list value_comma_list
%destructor { delete $$; } cell_transform_data tally_time_options number_token_list data_esplt_pairs data_tsplt_pairs
%destructor { delete $$; } comment_list number_paren_list number_paren_colon_list value_paren_colon_list data_vect_members value_list sdef_value_list
%destructor { delete $$; } data_sdef_members like_but_cell_params data_kopts_options data_rand_card_options value_token_list

%type <node_index> tmesh_xmesh_card tmesh_xmesh_member data_notrn_card
%type <node_indices> tmesh_xmesh_members
%destructor { delete $$; } tmesh_xmesh_members

%type <node_index> data_fmesh_card data_fmesh_member data_dxc_card data_unc_card data_nonu_card
%type <node_indices> data_fmesh_members
%destructor { delete $$; } data_fmesh_members

%type <node_index> data_thtme_card data_tm_card data_mx_card data_mphys_card data_tropt_card data_tropt_member
%type <node_indices> data_tropt_members
%destructor { delete $$; } data_tropt_members

%type <node_index> data_pert_card data_pert_member
%type <node_indices> data_pert_members
%destructor { delete $$; } data_pert_members

%type <node_index> data_embed_card data_embed_member
%type <node_indices> data_embed_members
%destructor { delete $$; } data_embed_members

%type <node_index> data_embee_card data_embee_member data_ssw_member
%type <node_indices> data_embee_members data_ssw_members
%destructor { delete $$; } data_embee_members data_ssw_members

%type <node_index> data_embeb_card data_embem_card data_embtb_card data_embtm_card data_embde_card data_embdf_card data_void_card
%type <node_index> data_drxs_card data_ctme_card data_rdum_card data_idum_card data_ssw_card

%type <node_index> data_ssr_card data_ssr_member
%type <node_indices> data_ssr_members
%destructor { delete $$; } data_ssr_members

%type <node_index> data_mesh_card data_mesh_member
%type <node_indices> data_mesh_members
%destructor { delete $$; } data_mesh_members

%type <node_index> data_fmult_card data_fmult_member
%type <node_indices> data_fmult_members
%destructor { delete $$; } data_fmult_members

%type <node_index> data_kpert_card data_kpert_member
%type <node_indices> data_kpert_members
%destructor { delete $$; } data_kpert_members

%type <node_index> data_ksen_card data_ksen_member
%type <node_indices> data_ksen_members
%destructor { delete $$; } data_ksen_members

%type <node_index> data_ptrac_card data_ptrac_member
%type <node_indices> data_ptrac_members
%destructor { delete $$; } data_ptrac_members

%type <node_index> data_column_wise_block data_pd_card
%type <token_indices> data_column_wise_row_members data_column_wise_header_members
%type <table_token_indices> data_column_wise_table
%destructor { delete $$; } data_column_wise_row_members data_column_wise_header_members
%destructor { for(auto i = 0; i < $$->size(); ++i) delete $$->at(i); delete $$; } data_column_wise_table

%type <node_index> data_bfld_card data_bfld_member  data_cosy_card
%type <node_indices> data_bfld_members
%destructor { delete $$; } data_bfld_members

%type <node_indices> material_zaid_pairs
%destructor { delete $$; } material_zaid_pairs
%{

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

%}

%% /*** Grammar Rules ***/

 /*** BEGIN - Change the wasp grammar rules below ***/


 /*--------------------------------------------------------------------------*/
comment : COMMENT{$$ = interpreter.push_leaf(wasp::mcnpi::COMMENT, "comment", $1);}

 /*--------------------------------------------------------------------------*/
comment_list : comment
{
    $$ = new std::vector<size_t>();
    $$->push_back($1);
}
| comment_list comment
{
    $$ = $1;
    $$->push_back($2);
}

 /*--------------------------------------------------------------------------*/
assign : ASSIGN {$$ = interpreter.push_leaf(wasp::mcnpi::ASSIGN, "=", $1);}

 /*--------------------------------------------------------------------------*/
PRIMITIVE : STRING | INTEGER | DOUBLE | VALUE | NO
NUMBER : INTEGER | DOUBLE

 /*--------------------------------------------------------------------------*/
int_value : INTEGER{$$ = interpreter.push_leaf(wasp::mcnpi::VALUE, "value" ,$1);}

 /*--------------------------------------------------------------------------*/
value : PRIMITIVE{$$ = interpreter.push_leaf(wasp::mcnpi::VALUE, "value", $1);}
 /*--------------------------------------------------------------------------*/
number : NUMBER{$$ = interpreter.push_leaf(wasp::mcnpi::VALUE, "value", $1);}
 /*--------------------------------------------------------------------------*/
id : INTEGER{$$ = interpreter.push_leaf(wasp::mcnpi::ID, "id", $1);}
 /*--------------------------------------------------------------------------*/
card_id : ID{$$ = interpreter.push_leaf(wasp::mcnpi::ID, "id", $1);}
 /*--------------------------------------------------------------------------*/
message_card : MESSAGE_CARD{$$ = interpreter.push_leaf(wasp::mcnpi::MESSAGE_CARD, "decl", $1);}
 /*--------------------------------------------------------------------------*/
execution_filename_key : DATAPATH{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_DATAPATH).data(), $1);}
|INP{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_INP).data(), $1);}
|OUTP{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_OUTP).data(), $1);}
|RUNTPE{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_RUNTPE).data(), $1);}
|WWINP{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_WWINP).data(), $1);}
|WWOUT{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_WWOUT).data(), $1);}
|WWONE{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_WWONE).data(), $1);}
|PARTINP{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_PARTINP).data(), $1);}
|LINKIN{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_LINKIN).data(), $1);}
|LINKOUT{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_LINKOUT).data(), $1);}
|KSENTAL{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, "ksental", $1);}
|HISTP{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_HISTP).data(), $1);}
|COM{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_COM).data(), $1);}
|COMOUT{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_COMOUT).data(), $1);}
|PLOTM{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_PLOTM).data(), $1);}
|MCTAL{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_MCTAL).data(), $1);}
|MESHTAL{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_MESHTAL).data(), $1);}
|MDATA{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_MDATA).data(), $1);}
|NAME{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_NAME).data(), $1);}
|SRCTP{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_SRCTP).data(), $1);}
|WSSA{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_WSSA).data(), $1);}
|RSSA{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_RSSA).data(), $1);}

 /*--------------------------------------------------------------------------*/
filename_value : execution_filename_key value
{
    $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                            ,interpreter.name($1)
                            ,{$1, $2});
}

 /*--------------------------------------------------------------------------*/
 // Optional integer argument follows these flag options
other_option_optional_int : CONTINUE_RUN {auto c = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1); $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE, symbol_name(symbol_kind::S_CONTINUE_RUN).data(), {c});}
|CONTINUE_RUN int_value{auto c = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1); $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE, symbol_name(symbol_kind::S_CONTINUE_RUN).data(), {c, $2});}
|CONTINUE_RUN_TPE {auto c = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1); $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE, symbol_name(symbol_kind::S_CONTINUE_RUN_TPE).data(), {c});}
|CONTINUE_RUN_TPE int_value{auto c = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1); $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE, symbol_name(symbol_kind::S_CONTINUE_RUN_TPE).data(), {c, $2});}
|DBUG int_value{auto c = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1); $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE, symbol_name(symbol_kind::S_DBUG).data(), {c, $2});}
|TASKS int_value{auto c = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1); $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE, symbol_name(symbol_kind::S_TASKS).data(), {c, $2});}

 /*--------------------------------------------------------------------------*/
execution_other_option: other_option_optional_int
|DEVTEST {$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_DEVTEST).data(), $1);}
|NOTEK {$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_NOTEK).data(), $1);}
|FATAL {$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_FATAL).data(), $1);}
|PRINT {$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_PRINT).data(), $1);}
|BALANCE {$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_BALANCE).data(), $1);}
|MCNPEOL {$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_MCNPEOL).data(), $1);}
|INPUT {$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_INPUT).data(), $1);}
|PLOT_GEOM {$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_PLOT_GEOM).data(), $1);}
|XS {$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_XS).data(), $1);}
|TRANSPORT {$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_TRANSPORT).data(), $1);}
|PLOT_DATA {$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_PLOT_DATA).data(), $1);}
|PARTISN_INPUT {$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_PARTISN_INPUT).data(), $1);}

 /*--------------------------------------------------------------------------*/
execution_other_options : execution_other_option
{
    $$ = new std::vector<size_t>();
    $$->push_back($1);
}
| execution_other_options execution_other_option
{
    $$ = $1;
    $$->push_back($2);
}

 /*--------------------------------------------------------------------------*/
execution_filenames : filename_value
{
    $$ = new std::vector<size_t>();
    $$->push_back($1);
}
| execution_filenames filename_value
{
    $$ = $1;
    $$->push_back($2);
}

 /*--------------------------------------------------------------------------*/
message_block : message_card execution_filenames
{
    $2->insert($2->begin(), $1);
    $$ = interpreter.push_parent(wasp::OBJECT
                                ,"msg_block"
                                ,*$2);
    delete $2;
}
| message_card execution_other_options
{
    $2->insert($2->begin(), $1);
    $$ = interpreter.push_parent(wasp::OBJECT
                                ,"msg_block"
                                ,*$2);
    delete $2;
}
| message_card execution_filenames execution_other_options
{
    $2->insert($2->begin(), $1);
    $2->insert($2->end(), $3->begin(), $3->end());
    $$ = interpreter.push_parent(wasp::OBJECT
                                ,"msg_block"
                                ,*$2);
    delete $2;
    delete $3;
}
| message_card
{
    $$ = interpreter.push_parent(wasp::OBJECT
                                ,"msg_block"
                                ,{$1});
}
 /*--------------------------------------------------------------------------*/
continue : CONTINUE
{
    $$ = interpreter.push_leaf(wasp::mcnpi::CONTINUE_PROBLEM, "continue", $1);
}
 /*--------------------------------------------------------------------------*/
blank_line : BLANK_LINE
{
    $$ = interpreter.push_leaf(wasp::mcnpi::BLANK_LINE, "BL", $1);
}

 /*--------------------------------------------------------------------------*/
title_card : TITLE_CARD
{
    $$ = interpreter.push_leaf(wasp::mcnpi::TITLE_CARD, "title", $1);
}
 /*--------------------------------------------------------------------------*/
cell_id : CELL_ID
{
    $$ = interpreter.push_leaf(wasp::mcnpi::ID, "id", $1);
}
 /*--------------------------------------------------------------------------*/
material_id : MATERIAL_ID
{
    $$ = interpreter.push_leaf(wasp::mcnpi::MATERIAL, "material", $1);
}
 /*--------------------------------------------------------------------------*/
density : DENSITY
{
    $$ = interpreter.push_leaf(wasp::mcnpi::DENSITY, "rho", $1);
}
 /*--------------------------------------------------------------------------*/
geom_op_union : UNION
{
    $$ = interpreter.push_leaf(wasp::mcnpi::UNION, "U", $1);
}
 /*--------------------------------------------------------------------------*/
geom_op_complement : COMPLEMENT
{
    $$ = interpreter.push_leaf(wasp::mcnpi::COMPLEMENT, "#", $1);
}
 /*--------------------------------------------------------------------------*/
geom_id : NUMBER
{
    $$ = interpreter.push_leaf(wasp::mcnpi::ID, "id", $1);
}

 /*--------------------------------------------------------------------------*/
lparen : LPAREN
{
    $$ = interpreter.push_leaf(wasp::mcnpi::LPAREN, "LP", $1);
}
 /*--------------------------------------------------------------------------*/
rparen : RPAREN
{
    $$ = interpreter.push_leaf(wasp::mcnpi::RPAREN, "RP", $1);
}
 /*--------------------------------------------------------------------------*/
%left UNION;
%left INTERSECTION;
%left COMPLEMENT;
%left LPAREN;
cell_geom_expression : geom_id
{
    $$ = new std::vector<size_t>();
    $$->push_back($1);
}
| cell_geom_expression cell_geom_expression %prec INTERSECTION
{ // whitespace between geometry ids is intersection operator
    $$ = $1;
    $$->insert($$->end(), $2->begin(), $2->end());
    delete $2;
}
| lparen cell_geom_expression rparen  %prec LPAREN
{
    $$ = $2;
    $$->insert($$->begin(), $1);
    $$->insert($$->end(), $3);
}
| cell_geom_expression geom_op_union cell_geom_expression %prec UNION
{
    $$ = $1;
    $$->push_back($2);
    $$->insert($$->end(), $3->begin(), $3->end());
    delete $3;
}
| geom_op_complement cell_geom_expression %prec COMPLEMENT
{
    $$ = $2;
    $$->insert($$->begin(), $1);
}


 /*--------------------------------------------------------------------------*/
cell_geom : cell_geom_expression
{

    $$ = interpreter.push_parent(wasp::mcnpi::CELL_GEOM
                                ,"geom"
                                ,*$1);
    delete $1;
}

/*--------------------------------------------------------------------------*/
cell_param_key: VOL{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_VOL).data(), $1);}
|PWT{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_PWT).data(), $1);}
|NONU{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_NONU).data(), $1);}
|PD{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_PD).data(), $1);}
|TMP{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_TMP).data(), $1);}
|UNIVERSE{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_UNIVERSE).data(), $1);}
|LAT{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_LAT).data(), $1);}
|COSY{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_COSY).data(), $1);}
|BFLCL{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_BFLCL).data(), $1);}

/*--------------------------------------------------------------------------*/
 // capture additional forms fill data FILL=n [m] |FILL=n [(o1 o2 o3 [xx’ yx’ zx’ xy’ yy’ zy’ xz’ yz’ zz’ m])] | FILL=i1:i2 j1:j2 k1:k2 n111 n211 ... ni1j1k1 ... ni2j2k2
cell_fill_data : value
{
    $$ = new std::vector<size_t>();
    $$->push_back($1);
}
| value lparen number_token_list rparen
{
    std::vector<size_t> children = {$value, $lparen};

    if ($number_token_list->size() == 1 ) // only 'm' captured
        assign_names(interpreter, {"m"}, {mcnpi::ID}, $number_token_list, children);
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
                            mcnpi::ID}, $number_token_list, children);
    children.push_back($rparen);
    $$ = new std::vector<size_t>(children);
}
| INTEGER UNION INTEGER INTEGER UNION INTEGER INTEGER UNION INTEGER cell_fill_list
 {
    auto is = interpreter.push_leaf(wasp::mcnpi::VALUE, "istart", $1);
    auto l1 = interpreter.push_leaf(wasp::mcnpi::VALUE, ":", $2);
    auto ie = interpreter.push_leaf(wasp::mcnpi::VALUE, "iend", $3);

    auto js = interpreter.push_leaf(wasp::mcnpi::VALUE, "jstart", $4);
    auto l2 = interpreter.push_leaf(wasp::mcnpi::VALUE, ":", $5);
    auto je = interpreter.push_leaf(wasp::mcnpi::VALUE, "jend", $6);

    auto ks = interpreter.push_leaf(wasp::mcnpi::VALUE, "kstart", $7);
    auto l3 = interpreter.push_leaf(wasp::mcnpi::VALUE, ":", $8);
    auto ke = interpreter.push_leaf(wasp::mcnpi::VALUE, "kend", $9);

    auto cfl = interpreter.push_parent(wasp::mcnpi::FILL_LIST, "list", *$10);
    delete $10;

    $$ = new std::vector<size_t>();
    *$$ = {is, l1, ie, js, l2, je, ks, l3, ke, cfl};
 }

 /*--------------------------------------------------------------------------*/
 // capture additional forms FILL=n [m] |FILL=n [(o1 o2 o3 [xx’ yx’ zx’ xy’ yy’ zy’ xz’ yz’ zz’ m])] | FILL=i1:i2 j1:j2 k1:k2 n111 n211 ... ni1j1k1 ... ni2j2k2
 cell_fill  : FILL assign cell_fill_data
 {
    auto n = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_FILL).data(), $1);
    $3->insert($3->begin(), $2);
    $3->insert($3->begin(), n);
    $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , "fill"
                                ,*$3);
    delete $3;
 }
 | FILL cell_fill_data
 {
    auto n = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_FILL).data(), $1);
    $2->insert($2->begin(), n);
    $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , "fill"
                                ,*$2);
    delete $2;
 }
 | ASTERISK FILL assign cell_fill_data
 {
    auto angle_flag = interpreter.push_leaf(wasp::mcnpi::DECL, "angle_flag", $1);
    auto n = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_FILL).data(), $2);
    $4->insert($4->begin(), $3);
    $4->insert($4->begin(), n);
    $4->insert($4->begin(), angle_flag);
    $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , "fill"
                                ,*$4);
    delete $4;
 }
 | ASTERISK FILL cell_fill_data
 {
    auto angle_flag = interpreter.push_leaf(wasp::mcnpi::DECL, "angle_flag", $1);
    auto n = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_FILL).data(), $2);
    $3->insert($3 ->begin(), n);
    $3->insert($3 ->begin(), angle_flag);
    $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , "fill"
                                ,*$3);
    delete $3;
 }

 /*--------------------------------------------------------------------------*/
 cell_transform_data : id {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | lparen NUMBER NUMBER NUMBER rparen {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
    auto ox = interpreter.push_leaf(wasp::mcnpi::DECL, "o", $2);
    auto oy = interpreter.push_leaf(wasp::mcnpi::DECL, "o", $3);
    auto oz = interpreter.push_leaf(wasp::mcnpi::DECL, "o", $4);
    $$->push_back(ox);
    $$->push_back(oy);
    $$->push_back(oz);
    $$->push_back($5);
 }
 | lparen NUMBER NUMBER NUMBER
        NUMBER NUMBER NUMBER
        NUMBER NUMBER NUMBER
        NUMBER NUMBER NUMBER rparen {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
    auto ox = interpreter.push_leaf(wasp::mcnpi::ORIGIN, "o", $2);
    auto oy = interpreter.push_leaf(wasp::mcnpi::ORIGIN, "o", $3);
    auto oz = interpreter.push_leaf(wasp::mcnpi::ORIGIN, "o", $4);

    auto xx = interpreter.push_leaf(wasp::mcnpi::ROTATION, "xx", $5);
    auto yx = interpreter.push_leaf(wasp::mcnpi::ROTATION, "yx", $6);
    auto zx = interpreter.push_leaf(wasp::mcnpi::ROTATION, "zx", $7);

    auto xy = interpreter.push_leaf(wasp::mcnpi::ROTATION, "xy", $8);
    auto yy = interpreter.push_leaf(wasp::mcnpi::ROTATION, "yy", $9);
    auto zy = interpreter.push_leaf(wasp::mcnpi::ROTATION, "zy", $10);

    auto xz = interpreter.push_leaf(wasp::mcnpi::ROTATION, "xz", $11);
    auto yz = interpreter.push_leaf(wasp::mcnpi::ROTATION, "yz", $12);
    auto zz = interpreter.push_leaf(wasp::mcnpi::ROTATION, "zz", $13);

    $$->push_back(ox);
    $$->push_back(oy);
    $$->push_back(oz);

    $$->push_back(xx);
    $$->push_back(yx);
    $$->push_back(zx);

    $$->push_back(xy);
    $$->push_back(yy);
    $$->push_back(zy);

    $$->push_back(xz);
    $$->push_back(yz);
    $$->push_back(zz);

    $$->push_back($rparen);
 }
 | lparen NUMBER NUMBER NUMBER
        NUMBER NUMBER NUMBER
        NUMBER NUMBER NUMBER
        NUMBER NUMBER NUMBER INTEGER rparen {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
    auto ox = interpreter.push_leaf(wasp::mcnpi::ORIGIN, "o", $2);
    auto oy = interpreter.push_leaf(wasp::mcnpi::ORIGIN, "o", $3);
    auto oz = interpreter.push_leaf(wasp::mcnpi::ORIGIN, "o", $4);

    auto xx = interpreter.push_leaf(wasp::mcnpi::ROTATION, "xx", $5);
    auto yx = interpreter.push_leaf(wasp::mcnpi::ROTATION, "yx", $6);
    auto zx = interpreter.push_leaf(wasp::mcnpi::ROTATION, "zx", $7);

    auto xy = interpreter.push_leaf(wasp::mcnpi::ROTATION, "xy", $8);
    auto yy = interpreter.push_leaf(wasp::mcnpi::ROTATION, "yy", $9);
    auto zy = interpreter.push_leaf(wasp::mcnpi::ROTATION, "zy", $10);

    auto xz = interpreter.push_leaf(wasp::mcnpi::ROTATION, "xz", $11);
    auto yz = interpreter.push_leaf(wasp::mcnpi::ROTATION, "yz", $12);
    auto zz = interpreter.push_leaf(wasp::mcnpi::ROTATION, "zz", $13);

    auto m = interpreter.push_leaf(wasp::mcnpi::FLAG, "m", $INTEGER);

    $$->push_back(ox);
    $$->push_back(oy);
    $$->push_back(oz);

    $$->push_back(xx);
    $$->push_back(yx);
    $$->push_back(zx);

    $$->push_back(xy);
    $$->push_back(yy);
    $$->push_back(zy);

    $$->push_back(xz);
    $$->push_back(yz);
    $$->push_back(zz);
    $$->push_back(m);
    $$->push_back($rparen);
 }
  // *?TRCL=n | (o1 o2 o3 [[xx’ yx’ zx’ xy’ yy’ zy’ xz’ yz’ zz’] [m]])

 cell_transformation  : TRCL assign cell_transform_data
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto d = $cell_transform_data;
    d->insert(d->begin(), $assign);
    d->insert(d->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::CELL_PARAM
                                ,"trcl"
                                ,*d);
    delete d;
 }
 | ASTERISK TRCL assign cell_transform_data
 {
    auto angle_flag = interpreter.push_leaf(wasp::mcnpi::DECL, "angle_flag", $1);
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $2);
    auto d = $cell_transform_data;
    d->insert(d->begin(), $assign);
    d->insert(d->begin(), decl);
    d->insert(d->begin(), angle_flag);
    $$ = interpreter.push_parent(wasp::mcnpi::CELL_PARAM
                                ,"trcl"
                                ,*d);
    delete d;
}
| TRCL  cell_transform_data
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto d = $cell_transform_data;
    d->insert(d->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::CELL_PARAM
                                ,"trcl"
                                ,*d);
    delete d;
 }
 | ASTERISK TRCL cell_transform_data
 {
    auto angle_flag = interpreter.push_leaf(wasp::mcnpi::DECL, "angle_flag", $1);
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $2);
    auto d = $cell_transform_data;
    d->insert(d->begin(), decl);
    d->insert(d->begin(), angle_flag);
    $$ = interpreter.push_parent(wasp::mcnpi::CELL_PARAM
                                ,"trcl"
                                ,*d);
    delete d;
}
 /*--------------------------------------------------------------------------*/
cell_fill_list : number_paren_list

 /*--------------------------------------------------------------------------*/
 // capture these particle designated parameters KEY:N|E|P or KEY:N,E,P a b c, or KEYindex:N,E,P etc
cell_param_particle_based_key : IMP{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_IMP).data(), $1);}
|EXT{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_EXT).data(), $1);}
 // DXC[m]:particle_type=value
|DXC{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_DXC).data(), $1);}
 // WWN[i]:particle_type=value
|WWN{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, "wwn", $1);}
|FCL{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, "fcl", $1);}
|ELPT{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_ELPT).data(), $1);}
|UNC{$$ = interpreter.push_leaf(wasp::mcnpi::DECL, symbol_name(symbol_kind::S_UNC).data(), $1);}


 /*--------------------------------------------------------------------------*/
colon : COLON {$$ = interpreter.push_leaf(wasp::mcnpi::COLON, ":", $1);}

 /*--------------------------------------------------------------------------*/
particle_symbol : PARTICLE_SYMBOL {$$ = interpreter.push_leaf(wasp::mcnpi::PARTICLE_SYMBOL, "pt", $1);}

 /*--------------------------------------------------------------------------*/
acell_param : cell_param_key assign value
{
    std::string key = interpreter.name($1);
    $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , key.data()
                                ,{$1, $2, $3});
}
| cell_param_key  value
{
    std::string key = interpreter.name($1);
    $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , key.data()
                                ,{$1, $2});
}
 // KEYn=value where n is an index to other data (e.g., time)
| cell_param_key card_id assign value
{
    std::string key = interpreter.name($1);
    $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , key.data()
                                ,{$1, $2, $3, $4});
}
| cell_param_particle_based_key colon particle_symbol_list assign value
{
    std::string key = interpreter.name($1);
    auto m = $particle_symbol_list;
    m->insert(m->begin(), $colon);
    m->insert(m->begin(), $1);
    m->push_back($assign);
    m->push_back($value);
    $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , key.data()
                                ,*m);
    delete m;
}
| cell_param_particle_based_key card_id colon particle_symbol_list assign value
{
    std::string key = interpreter.name($1);
    auto m = $particle_symbol_list;
    m->insert(m->begin(), $colon);
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), $1);
    m->push_back($assign);
    m->push_back($value);
    $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , key.data()
                                ,*m);
    delete m;
}
| cell_param_particle_based_key colon particle_symbol_list value
{
    std::string key = interpreter.name($1);
    auto m = $particle_symbol_list;
    m->insert(m->begin(), $colon);
    m->insert(m->begin(), $1);
    m->push_back($value);
    $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , key.data()
                                ,*m);
    delete m;
}
| cell_param_particle_based_key card_id colon particle_symbol_list value
{
    std::string key = interpreter.name($1);
    auto m = $particle_symbol_list;
    m->insert(m->begin(), $colon);
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), $1);
    m->push_back($value);
    $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , key.data()
                                ,*m);
    delete m;
}
| cell_fill
| cell_transformation
 /*--------------------------------------------------------------------------*/
like_but_cell_param : acell_param
| MAT assign value
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , "mat"
                                ,{decl, $assign, $value});
}
| MAT value {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , "mat"
                                ,{decl, $value});
}
| RHO assign value {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , "rho"
                                ,{decl, $assign, $value});
}
| RHO  value {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                , "rho"
                                ,{decl, $value});
}

/*--------------------------------------------------------------------------*/
cell_params : acell_param
{
    $$ = new std::vector<size_t>();
    $$->push_back($1);
}
| cell_params acell_param
{
    $$ = $1;
    $$->push_back($2);
}

/*--------------------------------------------------------------------------*/
like_but_cell_params : like_but_cell_param
{
    $$ = new std::vector<size_t>();
    $$->push_back($1);
}
| like_but_cell_params like_but_cell_param
{
    $$ = $1;
    $$->push_back($2);
}
 /*--------------------------------------------------------------------------*/
cell_param : cell_params
{
    $$ = interpreter.push_parent(wasp::mcnpi::CELL_PARAMS
                                ,"params"
                                ,*$1);
    delete $1;
}
 /*--------------------------------------------------------------------------*/
like_but_cell_param_object : like_but_cell_params
{
    $$ = interpreter.push_parent(wasp::mcnpi::CELL_PARAMS
                                ,"params"
                                ,*$1);
    delete $1;
}
 /*--------------------------------------------------------------------------*/
cell_card : cell_id material_id density cell_geom
{
    $$ = interpreter.push_parent(wasp::mcnpi::CELL_CARD
                                ,"cell"
                                ,{$1, $2, $3, $4});
}
| cell_id material_id density cell_geom cell_param
{
    $$ = interpreter.push_parent(wasp::mcnpi::CELL_CARD
                                ,"cell"
                                ,{$1, $2, $3, $4, $5});
}
| cell_id material_id cell_geom
{
    $$ = interpreter.push_parent(wasp::mcnpi::CELL_CARD
                                ,"cell"
                                ,{$1, $2, $3});
}
| cell_id material_id cell_geom cell_param
{
    $$ = interpreter.push_parent(wasp::mcnpi::CELL_CARD
                                ,"cell"
                                ,{$1, $2, $3, $4});
}
| cell_id LIKE INTEGER BUT like_but_cell_param_object
{
    auto l = interpreter.push_leaf(wasp::mcnpi::FLAG, "decl", $2);
    auto cid = interpreter.push_leaf(wasp::mcnpi::CELL_ID, "cell", $3);
    auto b = interpreter.push_leaf(wasp::mcnpi::FLAG, "but", $4);
    auto lp = interpreter.push_parent(wasp::mcnpi::CELL_CARD
                                ,"like"
                                ,{l, cid, b, $5});
    $$ = interpreter.push_parent(wasp::mcnpi::CELL_CARD
                                ,"cell"
                                ,{$1, lp});

}
/*--------------------------------------------------------------------------*/
cell_block : cell_card
{
    $$ = new std::vector<size_t>();
    $$->push_back($1);
}
| cell_block cell_card
{
    $$ = $1;
    $$->push_back($2);
}
/*--------------------------------------------------------------------------*/
PARCONE : XPARCONE | YPARCONE | ZPARCONE
/*--------------------------------------------------------------------------*/
TORUS : XTORUS | YTORUS | ZTORUS
/*--------------------------------------------------------------------------*/
surface_id : SURFACE_ID{$$ = interpreter.push_leaf(wasp::mcnpi::ID, "id", $1);}


 /*--------------------------------------------------------------------------*/
 surface_geom : PLANE number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::vector<size_t> children = {decl};
    auto t = $number_token_list;
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
    $$ = interpreter.push_parent(wasp::mcnpi::PLANE,name.data(),children);
    delete t;
 }
 | XPLANE NUMBER
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto d = interpreter.push_leaf(wasp::mcnpi::VALUE, "d", $2);
    $$ = interpreter.push_parent(wasp::mcnpi::XPLANE
                                ,"xpl"
                                ,{decl, d});
 }
 | YPLANE NUMBER
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto d = interpreter.push_leaf(wasp::mcnpi::VALUE, "d", $2);
    $$ = interpreter.push_parent(wasp::mcnpi::YPLANE
                                ,"ypl"
                                ,{decl, d});
 }
| ZPLANE NUMBER
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto d = interpreter.push_leaf(wasp::mcnpi::VALUE, "d", $2);
    $$ = interpreter.push_parent(wasp::mcnpi::ZPLANE
                                ,"zpl"
                                ,{decl, d});
 }
 | ORIGIN_SPHERE NUMBER
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto r = interpreter.push_leaf(wasp::mcnpi::VALUE, "r", $2);
    $$ = interpreter.push_parent(wasp::mcnpi::ORIGIN_SPHERE
                                ,"sph"
                                ,{decl, r});
 }
 | SPHERE number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"xbar", "ybar", "zbar","r"},
                                  {tt,tt,tt,tt}, t, children);

    $$ = interpreter.push_parent(wasp::mcnpi::SPHERE
                                ,"sph"
                                ,children);
    delete t;
 }
 | XSPHERE NUMBER NUMBER
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto x = interpreter.push_leaf(wasp::mcnpi::VALUE, "xbar", $2);
    auto r = interpreter.push_leaf(wasp::mcnpi::VALUE, "r", $3);
    $$ = interpreter.push_parent(wasp::mcnpi::SPHERE
                                ,"sph"
                                ,{decl, x, r});
 }
 | YSPHERE NUMBER NUMBER
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto y = interpreter.push_leaf(wasp::mcnpi::VALUE, "ybar", $2);
    auto r = interpreter.push_leaf(wasp::mcnpi::VALUE, "r", $3);
    $$ = interpreter.push_parent(wasp::mcnpi::SPHERE
                                ,"sph"
                                ,{decl, y, r});
 }
 | ZSPHERE NUMBER NUMBER
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto z = interpreter.push_leaf(wasp::mcnpi::VALUE, "zbar", $2);
    auto r = interpreter.push_leaf(wasp::mcnpi::VALUE, "r", $3);
    $$ = interpreter.push_parent(wasp::mcnpi::SPHERE
                                ,"sph"
                                ,{decl, z, r});
 }
 | XPARCYLINDER number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"ybar", "zbar","r"},
                                  {tt,tt,tt}, t, children);

    $$ = interpreter.push_parent(wasp::mcnpi::CYLINDER
                                ,"cyl"
                                ,children);
    delete t;
 }
| YPARCYLINDER number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"xbar", "zbar","r"},
                                  {tt,tt,tt}, t, children);

    $$ = interpreter.push_parent(wasp::mcnpi::CYLINDER
                                ,"cyl"
                                ,children);
    delete t;
 }
 | ZPARCYLINDER number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"xbar", "ybar","r"},
                                  {tt,tt,tt}, t, children);

    $$ = interpreter.push_parent(wasp::mcnpi::CYLINDER
                                ,"cyl"
                                ,children);
    delete t;
 }
 | XCYLINDER NUMBER
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto r = interpreter.push_leaf(wasp::mcnpi::VALUE, "r", $2);
    $$ = interpreter.push_parent(wasp::mcnpi::XCYLINDER
                                ,"xcyl"
                                ,{decl, r});
 }
 | YCYLINDER NUMBER
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto r = interpreter.push_leaf(wasp::mcnpi::VALUE, "r", $2);
    $$ = interpreter.push_parent(wasp::mcnpi::YCYLINDER
                                ,"ycyl"
                                ,{decl, r});
 }
 | ZCYLINDER NUMBER
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto r = interpreter.push_leaf(wasp::mcnpi::VALUE, "r", $2);
    $$ = interpreter.push_parent(wasp::mcnpi::ZCYLINDER
                                ,"zcyl"
                                ,{decl, r});
 }
 | PARCONE number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"xbar", "ybar", "zbar","t", "sense"},
                                  {tt,tt,tt,tt,tt}, t, children);
    wasp_check(std::string(interpreter.token_data($1)).find_first_of("xyzXYZ") != std::string::npos);
    std::string name(1,interpreter.token_data($1)[2]); // obtain axis from 'k/a' where a is x,y,z
    name.append("pcone");
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    wasp::mcnpi::NODE type = wasp::mcnpi::XPARCONE;
    if (name.at(0) == 'y') type = wasp::mcnpi::YPARCONE;
    else if (name.at(0) == 'z') type = wasp::mcnpi::ZPARCONE;

    $$ = interpreter.push_parent(type, name.data(), children);
    delete t;
 }
 | XCONE number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"xbar", "t", "sense"},
                                  {tt,tt,tt}, t, children);
    $$ = interpreter.push_parent(wasp::mcnpi::XCONE, "xcone", children);
    delete t;
 }
 | YCONE number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"ybar", "t", "sense"},
                                  {tt,tt,tt}, t, children);
    $$ = interpreter.push_parent(wasp::mcnpi::YCONE, "ycone", children);
    delete t;
 }
 | ZCONE number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"zbar", "t", "sense"},
                                  {tt,tt,tt}, t, children);
    $$ = interpreter.push_parent(wasp::mcnpi::ZCONE, "zcone",children);
    delete t;
 }
 | TORUS number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"xbar", "ybar", "zbar", "a", "b", "c"},
                                  {tt,tt,tt,tt,tt, tt}, t, children);
    wasp_check(std::string(interpreter.token_data($1)).find_first_of("xyzXYZ") != std::string::npos);
    std::string name(1,interpreter.token_data($1)[1]); // obtain axis from 'ta' where a is x,y,z
    name.append("torus");
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    wasp::mcnpi::NODE type = wasp::mcnpi::XTORUS;
    if (name.at(0) == 'y') type = wasp::mcnpi::YTORUS;
    else if (name.at(0) == 'z') type = wasp::mcnpi::ZTORUS;

    $$ = interpreter.push_parent(type, name.data(), children);
    delete t;
 }
| SQUADRIC number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"a","b","c","d","e","f","g","xbar","ybar","zbar"},
                                  {tt,tt,tt,tt,tt,tt,tt,tt,tt,tt}, t, children);

    $$ = interpreter.push_parent(wasp::mcnpi::SQUADRIC
                                ,"sq"
                                ,children);
    delete t;
 }
 | GQUADRIC number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"a","b","c","d","e","f","g","h","j","k"},
                               {tt, tt, tt, tt, tt, tt, tt, tt, tt,tt}, t, children);

    $$ = interpreter.push_parent(wasp::mcnpi::GQUADRIC
                                ,"gq"
                                ,children);
    delete t;
 }
 | XPOINTS number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"xp","rp","xp","rp","xp","rp"},
                               {tt, tt, tt, tt, tt, tt}, t, children);

    $$ = interpreter.push_parent(wasp::mcnpi::XPOINTS
                                ,"xpts"
                                ,children);
    delete t;
 }
 | YPOINTS number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"yp","rp","yp","rp","yp","rp"},
                               {tt, tt, tt, tt, tt, tt}, t, children);

    $$ = interpreter.push_parent(wasp::mcnpi::YPOINTS
                                ,"ypts"
                                ,children);
    delete t;
 }
 | ZPOINTS number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"zp","rp","zp","rp","zp","rp"},
                               {tt, tt, tt, tt, tt, tt}, t, children);

    $$ = interpreter.push_parent(wasp::mcnpi::ZPOINTS
                                ,"zpts"
                                ,children);
    delete t;
 }
 | BOX number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"vx","vy","vz","ax","ay","az","ax","ay","az","ax","ay","az"},
                                  {tt,tt,tt,tt,tt,tt,tt,tt,tt,tt,tt,tt}, t, children);

    $$ = interpreter.push_parent(wasp::mcnpi::BOX
                                ,"box"
                                ,children);
    delete t;
 }
 |RECTANGULAR_PPIPED number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"xmin","xmax","ymin","ymax","zmin","zmax"},
                                  {tt,tt,tt,tt,tt,tt}, t, children);

    $$ = interpreter.push_parent(wasp::mcnpi::RECTANGULAR_PPIPED
                                ,"rppiped"
                                ,children);
    delete t;
 }
 |RIGHT_CIRCULAR_CYLINDER  number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"vx","vy","vz","hx","hy","hz","r"},
                                  {tt,tt,tt,tt,tt,tt, tt}, t, children);

    $$ = interpreter.push_parent(wasp::mcnpi::RIGHT_CIRCULAR_CYLINDER
                                ,"rrectcyl"
                                ,children);
    delete t;
 }
 |RIGHT_ELLIPTICAL_CYLINDER number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
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

    $$ = interpreter.push_parent(wasp::mcnpi::RIGHT_ELLIPTICAL_CYLINDER
                                ,"relipcyl"
                                ,children);
    delete t;
}
 |HEX number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"vx","vy","vz","hx","hy","hz","rx","ry","rz","sx","sy","sz","tx","ty","tz"},
                              {tt,tt,tt,tt,tt,tt,tt,tt,tt,tt,tt,tt,tt,tt,tt}, t, children);

    $$ = interpreter.push_parent(wasp::mcnpi::HEX
                                ,"hex"
                                ,children);
    delete t;
 }
 |TRUNC_RIGHT_ANGLE_CONE  number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"vx","vy","vz","hx","hy","hz","r1","r2"},
                              {tt,tt,tt,tt,tt,tt,tt,tt}, t, children);

    $$ = interpreter.push_parent(wasp::mcnpi::CONE
                                ,"cone"
                                ,children);
    delete t;
 }
 |ELLIPSOID number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;
    assign_names(interpreter, {"vx","vy","vz","vx","vy","vz","rm"},
                              {tt,tt,tt,tt,tt,tt,tt}, t, children);

    $$ = interpreter.push_parent(wasp::mcnpi::ELLIPSOID
                                ,"ellipsoid"
                                ,children);
    delete t;
 }
 | WEDGE number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
    std::vector<size_t> children = {decl};
    auto tt = wasp::mcnpi::VALUE;

    assign_names(interpreter, {"vx","vy", "vz", "vx1","vy1","vz1","vx2","vy2","vz2","vx3","vy3","vz3"},
                                  {tt,tt,tt,tt,tt,tt,tt,tt,tt,tt,tt,tt,}, t, children);

    $$ = interpreter.push_parent(wasp::mcnpi::WEDGE
                                ,"wedge"
                                ,children);
    delete t;
 }
 | ARBITRARY_POLYHEDRON number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
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

    $$ = interpreter.push_parent(wasp::mcnpi::ARBITRARY_POLYHEDRON
                                ,"arbpoly"
                                ,children);
    delete t;
 }
 /*--------------------------------------------------------------------------*/
 surface_card :  surface_id surface_geom
 {
    $$ = interpreter.push_parent(wasp::mcnpi::SURFACE_CARD
                                ,"surface"
                                ,{$1, $2});
 }
 | surface_id INTEGER surface_geom
 {
    int n = std::stoi(interpreter.token_data($2));

    size_t ni = 0;
    if (n < 0)
    {
        ni = interpreter.push_leaf(wasp::mcnpi::SURFACE_ID, "periodic", $2);
    }
    else
    {
        ni = interpreter.push_leaf(wasp::mcnpi::TRANSFORM_ID, "transform", $2);
    }
    $$ = interpreter.push_parent(wasp::mcnpi::SURFACE_CARD
                                ,"surface"
                                ,{$1, ni, $3});
 }
 /*--------------------------------------------------------------------------*/
surface_block : surface_card
{
    $$ = new std::vector<size_t>();
    $$->push_back($1);
}
| surface_block surface_card
{
    $$ = $1;
    $$->push_back($2);
}

 /*--------------------------------------------------------------------------*/
 array_jump : ARRAY_JUMP {$$ = interpreter.push_leaf(wasp::mcnpi::ARRAY_JUMP, "jump", $1);}
 /*--------------------------------------------------------------------------*/
 array_multiply : ARRAY_MULTIPLY {$$ = interpreter.push_leaf(wasp::mcnpi::ARRAY_MULTIPLY, "multiply", $1);}

 /*--------------------------------------------------------------------------*/
 array_repeat : ARRAY_REPEAT
 {
    $$ = interpreter.push_leaf(wasp::mcnpi::ARRAY_REPEAT, "repeat", $1);
 }
 /*--------------------------------------------------------------------------*/
 array_log_interp : ARRAY_LOG_INTERP
 {
    $$ = interpreter.push_leaf(wasp::mcnpi::ARRAY_LOG_INTERP, "log", $1);
 }
 /*--------------------------------------------------------------------------*/
 array_lin_interp : ARRAY_LIN_INTERP
 {
    $$ = interpreter.push_leaf(wasp::mcnpi::ARRAY_LIN_INTERP, "linear", $1);
 }

 /*--------------------------------------------------------------------------*/
 number_list : number_or_sc
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | number_list number_or_sc
 {
    $$ = $1;
    $$->push_back($2);
 }


/*--------------------------------------------------------------------------*/
 number_paren_list : number_or_sc
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | number_paren_list number_or_sc
 {
    $$ = $1;
    $$->push_back($2);
 }
 | lparen number_paren_list rparen
 {
    $$ = $2;
    $$->insert($$->begin(), $lparen);
    $$->push_back($rparen);
 }
 |  number_paren_list lparen number_paren_list rparen
 {
    $$ = $1;
    $$->push_back($lparen);
    $$->insert($$->end(), $3->begin(), $3->end());
    $$->push_back($rparen);
    delete $3;
 }
/*--------------------------------------------------------------------------*/
 number_paren_colon_list :  number_or_sc
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | number_paren_colon_list number_or_sc
 {
    $$ = $1;
    $$->push_back($2);
 }
 | lparen number_paren_colon_list rparen
 {
    $$ = $2;
    $$->insert($$->begin(), $lparen);
    $$->push_back($rparen);
 }
 |  number_paren_colon_list lparen number_paren_colon_list rparen
 {
    $$ = $1;
    $$->push_back($lparen);
    $$->insert($$->end(), $3->begin(), $3->end());
    $$->push_back($rparen);
    delete $3;
 }
 | number_paren_colon_list colon number_paren_colon_list
 {
    $$ = $1;
    $$->push_back($colon);
    $$->insert($$->end(), $3->begin(), $3->end());
    delete $3;
 }
/*--------------------------------------------------------------------------*/
value_or_sc : value | array_jump | array_lin_interp | array_log_interp | array_multiply | array_repeat
/*--------------------------------------------------------------------------*/
 value_paren_colon_list :  value_or_sc
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | value_paren_colon_list value_or_sc
 {
    $$ = $1;
    $$->push_back($2);
 }
 | lparen value_paren_colon_list rparen
 {
    $$ = $2;
    $$->insert($$->begin(), $lparen);
    $$->push_back($rparen);
 }
 |  value_paren_colon_list lparen value_paren_colon_list rparen
 {
    $$ = $1;
    $$->push_back($lparen);
    $$->insert($$->end(), $3->begin(), $3->end());
    $$->push_back($rparen);
    delete $3;
 }
 | value_paren_colon_list colon value_paren_colon_list
 {
    $$ = $1;
    $$->push_back($colon);
    $$->insert($$->end(), $3->begin(), $3->end());
    delete $3;
 }
 | value_paren_colon_list lt value_or_sc
{
    $$ = $1;
    $$->push_back($2);
    $$->push_back($3);
}
| value_paren_colon_list lt lparen value_paren_colon_list rparen
{
    $$ = $1;
    $$->insert($$->end(), $2);
    $$->insert($$->end(), $3);
    $$->insert($$->end(), $4->begin(), $4->end());
    $$->push_back($5);
    delete $4;
}
| value_paren_colon_list lbracket value_paren_colon_list rbracket
{
    $$ = $1;
    $$->insert($$->end(), $2);
    $$->insert($$->end(), $3->begin(), $3->end());
    $$->push_back($4);
    delete $3;
}
| STRING assign value
{
    $$ = new std::vector<size_t>();
    std::string name = interpreter.token_data($STRING);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $STRING);
    auto kv = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE, name.data(), {decl, $assign, $value});
    $$->push_back(kv);
}
/*--------------------------------------------------------------------------*/
number_or_sc : number | array_jump | array_lin_interp | array_log_interp | array_multiply | array_repeat
/*--------------------------------------------------------------------------*/
NUMBER_LIST_TOKEN : NUMBER | ARRAY_JUMP | ARRAY_REPEAT | ARRAY_MULTIPLY
                   | ARRAY_LIN_INTERP | ARRAY_LOG_INTERP | VN
/*--------------------------------------------------------------------------*/
VALUE_LIST_TOKEN : NUMBER_LIST_TOKEN | VALUE | STRING | LIB_ID | NO | COLON
VALUE_COMMA_LIST_TOKEN : VALUE_LIST_TOKEN | COMMA
/*--------------------------------------------------------------------------*/
 value_token_list : VALUE_LIST_TOKEN
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | value_token_list VALUE_LIST_TOKEN
 {
    $$ = $1;
    $$->push_back($2);
 }

/*--------------------------------------------------------------------------*/
 number_token_list : NUMBER_LIST_TOKEN
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | number_token_list NUMBER_LIST_TOKEN
 {
    $$ = $1;
    $$->push_back($2);
 }
 /*--------------------------------------------------------------------------*/
value_list_item : VALUE_LIST_TOKEN
{
    $$ = interpreter.push_leaf(wasp::mcnpi::VALUE,"value",$1);
}
 /*--------------------------------------------------------------------------*/
value_comma_list_item : VALUE_COMMA_LIST_TOKEN
{
    $$ = interpreter.push_leaf(wasp::mcnpi::VALUE,"value",$1);
}
/*--------------------------------------------------------------------------*/
value_list : value_list_item
{
    $$ = new std::vector<size_t>();
    $$->push_back($1);
}
| value_list value_list_item
{
    $$ = $1;
    $$->push_back($2);
}
/*--------------------------------------------------------------------------*/
value_comma_list : value_comma_list_item
{
    $$ = new std::vector<size_t>();
    $$->push_back($1);
}
| value_comma_list value_comma_list_item
{
    $$ = $1;
    $$->push_back($2);
}
 /*--------------------------------------------------------------------------*/
 string_list : STRING
 {
    $$ = new std::vector<size_t>();
    auto s = interpreter.push_leaf(wasp::mcnpi::VALUE,"value",$1);
    $$->push_back(s);
 }
 | string_list STRING
 {
    $$ = $1;
    auto s = interpreter.push_leaf(wasp::mcnpi::VALUE,"value",$2);
    $$->push_back(s);
 }

/*--------------------------------------------------------------------------*/
data_vol_card : VOL number_list {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $2->insert($2->begin(),decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"vol"
                                ,*$2);
    delete $2;
 }
 | VOL NO number_list {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto n = interpreter.push_leaf(wasp::mcnpi::NO, "no", $2);
    $3->insert($3->begin(),n);
    $3->insert($3->begin(),decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"vol"
                                ,*$3);
    delete $3;
 }
 | VOL NO {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto n = interpreter.push_leaf(wasp::mcnpi::NO, "no", $2);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"vol"
                                ,{decl, n});

 }

 /*--------------------------------------------------------------------------*/
data_area_card : AREA number_list {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $2->insert($2->begin(),decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"area"
                                ,*$2);
    delete $2;
}

 /*--------------------------------------------------------------------------*/
 /* apparently the asterisk can also follow the flag */
data_transform_card : TRANSFORMATION card_id number_list {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $3->insert($3->begin(), $card_id);
    $3->insert($3->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"trcl"
                                ,*$3);
    delete $3;
}
| ASTERISK TRANSFORMATION card_id number_list {
    auto angle_flag = interpreter.push_leaf(wasp::mcnpi::DECL, "angle_flag", $1);
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $2);
    $4->insert($4->begin(), $card_id);
    $4->insert($4->begin(), decl);
    $4->insert($4->begin(), angle_flag);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"trcl"
                                ,*$4);
    delete $4;
}
| TRANSFORMATION card_id ASTERISK number_list {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto angle_flag = interpreter.push_leaf(wasp::mcnpi::DECL, "angle_flag", $3);
    $4->insert($4->begin(), angle_flag);
    $4->insert($4->begin(), $card_id);
    $4->insert($4->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"trcl"
                                ,*$4);
    delete $4;
}
 /*--------------------------------------------------------------------------*/
data_universe_card : UNIVERSE number_list {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $2->insert($2->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"universe"
                                ,*$2);
    delete $2;
}

 /*--------------------------------------------------------------------------*/
data_lat_card : LAT number_list {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $2->insert($2->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"lat"
                                ,*$2);
    delete $2;
}
 /*--------------------------------------------------------------------------*/
data_fill_card : FILL number_list {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $2->insert($2->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fill"
                                ,*$2);
    delete $2;
}
 /*--------------------------------------------------------------------------*/
 uran_members : INTEGER NUMBER NUMBER NUMBER
 {
    auto id = interpreter.push_leaf(wasp::mcnpi::ID, "id", $1);
    auto dx = interpreter.push_leaf(wasp::mcnpi::VALUE, "dx", $2);
    auto dy = interpreter.push_leaf(wasp::mcnpi::VALUE, "dy", $3);
    auto dz = interpreter.push_leaf(wasp::mcnpi::VALUE, "dz", $4);
    $$ = new std::vector<size_t>();
    *$$ = {id, dx, dy, dz};
 }
 | uran_members INTEGER NUMBER NUMBER NUMBER
 {
    $$ = $1;
    auto id = interpreter.push_leaf(wasp::mcnpi::ID, "id", $2);
    auto dx = interpreter.push_leaf(wasp::mcnpi::VALUE, "dx", $3);
    auto dy = interpreter.push_leaf(wasp::mcnpi::VALUE, "dy", $4);
    auto dz = interpreter.push_leaf(wasp::mcnpi::VALUE, "dz", $5);
    *$$ = {id, dx, dy, dz};
 }

 /*--------------------------------------------------------------------------*/
data_uran_card : URAN uran_members {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $2->insert($2->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"uran"
                                ,*$2);
    delete $2;
}
/*--------------------------------------------------------------------------*/
MATERIAL_SCALAR_KEY : GAS | REFI  | COND | ESTEP | HSTEP
MATERIAL_LIST_KEY : REFC | REFS
 /*--------------------------------------------------------------------------*/
amaterial_option :  MATERIAL_LIST_KEY assign number_list
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $3->insert($3->begin(), $assign);
    $3->insert($3->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
            [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::MATERIAL_PARAM
                                ,name.data()
                                ,*$3);
    delete $3;
}
|  MATERIAL_LIST_KEY  number_list
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $2->insert($2->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
            [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::MATERIAL_PARAM
                                ,name.data()
                                ,*$2);
    delete $2;
}
| MATERIAL_SCALAR_KEY assign value
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
            [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::MATERIAL_PARAM
                                ,name.data()
                                ,{decl, $assign, $value});
}
|  MATERIAL_SCALAR_KEY  value
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
            [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::MATERIAL_PARAM
                                ,name.data()
                                ,{decl, $value});
}
| MATERIAL_LIB assign LIB_ID
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto lid = interpreter.push_leaf(wasp::mcnpi::DECL, "lib_id", $3);
    $$ = interpreter.push_parent(wasp::mcnpi::MATERIAL_PARAM
                                ,interpreter.token_data($1)
                                ,{decl, $2, lid});
}
| MATERIAL_LIB LIB_ID
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto lid = interpreter.push_leaf(wasp::mcnpi::DECL, "lib_id", $2);
    $$ = interpreter.push_parent(wasp::mcnpi::MATERIAL_PARAM
                                ,interpreter.token_data($1)
                                ,{decl, lid});
}
 /*--------------------------------------------------------------------------*/
material_zaid_entry : INTEGER number
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::ID, "id", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::ZAID
                                ,"zaid"
                                ,{decl, $2});
}
| INTEGER assign number
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::ID, "id", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::ZAID
                                ,"zaid"
                                ,{decl, $2, $3});
}
| INTEGER LIB_ID number
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::ID, "id", $1);
    auto lib = interpreter.push_leaf(wasp::mcnpi::LIB_ID, "lib", $2);
    $$ = interpreter.push_parent(wasp::mcnpi::ZAID
                                ,"zaid"
                                ,{decl, lib, $3});
}
| INTEGER LIB_ID assign number
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::ID, "id", $1);
    auto lib = interpreter.push_leaf(wasp::mcnpi::LIB_ID, "lib", $2);
    $$ = interpreter.push_parent(wasp::mcnpi::ZAID
                                ,"zaid"
                                ,{decl, lib, $3, $4});
}
| DOUBLE number
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::ID, "id", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::ZAID
                                ,"zaid"
                                ,{decl, $2});
}
| DOUBLE assign number
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::ID, "id", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::ZAID
                                ,"zaid"
                                ,{decl, $2, $3});
}
 /*--------------------------------------------------------------------------*/
material_options : amaterial_option
{
    $$ = new std::vector<size_t>();
    $$->push_back($1);
}
| material_options amaterial_option
{
    $$ = $1;
    $$->push_back($2);
}
| material_zaid_entry
{
    $$ = new std::vector<size_t>();
    *$$ = {$1};
}
| material_options material_zaid_entry
{
    $$ = $1;
    $$->push_back($2);
}

 /*--------------------------------------------------------------------------*/
data_material_card : MATERIAL_KEY id material_options
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $3->insert($3->begin(), $id);
    $3->insert($3->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"material"
                                ,*$3);
    delete $3;
}
| MATERIAL_KEY id
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"material"
                                ,{decl, $id});
}
/*--------------------------------------------------------------------------*/
data_cutoff_card :  CUTOFF colon particle_symbol number_list
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $4->insert($4->begin(), $3);
    $4->insert($4->begin(), $2);
    $4->insert($4->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"cutoff"
                                ,*$4);
    delete $4;
}
|CUTOFF colon particle_symbol
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"cutoff"
                                ,{decl, $colon, $particle_symbol});
}
/*--------------------------------------------------------------------------*/
data_elpt_card :  ELPT colon particle_symbol number_list
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $4->insert($4->begin(), $3);
    $4->insert($4->begin(), $2);
    $4->insert($4->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"elpt"
                                ,*$4);
    delete $4;
}
 /*--------------------------------------------------------------------------*/
ACTIVATION_OPTION_KEY : FISSION | NONFISS | DN | DG | THRESH | DNBIAS | NAP
                      | DNEB | DPEB | PECUT | HLCUT | SAMPLE
 /*--------------------------------------------------------------------------*/
comma : COMMA { $$=interpreter.push_leaf(wasp::mcnpi::DATA_COMMA, "comma", $1);}
 /*--------------------------------------------------------------------------*/
comma_sep_list : PRIMITIVE
{
    auto v = interpreter.push_leaf(wasp::mcnpi::VALUE, "value", $1);
    $$ = new std::vector<size_t>();
    $$->push_back(v);
}
| comma_sep_list comma PRIMITIVE
{
    auto v = interpreter.push_leaf(wasp::mcnpi::VALUE, "value", $3);
    $$ = $1;
    $$->push_back($comma);
    $$->push_back(v);
}

 /*--------------------------------------------------------------------------*/
activation_option : ACTIVATION_OPTION_KEY assign comma_sep_list {
    auto d = interpreter.push_leaf(wasp::mcnpi::DECL,"decl",$1);
    $3->insert($3->begin(), $2);
    $3->insert($3->begin(), d);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,interpreter.token_data($1)
                                ,*$3);
    delete $3;
}
| ACTIVATION_OPTION_KEY  comma_sep_list {
    auto d = interpreter.push_leaf(wasp::mcnpi::DECL,"decl",$1);
    $2->insert($2->begin(), d);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,interpreter.token_data($1)
                                ,*$2);
    delete $2;
}

 /*--------------------------------------------------------------------------*/
 activation_options : activation_option {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | activation_options activation_option
 {
    $$ = $1;
    $$->push_back($2);
 }
 /*--------------------------------------------------------------------------*/
 // ACT KEYWORD=value(s) ...
data_activation_card :  ACTIVATION_OPTIONS activation_options
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $2->insert($2->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"act"
                                ,*$2);
    delete $2;
}
 /*--------------------------------------------------------------------------*/
data_fq_card :  FQ card_id string_list {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $3->insert($3->begin(), $2);
    $3->insert($3->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fq"
                                ,*$3);
    delete $3;
}
| FQ string_list {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $2->insert($2->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fq"
                                ,*$2);
    delete $2;
}
 /*--------------------------------------------------------------------------*/
data_fc_card :  FC card_id comment_list {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $comment_list->insert($comment_list->begin(), $card_id);
    $comment_list->insert($comment_list->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fc"
                                ,*$comment_list);
    delete $comment_list;
}
 /*--------------------------------------------------------------------------*/
data_sc_card :  SC card_id comment_list {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $comment_list->insert($comment_list->begin(), $card_id);
    $comment_list->insert($comment_list->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"sc"
                                ,*$comment_list);
    delete $comment_list;
}
 /*--------------------------------------------------------------------------*/
particle_symbol_list : particle_symbol {
    $$ = new std::vector<size_t>();
    $$->push_back($particle_symbol);
}
 /* NOTE: the lexer manages the particle_designator_state via the trailing COMMA
 changes to this logic will require coordinated changes in the lexer*/
| particle_symbol_list comma particle_symbol {
    $$ = $1;
    $$->push_back($comma);
    $$->push_back($particle_symbol);
}

 /*--------------------------------------------------------------------------*/
mode_particle_symbol_list : particle_symbol {
    $$ = new std::vector<size_t>();
    $$->push_back($particle_symbol);
}
| mode_particle_symbol_list particle_symbol {
    $$ = $1;
    $$->push_back($particle_symbol);
}
/*--------------------------------------------------------------------------*/
lt : LT
{
    $$ = interpreter.push_leaf(wasp::mcnpi::LT, "LT", $1);
}
/*--------------------------------------------------------------------------*/
rbracket : RBRACKET
{
    $$ = interpreter.push_leaf(wasp::mcnpi::RBRACKET, "RB", $1);
}
/*--------------------------------------------------------------------------*/
lbracket : LBRACKET
{
    $$ = interpreter.push_leaf(wasp::mcnpi::LBRACKET, "LB", $1);
}
 /*--------------------------------------------------------------------------*/
SURFACE_OR_CELL_TALLY : F1_TALLY | F2_TALLY | F4_TALLY | F7_TALLY | F8_TALLY /*F6 is special*/
/*--------------------------------------------------------------------------*/
tally_surface_list : number_or_sc
{
    $$ = new std::vector<size_t>();
    $$->push_back($1);
}
| number_or_sc  colon number_or_sc
{
    $$ = new std::vector<size_t>();
    $$->push_back($1);
    $$->push_back($2);
    $$->push_back($3);
}
| lparen tally_surface_list rparen
{
    $$ = $2;
    $$->insert($$->begin(), $1);
    $$->push_back($3);
}
| tally_surface_list number_or_sc
{
    $$ = $1;
    $$->push_back($2);
}
| tally_surface_list number_or_sc colon number_or_sc
{
    $$ = $1;
    $$->push_back($2);
    $$->push_back($3);
    $$->push_back($4);
}
| tally_surface_list lparen tally_surface_list rparen
{
    $$ = $1;
    $$->insert($$->begin(), $2);
    $$->insert($$->end(), $3->begin(), $3->end());
    $$->push_back($4);
    delete $3;
}
| tally_surface_list lt number_or_sc
{
    $$ = $1;
    $$->push_back($2);
    $$->push_back($3);
}
| tally_surface_list lt lparen tally_surface_list rparen
{
    $$ = $1;
    $$->insert($$->end(), $2);
    $$->insert($$->end(), $3);
    $$->insert($$->end(), $4->begin(), $4->end());
    $$->push_back($5);
    delete $4;
}
| tally_surface_list lbracket tally_surface_list rbracket
{
    $$ = $1;
    $$->insert($$->end(), $2);
    $$->insert($$->end(), $3->begin(), $3->end());
    $$->push_back($4);
    delete $3;
}

/*--------------------------------------------------------------------------*/
f5_tally_member : NUMBER NUMBER NUMBER NUMBER
{
    auto x = interpreter.push_leaf(wasp::mcnpi::VALUE, "x", $1);
    auto y = interpreter.push_leaf(wasp::mcnpi::VALUE, "y", $2);
    auto z = interpreter.push_leaf(wasp::mcnpi::VALUE, "z", $3);
    auto r = interpreter.push_leaf(wasp::mcnpi::VALUE, "r", $4);

    $$ = interpreter.push_parent(wasp::mcnpi::TALLY_POINT
                                ,"point"
                                ,{x, y, z, r});
}
/*--------------------------------------------------------------------------*/
f5a_tally_member : NUMBER NUMBER NUMBER
{
    auto ao = interpreter.push_leaf(wasp::mcnpi::VALUE, "o", $1);
    auto r = interpreter.push_leaf(wasp::mcnpi::VALUE, "r", $2);
    auto ro = interpreter.push_leaf(wasp::mcnpi::VALUE, "ro", $3);

    $$ = interpreter.push_parent(wasp::mcnpi::TALLY_POINT
                                ,"point"
                                ,{ao, r, ro});
}
/*--------------------------------------------------------------------------*/
f5_tally_members : f5_tally_member
{
    $$ = new std::vector<size_t>();
    $$->push_back($1);
}
| f5_tally_members f5_tally_member
{
    $$ = $1;
    $$->push_back($2);
}
/*--------------------------------------------------------------------------*/
f5a_tally_members : f5a_tally_member
{
    $$ = new std::vector<size_t>();
    $$->push_back($1);
}
| f5a_tally_members f5a_tally_member
{
    $$ = $1;
    $$->push_back($2);
}

/*--------------------------------------------------------------------------*/
data_ftally_card :  SURFACE_OR_CELL_TALLY card_id colon particle_symbol_list tally_surface_list {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);

    $4->insert($4->begin(), $3);
    $4->insert($4->begin(), $card_id);
    $4->insert($4->begin(), decl);
    $4->insert($4->end(), $5->begin(), $5->end());
    std::string tally_name = interpreter.data($2);
    // we only care about the last number of the INTEGER which dictates the type
    tally_name = tally_name.substr(tally_name.size()-1);
    tally_name += "_tally";
    // create Fn_TALLY
    tally_name = "f"+tally_name;
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,tally_name.data()
                                ,*$4);
    delete $4;
    delete $5;
}
| SURFACE_OR_CELL_TALLY card_id colon particle_symbol_list tally_surface_list STRING {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $4->insert($4->begin(), $3);
    $4->insert($4->begin(), $card_id);
    $4->insert($4->begin(), decl);
    $4->insert($4->end(), $5->begin(), $5->end());
    auto T = interpreter.push_leaf(wasp::mcnpi::FLAG, "accumulator", $STRING);
    $4->push_back(T);
    std::string tally_name = interpreter.data($2);
    tally_name = tally_name.substr(tally_name.size()-1);
    tally_name += "_tally";
    tally_name = "f"+tally_name;
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,tally_name.data()
                                ,*$4);
    delete $4;
    delete $5;
}
| F6_TALLY card_id tally_surface_list STRING {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $3->insert($3->begin(), $card_id);
    $3->insert($3->begin(), decl);
    auto T = interpreter.push_leaf(wasp::mcnpi::FLAG, "accumulator", $STRING);
    $3->push_back(T);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"f6_tally"
                                ,*$3);
    delete $3;
}
| F6_TALLY card_id tally_surface_list {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $3->insert($3->begin(), $card_id);
    $3->insert($3->begin(), decl);

    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"f6_tally"
                                ,*$3);
    delete $3;
}
| F6_TALLY card_id colon particle_symbol_list tally_surface_list {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $4->insert($4->begin(), $3);
    $4->insert($4->begin(), $card_id);
    $4->insert($4->begin(), decl);
    $4->insert($4->end(), $5->begin(), $5->end());
    std::string tally_name = interpreter.data($2);

    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"f6_tally"
                                ,*$4);
    delete $4;
    delete $5;
}
| F6_TALLY card_id colon particle_symbol_list tally_surface_list STRING {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $4->insert($4->begin(), $3);
    $4->insert($4->begin(), $card_id);
    $4->insert($4->begin(), decl);
    $4->insert($4->end(), $5->begin(), $5->end());
    auto T = interpreter.push_leaf(wasp::mcnpi::FLAG, "accumulator", $STRING);
    $4->push_back(T);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"f6_tally"
                                ,*$4);
    delete $4;
    delete $5;
}
| F5_TALLY card_id colon particle_symbol f5_tally_members
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);

    $5->insert($5->begin(), $particle_symbol);
    $5->insert($5->begin(), $colon);
    $5->insert($5->begin(), $card_id);
    $5->insert($5->begin(), decl);

    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"f5_tally"
                                ,*$5);
    delete $5;
}
| F5_TALLY card_id colon particle_symbol f5_tally_members STRING
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto nd = interpreter.push_leaf(wasp::mcnpi::FLAG, "nd", $6);
    $5->insert($5->begin(), $particle_symbol);
    $5->insert($5->begin(), $colon);
    $5->insert($5->begin(), $card_id);
    $5->insert($5->begin(), decl);
    $5->push_back(nd);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"f5_tally"
                                ,*$5);
    delete $5;
}
| F5A_TALLY card_id STRING colon particle_symbol f5a_tally_members
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto axis = interpreter.push_leaf(wasp::mcnpi::AXIS, "axis", $3);

    auto m = $f5a_tally_members;
    m->insert(m->begin(), $particle_symbol);
    m->insert(m->begin(), $colon);
    m->insert(m->begin(), axis);
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);

    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"f5a_tally"
                                ,*m);
    delete m;
}
| F5A_TALLY card_id STRING colon particle_symbol f5a_tally_members STRING
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto axis = interpreter.push_leaf(wasp::mcnpi::AXIS, "axis", $3);
    auto nd = interpreter.push_leaf(wasp::mcnpi::FLAG, "nd", $7);

    auto m = $f5a_tally_members;
    m->insert(m->begin(), $particle_symbol);
    m->insert(m->begin(), $colon);
    m->insert(m->begin(), axis);
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);

    m->push_back(nd);

    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"f5a_tally"
                                ,*m);
    delete m;
}
| FA5_TALLY card_id colon particle_symbol f5a_tally_members STRING
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto nd = interpreter.push_leaf(wasp::mcnpi::FLAG, "nd", $6);

    auto m = $f5a_tally_members;
    m->insert(m->begin(), $particle_symbol);
    m->insert(m->begin(), $colon);
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);

    m->push_back(nd);

    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fa5_tally"
                                ,*m);
    delete m;
}
| FA5_TALLY card_id colon particle_symbol f5a_tally_members
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);

    auto m = $f5a_tally_members;
    m->insert(m->begin(), $particle_symbol);
    m->insert(m->begin(), $colon);
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);

    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fa5_tally"
                                ,*m);
    delete m;
}
| FI_TALLY card_id colon particle_symbol
                             NUMBER NUMBER NUMBER
                             NUMBER
                             NUMBER NUMBER NUMBER
                             NUMBER NUMBER NUMBER {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);

    auto x1 = interpreter.push_leaf(wasp::mcnpi::VALUE, "x1", $5);
    auto y1 = interpreter.push_leaf(wasp::mcnpi::VALUE, "y1", $6);
    auto z1 = interpreter.push_leaf(wasp::mcnpi::VALUE, "z1", $7);

    auto ro = interpreter.push_leaf(wasp::mcnpi::VALUE, "ro", $8);

    auto x2 = interpreter.push_leaf(wasp::mcnpi::VALUE, "x2", $9);
    auto y2 = interpreter.push_leaf(wasp::mcnpi::VALUE, "y2", $10);
    auto z2 = interpreter.push_leaf(wasp::mcnpi::VALUE, "z2", $11);

    auto f1 = interpreter.push_leaf(wasp::mcnpi::VALUE, "f1", $12);
    auto f2 = interpreter.push_leaf(wasp::mcnpi::VALUE, "f2", $13);
    auto f3 = interpreter.push_leaf(wasp::mcnpi::VALUE, "f3", $14);

    std::string tally_name = interpreter.token_data($1);
    tally_name += "_tally";
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,tally_name.data()
                                ,{decl, $card_id, $colon, $particle_symbol,
                                   x1, y1, z1, ro, x2, y2, z2, f1, f2, f3});
}
| ASTERISK data_ftally_card
{
    auto energy_x_weight_flag = interpreter.push_leaf(wasp::mcnpi::FLAG, "exw", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"exw_tally"
                                ,{energy_x_weight_flag, $2});
}
| PLUS data_ftally_card
{
    auto jerks_flag = interpreter.push_leaf(wasp::mcnpi::FLAG, "jflag", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"jerks_tally"
                                ,{jerks_flag, $2});
}
 /*--------------------------------------------------------------------------*/
 data_mode_card : MODE mode_particle_symbol_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $2->insert($2->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"problem_type"
                                ,*$2);
    delete $2;
 }
  /*--------------------------------------------------------------------------*/
TALLY_TIME_OPTION_KEY : CBEG | CFRQ | COFI | CONI | CSUB | CEND
tally_time_option : TALLY_TIME_OPTION_KEY assign  number
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,interpreter.token_data($1)
                                ,{decl, $2, $3});
}
| TALLY_TIME_OPTION_KEY number
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,interpreter.token_data($1)
                                ,{decl, $2});
}
 /*--------------------------------------------------------------------------*/
tally_time_options : tally_time_option
{
    $$ = new std::vector<size_t>();
    $$->push_back($1);
}
| tally_time_options tally_time_option
{
    $$ = $1;
    $$->push_back($2);
}
  /*--------------------------------------------------------------------------*/
 data_tally_time_card : TALLY_TIME card_id number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_time"
                                ,*m);
    delete m;
 }
 | TALLY_TIME card_id number_list STRING STRING
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);

    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);

    std::string o1 = interpreter.token_data($4);
    std::string o2 = interpreter.token_data($5);
    if (o1.size() > 1)
        o1 = "no_total";
    else o1 = "cumulative";

    if (o2.size() > 1)
        o2 = "no_total";
    else o2 = "cumulative";

    auto o1i = interpreter.push_leaf(wasp::mcnpi::DECL, o1.data(), $4);
    auto o2i = interpreter.push_leaf(wasp::mcnpi::DECL, o2.data(), $5);


    m->push_back(o1i);
    m->push_back(o2i);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_time"
                                ,*m);
    delete m;
 }
 | TALLY_TIME card_id number_list STRING
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);

    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);

    std::string o1 = interpreter.token_data($4);

    if (o1.size() > 1)
        o1 = "no_total";
    else o1 = "cumulative";

    auto o1i = interpreter.push_leaf(wasp::mcnpi::DECL, o1.data(), $4);

    m->push_back(o1i);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_time"
                                ,*m);
    delete m;
 }
 | TALLY_TIME card_id tally_time_options
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $tally_time_options;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_time"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
data_kcode_card : KCODE number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    static std::vector<std::string> names = {"nsrck", "rkk", "ikz", "kct", "msrk", "knrm", "mrkp", "kc8"};
    static std::vector<mcnpi::NODE> types = {mcnpi::VALUE, mcnpi::VALUE, mcnpi::VALUE, mcnpi::VALUE,
                                             mcnpi::VALUE, mcnpi::VALUE, mcnpi::VALUE, mcnpi::VALUE};
    std::vector<size_t> children;
    children.push_back(decl);
    assign_names(interpreter, names, types, $number_token_list, children);

    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"kcode"
                                ,children);
    delete $number_token_list;
 }
 /*--------------------------------------------------------------------------*/
data_nps_card : NPS number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    static std::vector<std::string> names = {"npp", "npsmg"};
    static std::vector<mcnpi::NODE> types = {mcnpi::VALUE, mcnpi::VALUE};
    std::vector<size_t> children;
    children.push_back(decl);
    assign_names(interpreter, names, types, $number_token_list, children);

    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"nps"
                                ,children);
    delete $number_token_list;
 }
  /*--------------------------------------------------------------------------*/
 data_print_card : PRINT number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"print"
                                ,*m);
    delete m;
 }
 | PRINT
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"print"
                                ,{decl});
 }
  /*--------------------------------------------------------------------------*/
 data_talnp_card : TALNP number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"talnp"
                                ,*m);
    delete m;
 }
 | TALNP
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"talnp"
                                ,{decl});
 }
 /*--------------------------------------------------------------------------*/
data_prdmp_card : PRDMP number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    static std::vector<std::string> names = {"ndp", "ndm", "mct", "ndmp", "dmmp"};
    static std::vector<mcnpi::NODE> types = {mcnpi::VALUE, mcnpi::VALUE, mcnpi::VALUE, mcnpi::VALUE, mcnpi::VALUE};
    std::vector<size_t> children;
    children.push_back(decl);
    assign_names(interpreter, names, types, $number_token_list, children);

    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"prdmp"
                                ,children);
    delete $number_token_list;
 }

 /*--------------------------------------------------------------------------*/
data_mt_card : MT_CARD card_id string_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $string_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"mt"
                                ,*m);
    delete m;
 }
 | MT_CARD card_id
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"mt"
                                ,{decl, $card_id});
 }
  /*--------------------------------------------------------------------------*/
data_imp_card : IMP colon particle_symbol_list number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $particle_symbol_list;
    m->insert(m->begin(), $colon);
    m->insert(m->begin(), decl);
    m->insert(m->end(), $number_list->begin(), $number_list->end());
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"imp"
                                ,*m);
    delete m;
    delete $number_list;
 }
 /*--------------------------------------------------------------------------*/
data_ksrc_card : KSRC number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ksrc"
                                ,*m);
    delete m;
 }

  /*--------------------------------------------------------------------------*/
 data_tally_energy_card : TALLY_ENERGY card_id number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_energy"
                                ,*m);
    delete m;
 }
 | TALLY_ENERGY card_id number_list STRING STRING
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);

    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);

    std::string o1 = interpreter.token_data($4);
    std::string o2 = interpreter.token_data($5);
    if (o1.size() > 1)
        o1 = "no_total";
    else o1 = "cumulative";

    if (o2.size() > 1)
        o2 = "no_total";
    else o2 = "cumulative";

    auto o1i = interpreter.push_leaf(wasp::mcnpi::DECL, o1.data(), $4);
    auto o2i = interpreter.push_leaf(wasp::mcnpi::DECL, o2.data(), $5);


    m->push_back(o1i);
    m->push_back(o2i);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_energy"
                                ,*m);
    delete m;
 }
 | TALLY_ENERGY card_id number_list STRING
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);

    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);

    std::string o1 = interpreter.token_data($4);

    if (o1.size() > 1)
        o1 = "no_total";
    else o1 = "cumulative";

    auto o1i = interpreter.push_leaf(wasp::mcnpi::DECL, o1.data(), $4);

    m->push_back(o1i);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_energy"
                                ,*m);
    delete m;
 }
 | TALLY_ENERGY card_id colon particle_symbol number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);

    auto m = $number_list;
    m->insert(m->begin(), $particle_symbol);
    m->insert(m->begin(), $colon);
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);

    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_energy"
                                ,*m);
    delete m;
 }
 | TALLY_ENERGY INTEGER tally_time_options
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto id = interpreter.push_leaf(wasp::mcnpi::ID, "id", $2);
    auto m = $tally_time_options;
    m->insert(m->begin(), id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_energy"
                                ,*m);
    delete m;
 }

  /*--------------------------------------------------------------------------*/
 data_tally_multiplier_card : TALLY_MULTIPLIER card_id number_paren_colon_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_paren_colon_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_multiplier"
                                ,*m);
    delete m;
 }
 | TALLY_MULTIPLIER card_id number_paren_colon_list STRING STRING
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);

    auto m = $number_paren_colon_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);

    std::string o1 = interpreter.token_data($4);
    std::string o2 = interpreter.token_data($5);
    if (o1.size() > 1)
        o1 = "no_total";
    else o1 = "cumulative";

    if (o2.size() > 1)
        o2 = "no_total";
    else o2 = "cumulative";

    auto o1i = interpreter.push_leaf(wasp::mcnpi::DECL, o1.data(), $4);
    auto o2i = interpreter.push_leaf(wasp::mcnpi::DECL, o2.data(), $5);


    m->push_back(o1i);
    m->push_back(o2i);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_multiplier"
                                ,*m);
    delete m;
 }
 | TALLY_MULTIPLIER card_id number_paren_colon_list STRING
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);

    auto m = $number_paren_colon_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);

    std::string o1 = interpreter.token_data($4);

    if (o1.size() > 1)
        o1 = "no_total";
    else o1 = "cumulative";

    auto o1i = interpreter.push_leaf(wasp::mcnpi::DECL, o1.data(), $4);

    m->push_back(o1i);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_multiplier"
                                ,*m);
    delete m;
 }
 | PLUS data_tally_multiplier_card
 {
    auto f = interpreter.push_leaf(wasp::mcnpi::FLAG, "flag", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ftm"
                                ,{f, $2});
 }
 /*--------------------------------------------------------------------------*/
 data_sd_card : SEGMENT_DIVISOR card_id number_paren_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_paren_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"sd"
                                ,*m);
    delete m;
 }
  /*--------------------------------------------------------------------------*/

SDEF_SCALAR_VALUE_KEY :  SDEF_SUR | ERG | SDEF_TME | SDEF_DIR
                       | SDEF_NRM | SDEF_RAD | EXT | SDEF_CCC | SDEF_ARA
                       | WGT | TR | SDEF_EFF | SDEF_X | SDEF_Y | SDEF_Z | SDEF_PAR

SDEF_LIST_VALUE_KEY :  SDEF_DAT | SDEF_LOC | SDEF_BEM | SDEF_BAP | SDEF_POS | VEC | AXS | CEL
/*--------------------------------------------------------------------------*/
sdef_value : value_list_item
| SDEF_PREFIXED_VAR assign value
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    name = name.substr(1); // remove 'F' from prefix to obtain variable name
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                ,name.data()
                                ,{decl, $assign, $value});
}
| SDEF_PREFIXED_VAR value
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    name = name.substr(1); // remove 'F' from prefix to obtain variable name
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                ,name.data()
                                ,{decl, $value});
}
sdef_value_list : sdef_value
{
    $$ = new std::vector<size_t> ();
    $$->push_back($1);
}
| sdef_value_list sdef_value
{
    $$ = $1;
    $$->push_back($2);
}


/*--------------------------------------------------------------------------*/
data_sdef_member : SDEF_SCALAR_VALUE_KEY assign sdef_value
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, $assign, $sdef_value});
 }
 |SDEF_LIST_VALUE_KEY assign sdef_value_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $sdef_value_list;
    m->insert(m->begin(), $assign);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 |SDEF_SCALAR_VALUE_KEY sdef_value
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, $sdef_value});
 }
 |SDEF_LIST_VALUE_KEY sdef_value_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $sdef_value_list;
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
  /*--------------------------------------------------------------------------*/
data_sdef_members : data_sdef_member
{
    $$ = new std::vector<size_t>();
    $$->push_back($1);
}
| data_sdef_members data_sdef_member
{
    $$ = $1;
    $$->push_back($2);
}
  /*--------------------------------------------------------------------------*/
 data_sdef_card : GENERAL_SOURCE
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"sdef"
                                ,{decl});
 }
 | GENERAL_SOURCE data_sdef_members
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);

    auto m = $data_sdef_members;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"sdef"
                                ,*m);
    delete m;
 }

  /*--------------------------------------------------------------------------*/
 data_si_card : SOURCE_INFO card_id STRING value_paren_colon_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto op = interpreter.push_leaf(wasp::mcnpi::DECL, "option", $3);
    auto m = $value_paren_colon_list;
    m->insert(m->begin(), op);
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"si"
                                ,*m);
    delete m;
 }
 | SOURCE_INFO card_id value_paren_colon_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $value_paren_colon_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"si"
                                ,*m);
    delete m;
 }
  /*--------------------------------------------------------------------------*/
 data_sp_card : SOURCE_PROB card_id number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
    // check for 'built-in' function flag '-f' (negative integer)
    auto fi = t->front();
    std::vector<size_t> m = {decl, $card_id};
    if (interpreter.token_data(fi)[0] == '-')
    {
        m.push_back(interpreter.push_leaf(wasp::mcnpi::FLAG, "function", fi));
        t->erase(t->begin()); // remove the function token
    }
    for (auto ti : *t)
    {
        m.push_back(interpreter.push_leaf(wasp::mcnpi::VALUE, "value", ti));
    }

    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "sp", m);
    delete t;
 }
 | SOURCE_PROB card_id STRING number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto op = interpreter.push_leaf(wasp::mcnpi::DECL, "option", $3);
    auto m = $number_list;
    m->insert(m->begin(), op);
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"sp"
                                ,*m);
    delete m;
 }
 | SOURCE_PROB card_id STRING
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto op = interpreter.push_leaf(wasp::mcnpi::DECL, "option", $3);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"sp"
                                ,{decl, $card_id, op});
 }
  /*--------------------------------------------------------------------------*/
 data_sb_card : SOURCE_BIAS card_id number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
    // check for 'built-in' function flag '-f' (negative integer)
    auto fi = t->front();
    std::vector<size_t> m = {decl, $card_id};
    if (interpreter.token_data(fi)[0] == '-')
    {
        m.push_back(interpreter.push_leaf(wasp::mcnpi::FLAG, "function", fi));
        t->erase(t->begin()); // remove the function token
    }
    for (auto ti : *t)
    {
        m.push_back(interpreter.push_leaf(wasp::mcnpi::VALUE, "value", ti));
    }

    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "sb", m);
    delete t;
 }
 | SOURCE_BIAS card_id STRING number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto op = interpreter.push_leaf(wasp::mcnpi::DECL, "option", $3);
    auto m = $number_list;
    m->insert(m->begin(), op);
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"sb"
                                ,*m);
    delete m;
 }
/*--------------------------------------------------------------------------*/
data_totnu_card : TOTNU
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"totnu"
                                ,{decl});
 }
 | TOTNU NO
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto n = interpreter.push_leaf(wasp::mcnpi::FLAG, "no", $2);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"totnu"
                                ,{decl, n});
 }

   /*--------------------------------------------------------------------------*/
 data_de_card : DOSE_ENERGY card_id number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_list;
    t->insert(t->begin(),$card_id);
    t->insert(t->begin(),decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "de", *t);
    delete t;
 }
 | DOSE_ENERGY card_id STRING number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto interp = interpreter.push_leaf(wasp::mcnpi::FLAG, "interp", $3);
    auto t = $number_list;
    t->insert(t->begin(),interp);
    t->insert(t->begin(),$card_id);
    t->insert(t->begin(),decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "de", *t);
    delete t;
 }
/*--------------------------------------------------------------------------*/
 data_df_card : DOSE_FUNCTION card_id number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_list;
    t->insert(t->begin(),$card_id);
    t->insert(t->begin(),decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "de", *t);
    delete t;
 }
 | DOSE_FUNCTION card_id STRING number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto interp = interpreter.push_leaf(wasp::mcnpi::FLAG, "interp", $3);
    auto t = $number_list;
    t->insert(t->begin(),interp);
    t->insert(t->begin(),$card_id);
    t->insert(t->begin(),decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "df", *t);
    delete t;
 }
 /*--------------------------------------------------------------------------*/
 data_tmp_card : TMP card_id number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_list;
    t->insert(t->begin(),$card_id);
    t->insert(t->begin(),decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "tmp", *t);
    delete t;
 }
 | TMP number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_list;
    t->insert(t->begin(),decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "tmp", *t);
    delete t;
 }
  /*--------------------------------------------------------------------------*/
 data_fs_card : TALLY_SEGMENT card_id number_list STRING STRING
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_list;
    t->insert(t->begin(),$card_id);
    t->insert(t->begin(),decl);

    std::string o1 = interpreter.token_data($4);
    std::string o2 = interpreter.token_data($5);
    if (o1.find_first_of("tT") == std::string::npos)
        o1 = "total";
    else o1 = "cumulative";

    if (o2.find_first_of("tT") == std::string::npos)
        o2 = "total";
    else o2 = "cumulative";

    auto o1i = interpreter.push_leaf(wasp::mcnpi::FLAG, o1.data(), $4);
    auto o2i = interpreter.push_leaf(wasp::mcnpi::FLAG, o2.data(), $5);

    t->push_back(o1i);
    t->push_back(o2i);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "fs", *t);
    delete t;
 }
 | TALLY_SEGMENT card_id number_list STRING
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_list;
    t->insert(t->begin(),$card_id);
    t->insert(t->begin(),decl);

    std::string o1 = interpreter.token_data($4);
    if (o1.find_first_of("tT") == std::string::npos)
        o1 = "total";
    else o1 = "cumulative";

    auto o1i = interpreter.push_leaf(wasp::mcnpi::FLAG, o1.data(), $4);

    t->push_back(o1i);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "fs", *t);
    delete t;
 }
 | TALLY_SEGMENT card_id number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_list;
    t->insert(t->begin(),$card_id);
    t->insert(t->begin(),decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "fs", *t);
    delete t;
 }
  | TALLY_SEGMENT card_id colon particle_symbol number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_list;
    t->insert(t->begin(),$particle_symbol);
    t->insert(t->begin(),$colon);
    t->insert(t->begin(),$card_id);
    t->insert(t->begin(),decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "fs", *t);
    delete t;
 }
 | TALLY_SEGMENT number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_list;
    t->insert(t->begin(),decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "fs", *t);
    delete t;
 }
  /*--------------------------------------------------------------------------*/
 data_tf_card : TALLY_FLUCTUATION card_id number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_list;
    t->insert(t->begin(),$card_id);
    t->insert(t->begin(),decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "tf", *t);
    delete t;
 }
   /*--------------------------------------------------------------------------*/
 data_lost_card : LOST number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_list;
    t->insert(t->begin(),decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "lost", *t);
    delete t;
 }
 /*--------------------------------------------------------------------------*/
data_hsrc_card : HSRC number_token_list
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::vector<size_t> children = {decl};
    auto t = $number_token_list;
    assign_names(interpreter,
                            {"nx", "xmin", "xmax",
                            "ny", "ymin", "ymax",
                            "nz", "zmin", "zmax"},
                            {mcnpi::VALUE, mcnpi::VALUE, mcnpi::VALUE,
                            mcnpi::VALUE, mcnpi::VALUE, mcnpi::VALUE,
                            mcnpi::VALUE, mcnpi::VALUE, mcnpi::VALUE}, t, children);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "hsrc", children);
    delete t;
}
 /*--------------------------------------------------------------------------*/
data_xs_card : XS card_id material_zaid_entry value_token_list
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::vector<size_t> children = {decl,$card_id, $3};
    auto t = $value_token_list;

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
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "xs", children);
    delete t;
}
 /*--------------------------------------------------------------------------*/
data_phys_card : PHYS colon particle_symbol number_token_list
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::vector<size_t> children = {decl,$colon,$particle_symbol};
    auto t = $number_token_list;
    std::string ps = interpreter.data($particle_symbol);
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
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "phys", children);
    delete t;
}
 /*--------------------------------------------------------------------------*/
KOPTS_OPTION :  BLOCKSIZE | KINETICS | PRECURSOR | KSENTAL | FMAT
                  | FMATSKIP | FMATSPACE | FMATACCEL | FMATREDUCE | FMATNX
                  | FMATNY | FMATNZ
/*--------------------------------------------------------------------------*/
data_kopts_option : KOPTS_OPTION value
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, name.data(), {decl, $value});
}
|KOPTS_OPTION assign value
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, name.data(), {decl, $assign, $value});
}
/*--------------------------------------------------------------------------*/
data_kopts_options : data_kopts_option
{
    $$ = new std::vector<size_t> ();
    $$->push_back($1);
}
| data_kopts_options data_kopts_option
{
    $$ = $1;
    $$->push_back($2);
}

 /*--------------------------------------------------------------------------*/
data_kopts_card : KOPTS data_kopts_options
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $data_kopts_options;
    t->insert(t->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "kopts", *t);
    delete t;
}

 /*--------------------------------------------------------------------------*/
 data_tally_cosine_card : TALLY_COSINE card_id number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_cosine"
                                ,*m);
    delete m;
 }
 | TALLY_COSINE card_id number_list STRING STRING
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);

    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);

    std::string o1 = interpreter.token_data($4);
    std::string o2 = interpreter.token_data($5);
    if (o1.size() > 1)
        o1 = "no_total";
    else o1 = "cumulative";

    if (o2.size() > 1)
        o2 = "no_total";
    else o2 = "cumulative";

    auto o1i = interpreter.push_leaf(wasp::mcnpi::DECL, o1.data(), $4);
    auto o2i = interpreter.push_leaf(wasp::mcnpi::DECL, o2.data(), $5);


    m->push_back(o1i);
    m->push_back(o2i);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_cosine"
                                ,*m);
    delete m;
 }
 | TALLY_COSINE card_id number_list STRING
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);

    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);

    std::string o1 = interpreter.token_data($4);

    if (o1.size() > 1)
        o1 = "no_total";
    else o1 = "cumulative";

    auto o1i = interpreter.push_leaf(wasp::mcnpi::DECL, o1.data(), $4);

    m->push_back(o1i);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_cosine"
                                ,*m);
    delete m;
 }
 | ASTERISK data_tally_cosine_card
 {
    auto a = interpreter.push_leaf(wasp::mcnpi::FLAG, "degrees", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"c_degrees"
                                ,{a, $2});
 }
  /*--------------------------------------------------------------------------*/
data_mgopt_card : MGOPT STRING number_token_list
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto mcal = interpreter.push_leaf(wasp::mcnpi::FLAG, "mcal", $2);
    std::vector<size_t> children = {decl, mcal};
    auto t = $number_token_list;
    std::vector<std::string> node_names = {"igm", "iplt", "isb", "icw", "fnw", "rim"};
    std::vector<mcnpi::NODE> node_types(node_names.size(), wasp::mcnpi::VALUE);
    assign_names(interpreter, node_names, node_types, t, children);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "mgopt", children);
    delete t;
}

 /*--------------------------------------------------------------------------*/
RAND_OPTION :  GEN | SEED | STRIDE | HIST
/*--------------------------------------------------------------------------*/
data_rand_card_option : RAND_OPTION value
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, name.data(), {decl, $value});
}
|RAND_OPTION assign value
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, name.data(), {decl, $assign, $value});
}
/*--------------------------------------------------------------------------*/
data_rand_card_options : data_rand_card_option
{
    $$ = new std::vector<size_t> ();
    $$->push_back($1);
}
| data_rand_card_options data_rand_card_option
{
    $$ = $1;
    $$->push_back($2);
}

 /*--------------------------------------------------------------------------*/
data_rand_card : RAND data_rand_card_options
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $data_rand_card_options;
    t->insert(t->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "rand", *t);
    delete t;
}
| RAND
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM, "rand", {decl});
}
/*--------------------------------------------------------------------------*/
 data_wwe_card : WWE colon particle_symbol number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $4->insert($4->begin(), $3);
    $4->insert($4->begin(), $2);
    $4->insert($4->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"wwe"
                                ,*$4);
    delete $4;
 }
 /*--------------------------------------------------------------------------*/
 data_wwg_card : WWG number_token_list
 {
    // it ic wg J J J J iE
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    static std::vector<std::string> names = {"it", "ic", "wg", "unused", "unused", "unused", "unused", "iE"};
    static std::vector<mcnpi::NODE> types(names.size(), wasp::mcnpi::VALUE);
    std::vector<size_t> children;
    children.push_back(decl);
    assign_names(interpreter, names, types, $number_token_list, children);

    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"wwg"
                                ,children);
    delete $number_token_list;
 }
/*--------------------------------------------------------------------------*/
 data_wwge_card : WWGE colon particle_symbol number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $4->insert($4->begin(), $3);
    $4->insert($4->begin(), $2);
    $4->insert($4->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"wwge"
                                ,*$4);
    delete $4;
 }
 /*--------------------------------------------------------------------------*/
 data_wwgt_card : WWGT colon particle_symbol number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $4->insert($4->begin(), $3);
    $4->insert($4->begin(), $2);
    $4->insert($4->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"wwgt"
                                ,*$4);
    delete $4;
 }
 /*--------------------------------------------------------------------------*/
 data_wwn_card : WWN card_id colon particle_symbol number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);

    $5->insert($5->begin(), $particle_symbol);
    $5->insert($5->begin(), $colon);
    $5->insert($5->begin(), $card_id);
    $5->insert($5->begin(), decl);

    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"wwn"
                                ,*$5);
    delete $5;
 }
 |WWN colon particle_symbol number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);

    $4->insert($4->begin(), $particle_symbol);
    $4->insert($4->begin(), $colon);
    $4->insert($4->begin(), decl);

    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"wwn"
                                ,*$4);
    delete $4;
 }
 /*--------------------------------------------------------------------------*/
 data_wwp_card : WWP colon particle_symbol_list number_token_list
 {
    // wupn wsurvn mxspln mwhere switchn mtime wnorm etsplt wu
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $particle_symbol_list;
    m->insert(m->begin(), $colon);
    m->insert(m->begin(), decl);
    static std::vector<std::string> names = {"wupn", "wsurvn", "mxspln", "mwhere", "switchn", "mtime", "wnorm", "etsplt", "wu"};
    static std::vector<mcnpi::NODE> types(names.size(), wasp::mcnpi::VALUE);
    assign_names(interpreter, names, types, $number_token_list, *m);

    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"wwp"
                                ,*m);
    delete m;
    delete $number_token_list;
 }
 /*--------------------------------------------------------------------------*/
 data_wwt_card : WWT colon particle_symbol number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $4->insert($4->begin(), $3);
    $4->insert($4->begin(), $2);
    $4->insert($4->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"wwt"
                                ,*$4);
    delete $4;
 }
  /*--------------------------------------------------------------------------*/
 data_dbcn_card : DBCN number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::vector<size_t> children; children.push_back(decl);
    auto t = $number_token_list;
    std::vector<std::string> node_names(100); for (int i = 1; i <= 100; i++)node_names[i-1] = "x"+std::to_string(i);
    std::vector<wasp::mcnpi::NODE> node_types(node_names.size(), mcnpi::VALUE);
    assign_names(interpreter, node_names, node_types, t, children);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"dbcn"
                                ,children);
    delete t;
 }
 /*--------------------------------------------------------------------------*/
 data_fcl_card : FCL colon particle_symbol number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $4->insert($4->begin(), $3);
    $4->insert($4->begin(), $2);
    $4->insert($4->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fcl"
                                ,*$4);
    delete $4;
 }
 /*--------------------------------------------------------------------------*/
 data_var_card : VAR VAR_RR assign value
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto rd = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $2);
    auto rr = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE, "rr", {rd, $assign, $value});
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"var"
                                ,{decl, rr});
}
| VAR VAR_RR value
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto rd = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $2);
    auto rr = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE, "rr", {rd, $value});
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"var"
                                ,{decl, rr});
}
 /*--------------------------------------------------------------------------*/
data_cf_card : CELL_FLAGGING card_id number_list {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $3->insert($3->begin(), $card_id);
    $3->insert($3->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"cf"
                                ,*$3);
    delete $3;
}
/*--------------------------------------------------------------------------*/
 /* TODO improve handlign of the QVm construct... current Q is seperate token */
 data_ext_card : EXT_CARD colon particle_symbol value_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $value_token_list;
    std::vector<size_t> children = {decl, $2, $3};
    for (auto ti : *t) children.push_back(interpreter.push_leaf(wasp::mcnpi::VALUE, "value", ti));
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ext"
                                ,children);
    delete t;
 }

/*--------------------------------------------------------------------------*/
data_vect_member : VN number number number
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"vn"
                                ,{decl, $2, $2, $3});
}
/*--------------------------------------------------------------------------*/
data_vect_members : data_vect_member
{
    $$ = new std::vector<size_t>();
    $$->push_back($1);
}
| data_vect_members data_vect_member
{
    $$ = $1;
    $$->push_back($2);
}
  /*--------------------------------------------------------------------------*/
 data_vect_card : VECT data_vect_members
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $data_vect_members;
    t->insert(t->begin(), decl);

    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"vect"
                                ,*t);
    delete t;
 }
 /*--------------------------------------------------------------------------*/
 data_esplt_pair : NUMBER NUMBER
 {
    auto r = interpreter.push_leaf(wasp::mcnpi::VALUE, "r", $1);
    auto e = interpreter.push_leaf(wasp::mcnpi::VALUE, "e", $2);
    $$ = interpreter.push_parent(wasp::mcnpi::PAIR
                                ,"re"
                                ,{r,e});
 }
 /*--------------------------------------------------------------------------*/
 data_esplt_pairs : data_esplt_pair
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | data_esplt_pairs data_esplt_pair
 {
    $$ = $1;
    $$->push_back($2);
 }
 /*--------------------------------------------------------------------------*/
 data_esplt_card : ESPLT colon particle_symbol data_esplt_pairs
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $data_esplt_pairs;
    t->insert(t->begin(), $particle_symbol);
    t->insert(t->begin(), $colon);
    t->insert(t->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"esplt"
                                ,*t);
    delete t;
 }
 /*--------------------------------------------------------------------------*/
 data_tsplt_pair : NUMBER NUMBER
 {
    auto t = interpreter.push_leaf(wasp::mcnpi::VALUE, "t", $1);
    auto e = interpreter.push_leaf(wasp::mcnpi::VALUE, "e", $2);
    $$ = interpreter.push_parent(wasp::mcnpi::PAIR
                                ,"te"
                                ,{t,e});
 }
 /*--------------------------------------------------------------------------*/
 data_tsplt_pairs : data_tsplt_pair
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | data_tsplt_pairs data_tsplt_pair
 {
    $$ = $1;
    $$->push_back($2);
 }
 /*--------------------------------------------------------------------------*/
 data_tsplt_card : TSPLT colon particle_symbol data_tsplt_pairs
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $data_tsplt_pairs;
    t->insert(t->begin(), $particle_symbol);
    t->insert(t->begin(), $colon);
    t->insert(t->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tsplt"
                                ,*t);
    delete t;
 }
 /*--------------------------------------------------------------------------*/
 data_dd_card : DD number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_list;
    t->insert(t->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"dd"
                                ,*t);
    delete t;
 }
 | DD card_id number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_list;
    t->insert(t->begin(), $card_id);
    t->insert(t->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"dd"
                                ,*t);
    delete t;
}
 /*--------------------------------------------------------------------------*/
 data_dxt_card : DXT colon particle_symbol number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::vector<size_t> children = {decl, $colon, $particle_symbol};
    size_t i = 0;
    auto t = $number_token_list;
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
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"dxt"
                                ,children);
    delete $number_token_list;
 }
  /*--------------------------------------------------------------------------*/
 data_cm_card : CM card_id number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"cm"
                                ,*m);
    delete m;
 }
 | ASTERISK CM card_id number_list
 {
    auto f = interpreter.push_leaf(wasp::mcnpi::FLAG, "flag", $1);
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $2);
    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    m->insert(m->begin(), f);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"cm"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
FT_MEMBER_KEY : FRV | GEB|TMC|INC|ICD|SCX|SCD|ELC|PTT|PHL|CAP|RES|TAG|LET|ROC
            |PDS|FFT|COM
 data_ft_member : FT_MEMBER_KEY number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 | FT_MEMBER_KEY
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl});
 }
 /*--------------------------------------------------------------------------*/
 data_ft_members : data_ft_member
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | data_ft_members data_ft_member
 {
    $$ = $1;
    $$->push_back($2);
 }
 /*--------------------------------------------------------------------------*/
 data_ft_card : FT card_id data_ft_members
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $data_ft_members;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ft"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 data_sf_card : SF card_id number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"sf"
                                ,*m);
    delete m;
 }
  /*--------------------------------------------------------------------------*/
 data_ds_card : DS card_id STRING value_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto op = interpreter.push_leaf(wasp::mcnpi::DECL, "option", $3);
    auto m = $value_list;
    m->insert(m->begin(), op);
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ds"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 data_fu_card : FU card_id
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fu"
                                ,{decl, $card_id});
 }
 | FU card_id number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tally_time"
                                ,*m);
    delete m;
 }
 | FU card_id number_list STRING STRING
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);

    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);

    std::string o1 = interpreter.token_data($4);
    std::string o2 = interpreter.token_data($5);
    if (o1.size() > 1)
        o1 = "no_total";
    else o1 = "cumulative";

    if (o2.size() > 1)
        o2 = "no_total";
    else o2 = "cumulative";

    auto o1i = interpreter.push_leaf(wasp::mcnpi::DECL, o1.data(), $4);
    auto o2i = interpreter.push_leaf(wasp::mcnpi::DECL, o2.data(), $5);


    m->push_back(o1i);
    m->push_back(o2i);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fu"
                                ,*m);
    delete m;
 }
 | FU card_id number_list STRING
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);

    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);

    std::string o1 = interpreter.token_data($4);

    if (o1.size() > 1)
        o1 = "no_total";
    else o1 = "cumulative";

    auto o1i = interpreter.push_leaf(wasp::mcnpi::DECL, o1.data(), $4);

    m->push_back(o1i);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fu"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 data_em_card : EM card_id number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"em"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 data_bbrem_card : BBREM number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto t = $number_token_list;
    std::vector<size_t> children = {decl};
    std::vector<std::string> names(49); for(size_t i = 0; i < 49; ++i) names[i] = "b";
    for (size_t i = 0; i < t->size(); ++i) names.push_back("m");
    std::vector<mcnpi::NODE> nodes(names.size(), mcnpi::VALUE);
    assign_names(interpreter, names, nodes, t, children);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"bbrem"
                                ,children);
    delete t;
 }
 /*--------------------------------------------------------------------------*/
CORX : CORA | CORB| CORC
 tmesh_corx_card : CORX card_id number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 XMESH_FLAG_KEYWORD : TRAKS| FLUX | POPUL | PEDEP | TOTAL | DEDX | RECOL | TLEST | EDLCT
 /*--------------------------------------------------------------------------*/
 XMESH_VALUE_KEYWORD : MFACT | TRANS
 /*--------------------------------------------------------------------------*/
 tmesh_xmesh_member : XMESH_FLAG_KEYWORD
 {
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_leaf(wasp::mcnpi::FLAG, name.data(), $1);
 }
 | XMESH_VALUE_KEYWORD assign number
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::MATERIAL_PARAM
                                ,name.data()
                                ,{decl, $assign, $3});
}
| DOSE number_token_list
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    static std::vector<std::string> names = {"ic", "int", "iu", "fac"};
    static std::vector<mcnpi::NODE> types(names.size(), mcnpi::VALUE);
    std::vector<size_t> children = {decl};
    assign_names(interpreter, names, types, $number_token_list, children);

    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"dose"
                                ,children);
    delete $number_token_list;
}

 /*--------------------------------------------------------------------------*/
 tmesh_xmesh_members : tmesh_xmesh_member
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | tmesh_xmesh_members tmesh_xmesh_member
 {
    $$ = $1;
    $$->push_back($2);
 }
 /*--------------------------------------------------------------------------*/
 XMESH : CMESH | RMESH | SMESH
 /*--------------------------------------------------------------------------*/
 tmesh_xmesh_card : XMESH card_id colon particle_symbol tmesh_xmesh_members
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $tmesh_xmesh_members;
    m->insert(m->begin(), $particle_symbol);
    m->insert(m->begin(), $colon);
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 | XMESH card_id
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, $card_id});
 }
 | XMESH card_id tmesh_xmesh_members
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $tmesh_xmesh_members;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 data_tmesh_member : tmesh_corx_card  | tmesh_xmesh_card
 /*--------------------------------------------------------------------------*/
 data_tmesh_members : data_tmesh_member
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | data_tmesh_members data_tmesh_member
 {
    $$ =$1;
    $$->push_back($2);
 }
 /*--------------------------------------------------------------------------*/
 data_tmesh_card : TMESH data_tmesh_members ENDMD
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto term = interpreter.push_leaf(wasp::mcnpi::TERMINATOR, "endmd", $3);
    auto m = $data_tmesh_members;
    m->insert(m->begin(), decl);
    m->push_back(term);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tmesh"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
FMESH_KEY : GEOM | ORIGIN | AXS | VEC | IMESH | IINTS | JMESH | JINTS
          | KMESH | KINTS | EMESH | EINTS | ENORM | FMESH_TMESH | TINTS | TNORM
          | FACTOR | MCNP_OUT | TR | TYPE | KCLEAR | INC
/*--------------------------------------------------------------------------*/
 data_fmesh_member : FMESH_KEY assign value_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    auto m = $value_list;
    m->insert(m->begin(), $assign);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                ,name.data()
                                ,*m);
    delete m;
 }
 | FMESH_KEY value_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    auto m = $value_list;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                ,name.data()
                                ,*m);
    delete m;
  }
 /*--------------------------------------------------------------------------*/
 data_fmesh_members : data_fmesh_member
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | data_fmesh_members data_fmesh_member
 {
    $$ = $1;
    $$->push_back($2);
 }
 /*--------------------------------------------------------------------------*/
 data_fmesh_card : FMESH card_id colon particle_symbol data_fmesh_members
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $data_fmesh_members;
    m->insert(m->begin(), $particle_symbol);
    m->insert(m->begin(), $colon);
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fmesh"
                                ,*m);
    delete m;
 }
/*--------------------------------------------------------------------------*/
MESH_KEY : GEOM | ORIGIN | AXS | REF | VEC | IMESH | IINTS | JMESH | JINTS
          | KMESH | KINTS
/*--------------------------------------------------------------------------*/
 data_mesh_member : MESH_KEY assign value_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    auto m = $value_list;
    m->insert(m->begin(), $assign);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                ,name.data()
                                ,*m);
    delete m;
 }
 | MESH_KEY value_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    auto m = $value_list;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::KEYED_VALUE
                                ,name.data()
                                ,*m);
    delete m;
  }
 /*--------------------------------------------------------------------------*/
 data_mesh_members : data_mesh_member
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | data_mesh_members data_mesh_member
 {
    $$ = $1;
    $$->push_back($2);
 }
 /*--------------------------------------------------------------------------*/
 data_mesh_card : MESH data_mesh_members
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $data_mesh_members;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"mesh"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 data_thtme_card : THTME number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"thtme"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 data_tm_card : TM card_id number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tm"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 data_mx_card : MX card_id colon particle_symbol value_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $value_list;
    m->insert(m->begin(), $particle_symbol);
    m->insert(m->begin(), $colon);
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"mx"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 data_mphys_card : MPHYS STRING
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto f = interpreter.push_leaf(wasp::mcnpi::DECL, "flag", $2);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"mphys"
                                ,{decl, f});
 }
 /*--------------------------------------------------------------------------*/
 TROPT_KEY : MCSCAT | ELOSS | NREACT |  NESCAT | GENXS
 /*--------------------------------------------------------------------------*/
 data_tropt_member : TROPT_KEY assign value
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });

    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, $assign, $value});
 }
 | TROPT_KEY value
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });

    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, $value});
 }
 /*--------------------------------------------------------------------------*/
 data_tropt_members : data_tropt_member
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | data_tropt_members data_tropt_member
 {
    $$ = $1;
    $$->push_back($2);
 }
 /*--------------------------------------------------------------------------*/
 data_tropt_card : TROPT data_tropt_members
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $data_tropt_members;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"tropt"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 PERT_SCALAR_KEY : MAT | RHO | METHOD
 PERT_LIST_KEY : CEL | ERG | RXN
 /*--------------------------------------------------------------------------*/
 data_pert_member : PERT_LIST_KEY assign value_comma_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $value_comma_list;
    m->insert(m->begin(), $assign);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 | PERT_LIST_KEY value_comma_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $value_comma_list;
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 |PERT_SCALAR_KEY assign value
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, $assign, $value});
 }
 | PERT_SCALAR_KEY value
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, $value});
 }

 /*--------------------------------------------------------------------------*/
 data_pert_members : data_pert_member
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | data_pert_members data_pert_member
 {
    $$ = $1;
    $$->push_back($2);
 }
 /*--------------------------------------------------------------------------*/
 data_pert_card : PERT card_id colon particle_symbol_list data_pert_members
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $particle_symbol_list;
    m->insert(m->begin(), $colon);
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    auto m2 = $data_pert_members;
    m->insert(m->end(), m2->begin(), m2->end());
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"pert"
                                ,*m);
    delete m;
    delete m2;
 }
 /*--------------------------------------------------------------------------*/
 data_lca_card : LCA number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::vector<size_t> children = {decl};
    std::vector<std::string> names = {"ielas", "ipreq", "iexisa", "ichoic", "jcoul", "nexite", "npidk", "noact", "icem", "ilaq", "nevtype"};
    std::vector<mcnpi::NODE> types(names.size(), mcnpi::VALUE);
    assign_names(interpreter, names, types, $number_token_list, children);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"lca"
                                ,children);
    delete $number_token_list;
 }
 /*--------------------------------------------------------------------------*/
 data_lcb_card : LCB number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::vector<size_t> children = {decl};
    std::vector<std::string> names = {"flenb1", "flenb2", "flenb3", "flenb4", "flenb5", "flenb6", "ctofe", "flim0"};
    std::vector<mcnpi::NODE> types(names.size(), mcnpi::VALUE);
    assign_names(interpreter, names, types, $number_token_list, children);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"lcb"
                                ,children);
    delete $number_token_list;
 }
 /*--------------------------------------------------------------------------*/
 data_lcc_card : LCC number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::vector<size_t> children = {decl};
    std::vector<std::string> names = {"stincl", "v0incl", "xfoisaincl", "npaulincl", "nosurfincl", "unused", "unused", "ecutincl", "ebankincl", "ebankabla"};
    std::vector<mcnpi::NODE> types(names.size(), mcnpi::VALUE);
    assign_names(interpreter, names, types, $number_token_list, children);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"lcc"
                                ,children);
    delete $number_token_list;
 }
 /*--------------------------------------------------------------------------*/
 data_lea_card : LEA number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::vector<size_t> children = {decl};
    std::vector<std::string> names = {"ipht", "icc", "nobalc", "nobale", "ifbrk", "ilvden", "ievap", "nofis"};
    std::vector<mcnpi::NODE> types(names.size(), mcnpi::VALUE);
    assign_names(interpreter, names, types, $number_token_list, children);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"lea"
                                ,children);
    delete $number_token_list;
 }
 /*--------------------------------------------------------------------------*/
 data_leb_card : LEB number_token_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::vector<size_t> children = {decl};
    std::vector<std::string> names = {"yzere", "bzere", "yzero", "bzero"};
    std::vector<mcnpi::NODE> types(names.size(), mcnpi::VALUE);
    assign_names(interpreter, names, types, $number_token_list, children);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"leb"
                                ,children);
    delete $number_token_list;
 }
 /*--------------------------------------------------------------------------*/
 /* TODO update to process records into
 zaid1 ipik1 [mt1,1 pmt1,1 ... mt1,ipik1 pmt1,ipik1]
 zaidn ipikn [mtn,1 pmtn,1 ... mtn,ipikn pmtn,ipikn]
*/
 data_pikmt_card : PIKMT number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"pikmt"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
data_dxc_card :  DXC card_id colon particle_symbol number_list
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $5->insert($5->begin(), $4);
    $5->insert($5->begin(), $3);
    $5->insert($5->begin(), $2);
    $5->insert($5->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"dxc"
                                ,*$5);
    delete $5;
}
| DXC colon particle_symbol number_list
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $4->insert($4->begin(), $3);
    $4->insert($4->begin(), $2);
    $4->insert($4->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"dxc"
                                ,*$4);
    delete $4;
}
 /*--------------------------------------------------------------------------*/
data_unc_card :  UNC colon particle_symbol number_list
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $4->insert($4->begin(), $3);
    $4->insert($4->begin(), $2);
    $4->insert($4->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"unc"
                                ,*$4);
    delete $4;
}
 /*--------------------------------------------------------------------------*/
data_nonu_card :  NONU
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"nonu"
                                ,{decl});
}
| NONU number_list
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $2->insert($2->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"nonu"
                                ,*$2);
    delete $2;
}
 /*--------------------------------------------------------------------------*/
data_notrn_card :  NOTRN
{
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"notrn"
                                ,{decl});
}
/*--------------------------------------------------------------------------*/
 EMBED_SCALAR_KEY : BACKGROUND | MESHGEO | MGEOIN | MEEOUT | MEEIN | CALC_VOLS | DEBUG
                 | FILETYPE | GMVFILE | LENGTH | MCNPUMFILE
 EMBED_LIST_KEY : MATCELL | OVERLAP
 /*--------------------------------------------------------------------------*/
 data_embed_member : EMBED_LIST_KEY assign value_comma_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $value_comma_list;
    m->insert(m->begin(), $assign);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 | EMBED_LIST_KEY value_comma_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $value_comma_list;
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 | EMBED_SCALAR_KEY assign value
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, $assign, $value});
 }
 | EMBED_SCALAR_KEY value
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, $value});
 }

 /*--------------------------------------------------------------------------*/
 data_embed_members : data_embed_member
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | data_embed_members data_embed_member
 {
    $$ = $1;
    $$->push_back($2);
 }
 /*--------------------------------------------------------------------------*/
 data_embed_card : EMBED_CARD card_id data_embed_members
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $data_embed_members;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"embed"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 EMBEE_SCALAR_KEY : EMBED | ENERGY | TIME | MAT | ATOM | FACTOR | MTYPE | ERRORS | COMMENT_FLAG
 EMBEE_LIST_KEY : LIST
 /*--------------------------------------------------------------------------*/
 data_embee_member : EMBEE_LIST_KEY assign value_comma_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $value_comma_list;
    m->insert(m->begin(), $assign);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 | EMBEE_LIST_KEY value_comma_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $value_comma_list;
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 | EMBEE_SCALAR_KEY assign value
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, $assign, $value});
 }
 | EMBEE_SCALAR_KEY value
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, $value});
 }

 /*--------------------------------------------------------------------------*/
 data_embee_members : data_embee_member
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | data_embee_members data_embee_member
 {
    $$ = $1;
    $$->push_back($2);
 }
 /*--------------------------------------------------------------------------*/
 data_embee_card : EMBEE card_id colon particle_symbol_list data_embee_members
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $particle_symbol_list;
    auto e = $data_embee_members;
    m->insert(m->begin(), $colon);
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    m->insert(m->end(), e->begin(), e->end());
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"embee"
                                ,*m);
    delete m;
    delete e;
 }
 /*--------------------------------------------------------------------------*/
 data_embeb_card : EMBEB card_id number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"embeb"
                                ,*m);
    delete m;
 }
 | EMBEB card_id colon particle_symbol number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), $particle_symbol);
    m->insert(m->begin(), $colon);
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"embeb"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 data_embem_card : EMBEM card_id number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"embem"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 data_embtb_card : EMBTB card_id number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"embtb"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 data_embtm_card : EMBTM card_id number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"embtm"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 data_embde_card : EMBDE card_id number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"embde"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 data_embdf_card : EMBDF card_id number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"embdf"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 data_void_card : MCNP_VOID
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"void"
                                ,{decl});
 }
 | MCNP_VOID  number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"void"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 data_drxs_card : DRXS
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"drxs"
                                ,{decl});
 }
 | DRXS  number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"drxs"
                                ,*m);
    delete m;
 }
  /*--------------------------------------------------------------------------*/
 data_ctme_card : CTME number
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ctme"
                                ,{decl, $2});
 }
 /*--------------------------------------------------------------------------*/
 data_rdum_card : RDUM
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"rdum"
                                ,{decl});
 }
 | RDUM  number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"rdum"
                                ,*m);
    delete m;
 }
  /*--------------------------------------------------------------------------*/
 data_idum_card : IDUM
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"idum"
                                ,{decl});
 }
 | IDUM  number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"idum"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 pty : PTY assign particle_symbol_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $particle_symbol_list;
    m->insert(m->begin(), $assign);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"pty"
                                ,*m);
    delete m;
 }
 | PTY particle_symbol_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $particle_symbol_list;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"pty"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 SSW_KEY : SYM | CEL
 /*--------------------------------------------------------------------------*/
 data_ssw_member : SSW_KEY assign number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), $assign);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 | SSW_KEY number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 | pty
 data_ssw_members : data_ssw_member
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | data_ssw_members data_ssw_member
 {
    $$ = $1;
    $$->push_back($2);
 }
  /*--------------------------------------------------------------------------*/
 data_ssw_card : SSW
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ssw"
                                ,{decl});
 }
 | SSW  number_paren_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_paren_list;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ssw"
                                ,*m);
    delete m;
 }
 | SSW  number_paren_list data_ssw_members
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_paren_list;
    m->insert(m->begin(), decl);
    m->insert(m->end(), $data_ssw_members->begin(), $data_ssw_members->end());
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ssw"
                                ,*m);
    delete m;
 }
 | SSW  data_ssw_members
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $data_ssw_members;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ssw"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 SSR_KEY : OLD | NEW | WGT | PSC  | POA | COL | CEL | BCW | AXS | TR | EXT

 /*--------------------------------------------------------------------------*/
 data_ssr_member : SSR_KEY assign value_comma_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $value_comma_list;
    m->insert(m->begin(), $assign);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 | SSR_KEY value_comma_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $value_comma_list;
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 | pty

 /*--------------------------------------------------------------------------*/
 data_ssr_members : data_ssr_member
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | data_ssr_members data_ssr_member
 {
    $$ = $1;
    $$->push_back($2);
 }

  /*--------------------------------------------------------------------------*/
 data_ssr_card : SSR
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ssr"
                                ,{decl});
 }
 | SSR  data_ssr_members
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $data_ssr_members;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ssr"
                                ,*m);
    delete m;
 }

 /*--------------------------------------------------------------------------*/
 FMULT_SCALAR_KEY : WIDTH | METHOD | DATA | SHIFT
 FMULT_LIST_KEY : SFNU | WATT
 /*--------------------------------------------------------------------------*/
 data_fmult_member : FMULT_LIST_KEY assign value_comma_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $value_comma_list;
    m->insert(m->begin(), $assign);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 | FMULT_LIST_KEY value_comma_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $value_comma_list;
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 |FMULT_SCALAR_KEY assign value
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, $assign, $value});
 }
 | FMULT_SCALAR_KEY value
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, $value});
 }

 /*--------------------------------------------------------------------------*/
 data_fmult_members : data_fmult_member
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | data_fmult_members data_fmult_member
 {
    $$ = $1;
    $$->push_back($2);
 }
 /*--------------------------------------------------------------------------*/
 data_fmult_card : FMULT data_fmult_members
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $data_fmult_members;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fmult"
                                ,*m);
    delete m;
 }
 | FMULT INTEGER data_fmult_members
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto z = interpreter.push_leaf(wasp::mcnpi::ZAID, "zaid", $2);
    auto m = $data_fmult_members;
    m->insert(m->begin(), z);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"fmult"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 KPERT_SCALAR_KEY : LINEAR
 KPERT_LIST_KEY : CEL | MAT | RHO | ISO | RXN | ERG
 /*--------------------------------------------------------------------------*/
 data_kpert_member : KPERT_LIST_KEY assign value_comma_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $value_comma_list;
    m->insert(m->begin(), $assign);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 | KPERT_LIST_KEY value_comma_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $value_comma_list;
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 |KPERT_SCALAR_KEY assign value
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, $assign, $value});
 }
 | KPERT_SCALAR_KEY value
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, $value});
 }

 /*--------------------------------------------------------------------------*/
 data_kpert_members : data_kpert_member
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | data_kpert_members data_kpert_member
 {
    $$ = $1;
    $$->push_back($2);
 }
 /*--------------------------------------------------------------------------*/
 data_kpert_card : KPERT card_id data_kpert_members
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $data_kpert_members;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"kpert"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 KSEN_SCALAR_KEY : LEGENDRE | CONSTRAIN
 KSEN_LIST_KEY : ISO | RXN | MT | ERG | EIN | COS
 /*--------------------------------------------------------------------------*/
 data_ksen_member : KSEN_LIST_KEY assign value_comma_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $value_comma_list;
    m->insert(m->begin(), $assign);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 | KSEN_LIST_KEY value_comma_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $value_comma_list;
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 |KSEN_SCALAR_KEY assign value
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, $assign, $value});
 }
 | KSEN_SCALAR_KEY value
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, $value});
 }

 /*--------------------------------------------------------------------------*/
 data_ksen_members : data_ksen_member
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | data_ksen_members data_ksen_member
 {
    $$ = $1;
    $$->push_back($2);
 }
 /*--------------------------------------------------------------------------*/
 data_ksen_card : KSEN card_id STRING data_ksen_members
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto xs = interpreter.push_leaf(wasp::mcnpi::FLAG, "xs", $3);
    auto m = $data_ksen_members;
    m->insert(m->begin(), xs);
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ksen"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 data_pwt_card : PWT  number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"pwt"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 PTRAC_SCALAR_KEY : COINC | WRITE | MEPH | MAX | FILE | BUFFER
 PTRAC_LIST_KEY :  TALLY | SURFACE | CEL | NPS | TYPE | FILTER | EVENT | VALUE_KEY
 /*--------------------------------------------------------------------------*/
 data_ptrac_member : PTRAC_LIST_KEY assign value_comma_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $value_comma_list;
    m->insert(m->begin(), $assign);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 | PTRAC_LIST_KEY value_comma_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $value_comma_list;
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 |PTRAC_SCALAR_KEY assign value
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, $assign, $value});
 }
 | PTRAC_SCALAR_KEY value
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, $value});
 }

 /*--------------------------------------------------------------------------*/
 data_ptrac_members : data_ptrac_member
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | data_ptrac_members data_ptrac_member
 {
    $$ = $1;
    $$->push_back($2);
 }
 /*--------------------------------------------------------------------------*/
 data_ptrac_card : PTRAC data_ptrac_members
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $data_ptrac_members;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"ptrac"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 data_column_wise_header_members : STRING
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | data_column_wise_header_members STRING
 {
    $$ = $1;
    $$->push_back($2);
 }
 | data_column_wise_header_members ID
 {
    $$ = $1;
    $$->push_back($2);
 }
 | data_column_wise_header_members COLON
 {
    $$ = $1;
    $$->push_back($2);
 }
 | data_column_wise_header_members PARTICLE_SYMBOL
 {
    $$ = $1;
    $$->push_back($2);
 }
 /*--------------------------------------------------------------------------*/
  data_column_wise_row_members : VALUE_COMMA_LIST_TOKEN
  {
     $$ = new std::vector<size_t>();
     $$->push_back($1);
 }
 | data_column_wise_row_members VALUE_COMMA_LIST_TOKEN
 {
    $$ = $1;
    $$->push_back($2);
 }
 /*--------------------------------------------------------------------------*/
 data_column_wise_table : data_column_wise_row_members NEWLINE
 {
    $$ = new std::vector<std::vector<size_t>*>();
    $$->push_back($1);
    // ignore NEWLINE... only a used as a sentinel
 }
 | data_column_wise_table data_column_wise_row_members NEWLINE
 {
    $$ = $1;
    $$->push_back($2);
 }
 | data_column_wise_table  NEWLINE
 {
    $$ = $1;
 }
 /*--------------------------------------------------------------------------*/
 data_column_wise_block : COLUMN_WISE_FLAG data_column_wise_header_members NEWLINE data_column_wise_table
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    // ignore newline as it is only a sentinel value

    // first construct the header
    // Header could be simple X Y Z or more complex particle-based Xn:p Yn:p, etc. or combination
    std::vector<std::vector<size_t>> cnodes(1);
    cnodes.front().push_back(decl); // the first column of data is the '#' flag which can optionally have cell ids, etc in text column 1-5
    auto htokens = $data_column_wise_header_members;
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
    auto table_tokens = $data_column_wise_table;
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
    $$ = interpreter.push_parent(wasp::mcnpi::TABLE
                                ,"table"
                                ,children);

    delete $data_column_wise_header_members;
    // TODO delete token vectors

 }
 /*--------------------------------------------------------------------------*/
 BFLD_SCALAR_KEY : FIELD | MXDEFLC | MAXSTEP
 BFLD_LIST_KEY : VEC | AXS | FFEDGES | REFPNT
 /*--------------------------------------------------------------------------*/
 data_bfld_member : BFLD_LIST_KEY assign value_comma_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $value_comma_list;
    m->insert(m->begin(), $assign);
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 | BFLD_LIST_KEY value_comma_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $value_comma_list;
    m->insert(m->begin(), decl);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,*m);
    delete m;
 }
 |BFLD_SCALAR_KEY assign value
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, $assign, $value});
 }
 | BFLD_SCALAR_KEY value
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    std::string name = interpreter.token_data($1);
    std::transform(name.begin(), name.end(), name.begin(),
        [](unsigned char c){ return std::tolower(c); });
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,name.data()
                                ,{decl, $value});
 }

 /*--------------------------------------------------------------------------*/
 data_bfld_members : data_bfld_member
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | data_bfld_members data_bfld_member
 {
    $$ = $1;
    $$->push_back($2);
 }
 /*--------------------------------------------------------------------------*/
 data_bfld_card : BFLD card_id STRING data_bfld_members
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto xs = interpreter.push_leaf(wasp::mcnpi::FLAG, "type", $3);
    auto m = $data_bfld_members;
    m->insert(m->begin(), xs);
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"bfld"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 material_zaid_pairs : material_zaid_entry
 {
    $$ = new std::vector<size_t>();
    $$->push_back($1);
 }
 | material_zaid_pairs material_zaid_entry
 {
    $$ = $1;
    $$->push_back($2);
 }
 /*--------------------------------------------------------------------------*/
 data_awtab_card : AWTAB material_zaid_pairs
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $material_zaid_pairs;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"awtab"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 data_cosy_card : COSY number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"cosy"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 data_cosyp_card : COSYP number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"cosyp"
                                ,*m);
    delete m;
 }
 /*--------------------------------------------------------------------------*/
 data_pd_card : PD card_id number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), $card_id);
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"pd"
                                ,*m);
    delete m;
 }
 | PD number_list
 {
    auto decl = interpreter.push_leaf(wasp::mcnpi::DECL, "decl", $1);
    auto m = $number_list;
    m->insert(m->begin(), decl);
    $$ = interpreter.push_parent(wasp::mcnpi::DATA_PARAM
                                ,"pd"
                                ,*m);
    delete m;
}
 /*--------------------------------------------------------------------------*/
 data_card : data_vol_card
 | data_awtab_card
 | data_pd_card
 | data_cosy_card
 | data_cosyp_card
 | data_ptrac_card
 | data_bfld_card
 | data_fmult_card
 | data_pwt_card
 | data_kpert_card
 | data_ksen_card
 | data_mesh_card
 | data_rdum_card
 | data_idum_card
 | data_ssw_card
 | data_ssr_card
 | data_void_card
 | data_ctme_card
 | data_drxs_card
 | data_embed_card
 | data_embee_card
 | data_embeb_card
 | data_embem_card
 | data_embtb_card
 | data_embtm_card
 | data_embde_card
 | data_embdf_card
 | data_nonu_card
 | data_notrn_card
 | data_dxc_card
 | data_unc_card
 | data_pikmt_card
 | data_lca_card
 | data_lcb_card
 | data_lcc_card
 | data_lea_card
 | data_leb_card
 | data_tropt_card
 | data_pert_card
 | data_mphys_card
 | data_mx_card
 | data_tm_card
 | data_thtme_card
 | data_tmesh_card
 | data_fmesh_card
 | data_bbrem_card
 | data_em_card
 | data_fu_card
 | data_ds_card
 | data_sf_card
 | data_ft_card
 | data_cm_card
 | data_dxt_card
 | data_area_card
 | data_transform_card
 | data_universe_card
 | data_lat_card
 | data_fill_card
 | data_uran_card
 | data_material_card
 | data_cutoff_card
 | data_elpt_card
 | data_activation_card
 | data_fq_card
 | data_fc_card
 | data_ftally_card
 | data_mode_card
 | data_tally_time_card
 | data_kcode_card
 | data_nps_card
 | data_print_card
 | data_talnp_card
 | data_prdmp_card
 | data_mt_card
 | data_imp_card
 | data_ksrc_card
 | data_tally_energy_card
 | data_tally_multiplier_card
 | data_sd_card
 | data_sdef_card
 | data_si_card
 | data_sp_card
 | data_sb_card
 | data_sc_card
 | data_totnu_card
 | data_de_card
 | data_df_card
 | data_tmp_card
 | data_fs_card
 | data_tf_card
 | data_lost_card
 | data_hsrc_card
 | data_xs_card
 | data_phys_card
 | data_kopts_card
 | data_tally_cosine_card
 | data_mgopt_card
 | data_rand_card
 | data_wwe_card
 | data_wwg_card
 | data_wwge_card
 | data_wwgt_card
 | data_wwn_card
 | data_wwp_card
 | data_wwt_card
 | data_dbcn_card
 | data_fcl_card
 | data_var_card
 | data_cf_card
 | data_ext_card
 | data_vect_card
 | data_esplt_card
 | data_tsplt_card
 | data_dd_card
 | data_column_wise_block

 /*--------------------------------------------------------------------------*/
data_block : data_card
{
    $$ = new std::vector<size_t>();
    $$->push_back($1);
}
| data_block data_card
{
    $$ = $1;
    $$->push_back($2);
}

 /*--------------------------------------------------------------------------*/
start   : /** empty **/
        | message_block blank_line title_card cell_block blank_line surface_block blank_line data_block
        {
            std::vector<size_t> document_nodes;
            document_nodes.push_back($message_block);
            document_nodes.push_back($2);
            document_nodes.push_back($title_card);
            document_nodes.insert(document_nodes.end(), $cell_block->begin(), $cell_block->end());
            delete $cell_block;
            document_nodes.push_back($5);
            document_nodes.insert(document_nodes.end(), $surface_block->begin(), $surface_block->end());
            delete $surface_block;
            document_nodes.push_back($7);
            document_nodes.insert(document_nodes.end(), $data_block->begin(), $data_block->end());
            delete $data_block;
            interpreter.push_staged_child(document_nodes);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
        | message_block
        {
            std::vector<size_t> document_nodes;
            document_nodes.push_back($message_block);
            interpreter.push_staged_child(document_nodes);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
        | message_block blank_line title_card
        {
            std::vector<size_t> document_nodes;
            document_nodes.push_back($message_block);
            document_nodes.push_back($2);
            document_nodes.push_back($title_card);
            interpreter.push_staged_child(document_nodes);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
        | message_block blank_line title_card cell_block
        {
            std::vector<size_t> document_nodes;
            document_nodes.push_back($message_block);
            document_nodes.push_back($2);
            document_nodes.push_back($title_card);
            document_nodes.insert(document_nodes.end(), $cell_block->begin(), $cell_block->end());
            delete $cell_block;
            interpreter.push_staged_child(document_nodes);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
        | message_block blank_line title_card cell_block blank_line surface_block
        {
            std::vector<size_t> document_nodes;
            document_nodes.push_back($message_block);
            document_nodes.push_back($2);
            document_nodes.push_back($title_card);
            document_nodes.insert(document_nodes.end(), $cell_block->begin(), $cell_block->end());
            delete $cell_block;
            document_nodes.push_back($5);
            document_nodes.insert(document_nodes.end(), $surface_block->begin(), $surface_block->end());
            delete $surface_block;
            interpreter.push_staged_child(document_nodes);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
        | title_card cell_block blank_line surface_block blank_line data_block
        {
            std::vector<size_t> document_nodes;
            document_nodes.push_back($title_card);
            document_nodes.insert(document_nodes.end(), $cell_block->begin(), $cell_block->end());
            delete $cell_block;
            document_nodes.push_back($3);
            document_nodes.insert(document_nodes.end(), $surface_block->begin(), $surface_block->end());
            delete $surface_block;
            document_nodes.push_back($5);
            document_nodes.insert(document_nodes.end(), $data_block->begin(), $data_block->end());
            delete $data_block;
            interpreter.push_staged_child(document_nodes);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
        | title_card cell_block blank_line surface_block
        {
            std::vector<size_t> document_nodes;
            document_nodes.push_back($title_card);
            document_nodes.insert(document_nodes.end(), $cell_block->begin(), $cell_block->end());
            delete $cell_block;
            document_nodes.push_back($3);
            document_nodes.insert(document_nodes.end(), $surface_block->begin(), $surface_block->end());
            delete $surface_block;
            interpreter.push_staged_child(document_nodes);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
        | title_card cell_block
        {
            std::vector<size_t> document_nodes;
            document_nodes.push_back($title_card);
            document_nodes.insert(document_nodes.end(), $cell_block->begin(), $cell_block->end());
            delete $cell_block;
            interpreter.push_staged_child(document_nodes);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
        | title_card
        {
            std::vector<size_t> document_nodes;
            document_nodes.push_back($title_card);
            interpreter.push_staged_child(document_nodes);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
        | continue title_card data_block
        {
            std::vector<size_t> document_nodes;
            document_nodes.push_back($continue);
            document_nodes.push_back($title_card);
            document_nodes.insert(document_nodes.end(), $data_block->begin(), $data_block->end());
            delete $data_block;
            interpreter.push_staged_child(document_nodes);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
        | continue data_block
        {
            std::vector<size_t> document_nodes;
            document_nodes.push_back($continue);
            document_nodes.insert(document_nodes.end(), $data_block->begin(), $data_block->end());
            delete $data_block;
            interpreter.push_staged_child(document_nodes);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
        | start blank_line{interpreter.push_staged_child($2); if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}}
        | start ANYTHING_BLOCK
        {
            // This is an optional trailing block treated as a comment
            // that should consume the rest of the file
            // The ANYTHING_BLOCK token will only be triggered by the lexer after a
            // blank_line is encountered (following a data_block)
            auto a = interpreter.push_leaf(wasp::mcnpi::COMMENT, "anything", $2);
            interpreter.push_staged_child(a);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }

 /*--------------------------------------------------------------------------*/

 /*** END RULES - Change the wasp grammar rules above ***/

%% /*** Additional Code ***/

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