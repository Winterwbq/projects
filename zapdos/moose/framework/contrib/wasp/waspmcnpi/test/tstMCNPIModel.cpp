#include "waspmcnpi/MCNPInterpreter.h"
#include "waspmcnpi/Model.h"
#include "gtest/gtest.h"
#include <iostream>
#include <string>
using namespace wasp;

void dotest(std::stringstream& input,
            const std::vector<std::vector<double>>& coeffs,
            const std::vector<mcnpi::Model::Surface_Type>& types, 
            const std::string& expected_cell_desc)
{
    ASSERT_EQ(coeffs.size(), types.size());
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    mcnpi::Model model;
    ASSERT_TRUE(model.build(mcnpi.root(), std::cerr));
    for(size_t isurface = 0; isurface < coeffs.size(); ++isurface)
    {
      SCOPED_TRACE(isurface);
      const auto& expected_surf_coeffs = coeffs[isurface];
      ASSERT_EQ(expected_surf_coeffs, model.surface_coefficients(isurface));
      ASSERT_EQ(types[isurface], model.surface_type(isurface));
    }
    std::stringstream cell_desc;
    model.describe_cells(cell_desc);
    ASSERT_EQ(expected_cell_desc, cell_desc.str());
}

static char generic_cell_desc[] = "cell 1 mat: 0 rho: 0 csg: (+9)\n"; 

TEST(MCNPModel, sphere)
{
    std::stringstream input;
    input << R"I(test
1 0 9

9 so 22.55
10 sx 1 22.55
11 sy 2 22.55
12 sz 3 22.55
)I";
    
    std::vector<std::vector<double>> coeffs = {{0,0,0, 22.55},
                                {1,0,0, 22.55},
                                {0,2,0, 22.55},
                                {0,0,3, 22.55}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::SPH, 
                                                    mcnpi::Model::Surface_Type::SPH,
                                                    mcnpi::Model::Surface_Type::SPH,
                                                    mcnpi::Model::Surface_Type::SPH};
    dotest(input, coeffs, types, generic_cell_desc);
}

TEST(MCNPModel, sq)
{
    std::stringstream input;
    input << R"I(test
1 0 9

9 sq 1 2 3 4 5 6 7 -1 .2 .3
)I";
    std::vector<std::vector<double>> coeffs = {{1, 2, 3, 4, 5, 6, 7, -1, .2, .3}};    
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::SQ};
    dotest(input, coeffs, types, generic_cell_desc);
}

TEST(MCNPModel, gq)
{
    std::stringstream input;
    input << R"I(test
1 0 9

9 gq 1 2 3 4 5 6 7 -1 .2 .3
)I";
    std::vector<std::vector<double>> coeffs = {{1, 2, 3, 4, 5, 6, 7, -1, .2, .3}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::GQ};
    dotest(input, coeffs, types, generic_cell_desc);
    
}


TEST(MCNPModel, cylinder)
{
    std::stringstream input;
    input << R"I(test
1 0 9

9 cx 1
10 cy 2
11 cz 3
)I";
    std::vector<std::vector<double>> coeffs = {{1}, {2}, {3}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::CX,mcnpi::Model::Surface_Type::CY, mcnpi::Model::Surface_Type::CZ};
    dotest(input, coeffs, types, generic_cell_desc);
}
TEST(MCNPModel, pcylinder)
{
    std::stringstream input;
    input << R"I(test
1 0 9

9 c/x 1 2 3
10 c/y 2 3 4
11 c/z 3 4 5
)I";
    std::vector<std::vector<double>> coeffs = {{1,2,3}, {2,3,4}, {3,4,5}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::C_X,mcnpi::Model::Surface_Type::C_Y, mcnpi::Model::Surface_Type::C_Z};
    dotest(input, coeffs, types, generic_cell_desc);
}

TEST(MCNPModel, cone)
{
    std::stringstream input;
    input << R"I(test
1 0 9

9 kx 1 2 1
10 ky 2 3 -1
11 kz 3 4 1
)I";

    std::vector<std::vector<double>> coeffs = {{1,2,1}, {2,3,-1}, {3,4,1}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::KX,mcnpi::Model::Surface_Type::KY, mcnpi::Model::Surface_Type::KZ};
    dotest(input, coeffs, types, generic_cell_desc);
}

TEST(MCNPModel, pcone)
{
    std::stringstream input;
    input << R"I(test
1 0 9

9 k/x 1 2 5 6 1
10 k/y 2 3 4 5 -1
11 k/z 1 2 3 4 1
)I";
    std::vector<std::vector<double>> coeffs = {{1,2,5,6,1}, {2,3,4,5,-1}, {1,2,3,4,1}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::K_X,mcnpi::Model::Surface_Type::K_Y, mcnpi::Model::Surface_Type::K_Z};
    dotest(input, coeffs, types, generic_cell_desc);
}

TEST(MCNPModel, torus)
{
    std::stringstream input;
    input << R"I(test
1 0 9

9 tx 1 2 3 4 5 6
10 ty 0 2 0  1 2 3 
11 tz 3.2 1 3.1 4 1 5
)I";
    std::vector<std::vector<double>> coeffs = {{1,2,3,4,5,6}, {0,2,0,1,2,3}, {3.2,1,3.1,4,1,5}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::TX,mcnpi::Model::Surface_Type::TY, mcnpi::Model::Surface_Type::TZ};
    dotest(input, coeffs, types, generic_cell_desc);
}

TEST(MCNPModel, plane)
{
    std::stringstream input;
    input << R"I(test
1 0 9

9 px 1 
10 py 0 
11 pz 3.2 
)I";
    std::vector<std::vector<double>> coeffs = {{1}, {0}, {3.2}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::PX,mcnpi::Model::Surface_Type::PY, mcnpi::Model::Surface_Type::PZ};
    dotest(input, coeffs, types, generic_cell_desc);
}

TEST(MCNPModel, points)
{
    std::stringstream input;
    input << R"I(test
1 0 9

9 x 0 1.11 22.3 2.67
10 y -10 3
11 z -1.3 17.98 -1.6 0.0
)I";
    std::vector<std::vector<double>> coeffs = {{0, 1.11,22.3,2.67}, {-10,3}, {-1.3,17.98,-1.6,0.0}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::X,mcnpi::Model::Surface_Type::Y, mcnpi::Model::Surface_Type::Z};
    dotest(input, coeffs, types, generic_cell_desc);
}


