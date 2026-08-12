#include "waspmcnpi/MCNPInterpreter.h"
#include "gtest/gtest.h"
#include <iostream>
#include <string>
using namespace wasp;

// Message block optionally exists at the beginning of the input
TEST(MCNPInterpreter, message_block_empty)
{
    std::stringstream input;
    input << R"I( meSSage:
)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/msg_block
/msg_block/decl (meSSage:)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}


// Test all filename (with values that collide with other execution options (c,i, m))
TEST(MCNPInterpreter, message_block_filename)
{
    std::stringstream input;
    input << R"I( meSSage: o=/some/path/file.out r=some/file.r wwi=a wwou=b &  
                           wwon=c pa=d linki=e linko=f  k=g com=h como=h pl=i &  
                           mc=j me=k md=l n=m s=n w=p r=q              &
                           datapath=somewhere
)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/msg_block
/msg_block/decl (meSSage:)
/msg_block/outp
/msg_block/outp/outp (o=)
/msg_block/outp/value (/some/path/file.out)
/msg_block/runtpe
/msg_block/runtpe/runtpe (r=)
/msg_block/runtpe/value (some/file.r)
/msg_block/wwinp
/msg_block/wwinp/wwinp (wwi=)
/msg_block/wwinp/value (a)
/msg_block/wwout
/msg_block/wwout/wwout (wwou=)
/msg_block/wwout/value (b)
/msg_block/LC (&)
/msg_block/wwone
/msg_block/wwone/wwone (wwon=)
/msg_block/wwone/value (c)
/msg_block/partinp
/msg_block/partinp/partinp (pa=)
/msg_block/partinp/value (d)
/msg_block/linkin
/msg_block/linkin/linkin (linki=)
/msg_block/linkin/value (e)
/msg_block/linkout
/msg_block/linkout/linkout (linko=)
/msg_block/linkout/value (f)
/msg_block/ksental
/msg_block/ksental/ksental (k=)
/msg_block/ksental/value (g)
/msg_block/com
/msg_block/com/com (com=)
/msg_block/com/value (h)
/msg_block/comout
/msg_block/comout/comout (como=)
/msg_block/comout/value (h)
/msg_block/plotm
/msg_block/plotm/plotm (pl=)
/msg_block/plotm/value (i)
/msg_block/LC (&)
/msg_block/mctal
/msg_block/mctal/mctal (mc=)
/msg_block/mctal/value (j)
/msg_block/meshtal
/msg_block/meshtal/meshtal (me=)
/msg_block/meshtal/value (k)
/msg_block/mdata
/msg_block/mdata/mdata (md=)
/msg_block/mdata/value (l)
/msg_block/name
/msg_block/name/name (n=)
/msg_block/name/value (m)
/msg_block/srctp
/msg_block/srctp/srctp (s=)
/msg_block/srctp/value (n)
/msg_block/wssa
/msg_block/wssa/wssa (w=)
/msg_block/wssa/value (p)
/msg_block/runtpe
/msg_block/runtpe/runtpe (r=)
/msg_block/runtpe/value (q)
/msg_block/LC (&)
/msg_block/datapath
/msg_block/datapath/datapath (datapath=)
/msg_block/datapath/value (somewhere)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}


// Test message block with only execution options provided
TEST(MCNPInterpreter, message_block_execution_continuation)
{
    std::stringstream input;
    input << R"I( meSSage: i=input &
            o=output &
            r=tpe
)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/msg_block
/msg_block/decl (meSSage:)
/msg_block/inp
/msg_block/inp/inp (i=)
/msg_block/inp/value (input)
/msg_block/LC (&)
/msg_block/outp
/msg_block/outp/outp (o=)
/msg_block/outp/value (output)
/msg_block/LC (&)
/msg_block/runtpe
/msg_block/runtpe/runtpe (r=)
/msg_block/runtpe/value (tpe)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}

// Test message block with only execution other options provided
TEST(MCNPInterpreter, message_block_execution_other_options)
{
    std::stringstream input;
    input << R"I( meSSage: C CN DBUG 2 DEV-TEST &
                           notek fatal print tasks 3 balance eol
)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/msg_block
/msg_block/decl (meSSage:)
/msg_block/c
/msg_block/c/decl (C)
/msg_block/cn
/msg_block/cn/decl (CN)
/msg_block/dbug
/msg_block/dbug/decl (DBUG)
/msg_block/dbug/value (2)
/msg_block/devtest (DEV-TEST)
/msg_block/LC (&)
/msg_block/notek (notek)
/msg_block/fatal (fatal)
/msg_block/print (print)
/msg_block/tasks
/msg_block/tasks/decl (tasks)
/msg_block/tasks/value (3)
/msg_block/balance (balance)
/msg_block/eol (eol)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}

// Test message block with both filename and execution options provided
TEST(MCNPInterpreter, message_block_filename_execution_other_options)
{
    std::stringstream input;
    input << R"I( meSSage: i=myin &
                           o=myout &
                           DBUG 2  &
                           fatal print tasks 3

the title
)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/msg_block
/msg_block/decl (meSSage:)
/msg_block/inp
/msg_block/inp/inp (i=)
/msg_block/inp/value (myin)
/msg_block/LC (&)
/msg_block/outp
/msg_block/outp/outp (o=)
/msg_block/outp/value (myout)
/msg_block/LC (&)
/msg_block/dbug
/msg_block/dbug/decl (DBUG)
/msg_block/dbug/value (2)
/msg_block/LC (&)
/msg_block/fatal (fatal)
/msg_block/print (print)
/msg_block/tasks
/msg_block/tasks/decl (tasks)
/msg_block/tasks/value (3)
/BL (
)
/title (the title)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}

TEST(MCNPInterpreter, message_block_blank_line_title)
{
    std::stringstream input;
    input << R"I( meSSage:

this is a title
)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/msg_block
/msg_block/decl (meSSage:)
/BL (
)
/title (this is a title)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}

TEST(MCNPInterpreter, message_block_multiline_blank_line_title)
{
    std::stringstream input;
    input << R"I( message:    o=inp.out $
            r=inp.rtp $
            s=inp.src $

this is a title
)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/msg_block
/msg_block/decl (message:)
/msg_block/outp
/msg_block/outp/outp (o=)
/msg_block/outp/value (inp.out)
/msg_block/comment ($)
/msg_block/runtpe
/msg_block/runtpe/runtpe (r=)
/msg_block/runtpe/value (inp.rtp)
/msg_block/comment ($)
/msg_block/srctp
/msg_block/srctp/srctp (s=)
/msg_block/srctp/value (inp.src)
/msg_block/comment ($)
/BL (
)
/title (this is a title)
)I";

    ASSERT_EQ(expected.str(), paths.str());
}

TEST(MCNPInterpreter, comments)
{
    std::stringstream input;
    input << R"I(This is a required title before comment cards
    C another comment
c
 c 
  c 
   c 
    c 
C coMMENT 
)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (This is a required title before comment cards)
/comment (    C another comment)
/comment (c)
/comment ( c )
/comment (  c )
/comment (   c )
/comment (    c )
/comment (C coMMENT )
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}

// Test basic cell (id, mat, rho, single geom)
TEST(MCNPInterpreter, basic_cell_input)
{
    std::stringstream input;
    input << R"I(Basic cell input test
C id  mat  rho surf
  1   2    1.0    3)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (Basic cell input test)
/comment (C id  mat  rho surf)
/cell
/cell/id (  1)
/cell/material (2)
/cell/rho (1.0)
/cell/geom
/cell/geom/id (3)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}

// Test basic void cell (id, mat, single geom)
TEST(MCNPInterpreter, basic_void_cell_input)
{
    std::stringstream input;
    input << R"I(Basic cell input test
C id  mat(void) surf
  1           0    3)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (Basic cell input test)
/comment (C id  mat(void) surf)
/cell
/cell/id (  1)
/cell/material (0)
/cell/geom
/cell/geom/id (3)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}

