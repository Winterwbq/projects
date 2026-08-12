#include "wasptoml/TOMLInterpreter.h"
#include "wasptoml/TOMLNodeView.h"
#include "waspcore/utils.h"
#include "gtest/gtest.h"
#include <iostream>
#include <string>
#include <iomanip>

using namespace wasp;

TEST(TOMLInterpreter, everything)

{
    std::stringstream input;
    input << R"INPUT(
int1 = +99
int2 = 42
int3 = 0
int4 = -17

# fractional
flt1 = +1.0
flt2 =3.1415
flt3 = -0.01

# exponent
flt4 = 5e+22
flt5 = 1e06
flt6 = -2E-2

# both
flt7 = 6.626e-34

integers = [ 1, 2, 3 ]
numbers = [ 0.1, 0.2, 0.5, 1, 2, 5 ]
nested_mixed_array = [ [ 1, 2 ], [1.1, 5e+22, -2E-2]]
stuff = {a=1, b=2}
[table-1]
key1 = "some string"
key2 = 123

[table-2]
key1 = "another string"
key2 = 456

[[products]]
name = "Hammer"
sku = 738594937

[[products]]  # empty table within the array

[[products]]
name = "Nail"
sku = 284758393
points = [ { x = 1, y = 2, z = 3 },
           { x = 7, y = 8, z = 9 },
            3.14149,
           { x = 2, y = 4, z = 8 } ]

color = "gray"
x.y.z = 3.14139
[[fruits]]
name = "apple"

[[fruits.varieties]]
name = "red delicious"

[fruits.varieties.x]
name = "bob"
    )INPUT";
    std::stringstream        error;
    DefaultTOMLInterpreter interpreter(error);
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream tree_print;
    TOMLNodeView  root_view = interpreter.root();
    ASSERT_FALSE(root_view.is_null());

    root_view.paths(tree_print);
    std::stringstream expected_tree;
    expected_tree <<
        R"INPUT(/