TEST(MCNPModel, abcplane)
{
    std::stringstream input;
    input << R"I(test
1 0 9

9 p 1.7320508076 -1.0 0.0 -52.75331
)I";
    std::vector<std::vector<double>> coeffs = {{1.7320508076, -1.0, 0.0, -52.75331}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::PLANE};
    dotest(input, coeffs, types, generic_cell_desc);
}

TEST(MCNPModel, box)
{
    std::stringstream input;
    input << R"I(test
1 0 9

9 box 1 2 3 10.0 0 0 11 1.3 4.5 5.5 6.5 7.8
)I";
    std::vector<std::vector<double>> coeffs = {{1, 2, 3, 10.0, 0, 0, 11, 1.3, 4.5, 5.5, 6.5, 7.8}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::BOX};
    dotest(input, coeffs, types, generic_cell_desc);
}

TEST(MCNPModel, hex)
{
    std::stringstream input;
    input << R"I(test
1 0 9

9 rhp 0 +849.90 0 0 4.0 0 -.055 0 .00001 0 0 .055 .055 0 .00001 
)I";
    std::vector<std::vector<double>> coeffs = {{0, +849.90, 0, 0, 4.0, 0, -.055, 0, .00001, 0, 0, .055, .055, 0, .00001}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::HEX};
    dotest(input, coeffs, types, generic_cell_desc);
}

TEST(MCNPModel, rpp)
{
    std::stringstream input;
    input << R"I(test
1 0 9

9 rpp -1.66 1.66 -1.66 1.66 0.65 4.3750
)I";
    std::vector<std::vector<double>> coeffs = {{-1.66, 1.66, -1.66, 1.66, 0.65, 4.3750}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::RPP};
    dotest(input, coeffs, types, generic_cell_desc);
}

TEST(MCNPModel, rcc)
{
    std::stringstream input;
    input << R"I(test
1 0 9

9 rcc 1 2 3 4 5 6 3.14
)I";
    std::vector<std::vector<double>> coeffs = {{1, 2, 3, 4, 5, 6, 3.14}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::RCC};
    dotest(input, coeffs, types, generic_cell_desc);
}

TEST(MCNPModel, rec)
{
    std::stringstream input;
    input << R"I(test
1 0 9

9 rec 1 2 3 4 5 6 7 8 9 10 11 12 $ explicit v2
10 rec 1 2 3 4 5 6 7 8 9 10 $ cross product of h and v1
)I";
    std::vector<std::vector<double>> coeffs = {{1, 2, 3, 4, 5, 6, 7,8,9,10,11,12}, {1, 2, 3, 4, 5, 6, 7,8,9,10}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::REC,mcnpi::Model::Surface_Type::REC};
    dotest(input, coeffs, types, generic_cell_desc);
}

TEST(MCNPModel, plpts)
{
    std::stringstream input;
    input << R"I(test
1 0 9

9 p 1 2 3 4 5 6 7 8 9
)I";
    std::vector<std::vector<double>> coeffs = {{1, 2, 3, 4, 5, 6, 7,8,9}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::PLANE};
    dotest(input, coeffs, types, generic_cell_desc);
}

TEST(MCNPModel, dup_id_erro)
{
    std::stringstream input;
    input << R"I(test
1 0 9

9 rcc 1 2 3 4 5 6 3.14
9 rcc 1 2 3 4 5 6 3.14
)I";
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));

    mcnpi::Model model;
    std::stringstream errors;
    ASSERT_FALSE(model.build(mcnpi.root(), errors));
    ASSERT_EQ("Error - line: 5 surface id (9) collides with surface defined at line 4!\n", errors.str());
}