// Test multiple basic cells
TEST(MCNPInterpreter, multi_basic_cell_input)
{
    std::stringstream input;
    input << R"I(Basic cell input test
C id  mat(void) surf
  1           0    3
C id  mat  rho surf
  2   2    1.0    5)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (Basic cell input test)
/comment (C id  mat(void) surf)
/cell
/cell/id (  1)
/cell/material (0)
/cell/geom
/cell/geom/id (3)
/comment (C id  mat  rho surf)
/cell
/cell/id (  2)
/cell/material (2)
/cell/rho (1.0)
/cell/geom
/cell/geom/id (5)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}

// Test multiple basic cells with hanging geometry definitions
// Hanging in that the geometry ids are on multiple lines
TEST(MCNPInterpreter, multi_basic_cell_hanging_geom_input)
{
    std::stringstream input;
    input << R"I(Basic cell input test
C id  mat(void) surf
  1           0    3 :
                   5 : 7
C id  mat  rho surf
  2   2    1.0    5
                  7)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (Basic cell input test)
/comment (C id  mat(void) surf)
/cell
/cell/id (  1)
/cell/material (0)
/cell/geom
/cell/geom/id (3)
/cell/geom/U (:)
/cell/geom/id (5)
/cell/geom/U (:)
/cell/geom/id (7)
/comment (C id  mat  rho surf)
/cell
/cell/id (  2)
/cell/material (2)
/cell/rho (1.0)
/cell/geom
/cell/geom/id (5)
/cell/geom/id (7)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}

// Test basic cell with simple parameters specified (no particle designated)
TEST(MCNPInterpreter, basic_cell_input_w_simple_params)
{
    std::stringstream input;
    input << R"I(Basic cell input test
c 
c id  mat  rho surf params
  1   2    1.0   +3 VOL=2.0 PWT=1 NONU=0 PD3=3.14 TMP=13 PD=7 TMP2=.14 U=90 
                    TRCL=1 LAT=2 FILL=4 COSY=3 BFLCL 3)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (Basic cell input test)
/comment (c )
/comment (c id  mat  rho surf params)
/cell
/cell/id (  1)
/cell/material (2)
/cell/rho (1.0)
/cell/geom
/cell/geom/id (+3)
/cell/params
/cell/params/vol
/cell/params/vol/vol (VOL)
/cell/params/vol/= (=)
/cell/params/vol/value (2.0)
/cell/params/pwt
/cell/params/pwt/pwt (PWT)
/cell/params/pwt/= (=)
/cell/params/pwt/value (1)
/cell/params/nonu
/cell/params/nonu/nonu (NONU)
/cell/params/nonu/= (=)
/cell/params/nonu/value (0)
/cell/params/pd
/cell/params/pd/pd (PD)
/cell/params/pd/id (3)
/cell/params/pd/= (=)
/cell/params/pd/value (3.14)
/cell/params/tmp
/cell/params/tmp/tmp (TMP)
/cell/params/tmp/= (=)
/cell/params/tmp/value (13)
/cell/params/pd
/cell/params/pd/pd (PD)
/cell/params/pd/= (=)
/cell/params/pd/value (7)
/cell/params/tmp
/cell/params/tmp/tmp (TMP)
/cell/params/tmp/id (2)
/cell/params/tmp/= (=)
/cell/params/tmp/value (.14)
/cell/params/universe
/cell/params/universe/universe (U)
/cell/params/universe/= (=)
/cell/params/universe/value (90)
/cell/params/trcl
/cell/params/trcl/decl (TRCL)
/cell/params/trcl/= (=)
/cell/params/trcl/id (1)
/cell/params/lat
/cell/params/lat/lat (LAT)
/cell/params/lat/= (=)
/cell/params/lat/value (2)
/cell/params/fill
/cell/params/fill/fill (FILL)
/cell/params/fill/= (=)
/cell/params/fill/value (4)
/cell/params/cosy
/cell/params/cosy/cosy (COSY)
/cell/params/cosy/= (=)
/cell/params/cosy/value (3)
/cell/params/bfcl
/cell/params/bfcl/bfcl (BFLCL)
/cell/params/bfcl/value (3)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}


// Test basic cell with particle designated parameters specified
TEST(MCNPInterpreter, basic_cell_input_w_particle_params)
{
    std::stringstream input;
    input << R"I(Basic cell input test
c 
c id  mat  rho surf params
  1   2    1.0   +3 BFLCL 3 fill=3 ext:q=7 dxc3:~=7 dxc:f=7 imp:n=0 wwn1:!=7 
                    wwn1:-=7 fcl:<=3 elpt:|=7 unc:+=4)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (Basic cell input test)
/comment (c )
/comment (c id  mat  rho surf params)
/cell
/cell/id (  1)
/cell/material (2)
/cell/rho (1.0)
/cell/geom
/cell/geom/id (+3)
/cell/params
/cell/params/bfcl
/cell/params/bfcl/bfcl (BFLCL)
/cell/params/bfcl/value (3)
/cell/params/fill
/cell/params/fill/fill (fill)
/cell/params/fill/= (=)
/cell/params/fill/value (3)
/cell/params/ext
/cell/params/ext/ext (ext)
/cell/params/ext/: (:)
/cell/params/ext/pt (q)
/cell/params/ext/= (=)
/cell/params/ext/value (7)
/cell/params/dxc
/cell/params/dxc/dxc (dxc)
/cell/params/dxc/id (3)
/cell/params/dxc/: (:)
/cell/params/dxc/pt (~)
/cell/params/dxc/= (=)
/cell/params/dxc/value (7)
/cell/params/dxc
/cell/params/dxc/dxc (dxc)
/cell/params/dxc/: (:)
/cell/params/dxc/pt (f)
/cell/params/dxc/= (=)
/cell/params/dxc/value (7)
/cell/params/imp
/cell/params/imp/imp (imp)
/cell/params/imp/: (:)
/cell/params/imp/pt (n)
/cell/params/imp/= (=)
/cell/params/imp/value (0)
/cell/params/wwn
/cell/params/wwn/wwn (wwn)
/cell/params/wwn/id (1)
/cell/params/wwn/: (:)
/cell/params/wwn/pt (!)
/cell/params/wwn/= (=)
/cell/params/wwn/value (7)
/cell/params/wwn
/cell/params/wwn/wwn (wwn)
/cell/params/wwn/id (1)
/cell/params/wwn/: (:)
/cell/params/wwn/pt (-)
/cell/params/wwn/= (=)
/cell/params/wwn/value (7)
/cell/params/fcl
/cell/params/fcl/fcl (fcl)
/cell/params/fcl/: (:)
/cell/params/fcl/pt (<)
/cell/params/fcl/= (=)
/cell/params/fcl/value (3)
/cell/params/elpt
/cell/params/elpt/elpt (elpt)
/cell/params/elpt/: (:)
/cell/params/elpt/pt (|)
/cell/params/elpt/= (=)
/cell/params/elpt/value (7)
/cell/params/unc
/cell/params/unc/unc (unc)
/cell/params/unc/: (:)
/cell/params/unc/pt (+)
/cell/params/unc/= (=)
/cell/params/unc/value (4)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}
// Test cell geom (id, mat, geom intersect ' ', union ':', and complement '#')
// Also test cell 'like but' constructs 
// Also test fortran float syntax being upconverted (3.2-4 -> 3.2e-4)
TEST(MCNPInterpreter, cell_geom_input)
{
    std::stringstream input;
    input << R"I(Basic cell input test
C id  mat(void) surf
  1           0    3 #4 (-1 : 5:7) #(-9 +10):(8)
2             0    3 $ a geom comment 
                  -1
3 like 2 but trcl=( 0.0   9.60   0.0)  mat=3 rho 3.2-4                
4 0 3)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (Basic cell input test)
/comment (C id  mat(void) surf)
/cell
/cell/id (  1)
/cell/material (0)
/cell/geom
/cell/geom/id (3)
/cell/geom/# (#)
/cell/geom/id (4)
/cell/geom/LP (()
/cell/geom/id (-1)
/cell/geom/U (:)
/cell/geom/id (5)
/cell/geom/U (:)
/cell/geom/id (7)
/cell/geom/RP ())
/cell/geom/# (#)
/cell/geom/LP (()
/cell/geom/id (-9)
/cell/geom/id (+10)
/cell/geom/RP ())
/cell/geom/U (:)
/cell/geom/LP (()
/cell/geom/id (8)
/cell/geom/RP ())
/cell
/cell/id (2)
/cell/material (0)
/cell/geom
/cell/geom/id (3)
/cell/geom/comment ($ a geom comment )
/cell/geom/id (-1)
/cell
/cell/id (3)
/cell/like
/cell/like/decl (like)
/cell/like/cell (2)
/cell/like/but (but)
/cell/like/params
/cell/like/params/trcl
/cell/like/params/trcl/decl (trcl)
/cell/like/params/trcl/= (=)
/cell/like/params/trcl/LP (()
/cell/like/params/trcl/o (0.0)
/cell/like/params/trcl/o (9.60)
/cell/like/params/trcl/o (0.0)
/cell/like/params/trcl/RP ())
/cell/like/params/mat
/cell/like/params/mat/decl (mat)
/cell/like/params/mat/= (=)
/cell/like/params/mat/value (3)
/cell/like/params/rho
/cell/like/params/rho/decl (rho)
/cell/like/params/rho/value (3.2e-4)
/cell
/cell/id (4)
/cell/material (0)
/cell/geom
/cell/geom/id (3)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}