/int1
/int1/decl (int1)
/int1/= (=)
/int1/value (+99)
/int2
/int2/decl (int2)
/int2/= (=)
/int2/value (42)
/int3
/int3/decl (int3)
/int3/= (=)
/int3/value (0)
/int4
/int4/decl (int4)
/int4/= (=)
/int4/value (-17)
/comment (# fractional)
/flt1
/flt1/decl (flt1)
/flt1/= (=)
/flt1/value (+1.0)
/flt2
/flt2/decl (flt2)
/flt2/= (=)
/flt2/value (3.1415)
/flt3
/flt3/decl (flt3)
/flt3/= (=)
/flt3/value (-0.01)
/comment (# exponent)
/flt4
/flt4/decl (flt4)
/flt4/= (=)
/flt4/value (5e+22)
/flt5
/flt5/decl (flt5)
/flt5/= (=)
/flt5/value (1e06)
/flt6
/flt6/decl (flt6)
/flt6/= (=)
/flt6/value (-2E-2)
/comment (# both)
/flt7
/flt7/decl (flt7)
/flt7/= (=)
/flt7/value (6.626e-34)
/integers
/integers/decl (integers)
/integers/= (=)
/integers/[ ([)
/integers/value (1)
/integers/, (,)
/integers/value (2)
/integers/, (,)
/integers/value (3)
/integers/] (])
/numbers
/numbers/decl (numbers)
/numbers/= (=)
/numbers/[ ([)
/numbers/value (0.1)
/numbers/, (,)
/numbers/value (0.2)
/numbers/, (,)
/numbers/value (0.5)
/numbers/, (,)
/numbers/value (1)
/numbers/, (,)
/numbers/value (2)
/numbers/, (,)
/numbers/value (5)
/numbers/] (])
/nested_mixed_array
/nested_mixed_array/decl (nested_mixed_array)
/nested_mixed_array/= (=)
/nested_mixed_array/[ ([)
/nested_mixed_array/value
/nested_mixed_array/value/[ ([)
/nested_mixed_array/value/value (1)
/nested_mixed_array/value/, (,)
/nested_mixed_array/value/value (2)
/nested_mixed_array/value/] (])
/nested_mixed_array/, (,)
/nested_mixed_array/value
/nested_mixed_array/value/[ ([)
/nested_mixed_array/value/value (1.1)
/nested_mixed_array/value/, (,)
/nested_mixed_array/value/value (5e+22)
/nested_mixed_array/value/, (,)
/nested_mixed_array/value/value (-2E-2)
/nested_mixed_array/value/] (])
/nested_mixed_array/] (])
/stuff
/stuff/decl (stuff)
/stuff/= (=)
/stuff/{ ({)
/stuff/a
/stuff/a/decl (a)
/stuff/a/= (=)
/stuff/a/value (1)
/stuff/, (,)
/stuff/b
/stuff/b/decl (b)
/stuff/b/= (=)
/stuff/b/value (2)
/stuff/} (})
/table-1
/table-1/[ ([)
/table-1/decl (table-1)
/table-1/] (])
/table-1/key1
/table-1/key1/decl (key1)
/table-1/key1/= (=)
/table-1/key1/value ("some string")
/table-1/key2
/table-1/key2/decl (key2)
/table-1/key2/= (=)
/table-1/key2/value (123)
/table-2
/table-2/[ ([)
/table-2/decl (table-2)
/table-2/] (])
/table-2/key1
/table-2/key1/decl (key1)
/table-2/key1/= (=)
/table-2/key1/value ("another string")
/table-2/key2
/table-2/key2/decl (key2)
/table-2/key2/= (=)
/table-2/key2/value (456)
/products
/products/[[ ([[)
/products/decl (products)
/products/]] (]])
/products/name
/products/name/decl (name)
/products/name/= (=)
/products/name/value ("Hammer")
/products/sku
/products/sku/decl (sku)
/products/sku/= (=)
/products/sku/value (738594937)
/products
/products/[[ ([[)
/products/decl (products)
/products/]] (]])
/products/comment (# empty table within the array)
/products
/products/[[ ([[)
/products/decl (products)
/products/]] (]])
/products/name
/products/name/decl (name)
/products/name/= (=)
/products/name/value ("Nail")
/products/sku
/products/sku/decl (sku)
/products/sku/= (=)
/products/sku/value (284758393)
/products/points
/products/points/decl (points)
/products/points/= (=)
/products/points/[ ([)
/products/points/value
/products/points/value/{ ({)
/products/points/value/x
/products/points/value/x/decl (x)
/products/points/value/x/= (=)
/products/points/value/x/value (1)
/products/points/value/, (,)
/products/points/value/y
/products/points/value/y/decl (y)
/products/points/value/y/= (=)
/products/points/value/y/value (2)
/products/points/value/, (,)
/products/points/value/z
/products/points/value/z/decl (z)
/products/points/value/z/= (=)
/products/points/value/z/value (3)
/products/points/value/} (})
/products/points/, (,)
/products/points/value
/products/points/value/{ ({)
/products/points/value/x
/products/points/value/x/decl (x)
/products/points/value/x/= (=)
/products/points/value/x/value (7)
/products/points/value/, (,)
/products/points/value/y
/products/points/value/y/decl (y)
/products/points/value/y/= (=)
/products/points/value/y/value (8)
/products/points/value/, (,)
/products/points/value/z
/products/points/value/z/decl (z)
/products/points/value/z/= (=)
/products/points/value/z/value (9)
/products/points/value/} (})
/products/points/, (,)
/products/points/value (3.14149)
/products/points/, (,)
/products/points/value
/products/points/value/{ ({)
/products/points/value/x
/products/points/value/x/decl (x)
/products/points/value/x/= (=)
/products/points/value/x/value (2)
/products/points/value/, (,)
/products/points/value/y
/products/points/value/y/decl (y)
/products/points/value/y/= (=)
/products/points/value/y/value (4)
/products/points/value/, (,)
/products/points/value/z
/products/points/value/z/decl (z)
/products/points/value/z/= (=)
/products/points/value/z/value (8)
/products/points/value/} (})
/products/points/] (])
/products/color
/products/color/decl (color)
/products/color/= (=)
/products/color/value ("gray")
/products/x
/products/x/y
/products/x/y/z
/products/x/y/z/decl (x)
/products/x/y/z/. (.)
/products/x/y/z/decl (y)
/products/x/y/z/. (.)
/products/x/y/z/decl (z)
/products/x/y/z/= (=)
/products/x/y/z/value (3.14139)
/fruits
/fruits/[[ ([[)
/fruits/decl (fruits)
/fruits/]] (]])
/fruits/name
/fruits/name/decl (name)
/fruits/name/= (=)
/fruits/name/value ("apple")
/fruits
/fruits/varieties
/fruits/varieties/[[ ([[)
/fruits/varieties/decl (fruits)
/fruits/varieties/. (.)
/fruits/varieties/decl (varieties)
/fruits/varieties/]] (]])
/fruits/varieties/name
/fruits/varieties/name/decl (name)
/fruits/varieties/name/= (=)
/fruits/varieties/name/value ("red delicious")
/fruits
/fruits/varieties
/fruits/varieties/x
/fruits/varieties/x/[ ([)
/fruits/varieties/x/decl (fruits)
/fruits/varieties/x/. (.)
/fruits/varieties/x/decl (varieties)
/fruits/varieties/x/. (.)
/fruits/varieties/x/decl (x)
/fruits/varieties/x/] (])
/fruits/varieties/x/name
/fruits/varieties/x/name/decl (name)
/fruits/varieties/x/name/= (=)
/fruits/varieties/x/name/value ("bob")
)INPUT";
    ASSERT_EQ(expected_tree.str(), tree_print.str());

    ASSERT_TRUE(interpreter.generate_object());
    std::stringstream actual_json;
    interpreter.object()->format_json(actual_json);
    std::stringstream expected_json;
    expected_json <<
        R"INPUT({
  "flt1" : 1
  ,"flt2" : 3.1415
  ,"flt3" : -0.01
  ,"flt4" : 5e+22
  ,"flt5" : 1e+06
  ,"flt6" : -0.02
  ,"flt7" : 6.626e-34
  ,"fruits" : [
    {
    "name" : "apple"
    ,"varieties" : [
      {
      "name" : "red delicious"
      ,"x" : {
        "name" : "bob"
      }
    }
    ]
  }
  ]
  ,"int1" : 99
  ,"int2" : 42
  ,"int3" : 0
  ,"int4" : -17
  ,"integers" : [
    1
    ,2
    ,3
  ]
  ,"nested_mixed_array" : [
    [
    1
    ,2
  ]
    ,[
    1.1
    ,5e+22
    ,-0.02
  ]
  ]
  ,"numbers" : [
    0.1
    ,0.2
    ,0.5
    ,1
    ,2
    ,5
  ]
  ,"products" : [
    {
    "name" : "Hammer"
    ,"sku" : 738594937
  }
    ,{}
    ,{
    "color" : "gray"
    ,"name" : "Nail"
    ,"points" : [
      {
      "x" : 1
      ,"y" : 2
      ,"z" : 3
    }
      ,{
      "x" : 7
      ,"y" : 8
      ,"z" : 9
    }
      ,3.14149
      ,{
      "x" : 2
      ,"y" : 4
      ,"z" : 8
    }
    ]
    ,"sku" : 284758393
    ,"x" : {
      "y" : {
      "z" : 3.14139
    }
    }
  }
  ]
  ,"stuff" : {
    "a" : 1
    ,"b" : 2
  }
  ,"table-1" : {
    "key1" : "some string"
    ,"key2" : 123
  }
  ,"table-2" : {
    "key1" : "another string"
    ,"key2" : 456
  }
})INPUT";
    ASSERT_EQ(expected_json.str(), actual_json.str());
}