TEST(MCNPModel, surfaces)
{
    std::stringstream input;
    input << R"I(test
1 0 9

1	box	-10.9455	14.606	0	3.647	0	0	0	3.647	0						
2	box	-12.485	849.9	-12.485	24.97	0	0	0	4	0	0	0	24.97			
3	c/x	-0.9525	-17.83715	0.3556												
4	c/y	-356.87	157.4	16.37												
5	c/y	8.89	92.105	7.065												
6	c/z	-0.1	-0.1732	0.4												
7	c/z	-0.20461	-0.35439	0.12192												
8	cx	0.00001														
9	cy	0.1														
10	cz	0.0001														
11	gq	0.5	0.5	1	-1	0	0	0	0	0	-0.16					
12	k/x	-18	1	0	0.22											
13	k/x	14.05001	0	2.80035	1											
14	k/x	18	0	0	0.22											
15	k/y	35	1405	105	0.64											
16	k/z	17.7038	0	-13.39215	1											
17	k/z	2.54	0	2.54	1	1										
18	kx	-1.00E-12	2.25	1												
19	KX	0.1	0.001	1												
20	kx	18	0.22													
21	ky	-1	0.3	-1												
22	ky	-208.36	0.01784283	1												
23	ky	-283.3	6.86E-04													
24	ky	13	1	-1												
25	ky	18	0.22													
26	kz	-0.11511	130.646	-1												
27	kz	-0.29083	1	1												
28	p	-0.3639702	1	0	0											
29	p	10.9455	-18.253	5	14.5925	-14.606	0	12.769	-16.4295	14						
30	p	28.6362533	1	0	0											
31	P	6.8707	133.225	29.86	-6.8707	101.286	31.05	6.8707	69.3467	32.23						
32	px	-0.0000001														
33	PX	0														
34	px	0.00001														
35	py	-0.0000001														
36	Py	0														
37	py	0.00001														
38	pz	-0.0002														
39	pz	-0.001														
40	PZ	0														
41	pz	0.00001														
42	pz	3.13182														
43	RCC	-1	-1	0	2	0	0	0.1								
44	rcc	6.1439	0	6.1439	0.08211	0	0.08211	1.11125								
45	rcc	67.46992	18.07581	-21.41474	0	0	133.985	0.9525								
46	rec	0	0	0	2	0	0	0	2	0	0	0	3			
47	rec	4	0	0	2	0	0	0	2	0	0	0	3			
48	rhp	0	0	-10	0	0	50	0	0.63501	0						
49	rhp	0	849.9	0	0	4	0	-0.045	0	0.000001	0	0	0.045	0.045	0	0.000001
50	rhp	0	849.9	0	0	4	0	-0.055	0	0.000001	0	0	0.055	0.055	0	0.000001
51	rpp	-0.0889	0.0889	-0.9525	4.1275	1.281684	1.429004									
52	rpp	0	5000	-2500	2500	-2500	2500									
53	rpp	0.1016	0.14097	0.1016	0.18288	1.02108	2.54									
54	rpp	54	115	-41	41	-41	41									
55	rpp	700	800	-850	-800	0	300									
56	s	10	0.9	0.9												
57	s	50	55	0	7											
58	so	0.0001														
59	sph	0	0	0	1											
60	sph	0	0	0	100											
61	sph	3.5	3.5	8.5	2.4											
62	sq	0.028	1	1	0	0	0	-1	0	15	-5					
63	sq	0.028	1	1	0	0	0	-1	0	15	5					
64	sq	0.05	0.2	1	0	0	0	-16	0	-6	-20					
65	sq	0.1	0	0.05	0	1	0	-4	0	-11	17					
66	sq	0.1	0	0.07	0	-0.3	0	-10	0	-1	16					
67	sq	1	0.00448	1	0	0	0	-1	-5	0	-5					
68	sq	1	2	0	0	0	0	-1	0.2	0	0					
69	sq	1	4	4	0	0	0	-6400	0	0	0					
70	sq	14.4926294	14.4926294	14.4568504	0	0	0	-1122.970371	0	0	0					
71	sq	2.0420524	2.0420524	2.0241629	0	0	0	-158.258257	0	0	0					
72	sq	2.0420524	2.0420524	2.0599399	0	0	0	-158.258257	0	0	0					
73	SQ	25	100	0	0	0	0	-4	0	0	0					
74	sq	61.0220372	61.0220372	60.9504916	0	0	0	-4724.729401	0	0	-0.004826					
75	sx	-109	10													
76	sy	-30	10													
77	sy	-60	18													
78	sy	0.01	15.4													
79	sz	-105.6727	102.8227													
80	sz	-21	15													
81	sz	-25	20													
82	sz	-4	1													
83	trc	-0.01	0	0	25.01	0	0	0	10.35948							
84	trc	0	0	-1.429004	0	0	0.55372	0.635	0.315309609							
85	tx	0	0	0	10	2	2									
86	ty	0	0	0	12	2	2.5									
87	ty	35	80	105	2330	1785	1785									
88	tz	0	0	-10	11	2.2	1.6									
89	tz	0	0	9.05129	1.42875	0.15875	0.15875									
90	x	-0.041	0.509	19.959	1.89											
91	x	-9	0	7	0	-1	8									
92	x	68.43516	0	-830.35884	438.3711											
93	y	-10	3													
94	y	930	0													
95	z	-1.3	17.98	-1.6	0											
96	z	7.5	3.1	12	4	14.8	5									
)I";
    std::vector<std::vector<double>> coeffs = {
          {-10.9455,14.606,0,3.647,0,0,0,3.647,0},
          {-12.485,849.9,-12.485,24.97,0,0,0,4,0,0,0,24.97},
          {-0.9525,-17.83715,0.3556},
          {-356.87,157.4,16.37},
          {8.89,92.105,7.065},
          {-0.1,-0.1732,0.4},
          {-0.20461,-0.35439,0.12192},
          {0.00001},
          {0.1},
          {0.0001},
          {0.5,0.5,1,-1,0,0,0,0,0,-0.16},
          {-18,1,0,0.22,0},
          {14.05001,0,2.80035,1,0},
          {18,0,0,0.22,0},
          {35,1405,105,0.64, 0},
          {17.7038,0,-13.39215,1, 0},
          {2.54,0,2.54,1,1},
          {-1.00E-12,2.25,1},
          {0.1,0.001,1},
          {18,0.22, 0},
          {-1,0.3,-1},
          {-208.36,0.01784283,1},
          {-283.3,6.86E-04, 0},
          {13,1,-1},
          {18,0.22, 0},
          {-0.11511,130.646,-1},
          {-0.29083,1,1},
          {-0.3639702,1,0,0},
          {10.9455,-18.253,5,14.5925,-14.606,0,12.769,-16.4295,14},
          {28.6362533,1,0,0},
          {6.8707,133.225,29.86,-6.8707,101.286,31.05,6.8707,69.3467,32.23},
          {-0.0000001},
          {0},
          {0.00001},
          {-0.0000001},
          {0},
          {0.00001},
          {-0.0002},
          {-0.001},
          {0},
          {0.00001},
          {3.13182},
          {-1,-1,0,2,0,0,0.1},
          {6.1439,0,6.1439,0.08211,0,0.08211,1.11125},
          {67.46992,18.07581,-21.41474,0,0,133.985,0.9525},
          {0,0,0,2,0,0,0,2,0,0,0,3},
          {4,0,0,2,0,0,0,2,0,0,0,3},
          {0,0,-10,0,0,50,0,0.63501,0},
          {0,849.9,0,0,4,0,-0.045,0,0.000001,0,0,0.045,0.045,0,0.000001},
          {0,849.9,0,0,4,0,-0.055,0,0.000001,0,0,0.055,0.055,0,0.000001},
          {-0.0889,0.0889,-0.9525,4.1275,1.281684,1.429004},
          {0,5000,-2500,2500,-2500,2500},
          {0.1016,0.14097,0.1016,0.18288,1.02108,2.54},
          {54,115,-41,41,-41,41},
          {700,800,-850,-800,0,300},
          {10,0.9,0.9, 1},
          {50,55,0,7},
          {0,0,0, 0.0001},
          {0,0,0,1},
          {0,0,0,100},
          {3.5,3.5,8.5,2.4},
          {0.028,1,1,0,0,0,-1,0,15,-5},
          {0.028,1,1,0,0,0,-1,0,15,5},
          {0.05,0.2,1,0,0,0,-16,0,-6,-20},
          {0.1,0,0.05,0,1,0,-4,0,-11,17},
          {0.1,0,0.07,0,-0.3,0,-10,0,-1,16},
          {1,0.00448,1,0,0,0,-1,-5,0,-5},
          {1,2,0,0,0,0,-1,0.2,0,0},
          {1,4,4,0,0,0,-6400,0,0,0},
          {14.4926294,14.4926294,14.4568504,0,0,0,-1122.970371,0,0,0},
          {2.0420524,2.0420524,2.0241629,0,0,0,-158.258257,0,0,0},
          {2.0420524,2.0420524,2.0599399,0,0,0,-158.258257,0,0,0},
          {25,100,0,0,0,0,-4,0,0,0},
          {61.0220372,61.0220372,60.9504916,0,0,0,-4724.729401,0,0,-0.004826},
          {-109,0,0,10},
          {0,-30,0,10},
          {0,-60,0,18},
          {0,0.01,0,15.4},
          {0,0,-105.6727,102.8227},
          {0,0,-21,15},
          {0,0,-25,20},
          {0,0,-4,1},
          {-0.01,0,0,25.01,0,0,0,10.35948},
          {0,0,-1.429004,0,0,0.55372,0.635,0.315309609},
          {0,0,0,10,2,2},
          {0,0,0,12,2,2.5},
          {35,80,105,2330,1785,1785},
          {0,0,-10,11,2.2,1.6},
          {0,0,9.05129,1.42875,0.15875,0.15875},
          {-0.041,0.509,19.959,1.89},
          {-9,0,7,0,-1,8},
          {68.43516,0,-830.35884,438.3711},
          {-10,3},
          {930,0},
          {-1.3,17.98,-1.6,0},
          {7.5,3.1,12,4,14.8,5},
          };
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::BOX,
                                                  mcnpi::Model::Surface_Type::BOX,
                                                  mcnpi::Model::Surface_Type::C_X,
                                                  mcnpi::Model::Surface_Type::C_Y,
                                                  mcnpi::Model::Surface_Type::C_Y,
                                                  mcnpi::Model::Surface_Type::C_Z,
                                                  mcnpi::Model::Surface_Type::C_Z,
                                                  mcnpi::Model::Surface_Type::CX,
                                                  mcnpi::Model::Surface_Type::CY,
                                                  mcnpi::Model::Surface_Type::CZ,
                                                  mcnpi::Model::Surface_Type::GQ,
                                                  mcnpi::Model::Surface_Type::K_X,
                                                  mcnpi::Model::Surface_Type::K_X,
                                                  mcnpi::Model::Surface_Type::K_X,
                                                  mcnpi::Model::Surface_Type::K_Y,
                                                  mcnpi::Model::Surface_Type::K_Z,
                                                  mcnpi::Model::Surface_Type::K_Z,
                                                  mcnpi::Model::Surface_Type::KX,
                                                  mcnpi::Model::Surface_Type::KX,
                                                  mcnpi::Model::Surface_Type::KX,
                                                  mcnpi::Model::Surface_Type::KY,
                                                  mcnpi::Model::Surface_Type::KY,
                                                  mcnpi::Model::Surface_Type::KY,
                                                  mcnpi::Model::Surface_Type::KY,
                                                  mcnpi::Model::Surface_Type::KY,
                                                  mcnpi::Model::Surface_Type::KZ,
                                                  mcnpi::Model::Surface_Type::KZ,
                                                  mcnpi::Model::Surface_Type::PLANE,
                                                  mcnpi::Model::Surface_Type::PLANE,
                                                  mcnpi::Model::Surface_Type::PLANE,
                                                  mcnpi::Model::Surface_Type::PLANE,
                                                  mcnpi::Model::Surface_Type::PX,
                                                  mcnpi::Model::Surface_Type::PX,
                                                  mcnpi::Model::Surface_Type::PX,
                                                  mcnpi::Model::Surface_Type::PY,
                                                  mcnpi::Model::Surface_Type::PY,
                                                  mcnpi::Model::Surface_Type::PY,
                                                  mcnpi::Model::Surface_Type::PZ,
                                                  mcnpi::Model::Surface_Type::PZ,
                                                  mcnpi::Model::Surface_Type::PZ,
                                                  mcnpi::Model::Surface_Type::PZ,
                                                  mcnpi::Model::Surface_Type::PZ,
                                                  mcnpi::Model::Surface_Type::RCC,
                                                  mcnpi::Model::Surface_Type::RCC,
                                                  mcnpi::Model::Surface_Type::RCC,
                                                  mcnpi::Model::Surface_Type::REC,
                                                  mcnpi::Model::Surface_Type::REC,
                                                  mcnpi::Model::Surface_Type::HEX,
                                                  mcnpi::Model::Surface_Type::HEX,
                                                  mcnpi::Model::Surface_Type::HEX,
                                                  mcnpi::Model::Surface_Type::RPP,
                                                  mcnpi::Model::Surface_Type::RPP,
                                                  mcnpi::Model::Surface_Type::RPP, 
                                                  mcnpi::Model::Surface_Type::RPP,
                                                  mcnpi::Model::Surface_Type::RPP,
                                                  mcnpi::Model::Surface_Type::SPH,
                                                  mcnpi::Model::Surface_Type::SPH,
                                                  mcnpi::Model::Surface_Type::SPH,
                                                  mcnpi::Model::Surface_Type::SPH,
                                                  mcnpi::Model::Surface_Type::SPH,
                                                  mcnpi::Model::Surface_Type::SPH,
                                                  mcnpi::Model::Surface_Type::SQ,
                                                  mcnpi::Model::Surface_Type::SQ,
                                                  mcnpi::Model::Surface_Type::SQ,
                                                  mcnpi::Model::Surface_Type::SQ,
                                                  mcnpi::Model::Surface_Type::SQ,
                                                  mcnpi::Model::Surface_Type::SQ,
                                                  mcnpi::Model::Surface_Type::SQ,
                                                  mcnpi::Model::Surface_Type::SQ,
                                                  mcnpi::Model::Surface_Type::SQ,
                                                  mcnpi::Model::Surface_Type::SQ,
                                                  mcnpi::Model::Surface_Type::SQ,
                                                  mcnpi::Model::Surface_Type::SQ,
                                                  mcnpi::Model::Surface_Type::SQ,
                                                  mcnpi::Model::Surface_Type::SPH,
                                                  mcnpi::Model::Surface_Type::SPH,
                                                  mcnpi::Model::Surface_Type::SPH,
                                                  mcnpi::Model::Surface_Type::SPH,
                                                  mcnpi::Model::Surface_Type::SPH,
                                                  mcnpi::Model::Surface_Type::SPH,
                                                  mcnpi::Model::Surface_Type::SPH,
                                                  mcnpi::Model::Surface_Type::SPH,
                                                  mcnpi::Model::Surface_Type::TRC,
                                                  mcnpi::Model::Surface_Type::TRC,
                                                  mcnpi::Model::Surface_Type::TX,
                                                  mcnpi::Model::Surface_Type::TY,
                                                  mcnpi::Model::Surface_Type::TY,
                                                  mcnpi::Model::Surface_Type::TZ,
                                                  mcnpi::Model::Surface_Type::TZ,
                                                  mcnpi::Model::Surface_Type::X,
                                                  mcnpi::Model::Surface_Type::X,
                                                  mcnpi::Model::Surface_Type::X,
                                                  mcnpi::Model::Surface_Type::Y,
                                                  mcnpi::Model::Surface_Type::Y,
                                                  mcnpi::Model::Surface_Type::Z,
                                                  mcnpi::Model::Surface_Type::Z,
                                                  };
    dotest(input, coeffs, types, generic_cell_desc);
}