// Test multiple cells with macrobody geometry facets
TEST(MCNPInterpreter, multi_cell_geom_faceted_input)
{
    std::stringstream input;
    input << R"I(Basic cell input test
C id  mat(void) surf
  1           0    3.2 3.1 5.6 )I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (Basic cell input test)
/comment (C id  mat(void) surf)
/cell
/cell/id (  1)
/cell/material (0)
/cell/geom
/cell/geom/id (3.2)
/cell/geom/id (3.1)
/cell/geom/id (5.6)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}

// Test multiple surfaces
TEST(MCNPInterpreter, multi_surface_geom_input)
{
    std::stringstream input;
    input << R"I(Surface input test
C simple cell card for syntax
  1           0    1 

C Surfaces
C General plane : A B C D 
1 p 1 2 3 4
C X,Y,Z plane : D 
2 px 4
3 py 4
4 pz 4
C Origin-centered sphere: R
5 so 4
C General Sphere: X Y Z R
6 s 1 2.4 3 4.0
6 sph 1 2.4 3 4.0
C X,Y,Z Sphere : X|Y|Z R
7 sx 1 4
8 sy 1 4
9 sz 1 4
C parallel-to-axis Cylinder : X|Y Z|Y R
10 c/x 1 2 3
11 c/y 1 2 3
12 c/z 1 2 3
C On-axis Cylinder : R
13 cx 3.1415
14 cy 3.1415
15 cz 3.1415
C Parallel-to-axis cone : X Y Z T [+/-1]
16 k/x 1 2 3 4 +1
16 k/x 1 2 3 4
17 k/y 1 2 3 4 +1
17 k/y 1 2 3 4
18 k/z 1 2 3 4 +1
18 k/z 1 2 3 4
C On-axis cone : X|Y|Z t [+/-1]
19 kx 3 4 +1
19 kx 3 4
20 ky 3 4 +1
20 ky 3 4
21 kz 3 4 +1
21 kz 3 4
C Simple quadric : A B C D E F G X Y Z
22 sq 1 2 3 4 5 6 7 8 9 10
C General quadric : A B C D E F G H J K
23 gq 1 2 3 4 5 6 7 8 9 10
C Torus : X Y Z A B C
24 tx 1 2 3 4 5 6 
25 TY 1 2 3 4 5 6
26 tz 1 2 3 4 5 6
C POINTS : X|Y|Z R [X|Y|Z R [X|Y|Z R]] 
27 x 1 2
27 x 1 2 3 4 
27 x 1 2 3 4 5 6
28 y 1 2
28 y 1 2 3 4 
28 y 1 2 3 4 5 6
29 z 1 2
29 z 1 2 3 4 
29 z 1 2 3 4 5 6
C General Plane via POINTS : x y z x y z x y z
30 P  1 2 3 4 5 6 7 8 9
C BOX : vx vy vz a1x a1y a1z a2x a2y a2z a3x a3y a3z
31 box 1 2 3 4 5 6 7 8 9 0 1 2
C RPP :  xmin xmax ymin ymax zmin zmax
32 RPP 0 1 2 3 4 5
C RCC : vx vy vz hx hy hz r
33 rcc 1 2 3 4 5 6 7
C RHP : v1 v2 v3 h1 h2 h3 r1 r2 r3 s1 s2 s3 t1 t2 t3
34 rhp 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 
C HEX 
34 hex 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 
C REC : vx vy vz hx hy hz v1x v1y v1z v2x v2y v2z
35 rec 1 2 3 4 5 6 7 8 9 0 1 2
C TRC : vx vy vz hx hy hz r1 r2
36 trc 1 2 3 4 5 6 7 8 
C ELL : v1x v1y v1z v2x v2y v2z rm
37 ell 1 2 3 4 5 6 7
C WED : vx vy vz v1x v1y v1z v2x v2y v2z v3x v3y v3z
38 wed 1 2 3 4 5 6 7 8 9 0 1 2
C ARB : ax ay az bx by bz...hx hy hz n1 n2 n3 n4 n5 n6
39 arb -5 -10 -5 -5 -10 5 5 -10 -5 5 -10 5  
        0 12 0 0 0 0 0 0 0 0 0 0 
        1234 1250 1350 2450 3450 0)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (Surface input test)