TEST(TOMLInterpreter, duplicate_key_error)

{
    std::stringstream input;
    input << R"INPUT(
key = +99
 key = "oops"
 key.a=10
 [[key]]
 [key]
)INPUT";
    std::stringstream        error;
    DefaultTOMLInterpreter interpreter(error);
    ASSERT_TRUE(interpreter.parse(input));
    TOMLNodeView  root_view = interpreter.root();
    ASSERT_FALSE(root_view.is_null());
    ASSERT_FALSE(interpreter.generate_object());
    
    ASSERT_EQ(4, interpreter.error_diagnostics().size());
    std::stringstream expected_erors;
    expected_erors <<"stream input:3.2: key is already defined!\n"
                   <<"stream input:4.2: key is already defined!\n"
                   <<"stream input:5.2: key is already defined but not as an array!\n"
                   <<"stream input:6.2: key is already defined but not as a table!\n";
    ASSERT_DIAGNOSTICS(interpreter, expected_erors);
    
}

TEST(TOMLInterpreter, redefine_table_as_array)

{
    std::stringstream input;
    input << R"INPUT(
[table.x.y.z]
[[table.x.y]]
)INPUT";
    std::stringstream        error;
    DefaultTOMLInterpreter interpreter(error);
    ASSERT_TRUE(interpreter.parse(input));
    TOMLNodeView  root_view = interpreter.root();
    ASSERT_FALSE(root_view.is_null());
    ASSERT_FALSE(interpreter.generate_object());
    
    ASSERT_EQ(1, interpreter.error_diagnostics().size());
    std::stringstream expected_erors;
    expected_erors <<"stream input:3.1: y is already defined but not as an array!\n";
    ASSERT_DIAGNOSTICS(interpreter, expected_erors);
    
}

TEST(TOMLInterpreter, redefine_array_as_table)