TEST(MCNPModel, csg_single_intersection)
{
    std::stringstream input;
    input << R"I(test
1 0 8 9

8	so	1														
9	sph	.5	0	0	.9											
10	sph	0	.5	0	1.1
)I";
    std::vector<std::vector<double>> coeffs = {{0,0,0,1},{.5,0,0,.9}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH};
    dotest(input, coeffs, types, "cell 1 mat: 0 rho: 0 csg: (+8^+9)\n");
}
TEST(MCNPModel, csg_single_parenthetical_intersection)
{
    std::stringstream input;
    input << R"I(test
1 0 (8) (9)

8	so	1														
9	sph	.5	0	0	.9											
10	sph	0	.5	0	1.1
)I";
    std::vector<std::vector<double>> coeffs = {{0,0,0,1},{.5,0,0,.9}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH};
    dotest(input, coeffs, types, "cell 1 mat: 0 rho: 0 csg: (+8^+9)\n");
}
TEST(MCNPModel, csg_union)
{
    std::stringstream input;
    input << R"I(test
1 0 8:9

8	so	1														
9	sph	.5	0	0	.9
)I";
    std::vector<std::vector<double>> coeffs = {{0,0,0,1},{.5,0,0,.9}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH};
    dotest(input, coeffs, types, "cell 1 mat: 0 rho: 0 csg: (+8U+9)\n");
}