/comment (C simple cell card for syntax)
/cell
/cell/id (  1)
/cell/material (0)
/cell/geom
/cell/geom/id (1)
/BL (
)
/comment (C Surfaces)
/comment (C General plane : A B C D )
/surface
/surface/id (1)
/surface/pl
/surface/pl/decl (p)
/surface/pl/a (1)
/surface/pl/b (2)
/surface/pl/c (3)
/surface/pl/d (4)
/comment (C X,Y,Z plane : D )
/surface
/surface/id (2)
/surface/xpl
/surface/xpl/decl (px)
/surface/xpl/d (4)
/surface
/surface/id (3)
/surface/ypl
/surface/ypl/decl (py)
/surface/ypl/d (4)
/surface
/surface/id (4)
/surface/zpl
/surface/zpl/decl (pz)
/surface/zpl/d (4)
/comment (C Origin-centered sphere: R)
/surface
/surface/id (5)
/surface/sph
/surface/sph/decl (so)
/surface/sph/r (4)
/comment (C General Sphere: X Y Z R)
/surface
/surface/id (6)
/surface/sph
/surface/sph/decl (s)
/surface/sph/xbar (1)
/surface/sph/ybar (2.4)
/surface/sph/zbar (3)
/surface/sph/r (4.0)
/surface
/surface/id (6)
/surface/sph
/surface/sph/decl (sph)
/surface/sph/xbar (1)
/surface/sph/ybar (2.4)
/surface/sph/zbar (3)
/surface/sph/r (4.0)
/comment (C X,Y,Z Sphere : X|Y|Z R)
/surface
/surface/id (7)
/surface/sph
/surface/sph/decl (sx)
/surface/sph/xbar (1)
/surface/sph/r (4)
/surface
/surface/id (8)
/surface/sph
/surface/sph/decl (sy)
/surface/sph/ybar (1)
/surface/sph/r (4)
/surface
/surface/id (9)
/surface/sph
/surface/sph/decl (sz)
/surface/sph/zbar (1)
/surface/sph/r (4)
/comment (C parallel-to-axis Cylinder : X|Y Z|Y R)
/surface
/surface/id (10)
/surface/cyl
/surface/cyl/decl (c/x)
/surface/cyl/ybar (1)
/surface/cyl/zbar (2)
/surface/cyl/r (3)
/surface
/surface/id (11)
/surface/cyl
/surface/cyl/decl (c/y)
/surface/cyl/xbar (1)
/surface/cyl/zbar (2)
/surface/cyl/r (3)
/surface
/surface/id (12)
/surface/cyl
/surface/cyl/decl (c/z)
/surface/cyl/xbar (1)
/surface/cyl/ybar (2)
/surface/cyl/r (3)
/comment (C On-axis Cylinder : R)
/surface
/surface/id (13)
/surface/xcyl
/surface/xcyl/decl (cx)
/surface/xcyl/r (3.1415)
/surface
/surface/id (14)
/surface/ycyl
/surface/ycyl/decl (cy)
/surface/ycyl/r (3.1415)
/surface
/surface/id (15)
/surface/zcyl
/surface/zcyl/decl (cz)
/surface/zcyl/r (3.1415)
/comment (C Parallel-to-axis cone : X Y Z T [+/-1])
/surface
/surface/id (16)
/surface/xpcone
/surface/xpcone/decl (k/x)
/surface/xpcone/xbar (1)
/surface/xpcone/ybar (2)
/surface/xpcone/zbar (3)
/surface/xpcone/t (4)
/surface/xpcone/sense (+1)
/surface
/surface/id (16)
/surface/xpcone
/surface/xpcone/decl (k/x)
/surface/xpcone/xbar (1)
/surface/xpcone/ybar (2)
/surface/xpcone/zbar (3)
/surface/xpcone/t (4)
/surface
/surface/id (17)
/surface/ypcone
/surface/ypcone/decl (k/y)
/surface/ypcone/xbar (1)
/surface/ypcone/ybar (2)
/surface/ypcone/zbar (3)
/surface/ypcone/t (4)
/surface/ypcone/sense (+1)
/surface
/surface/id (17)
/surface/ypcone
/surface/ypcone/decl (k/y)
/surface/ypcone/xbar (1)
/surface/ypcone/ybar (2)
/surface/ypcone/zbar (3)
/surface/ypcone/t (4)
/surface
/surface/id (18)
/surface/zpcone
/surface/zpcone/decl (k/z)
/surface/zpcone/xbar (1)
/surface/zpcone/ybar (2)
/surface/zpcone/zbar (3)
/surface/zpcone/t (4)
/surface/zpcone/sense (+1)
/surface
/surface/id (18)
/surface/zpcone
/surface/zpcone/decl (k/z)
/surface/zpcone/xbar (1)
/surface/zpcone/ybar (2)
/surface/zpcone/zbar (3)
/surface/zpcone/t (4)
/comment (C On-axis cone : X|Y|Z t [+/-1])
/surface
/surface/id (19)
/surface/xcone
/surface/xcone/decl (kx)
/surface/xcone/xbar (3)
/surface/xcone/t (4)
/surface/xcone/sense (+1)
/surface
/surface/id (19)
/surface/xcone
/surface/xcone/decl (kx)
/surface/xcone/xbar (3)
/surface/xcone/t (4)
/surface
/surface/id (20)
/surface/ycone
/surface/ycone/decl (ky)
/surface/ycone/ybar (3)
/surface/ycone/t (4)
/surface/ycone/sense (+1)
/surface
/surface/id (20)
/surface/ycone
/surface/ycone/decl (ky)
/surface/ycone/ybar (3)
/surface/ycone/t (4)
/surface
/surface/id (21)
/surface/zcone
/surface/zcone/decl (kz)
/surface/zcone/zbar (3)
/surface/zcone/t (4)
/surface/zcone/sense (+1)
/surface
/surface/id (21)
/surface/zcone
/surface/zcone/decl (kz)
/surface/zcone/zbar (3)
/surface/zcone/t (4)
/comment (C Simple quadric : A B C D E F G X Y Z)
/surface
/surface/id (22)
/surface/sq
/surface/sq/decl (sq)
/surface/sq/a (1)
/surface/sq/b (2)
/surface/sq/c (3)
/surface/sq/d (4)
/surface/sq/e (5)
/surface/sq/f (6)
/surface/sq/g (7)
/surface/sq/xbar (8)
/surface/sq/ybar (9)
/surface/sq/zbar (10)
/comment (C General quadric : A B C D E F G H J K)
/surface
/surface/id (23)
/surface/gq
/surface/gq/decl (gq)
/surface/gq/a (1)
/surface/gq/b (2)
/surface/gq/c (3)
/surface/gq/d (4)
/surface/gq/e (5)
/surface/gq/f (6)
/surface/gq/g (7)
/surface/gq/h (8)
/surface/gq/j (9)
/surface/gq/k (10)
/comment (C Torus : X Y Z A B C)
/surface
/surface/id (24)
/surface/xtorus
/surface/xtorus/decl (tx)
/surface/xtorus/xbar (1)
/surface/xtorus/ybar (2)
/surface/xtorus/zbar (3)
/surface/xtorus/a (4)
/surface/xtorus/b (5)
/surface/xtorus/c (6)
/surface
/surface/id (25)
/surface/ytorus
/surface/ytorus/decl (TY)
/surface/ytorus/xbar (1)
/surface/ytorus/ybar (2)
/surface/ytorus/zbar (3)
/surface/ytorus/a (4)
/surface/ytorus/b (5)
/surface/ytorus/c (6)
/surface
/surface/id (26)
/surface/ztorus
/surface/ztorus/decl (tz)
/surface/ztorus/xbar (1)
/surface/ztorus/ybar (2)
/surface/ztorus/zbar (3)
/surface/ztorus/a (4)
/surface/ztorus/b (5)
/surface/ztorus/c (6)
/comment (C POINTS : X|Y|Z R [X|Y|Z R [X|Y|Z R]] )
/surface
/surface/id (27)
/surface/xpts
/surface/xpts/decl (x)
/surface/xpts/xp (1)
/surface/xpts/rp (2)
/surface
/surface/id (27)
/surface/xpts
/surface/xpts/decl (x)
/surface/xpts/xp (1)
/surface/xpts/rp (2)
/surface/xpts/xp (3)
/surface/xpts/rp (4)
/surface
/surface/id (27)
/surface/xpts
/surface/xpts/decl (x)
/surface/xpts/xp (1)
/surface/xpts/rp (2)
/surface/xpts/xp (3)
/surface/xpts/rp (4)
/surface/xpts/xp (5)
/surface/xpts/rp (6)
/surface
/surface/id (28)
/surface/ypts
/surface/ypts/decl (y)
/surface/ypts/yp (1)
/surface/ypts/rp (2)
/surface
/surface/id (28)
/surface/ypts
/surface/ypts/decl (y)
/surface/ypts/yp (1)
/surface/ypts/rp (2)
/surface/ypts/yp (3)
/surface/ypts/rp (4)
/surface
/surface/id (28)
/surface/ypts
/surface/ypts/decl (y)
/surface/ypts/yp (1)
/surface/ypts/rp (2)
/surface/ypts/yp (3)
/surface/ypts/rp (4)
/surface/ypts/yp (5)
/surface/ypts/rp (6)
/surface
/surface/id (29)
/surface/zpts
/surface/zpts/decl (z)
/surface/zpts/zp (1)
/surface/zpts/rp (2)
/surface
/surface/id (29)
/surface/zpts
/surface/zpts/decl (z)
/surface/zpts/zp (1)
/surface/zpts/rp (2)
/surface/zpts/zp (3)
/surface/zpts/rp (4)
/surface
/surface/id (29)
/surface/zpts
/surface/zpts/decl (z)
/surface/zpts/zp (1)
/surface/zpts/rp (2)
/surface/zpts/zp (3)
/surface/zpts/rp (4)
/surface/zpts/zp (5)
/surface/zpts/rp (6)
/comment (C General Plane via POINTS : x y z x y z x y z)
/surface
/surface/id (30)
/surface/plpts
/surface/plpts/decl (P)
/surface/plpts/xp (1)
/surface/plpts/yp (2)
/surface/plpts/zp (3)
/surface/plpts/xp (4)
/surface/plpts/yp (5)
/surface/plpts/zp (6)
/surface/plpts/xp (7)
/surface/plpts/yp (8)
/surface/plpts/zp (9)
/comment (C BOX : vx vy vz a1x a1y a1z a2x a2y a2z a3x a3y a3z)
/surface
/surface/id (31)
/surface/box
/surface/box/decl (box)
/surface/box/vx (1)
/surface/box/vy (2)
/surface/box/vz (3)
/surface/box/ax (4)
/surface/box/ay (5)
/surface/box/az (6)
/surface/box/ax (7)
/surface/box/ay (8)
/surface/box/az (9)
/surface/box/ax (0)
/surface/box/ay (1)
/surface/box/az (2)
/comment (C RPP :  xmin xmax ymin ymax zmin zmax)
/surface
/surface/id (32)
/surface/rppiped
/surface/rppiped/decl (RPP)
/surface/rppiped/xmin (0)
/surface/rppiped/xmax (1)
/surface/rppiped/ymin (2)
/surface/rppiped/ymax (3)
/surface/rppiped/zmin (4)
/surface/rppiped/zmax (5)
/comment (C RCC : vx vy vz hx hy hz r)
/surface
/surface/id (33)
/surface/rrectcyl
/surface/rrectcyl/decl (rcc)
/surface/rrectcyl/vx (1)
/surface/rrectcyl/vy (2)
/surface/rrectcyl/vz (3)
/surface/rrectcyl/hx (4)
/surface/rrectcyl/hy (5)
/surface/rrectcyl/hz (6)
/surface/rrectcyl/r (7)
/comment (C RHP : v1 v2 v3 h1 h2 h3 r1 r2 r3 s1 s2 s3 t1 t2 t3)
/surface
/surface/id (34)
/surface/hex
/surface/hex/decl (rhp)
/surface/hex/vx (1)
/surface/hex/vy (2)
/surface/hex/vz (3)
/surface/hex/hx (4)
/surface/hex/hy (5)
/surface/hex/hz (6)
/surface/hex/rx (7)
/surface/hex/ry (8)
/surface/hex/rz (9)
/surface/hex/sx (0)
/surface/hex/sy (1)
/surface/hex/sz (2)
/surface/hex/tx (3)
/surface/hex/ty (4)
/surface/hex/tz (5)
/comment (C HEX )
/surface
/surface/id (34)
/surface/hex
/surface/hex/decl (hex)
/surface/hex/vx (1)
/surface/hex/vy (2)
/surface/hex/vz (3)
/surface/hex/hx (4)
/surface/hex/hy (5)
/surface/hex/hz (6)
/surface/hex/rx (7)
/surface/hex/ry (8)
/surface/hex/rz (9)
/surface/hex/sx (0)
/surface/hex/sy (1)
/surface/hex/sz (2)
/surface/hex/tx (3)
/surface/hex/ty (4)
/surface/hex/tz (5)
/comment (C REC : vx vy vz hx hy hz v1x v1y v1z v2x v2y v2z)
/surface
/surface/id (35)
/surface/relipcyl
/surface/relipcyl/decl (rec)
/surface/relipcyl/vx (1)
/surface/relipcyl/vy (2)
/surface/relipcyl/vz (3)
/surface/relipcyl/hx (4)
/surface/relipcyl/hy (5)
/surface/relipcyl/hz (6)
/surface/relipcyl/vx1 (7)
/surface/relipcyl/vy1 (8)
/surface/relipcyl/vz1 (9)
/surface/relipcyl/vx2 (0)
/surface/relipcyl/vy2 (1)
/surface/relipcyl/vz2 (2)
/comment (C TRC : vx vy vz hx hy hz r1 r2)
/surface
/surface/id (36)
/surface/cone
/surface/cone/decl (trc)
/surface/cone/vx (1)
/surface/cone/vy (2)
/surface/cone/vz (3)
/surface/cone/hx (4)
/surface/cone/hy (5)
/surface/cone/hz (6)
/surface/cone/r1 (7)
/surface/cone/r2 (8)
/comment (C ELL : v1x v1y v1z v2x v2y v2z rm)
/surface
/surface/id (37)
/surface/ellipsoid
/surface/ellipsoid/decl (ell)
/surface/ellipsoid/vx (1)
/surface/ellipsoid/vy (2)
/surface/ellipsoid/vz (3)
/surface/ellipsoid/vx (4)
/surface/ellipsoid/vy (5)
/surface/ellipsoid/vz (6)
/surface/ellipsoid/rm (7)
/comment (C WED : vx vy vz v1x v1y v1z v2x v2y v2z v3x v3y v3z)
/surface
/surface/id (38)
/surface/wedge
/surface/wedge/decl (wed)
/surface/wedge/vx (1)
/surface/wedge/vy (2)
/surface/wedge/vz (3)
/surface/wedge/vx1 (4)
/surface/wedge/vy1 (5)
/surface/wedge/vz1 (6)
/surface/wedge/vx2 (7)
/surface/wedge/vy2 (8)
/surface/wedge/vz2 (9)
/surface/wedge/vx3 (0)
/surface/wedge/vy3 (1)
/surface/wedge/vz3 (2)
/comment (C ARB : ax ay az bx by bz...hx hy hz n1 n2 n3 n4 n5 n6)
/surface
/surface/id (39)
/surface/arbpoly
/surface/arbpoly/decl (arb)
/surface/arbpoly/ax (-5)
/surface/arbpoly/ay (-10)
/surface/arbpoly/az (-5)
/surface/arbpoly/bx (-5)
/surface/arbpoly/by (-10)
/surface/arbpoly/bz (5)
/surface/arbpoly/cx (5)
/surface/arbpoly/cy (-10)
/surface/arbpoly/cz (-5)
/surface/arbpoly/dx (5)
/surface/arbpoly/dy (-10)
/surface/arbpoly/dz (5)
/surface/arbpoly/ex (0)
/surface/arbpoly/ey (12)
/surface/arbpoly/ez (0)
/surface/arbpoly/fx (0)
/surface/arbpoly/fy (0)
/surface/arbpoly/fz (0)
/surface/arbpoly/gx (0)
/surface/arbpoly/gy (0)
/surface/arbpoly/gz (0)
/surface/arbpoly/hx (0)
/surface/arbpoly/hy (0)
/surface/arbpoly/hz (0)
/surface/arbpoly/n (1234)
/surface/arbpoly/n (1250)
/surface/arbpoly/n (1350)
/surface/arbpoly/n (2450)
/surface/arbpoly/n (3450)
/surface/arbpoly/n (0)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}