{
    std::stringstream input;
    input << R"INPUT(
[[table.x.y]]
[table.x.y]
)INPUT";
    std::stringstream        error;
    DefaultTOMLInterpreter interpreter(error);
    ASSERT_TRUE(interpreter.parse(input));
    TOMLNodeView  root_view = interpreter.root();
    ASSERT_FALSE(root_view.is_null());
    ASSERT_FALSE(interpreter.generate_object());
    
    ASSERT_EQ(1, interpreter.error_diagnostics().size());
    std::stringstream expected_erors;
    expected_erors <<"stream input:3.1: y is already defined but not as a table!\n";
    ASSERT_DIAGNOSTICS(interpreter, expected_erors);
    
}

TEST(TOMLInterpreter, value_conversion_error)

{
    std::stringstream input;
    input << R"INPUT(
   table.a.b.c=12345678901234567890
)INPUT";
    std::stringstream        error;
    DefaultTOMLInterpreter interpreter(error);
    ASSERT_TRUE(interpreter.parse(input));
    TOMLNodeView  root_view = interpreter.root();
    ASSERT_FALSE(root_view.is_null());
    ASSERT_FALSE(interpreter.generate_object());
    
    ASSERT_EQ(1, interpreter.error_diagnostics().size());
    std::stringstream expected_erors;
    expected_erors <<"stream input:2.16: value (12345678901234567890) failed to convert!\n";
    ASSERT_DIAGNOSTICS(interpreter, expected_erors);
    
}

/*
* Test mixed TOML constructs
*/
TEST(TOMLInterpreter, toml_mixed)

{
    std::stringstream input;
    input << R"INPUT(# This is a TOML document.

title = "ImpalaPay Co."

[owner]
name = "Impala Co."
establishment=""


[database]
server = "192.168.1.1"
ports = [ 8000, 8001, 8002 ]
connection_max = 5000
enabled = true

[servers]

	# Indentation (tabs and/or spaces) is allowed but not required
	[servers.alpha]
	ip = "10.0.0.1"
	dc = "eqdc10"

	[servers.beta]
	ip = "10.0.0.2"
	dc = "eqdc10"

[clients]
data = [ ["gamma", "delta"], [1, 2], [true, false] ]

# Line breaks are OK when inside arrays
hosts = [
	"alpha",
	"omega"
])INPUT";
    std::stringstream        error;
    DefaultTOMLInterpreter interpreter(error);
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream tree_print;
    TOMLNodeView  root_view = interpreter.root();
    ASSERT_FALSE(root_view.is_null());
    root_view.paths(tree_print);
    std::stringstream expected_tree;
    expected_tree <<R"INPUT(/