// Test associativety of intersection of intersection
// Test left operand surface sense
TEST(MCNPModel, csg_double_intersection)
{
    std::stringstream input;
    input << R"I(test
1 0 8 -9 +10

8	so	1														
9	sph	.5	0	0	.9											
10	sph	0	.5	0	1.1
)I";
    std::vector<std::vector<double>> coeffs = {{0,0,0,1},{.5,0,0,.9}, {0,.5,0,1.1}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH};
    dotest(input, coeffs, types, "cell 1 mat: 0 rho: 0 csg: ((+8^-9)^+10)\n");
}

// Test precedence of union vs intersect
// Test right operand surface sense
TEST(MCNPModel, csg_intersection_union)
{
    std::stringstream input;
    input << R"I(test
1 0 8:9 -10

8	so	1														
9	sph	.5	0	0	.9
10	sph	0	.5	0	1.1
)I";
    std::vector<std::vector<double>> coeffs = {{0,0,0,1},{.5,0,0,.9},{0,.5,0,1.1}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH};
    dotest(input, coeffs, types, "cell 1 mat: 0 rho: 0 csg: (+8U(+9^-10))\n");
}

// Test precedence of union vs intersect
// Test right operand surface sense
// Test parenthesis configuration
TEST(MCNPModel, csg_intersection_union_p1)
{
    std::stringstream input;
    input << R"I(test
1 0 (8):(9) -10

8	so	1														
9	sph	.5	0	0	.9
10	sph	0	.5	0	1.1
)I";
    std::vector<std::vector<double>> coeffs = {{0,0,0,1},{.5,0,0,.9},{0,.5,0,1.1}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH};
    dotest(input, coeffs, types, "cell 1 mat: 0 rho: 0 csg: (+8U(+9^-10))\n");
}
// Test precedence of union vs intersect
// Test right operand surface sense
// Test parenthesis configuration
TEST(MCNPModel, csg_intersection_union_p2)
{
    std::stringstream input;
    input << R"I(test
1 0 (8:9) -10

8	so	1														
9	sph	.5	0	0	.9
10	sph	0	.5	0	1.1
)I";
    std::vector<std::vector<double>> coeffs = {{0,0,0,1},{.5,0,0,.9},{0,.5,0,1.1}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH};
    dotest(input, coeffs, types, "cell 1 mat: 0 rho: 0 csg: ((+8U+9)^-10)\n");
}
// Test precedence of union vs intersect
// Test right operand surface sense
TEST(MCNPModel, csg_complement)
{
    std::stringstream input;
    input << R"I(test
1 0 #8

8	so	1														
9	sph	.5	0	0	.9
10	sph	0	.5	0	1.1
)I";
    std::vector<std::vector<double>> coeffs = {{0,0,0,1},{.5,0,0,.9},{0,.5,0,1.1}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH};
    dotest(input, coeffs, types, "cell 1 mat: 0 rho: 0 csg: (#+8)\n");
}
// Test tricky implicit intersection and unary complement operators
TEST(MCNPModel, csg_intersection_trailing_complement)
{
    std::stringstream input;
    input << R"I(test
1 0 -9 #8

8	so	1														
9	sph	.5	0	0	.9
10	sph	0	.5	0	1.1
)I";
    std::vector<std::vector<double>> coeffs = {{0,0,0,1},{.5,0,0,.9},{0,.5,0,1.1}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH};
    dotest(input, coeffs, types, "cell 1 mat: 0 rho: 0 csg: (-9^(#+8))\n");
}
// Test right operand surface sense
// Test parenthesis configuration
TEST(MCNPModel, csg_complement_p1)
{
    std::stringstream input;
    input << R"I(test
1 0 #(8)

8	so	1														
9	sph	.5	0	0	.9
10	sph	0	.5	0	1.1
)I";
    std::vector<std::vector<double>> coeffs = {{0,0,0,1},{.5,0,0,.9},{0,.5,0,1.1}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH};
    dotest(input, coeffs, types, "cell 1 mat: 0 rho: 0 csg: (#+8)\n");
}
// Test precedence of complement vs union
// Test parenthesis configuration
TEST(MCNPModel, csg_complement_p2)
{
    std::stringstream input;
    input << R"I(test
1 0 #8:9

8	so	1														
9	sph	.5	0	0	.9
10	sph	0	.5	0	1.1
)I";
    std::vector<std::vector<double>> coeffs = {{0,0,0,1},{.5,0,0,.9},{0,.5,0,1.1}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH};
    dotest(input, coeffs, types, "cell 1 mat: 0 rho: 0 csg: ((#+8)U+9)\n");
}