// Test the additional variants of Fill syntax
// Specifically the (m) and (o rm m)
TEST(MCNPInterpreter, cell_geom_fill_input)
{
    std::stringstream input;
    input << R"I(Basic cell with fill input test
  1           0    3  fill 1 (3)    
  1           0    3  fill=2 (3)
  2           0    3  fill=3 (1 2 3 4 5 6 7 8 9 0 1 2 10))I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (Basic cell with fill input test)
/cell
/cell/id (  1)
/cell/material (0)
/cell/geom
/cell/geom/id (3)
/cell/params
/cell/params/fill
/cell/params/fill/fill (fill)
/cell/params/fill/value (1)
/cell/params/fill/LP (()
/cell/params/fill/m (3)
/cell/params/fill/RP ())
/cell
/cell/id (  1)
/cell/material (0)
/cell/geom
/cell/geom/id (3)
/cell/params
/cell/params/fill
/cell/params/fill/fill (fill)
/cell/params/fill/= (=)
/cell/params/fill/value (2)
/cell/params/fill/LP (()
/cell/params/fill/m (3)
/cell/params/fill/RP ())
/cell
/cell/id (  2)
/cell/material (0)
/cell/geom
/cell/geom/id (3)
/cell/params
/cell/params/fill
/cell/params/fill/fill (fill)
/cell/params/fill/= (=)
/cell/params/fill/value (3)
/cell/params/fill/LP (()
/cell/params/fill/o (1)
/cell/params/fill/o (2)
/cell/params/fill/o (3)
/cell/params/fill/rm (4)
/cell/params/fill/rm (5)
/cell/params/fill/rm (6)
/cell/params/fill/rm (7)
/cell/params/fill/rm (8)
/cell/params/fill/rm (9)
/cell/params/fill/rm (0)
/cell/params/fill/rm (1)
/cell/params/fill/rm (2)
/cell/params/fill/m (10)
/cell/params/fill/RP ())
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}