/comment (# This is a TOML document.)
/title
/title/decl (title)
/title/= (=)
/title/value ("ImpalaPay Co.")
/owner
/owner/[ ([)
/owner/decl (owner)
/owner/] (])
/owner/name
/owner/name/decl (name)
/owner/name/= (=)
/owner/name/value ("Impala Co.")
/owner/establishment
/owner/establishment/decl (establishment)
/owner/establishment/= (=)
/owner/establishment/value ("")
/database
/database/[ ([)
/database/decl (database)
/database/] (])
/database/server
/database/server/decl (server)
/database/server/= (=)
/database/server/value ("192.168.1.1")
/database/ports
/database/ports/decl (ports)
/database/ports/= (=)
/database/ports/[ ([)
/database/ports/value (8000)
/database/ports/, (,)
/database/ports/value (8001)
/database/ports/, (,)
/database/ports/value (8002)
/database/ports/] (])
/database/connection_max
/database/connection_max/decl (connection_max)
/database/connection_max/= (=)
/database/connection_max/value (5000)
/database/enabled
/database/enabled/decl (enabled)
/database/enabled/= (=)
/database/enabled/value (true)
/servers
/servers/[ ([)
/servers/decl (servers)
/servers/] (])
/servers/comment (# Indentation (tabs and/or spaces) is allowed but not required)
/servers
/servers/alpha
/servers/alpha/[ ([)
/servers/alpha/decl (servers)
/servers/alpha/. (.)
/servers/alpha/decl (alpha)
/servers/alpha/] (])
/servers/alpha/ip
/servers/alpha/ip/decl (ip)
/servers/alpha/ip/= (=)
/servers/alpha/ip/value ("10.0.0.1")
/servers/alpha/dc
/servers/alpha/dc/decl (dc)
/servers/alpha/dc/= (=)
/servers/alpha/dc/value ("eqdc10")
/servers
/servers/beta
/servers/beta/[ ([)
/servers/beta/decl (servers)
/servers/beta/. (.)
/servers/beta/decl (beta)
/servers/beta/] (])
/servers/beta/ip
/servers/beta/ip/decl (ip)
/servers/beta/ip/= (=)
/servers/beta/ip/value ("10.0.0.2")
/servers/beta/dc
/servers/beta/dc/decl (dc)
/servers/beta/dc/= (=)
/servers/beta/dc/value ("eqdc10")
/clients
/clients/[ ([)
/clients/decl (clients)
/clients/] (])
/clients/data
/clients/data/decl (data)
/clients/data/= (=)
/clients/data/[ ([)
/clients/data/value
/clients/data/value/[ ([)
/clients/data/value/value ("gamma")
/clients/data/value/, (,)
/clients/data/value/value ("delta")
/clients/data/value/] (])
/clients/data/, (,)
/clients/data/value
/clients/data/value/[ ([)
/clients/data/value/value (1)
/clients/data/value/, (,)
/clients/data/value/value (2)
/clients/data/value/] (])
/clients/data/, (,)
/clients/data/value
/clients/data/value/[ ([)
/clients/data/value/value (true)
/clients/data/value/, (,)
/clients/data/value/value (false)
/clients/data/value/] (])
/clients/data/] (])
/clients/comment (# Line breaks are OK when inside arrays)
/clients/hosts
/clients/hosts/decl (hosts)
/clients/hosts/= (=)
/clients/hosts/[ ([)
/clients/hosts/value ("alpha")
/clients/hosts/, (,)
/clients/hosts/value ("omega")
/clients/hosts/] (])
)INPUT";
    ASSERT_EQ(expected_tree.str(), tree_print.str());

    ASSERT_TRUE(interpreter.generate_object());
    std::stringstream actual_json;
    interpreter.object()->format_json(actual_json);
    std::stringstream expected_json;
    expected_json <<R"INPUT({
  "clients" : {
  "data" : [
  [
  "gamma"
  ,"delta"
]
  ,[
  1
  ,2
]
  ,[
  true
  ,false
]
]
  ,"hosts" : [
    "alpha"
    ,"omega"
  ]
}
  ,"database" : {
    "connection_max" : 5000
    ,"enabled" : true
    ,"ports" : [
      8000
      ,8001
      ,8002
    ]
    ,"server" : "192.168.1.1"
  }
  ,"owner" : {
    "establishment" : ""
    ,"name" : "Impala Co."
  }
  ,"servers" : {
    "alpha" : {
    "dc" : "eqdc10"
    ,"ip" : "10.0.0.1"
  }
    ,"beta" : {
      "dc" : "eqdc10"
      ,"ip" : "10.0.0.2"
    }
  }
  ,"title" : "ImpalaPay Co."
})INPUT";
    ASSERT_EQ(expected_json.str(), actual_json.str());
}


TEST(TOMLInterpreter, inline_table)

{
    std::stringstream input;
    input << R"INPUT(
x={a=3, b=4, c=5}
)INPUT";
    std::stringstream        error;
    DefaultTOMLInterpreter interpreter(error);
    ASSERT_TRUE(interpreter.parse(input));
    TOMLNodeView  root_view = interpreter.root();
    ASSERT_FALSE(root_view.is_null());
    ASSERT_TRUE(interpreter.generate_object());
    std::stringstream actual_json;
    interpreter.object()->pack_json(actual_json);
    ASSERT_EQ("{\"x\":{\"a\":3,\"b\":4,\"c\":5}}", actual_json.str());
    
    EXPECT_EQ(1, root_view.child_count_by_name("x"));
    EXPECT_TRUE(TOMLNodeView(interpreter.root()) == root_view);
    EXPECT_FALSE(root_view.is_leaf());
    EXPECT_FALSE(root_view.is_declarator());
    EXPECT_FALSE(root_view.is_terminator());
    
    auto x = root_view.first_child_by_name("x");
    EXPECT_TRUE(x < root_view);
    EXPECT_FALSE(root_view < x);
    // (4) x, a, b, c
    const auto& non_decorative_children = x.non_decorative_children();
    EXPECT_EQ(4, non_decorative_children.size());   
    EXPECT_EQ(4, x.non_decorative_children_count());
    
    EXPECT_EQ(1, x.child_count_by_name("a"));
    EXPECT_EQ(2, x.line());
    EXPECT_EQ(2, x.last_line());
    EXPECT_EQ(1, x.column());
    EXPECT_EQ(17, x.last_column());
    EXPECT_TRUE(x.child_at(x.child_count()-1).is_terminator());
}