// Test precedence of complement vs intersect
// Test right operand surface sense
TEST(MCNPModel, csg_complement_p3)
{
    std::stringstream input;
    input << R"I(test
1 0 #8 -9

8	so	1														
9	sph	.5	0	0	.9
10	sph	0	.5	0	1.1
)I";
    std::vector<std::vector<double>> coeffs = {{0,0,0,1},{.5,0,0,.9},{0,.5,0,1.1}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH};
    dotest(input, coeffs, types, "cell 1 mat: 0 rho: 0 csg: ((#+8)^-9)\n");
}

// Test precedence of complement vs intersect vs parenthesis
TEST(MCNPModel, csg_complement_p4)
{
    std::stringstream input;
    input << R"I(test
1 0 #(8 -9 : 10)

8	so	1														
9	sph	.5	0	0	.9
10	sph	0	.5	0	1.1
)I";
    std::vector<std::vector<double>> coeffs = {{0,0,0,1},{.5,0,0,.9},{0,.5,0,1.1}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH};
    dotest(input, coeffs, types, "cell 1 mat: 0 rho: 0 csg: (#((+8^-9)U+10))\n");
}

// Test precedence of complement vs intersect vs parenthesis
TEST(MCNPModel, csg_complement_p5)
{
    std::stringstream input;
    input << R"I(test
1 0 #(8 (-9 : 10))

8	so	1														
9	sph	.5	0	0	.9
10	sph	0	.5	0	1.1
)I";
    std::vector<std::vector<double>> coeffs = {{0,0,0,1},{.5,0,0,.9},{0,.5,0,1.1}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH};
    dotest(input, coeffs, types, "cell 1 mat: 0 rho: 0 csg: (#(+8^(-9U+10)))\n");
}

// Test precedence of complement vs intersect vs parenthesis
TEST(MCNPModel, csg_complement_p6)
{
    std::stringstream input;
    input << R"I(test
1 0 #8 (-9 : 10)

8	so	1														
9	sph	.5	0	0	.9
10	sph	0	.5	0	1.1
)I";
    std::vector<std::vector<double>> coeffs = {{0,0,0,1},{.5,0,0,.9},{0,.5,0,1.1}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH};
    dotest(input, coeffs, types, "cell 1 mat: 0 rho: 0 csg: ((#+8)^(-9U+10))\n");
}

// Test cell reference
TEST(MCNPModel, csg_cell_ref)
{
    std::stringstream input;
    input << R"I(test
1 0 (-9 : 10)
3 0 8 #1

8	so	1														
9	sph	.5	0	0	.9
10	sph	0	.5	0	1.1
)I";
    std::vector<std::vector<double>> coeffs = {{0,0,0,1},{.5,0,0,.9},{0,.5,0,1.1}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH};
    dotest(input, coeffs, types, "cell 1 mat: 0 rho: 0 csg: (-9U+10)\n"
                                 "cell 3 mat: 0 rho: 0 csg: (+8^(#(-9U+10)))\n");
}

// Test multiple cell reference
TEST(MCNPModel, csg_cell_refs)
{
    std::stringstream input;
    input << R"I(test
1 0 -9
2 0 10
3 0 8 #1 #2

8	so	1														
9	sph	.5	0	0	.9
10	sph	0	.5	0	1.1
)I";
    std::vector<std::vector<double>> coeffs = {{0,0,0,1},{.5,0,0,.9},{0,.5,0,1.1}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH};
    dotest(input, coeffs, types, "cell 1 mat: 0 rho: 0 csg: (-9)\n"
                                 "cell 2 mat: 0 rho: 0 csg: (+10)\n"
                                 "cell 3 mat: 0 rho: 0 csg: ((+8^(#(-9)))^(#(+10)))\n");
}

// Test multiple cell reference
TEST(MCNPModel, csg_cell_refs_unordered)
{
    std::stringstream input;
    input << R"I(test
2 0 10
1 0 -9
3 0 8 #1 #2

8	so	1														
9	sph	.5	0	0	.9
10	sph	0	.5	0	1.1
)I";
    std::vector<std::vector<double>> coeffs = {{0,0,0,1},{.5,0,0,.9},{0,.5,0,1.1}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH};
    dotest(input, coeffs, types, "cell 2 mat: 0 rho: 0 csg: (+10)\n"
                                 "cell 1 mat: 0 rho: 0 csg: (-9)\n"
                                 "cell 3 mat: 0 rho: 0 csg: ((+8^(#(-9)))^(#(+10)))\n");
}

// Test multiple cell reference
TEST(MCNPModel, csg_cell_like_but)
{
    std::stringstream input;
    input << R"I(test
2 0 10
1 0 -9
3 like 2 but mat 3 rho 1.43

8	so	1														
9	sph	.5	0	0	.9
10	sph	0	.5	0	1.1

m3 8016.81c 1.43
)I";
    std::vector<std::vector<double>> coeffs = {{0,0,0,1},{.5,0,0,.9},{0,.5,0,1.1}};
    std::vector<mcnpi::Model::Surface_Type> types = {mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH,
                                                        mcnpi::Model::Surface_Type::SPH};
    dotest(input, coeffs, types, "cell 2 mat: 0 rho: 0 csg: (+10)\n"
                                 "cell 1 mat: 0 rho: 0 csg: (-9)\n"
                                 "cell 3 mat: 3 rho: 1.43 csg: (+10)\n");
}