// Test fully specified fill cell card entry 
TEST(MCNPInterpreter, fully_specified_cell_card)
{
    std::stringstream input;
    input << R"I(Test the full fill statement
1 0 2 fill=-1:1 -2:2 0:0
      1  1  1  1  1  1  1  1
      1  1  1  1  1  1  1  1          )I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (Test the full fill statement)
/cell
/cell/id (1)
/cell/material (0)
/cell/geom
/cell/geom/id (2)
/cell/params
/cell/params/fill
/cell/params/fill/fill (fill)
/cell/params/fill/= (=)
/cell/params/fill/istart (-1)
/cell/params/fill/: (:)
/cell/params/fill/iend (1)
/cell/params/fill/jstart (-2)
/cell/params/fill/: (:)
/cell/params/fill/jend (2)
/cell/params/fill/kstart (0)
/cell/params/fill/: (:)
/cell/params/fill/kend (0)
/cell/params/fill/list
/cell/params/fill/list/value (1)
/cell/params/fill/list/value (1)
/cell/params/fill/list/value (1)
/cell/params/fill/list/value (1)
/cell/params/fill/list/value (1)
/cell/params/fill/list/value (1)
/cell/params/fill/list/value (1)
/cell/params/fill/list/value (1)
/cell/params/fill/list/value (1)
/cell/params/fill/list/value (1)
/cell/params/fill/list/value (1)
/cell/params/fill/list/value (1)
/cell/params/fill/list/value (1)
/cell/params/fill/list/value (1)
/cell/params/fill/list/value (1)
/cell/params/fill/list/value (1)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}

// Test surface optional transform or surface reflection id
TEST(MCNPInterpreter, surface_transform_reflected_surface)
{
    std::stringstream input;
    input << R"I(Test surface transform or surfae reflection
1 0 1

1  2 so 3.1415
2 -1 so 3.1415
3    so 3.1415)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (Test surface transform or surfae reflection)
/cell
/cell/id (1)
/cell/material (0)
/cell/geom
/cell/geom/id (1)
/BL (
)
/surface
/surface/id (1)
/surface/transform (2)
/surface/sph
/surface/sph/decl (so)
/surface/sph/r (3.1415)
/surface
/surface/id (2)
/surface/periodic (-1)
/surface/sph
/surface/sph/decl (so)
/surface/sph/r (3.1415)
/surface
/surface/id (3)
/surface/sph
/surface/sph/decl (so)
/surface/sph/r (3.1415)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}

// Test 80 character comment limit
TEST(MCNPInterpreter, column_forced_comment)
{
    std::stringstream input;
    input << R"I(Basic input test for 80 column input limit
1 0 1                                                                                                                           My basic cell comment
2 0 2
3 0 1                                                                                                                           comment in cell def
     -2)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (Basic input test for 80 column input limit)
/cell
/cell/id (1)
/cell/material (0)
/cell/geom
/cell/geom/id (1)
/cell/geom/comment (My basic cell comment)
/cell
/cell/id (2)
/cell/material (0)
/cell/geom
/cell/geom/id (2)
/cell
/cell/id (3)
/cell/material (0)
/cell/geom
/cell/geom/id (1)
/cell/geom/comment (comment in cell def)
/cell/geom/id (-2)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}

// Test basic data card 
TEST(MCNPInterpreter, basic_data_card_wshortcuts)
{
    std::stringstream input;
    input << R"I(Basic input test for data card
C required cells
1 0 1
     
C required surfaces
1 so 3.14
  
vol 1 2 3
vol no 
vol no 3 5 6 7
vol 3j 
vol 1 i 3 2 2i 4 3.14m j 2 5r 3log 4ilog
cut:n j 1.6
elpt:p 1 log 1000
)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (Basic input test for data card)
/comment (C required cells)
/cell
/cell/id (1)
/cell/material (0)
/cell/geom
/cell/geom/id (1)
/BL (     
)
/comment (C required surfaces)
/surface
/surface/id (1)
/surface/sph
/surface/sph/decl (so)
/surface/sph/r (3.14)
/BL (  
)
/vol
/vol/decl (vol)
/vol/value (1)
/vol/value (2)
/vol/value (3)
/vol
/vol/decl (vol)
/vol/no (no)
/vol
/vol/decl (vol)
/vol/no (no)
/vol/value (3)
/vol/value (5)
/vol/value (6)
/vol/value (7)
/vol
/vol/decl (vol)
/vol/jump (3j)
/vol
/vol/decl (vol)
/vol/value (1)
/vol/linear (i)
/vol/value (3)
/vol/value (2)
/vol/linear (2i)
/vol/value (4)
/vol/multiply (3.14m)
/vol/jump (j)
/vol/value (2)
/vol/repeat (5r)
/vol/log (3log)
/vol/log (4ilog)
/cutoff
/cutoff/decl (cut)
/cutoff/: (:)
/cutoff/pt (n)
/cutoff/jump (j)
/cutoff/value (1.6)
/elpt
/elpt/decl (elpt)
/elpt/: (:)
/elpt/pt (p)
/elpt/value (1)
/elpt/log (log)
/elpt/value (1000)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}

// Test material
TEST(MCNPInterpreter, material)
{
    std::stringstream input;
    input << R"I(Basic material input test
1 0 1

1 so 3.14

 m2000 6012.50c 1 8016.01p 2 NLIB=60c PNLIB=24u PLIB=02p
m1 $ my mat
  6000 1
)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (Basic material input test)
/cell
/cell/id (1)
/cell/material (0)
/cell/geom
/cell/geom/id (1)
/BL (
)
/surface
/surface/id (1)
/surface/sph
/surface/sph/decl (so)
/surface/sph/r (3.14)
/BL (
)
/material
/material/decl ( m)
/material/id (2000)
/material/zaid
/material/zaid/id (6012)
/material/zaid/lib (.50c)
/material/zaid/value (1)
/material/zaid
/material/zaid/id (8016)
/material/zaid/lib (.01p)
/material/zaid/value (2)
/material/NLIB
/material/NLIB/decl (NLIB)
/material/NLIB/= (=)
/material/NLIB/lib_id (60c)
/material/PNLIB
/material/PNLIB/decl (PNLIB)
/material/PNLIB/= (=)
/material/PNLIB/lib_id (24u)
/material/PLIB
/material/PLIB/decl (PLIB)
/material/PLIB/= (=)
/material/PLIB/lib_id (02p)
/material
/material/decl (m)
/material/id (1)
/material/comment ($ my mat)
/material/zaid
/material/zaid/id (6000)
/material/zaid/value (1)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}