void doerrortest(std::stringstream& input, const std::string& expected_error)
{
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    mcnpi::Model model;
    std::stringstream errors;
    ASSERT_FALSE(model.build(mcnpi.root(), errors));
    ASSERT_EQ(expected_error, errors.str());
}

// Testing bad surface reference in cell
TEST(MCNPModel, csg_surface_ref_errors)
{
    std::stringstream input;
    input << R"I(test
1 0 -90

9 sph .5 0 0 .9
)I";
    doerrortest(input, "***Error - line: 2, column: 5, unable to find surface with identifier '90'!\n");
}
// Testing bad surface/cell reference in complement
TEST(MCNPModel, csg_cell_ref_errors)
{
    std::stringstream input;
    input << R"I(test
1 0 -9
2 0 10
3 0 8 #1 #20

8	so	1														
9	sph	.5	0	0	.9
10	sph	0	.5	0	1.1
)I";
    doerrortest(input, "***Error - line: 4, column: 11, unable to find surface or cell with identifier '20'!\n");
}

// Test of JSON material output capability
TEST(MCNPModel, output_one_material_json)
{
    std::stringstream input;
    input << R"INP(Generated SCALE input for MCNP materials conversion
c Cell cards -------------------------------------------------------------------
1    1 6.621502e-02 -1                      imp:n=1
2    0              -2   1                  imp:n=1
3    0               2                      imp:n=0

c Surf cards -------------------------------------------------------------------
1 rpp -1 1 -1 1 -1 1
2 rpp -2 2 -2 2 -2 2

c Data cards -------------------------------------------------------------------
m1    78190.01c 7.945803e-06  78192.01c 5.178015e-04
      78194.01c 2.175826e-02  78195.01c 2.236743e-02
      78196.01c 1.669281e-02  78198.01c 4.870777e-03
c ------------------------------------------------------------------------------
mode   n
kcode  1000  1.0  3  203
ksrc   0.0  0.0  0.0
c ------------------------------------------------------------------------------
)INP";
 
    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    mcnpi::Model model;
    ASSERT_TRUE(model.build(mcnpi.root(), std::cerr));

    std::stringstream actual_out;
    model.describe_materials_json(actual_out);
 
    std::stringstream exp_out;
    exp_out << R"INP({
  "mixtures" : [
  {
  "cell" : 1
  ,"density" : 0.066215
  ,"metadata" : {
    "column" : 1
    ,"data" : [
      "c Data cards -------------------------------------------------------------------"
    ]
    ,"line" : 11
  }
  ,"mixture" : 1
  ,"nuclides" : [
    {
    "adens" : 7.9458e-06
    ,"zaid" : 78190
  }
    ,{
    "adens" : 0.000517801
    ,"zaid" : 78192
  }
    ,{
    "adens" : 0.0217583
    ,"zaid" : 78194
  }
    ,{
    "adens" : 0.0223674
    ,"zaid" : 78195
  }
    ,{
    "adens" : 0.0166928
    ,"zaid" : 78196
  }
    ,{
    "adens" : 0.00487078
    ,"zaid" : 78198
  }
  ]
}
]
}
)INP";
    EXPECT_EQ(exp_out.str(), actual_out.str());
}

TEST(MCNPModel, output_two_materials_json)
{
    std::stringstream input;
    input << R"INP(NUMACO MCNP input with two materials and comments
c Cell cards -------------------------------------------------------------------
1    1 6.621502e-02   -1                      imp:n=1
2    2 1.06577220E-01 -2   1                  imp:n=1
3    0                 2                      imp:n=0

c Surf cards -------------------------------------------------------------------
1 rpp -1 1 -1 1 -1 1
2 rpp -2 2 -2 2 -2 2

c Data cards -------------------------------------------------------------------
C MCNP Composition ID 1 @ 293K generated by NUMACO
C !NUMACO {"matnum": 33, "decay_time": 0.0, "decay_units": "seconds", "name": "Bone Equivalent Plastic, B-110", "db_name": "PNNL Compendium of Material Composition Data for Radiation Transport Modeling Rev02"}
m1     1001.80c 3.785452e-02   1002.80c 4.353771e-06
       7014.80c 3.035675e-03   7015.80c 1.109023e-05
       8016.80c 3.036177e-03   8017.80c 1.156557e-06
       9019.80c 1.410569e-02  20040.80c 6.835671e-03
      20042.80c 4.562238e-05  20043.80c 9.519353e-06
      20044.80c 1.470916e-04  20046.80c 2.820549e-07
      20048.80c 1.318607e-05   6000.80c 3.287346e-02
mt1    lwtr.20t  hwtr.20t
C MCNP Composition ID 2 @ 293K generated by NUMACO
C !NUMACO {"matnum": 316, "decay_time": 0.0, "decay_units": "seconds", "name": "Skin (ICRP)", "db_name": "PNNL Compendium of Material Composition Data for Radiation Transport Modeling Rev02"}
m2     1001.80c 6.609827e-02   1002.80c 7.602175e-06
       7014.80c 2.187381e-03   7015.80c 7.991153e-06
       8016.80c 2.556674e-02   8017.80c 9.739027e-06
      11023.80c 2.017005e-06  12024.80c 1.291702e-06
      12025.80c 1.635273e-07  12026.80c 1.800436e-07
      15031.80c 7.057706e-06  16032.80c 3.119992e-05
      16033.80c 2.463411e-07  16034.80c 1.395933e-06
      16036.80c 3.284548e-09  17035.80c 3.779725e-05
      17037.80c 1.209352e-05  19039.80c 1.343047e-05
      19040.80c 1.684964e-09  19041.80c 9.692428e-07
      20040.80c 2.403457e-06  20042.80c 1.604106e-08
      20043.80c 3.347053e-09  20044.80c 5.171816e-08
      20046.80c 9.917193e-11  20048.80c 4.636288e-09
      26054.80c 6.933361e-09  26056.80c 1.088389e-07
      26057.80c 2.513566e-09  26058.80c 3.345094e-10
      30064.80c 4.981944e-08  30066.80c 2.809626e-08
      30067.80c 4.093361e-09  30068.80c 1.869369e-08
      30070.80c 6.180570e-10   6000.80c 1.258895e-02
mt2    lwtr.20t  hwtr.20t
c ------------------------------------------------------------------------------
)INP";

    DefaultMCNPInterpreter mcnpi;
    ASSERT_TRUE(mcnpi.parse(input));
    mcnpi::Model model;
    ASSERT_TRUE(model.build(mcnpi.root(), std::cerr));

    std::stringstream actual_out;
    model.describe_materials_json(actual_out);

    std::stringstream exp_out;
    exp_out << R"INP({
  "mixtures" : [
  {
  "cell" : 1
  ,"density" : 0.066215
  ,"metadata" : {
    "column" : 1
    ,"data" : [
      "c Data cards -------------------------------------------------------------------"
      ,"C MCNP Composition ID 1 @ 293K generated by NUMACO"
      ,"C !NUMACO {\"matnum\": 33, \"decay_time\": 0.0, \"decay_units\": \"seconds\", \"name\": \"Bone Equivalent Plastic, B-110\", \"db_name\": \"PNNL Compendium of Material Composition Data for Radiation Transport Modeling Rev02\"}"
    ]
    ,"line" : 11
  }
  ,"mixture" : 1
  ,"nuclides" : [
    {
    "adens" : 0.0255838
    ,"zaid" : 1001
  }
    ,{
    "adens" : 2.94248e-06
    ,"zaid" : 1002
  }
    ,{
    "adens" : 0.00205165
    ,"zaid" : 7014
  }
    ,{
    "adens" : 7.49529e-06
    ,"zaid" : 7015
  }
    ,{
    "adens" : 0.00205199
    ,"zaid" : 8016
  }
    ,{
    "adens" : 7.81655e-07
    ,"zaid" : 8017
  }
    ,{
    "adens" : 0.00953328
    ,"zaid" : 9019
  }
    ,{
    "adens" : 0.00461986
    ,"zaid" : 20040
  }
    ,{
    "adens" : 3.08337e-05
    ,"zaid" : 20042
  }
    ,{
    "adens" : 6.43362e-06
    ,"zaid" : 20043
  }
    ,{
    "adens" : 9.94113e-05
    ,"zaid" : 20044
  }
    ,{
    "adens" : 1.90626e-07
    ,"zaid" : 20046
  }
    ,{
    "adens" : 8.91176e-06
    ,"zaid" : 20048
  }
    ,{
    "adens" : 0.0222174
    ,"zaid" : 6000
  }
  ]
}
  ,{
  "cell" : 2
  ,"density" : 0.106577
  ,"metadata" : {
    "column" : 1
    ,"data" : [
      "C MCNP Composition ID 2 @ 293K generated by NUMACO"
      ,"C !NUMACO {\"matnum\": 316, \"decay_time\": 0.0, \"decay_units\": \"seconds\", \"name\": \"Skin (ICRP)\", \"db_name\": \"PNNL Compendium of Material Composition Data for Radiation Transport Modeling Rev02\"}"
    ]
    ,"line" : 22
  }
  ,"mixture" : 2
  ,"nuclides" : [
    {
    "adens" : 0.0660983
    ,"zaid" : 1001
  }
    ,{
    "adens" : 7.60217e-06
    ,"zaid" : 1002
  }
    ,{
    "adens" : 0.00218738
    ,"zaid" : 7014
  }
    ,{
    "adens" : 7.99115e-06
    ,"zaid" : 7015
  }
    ,{
    "adens" : 0.0255667
    ,"zaid" : 8016
  }
    ,{
    "adens" : 9.73903e-06
    ,"zaid" : 8017
  }
    ,{
    "adens" : 2.017e-06
    ,"zaid" : 11023
  }
    ,{
    "adens" : 1.2917e-06
    ,"zaid" : 12024
  }
    ,{
    "adens" : 1.63527e-07
    ,"zaid" : 12025
  }
    ,{
    "adens" : 1.80044e-07
    ,"zaid" : 12026
  }
    ,{
    "adens" : 7.05771e-06
    ,"zaid" : 15031
  }
    ,{
    "adens" : 3.11999e-05
    ,"zaid" : 16032
  }
    ,{
    "adens" : 2.46341e-07
    ,"zaid" : 16033
  }
    ,{
    "adens" : 1.39593e-06
    ,"zaid" : 16034
  }
    ,{
    "adens" : 3.28455e-09
    ,"zaid" : 16036
  }
    ,{
    "adens" : 3.77972e-05
    ,"zaid" : 17035
  }
    ,{
    "adens" : 1.20935e-05
    ,"zaid" : 17037
  }
    ,{
    "adens" : 1.34305e-05
    ,"zaid" : 19039
  }
    ,{
    "adens" : 1.68496e-09
    ,"zaid" : 19040
  }
    ,{
    "adens" : 9.69243e-07
    ,"zaid" : 19041
  }
    ,{
    "adens" : 2.40346e-06
    ,"zaid" : 20040
  }
    ,{
    "adens" : 1.60411e-08
    ,"zaid" : 20042
  }
    ,{
    "adens" : 3.34705e-09
    ,"zaid" : 20043
  }
    ,{
    "adens" : 5.17182e-08
    ,"zaid" : 20044
  }
    ,{
    "adens" : 9.91719e-11
    ,"zaid" : 20046
  }
    ,{
    "adens" : 4.63629e-09
    ,"zaid" : 20048
  }
    ,{
    "adens" : 6.93336e-09
    ,"zaid" : 26054
  }
    ,{
    "adens" : 1.08839e-07
    ,"zaid" : 26056
  }
    ,{
    "adens" : 2.51357e-09
    ,"zaid" : 26057
  }
    ,{
    "adens" : 3.34509e-10
    ,"zaid" : 26058
  }
    ,{
    "adens" : 4.98194e-08
    ,"zaid" : 30064
  }
    ,{
    "adens" : 2.80963e-08
    ,"zaid" : 30066
  }
    ,{
    "adens" : 4.09336e-09
    ,"zaid" : 30067
  }
    ,{
    "adens" : 1.86937e-08
    ,"zaid" : 30068
  }
    ,{
    "adens" : 6.18057e-10
    ,"zaid" : 30070
  }
    ,{
    "adens" : 0.0125889
    ,"zaid" : 6000
  }
  ]
}
]
}
)INP";

    EXPECT_EQ(exp_out.str(), actual_out.str());
}