// Test ACT card
TEST(MCNPInterpreter, act)
{
    std::stringstream input;
    input << R"I(Basic input test
1 0 1 

1 so 1

act fission NONE fission=n fission=n,e,a fission ALL 
    nonfiss none nonfiss=n,p DN=model dn library dn both dn=prompt
    dg=lines dg mg dg none
    thresh 0.94 dnbias 9 nap 4 
    dneb .2, 1, .4, 2, .6, 3
    dpeb 0, 3, 1, 5 pecut 0 hlcut=.1
    sample=correlate sample nonfiss_cor
)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (Basic input test)
/cell
/cell/id (1)
/cell/material (0)
/cell/geom
/cell/geom/id (1)
/BL (
)
/surface
/surface/id (1)
/surface/sph
/surface/sph/decl (so)
/surface/sph/r (1)
/BL (
)
/act
/act/decl (act)
/act/fission
/act/fission/decl (fission)
/act/fission/value (NONE)
/act/fission
/act/fission/decl (fission)
/act/fission/= (=)
/act/fission/value (n)
/act/fission
/act/fission/decl (fission)
/act/fission/= (=)
/act/fission/value (n)
/act/fission/comma (,)
/act/fission/value (e)
/act/fission/comma (,)
/act/fission/value (a)
/act/fission
/act/fission/decl (fission)
/act/fission/value (ALL)
/act/nonfiss
/act/nonfiss/decl (nonfiss)
/act/nonfiss/value (none)
/act/nonfiss
/act/nonfiss/decl (nonfiss)
/act/nonfiss/= (=)
/act/nonfiss/value (n)
/act/nonfiss/comma (,)
/act/nonfiss/value (p)
/act/DN
/act/DN/decl (DN)
/act/DN/= (=)
/act/DN/value (model)
/act/dn
/act/dn/decl (dn)
/act/dn/value (library)
/act/dn
/act/dn/decl (dn)
/act/dn/value (both)
/act/dn
/act/dn/decl (dn)
/act/dn/= (=)
/act/dn/value (prompt)
/act/dg
/act/dg/decl (dg)
/act/dg/= (=)
/act/dg/value (lines)
/act/dg
/act/dg/decl (dg)
/act/dg/value (mg)
/act/dg
/act/dg/decl (dg)
/act/dg/value (none)
/act/thresh
/act/thresh/decl (thresh)
/act/thresh/value (0.94)
/act/dnbias
/act/dnbias/decl (dnbias)
/act/dnbias/value (9)
/act/nap
/act/nap/decl (nap)
/act/nap/value (4)
/act/dneb
/act/dneb/decl (dneb)
/act/dneb/value (.2)
/act/dneb/comma (,)
/act/dneb/value (1)
/act/dneb/comma (,)
/act/dneb/value (.4)
/act/dneb/comma (,)
/act/dneb/value (2)
/act/dneb/comma (,)
/act/dneb/value (.6)
/act/dneb/comma (,)
/act/dneb/value (3)
/act/dpeb
/act/dpeb/decl (dpeb)
/act/dpeb/value (0)
/act/dpeb/comma (,)
/act/dpeb/value (3)
/act/dpeb/comma (,)
/act/dpeb/value (1)
/act/dpeb/comma (,)
/act/dpeb/value (5)
/act/pecut
/act/pecut/decl (pecut)
/act/pecut/value (0)
/act/hlcut
/act/hlcut/decl (hlcut)
/act/hlcut/= (=)
/act/hlcut/value (.1)
/act/sample
/act/sample/decl (sample)
/act/sample/= (=)
/act/sample/value (correlate)
/act/sample
/act/sample/decl (sample)
/act/sample/value (nonfiss_cor)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}

// Test print hierarchy
TEST(MCNPInterpreter, print_hierarchy)
{
    std::stringstream input;
    input << R"I(Basic input test
1 0 1 

1 so 2

fq0   e t f d u s m c
)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (Basic input test)
/cell
/cell/id (1)
/cell/material (0)
/cell/geom
/cell/geom/id (1)
/BL (
)
/surface
/surface/id (1)
/surface/sph
/surface/sph/decl (so)
/surface/sph/r (2)
/BL (
)
/fq
/fq/decl (fq)
/fq/id (0)
/fq/value (e)
/fq/value (t)
/fq/value (f)
/fq/value (d)
/fq/value (u)
/fq/value (s)
/fq/value (m)
/fq/value (c)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}


// Test tally comment card
TEST(MCNPInterpreter, tally_comment_card)
{
    std::stringstream input;
    input << R"I(Basic input test
1000 0 10 

10 so 2

fc1000 this is a tally card comment
fc1001 multilne
     continued
fc1002 love column stuff      
)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (Basic input test)
/cell
/cell/id (1000)
/cell/material (0)
/cell/geom
/cell/geom/id (10)
/BL (
)
/surface
/surface/id (10)
/surface/sph
/surface/sph/decl (so)
/surface/sph/r (2)
/BL (
)
/fc
/fc/decl (fc)
/fc/id (1000)
/fc/comment ( this is a tally card comment)
/fc
/fc/decl (fc)
/fc/id (1001)
/fc/comment ( multilne)
/fc/comment (     continued)
/fc
/fc/decl (fc)
/fc/id (1002)
/fc/comment ( love column stuff      )
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}

// Test data f tally card
TEST(MCNPInterpreter, ftally_card)
{
    std::stringstream input;
    input << R"I(Basic input test
1000 0 10 

10 so 2

f5:! 1 2 3 3.14 4 5 6 3.14
fc5 tally comment for along x
f5x:~ 1 3 3.14 4 6 3.14
f18:E,P 1 2 3
f16:E,P 1 (2 3)
f1:E 1
f2:! 1
f4:E 5 T
f6:q 1
f7:u 313
*f211:n 3 
+F36 3
)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (Basic input test)
/cell
/cell/id (1000)
/cell/material (0)
/cell/geom
/cell/geom/id (10)
/BL (
)
/surface
/surface/id (10)
/surface/sph
/surface/sph/decl (so)
/surface/sph/r (2)
/BL (
)
/f5_tally
/f5_tally/decl (f)
/f5_tally/id (5)
/f5_tally/: (:)
/f5_tally/pt (!)
/f5_tally/point
/f5_tally/point/x (1)
/f5_tally/point/y (2)
/f5_tally/point/z (3)
/f5_tally/point/r (3.14)
/f5_tally/point
/f5_tally/point/x (4)
/f5_tally/point/y (5)
/f5_tally/point/z (6)
/f5_tally/point/r (3.14)
/fc
/fc/decl (fc)
/fc/id (5)
/fc/comment ( tally comment for along x)
/f5a_tally
/f5a_tally/decl (f)
/f5a_tally/id (5)
/f5a_tally/axis (x)
/f5a_tally/: (:)
/f5a_tally/pt (~)
/f5a_tally/point
/f5a_tally/point/o (1)
/f5a_tally/point/r (3)
/f5a_tally/point/ro (3.14)
/f5a_tally/point
/f5a_tally/point/o (4)
/f5a_tally/point/r (6)
/f5a_tally/point/ro (3.14)
/f8_tally
/f8_tally/decl (f)
/f8_tally/id (18)
/f8_tally/: (:)
/f8_tally/pt (E)
/f8_tally/comma (,)
/f8_tally/pt (P)
/f8_tally/value (1)
/f8_tally/value (2)
/f8_tally/value (3)
/f6_tally
/f6_tally/decl (f)
/f6_tally/id (16)
/f6_tally/: (:)
/f6_tally/pt (E)
/f6_tally/comma (,)
/f6_tally/pt (P)
/f6_tally/LP (()
/f6_tally/value (1)
/f6_tally/value (2)
/f6_tally/value (3)
/f6_tally/RP ())
/f1_tally
/f1_tally/decl (f)
/f1_tally/id (1)
/f1_tally/: (:)
/f1_tally/pt (E)
/f1_tally/value (1)
/f2_tally
/f2_tally/decl (f)
/f2_tally/id (2)
/f2_tally/: (:)
/f2_tally/pt (!)
/f2_tally/value (1)
/f4_tally
/f4_tally/decl (f)
/f4_tally/id (4)
/f4_tally/: (:)
/f4_tally/pt (E)
/f4_tally/value (5)
/f4_tally/accumulator (T)
/f6_tally
/f6_tally/decl (f)
/f6_tally/id (6)
/f6_tally/: (:)
/f6_tally/pt (q)
/f6_tally/value (1)
/f7_tally
/f7_tally/decl (f)
/f7_tally/id (7)
/f7_tally/: (:)
/f7_tally/pt (u)
/f7_tally/value (313)
/exw_tally
/exw_tally/exw (*)
/exw_tally/f1_tally
/exw_tally/f1_tally/decl (f)
/exw_tally/f1_tally/id (211)
/exw_tally/f1_tally/: (:)
/exw_tally/f1_tally/pt (n)
/exw_tally/f1_tally/value (3)
/jerks_tally
/jerks_tally/jflag (+)
/jerks_tally/f6_tally
/jerks_tally/f6_tally/decl (F)
/jerks_tally/f6_tally/id (36)
/jerks_tally/f6_tally/value (3)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}

// Test data mode card
TEST(MCNPInterpreter, data_mode_card)
{
    std::stringstream input;
    input << R"I(Basic input test
1000 0 10 

10 so 2

mode n q p e f | ! u < v > h g l b + _ - ~
mode -n -q -p -e -f -| -! -u -< -v -> -h -g -l -b -+ -_ -- -~
mode #
)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (Basic input test)
/cell
/cell/id (1000)
/cell/material (0)
/cell/geom
/cell/geom/id (10)
/BL (
)
/surface
/surface/id (10)
/surface/sph
/surface/sph/decl (so)
/surface/sph/r (2)
/BL (
)
/problem_type
/problem_type/decl (mode)
/problem_type/pt (n)
/problem_type/pt (q)
/problem_type/pt (p)
/problem_type/pt (e)
/problem_type/pt (f)
/problem_type/pt (|)
/problem_type/pt (!)
/problem_type/pt (u)
/problem_type/pt (<)
/problem_type/pt (v)
/problem_type/pt (>)
/problem_type/pt (h)
/problem_type/pt (g)
/problem_type/pt (l)
/problem_type/pt (b)
/problem_type/pt (+)
/problem_type/pt (_)
/problem_type/pt (-)
/problem_type/pt (~)
/problem_type
/problem_type/decl (mode)
/problem_type/pt (-n)
/problem_type/pt (-q)
/problem_type/pt (-p)
/problem_type/pt (-e)
/problem_type/pt (-f)
/problem_type/pt (-|)
/problem_type/pt (-!)
/problem_type/pt (-u)
/problem_type/pt (-<)
/problem_type/pt (-v)
/problem_type/pt (->)
/problem_type/pt (-h)
/problem_type/pt (-g)
/problem_type/pt (-l)
/problem_type/pt (-b)
/problem_type/pt (-+)
/problem_type/pt (-_)
/problem_type/pt (--)
/problem_type/pt (-~)
/problem_type
/problem_type/decl (mode)
/problem_type/pt (#)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}

// Test data tally times card
TEST(MCNPInterpreter, tally_times)
{
    std::stringstream input;
    input << R"I(Basic input test
1000 0 10 

10 so 2

t20 1 2.3  
t23 1 2.3 c
t25 1 T
t21 1 2.3 nt c
t22 1 2.3 c nt
t1 cbeg=1 cfrq 4 cofi 9 csub 1 cend 4
)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (Basic input test)
/cell
/cell/id (1000)
/cell/material (0)
/cell/geom
/cell/geom/id (10)
/BL (
)
/surface
/surface/id (10)
/surface/sph
/surface/sph/decl (so)
/surface/sph/r (2)
/BL (
)
/tally_time
/tally_time/decl (t)
/tally_time/id (20)
/tally_time/value (1)
/tally_time/value (2.3)
/tally_time
/tally_time/decl (t)
/tally_time/id (23)
/tally_time/value (1)
/tally_time/value (2.3)
/tally_time/cumulative (c)
/tally_time
/tally_time/decl (t)
/tally_time/id (25)
/tally_time/value (1)
/tally_time/cumulative (T)
/tally_time
/tally_time/decl (t)
/tally_time/id (21)
/tally_time/value (1)
/tally_time/value (2.3)
/tally_time/no_total (nt)
/tally_time/cumulative (c)
/tally_time
/tally_time/decl (t)
/tally_time/id (22)
/tally_time/value (1)
/tally_time/value (2.3)
/tally_time/cumulative (c)
/tally_time/no_total (nt)
/tally_time
/tally_time/decl (t)
/tally_time/id (1)
/tally_time/cbeg
/tally_time/cbeg/decl (cbeg)
/tally_time/cbeg/= (=)
/tally_time/cbeg/value (1)
/tally_time/cfrq
/tally_time/cfrq/decl (cfrq)
/tally_time/cfrq/value (4)
/tally_time/cofi
/tally_time/cofi/decl (cofi)
/tally_time/cofi/value (9)
/tally_time/csub
/tally_time/csub/decl (csub)
/tally_time/csub/value (1)
/tally_time/cend
/tally_time/cend/decl (cend)
/tally_time/cend/value (4)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}

// Test data kcode card
TEST(MCNPInterpreter, data_kcode)
{
    std::stringstream input;
    input << R"I(Basic input test
1000 0 10 

10 so 2

kcode 1000 1.0 30 100 4500 0 6500 0
kcode 1j 1.0 30 100 4500 2j 0
kcode 6j 6500 0
)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (Basic input test)
/cell
/cell/id (1000)
/cell/material (0)
/cell/geom
/cell/geom/id (10)
/BL (
)
/surface
/surface/id (10)
/surface/sph
/surface/sph/decl (so)
/surface/sph/r (2)
/BL (
)
/kcode
/kcode/decl (kcode)
/kcode/nsrck (1000)
/kcode/rkk (1.0)
/kcode/ikz (30)
/kcode/kct (100)
/kcode/msrk (4500)
/kcode/knrm (0)
/kcode/mrkp (6500)
/kcode/kc8 (0)
/kcode
/kcode/decl (kcode)
/kcode/jump (1j)
/kcode/rkk (1.0)
/kcode/ikz (30)
/kcode/kct (100)
/kcode/msrk (4500)
/kcode/jump (2j)
/kcode/kc8 (0)
/kcode
/kcode/decl (kcode)
/kcode/jump (6j)
/kcode/mrkp (6500)
/kcode/kc8 (0)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}


// Test data NPS card 
TEST(MCNPInterpreter, data_nps_card)
{
    std::stringstream input;
    input << R"I(title
1000 0 10 

10 so 2

nps 10000000
nps 100000  5000
nps j 10000 
)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (title)
/cell
/cell/id (1000)
/cell/material (0)
/cell/geom
/cell/geom/id (10)
/BL (
)
/surface
/surface/id (10)
/surface/sph
/surface/sph/decl (so)
/surface/sph/r (2)
/BL (
)
/nps
/nps/decl (nps)
/nps/npp (10000000)
/nps
/nps/decl (nps)
/nps/npp (100000)
/nps/npsmg (5000)
/nps
/nps/decl (nps)
/nps/jump (j)
/nps/npsmg (10000)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}

TEST(MCNPInterpreter, comment_scenarios)
{
    std::stringstream input;
    input << R"I(title
  1           0    3 $ comment 2
     #4  $ comment 3
C comment 4)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (title)
/cell
/cell/id (  1)
/cell/material (0)
/cell/geom
/cell/geom/id (3)
/cell/geom/comment ($ comment 2)
/cell/geom/# (#)
/cell/geom/id (4)
/cell/geom/comment ($ comment 3)
/comment (C comment 4)
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}

// Test surface with line continuations
TEST(MCNPInterpreter, surface_lc)
{
    std::stringstream input;
    input << R"I(the title
1 0 10 

158214   RPP  +55.390 +92.0  &
              -42.80 -105.0  &
              +48.70 +62.50
)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
/title (the title)
/cell
/cell/id (1)
/cell/material (0)
/cell/geom
/cell/geom/id (10)
/BL (
)
/surface
/surface/id (158214)
/surface/rppiped
/surface/rppiped/decl (RPP)
/surface/rppiped/xmin (+55.390)
/surface/rppiped/xmax (+92.0)
/surface/rppiped/LC (&)
/surface/rppiped/ymin (-42.80)
/surface/rppiped/ymax (-105.0)
/surface/rppiped/LC (&)
/surface/rppiped/zmin (+48.70)
/surface/rppiped/zmax (+62.50)
)I";
    ASSERT_EQ(expected.str(), paths.str());   
}

// Test data card template
TEST(MCNPInterpreter, DISABLED_data_template)
{
    std::stringstream input;
    input << R"I(title
1000 0 10 

10 so 2

)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}
// Test template
TEST(MCNPInterpreter, DISABLED_template)
{
    std::stringstream input;
    input << R"I(Basic input test
)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    std::stringstream paths;
    mcnpi.root().paths(paths);
    std::stringstream expected;
    expected << R"I(/
)I";

    ASSERT_EQ(expected.str(), paths.str());    
}
