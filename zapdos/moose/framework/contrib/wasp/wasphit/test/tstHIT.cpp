#include "wasphit/HITInterpreter.h"
#include "wasphit/HITNodeView.h"
#include "waspcore/utils.h"
#include "gtest/gtest.h"
#include <iostream>
#include <string>
using namespace wasp;


#include "wasphit/test/Paths.h"

std::string test_dir = TEST_DIR_ROOT;

TEST(HITInterpreter, int_real)

{
    std::stringstream input;
    input << R"INPUT(
    a=3
    b=3.1
    c=1e2
    d=3.e1
    e=3.2e3
    f=-3
    g=-3.E1
    h=-3.2E2
    i=1e23.3
    )INPUT";
    std::stringstream        error;
    DefaultHITInterpreter interpreter(error);
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream tree_print;
    HITNodeView  root_view = interpreter.root();
    ASSERT_FALSE(root_view.is_null());
    
    std::vector<double> expected = {3, 3.1, 1e2, 3.e1, 3.2e3, -3, -3.e1, -3.2E2, 1e23};
    std::vector<wasp::NODE> expected_type = {wasp::INTEGER, wasp::REAL, wasp::REAL, 
                                            wasp::REAL, wasp::REAL, wasp::INTEGER, 
                                            wasp::REAL, wasp::REAL, wasp::STRING};
    ASSERT_EQ(expected.size(), root_view.child_count());
    ASSERT_EQ(expected_type.size(), root_view.child_count());

    for(size_t i = 0; i < expected.size(); ++i)
    {
        ASSERT_EQ(3, root_view.child_at(i).child_count());
        auto value_node = root_view.child_at(i).child_at(2);
        SCOPED_TRACE(value_node.data());
        EXPECT_EQ(expected[i], value_node.to_double());
        EXPECT_EQ(expected_type[i], value_node.token_type()); 
    }
}
TEST(HITInterpreter, int_real_as_keys)

{
    std::stringstream input;
    input << R"INPUT(
    3=a
    3.1=b
    1e2=c
    3.e1=d
    3.2e3=e
    -3=f
    -3.E1=g
    -3.2E2=h
    1e23.3=i
    )INPUT";
    std::stringstream        error;
    DefaultHITInterpreter interpreter(error);
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream tree_print;
    HITNodeView  root_view = interpreter.root();
    ASSERT_FALSE(root_view.is_null());
    
    std::vector<double> expected = {3, 3.1, 1e2, 3.e1, 3.2e3, -3, -3.e1, -3.2E2, 1e23};
    ASSERT_EQ(expected.size(), root_view.child_count());

    for(size_t i = 0; i < expected.size(); ++i)
    {
        EXPECT_EQ(wasp::KEYED_VALUE, root_view.child_at(i).type());
        ASSERT_EQ(3, root_view.child_at(i).child_count());
        auto key_node = root_view.child_at(i).child_at(0);
        EXPECT_EQ(wasp::DECL, key_node.type());
        SCOPED_TRACE(key_node.data());
        EXPECT_EQ(expected[i], key_node.to_double());
        // HIT keys are always recognized as strings
        EXPECT_EQ(wasp::STRING, key_node.token_type()); 
    }
}

TEST(HITInterpreter, bad)

{
    std::stringstream input;
    input << R"INPUT([GlobalParams]
disp_x = disp_x
disp_y = disp_y
disp_z = disp_z
[../])INPUT";
    std::stringstream        error;
    DefaultHITInterpreter interpreter(error);
    ASSERT_TRUE(interpreter.parse(input));
    std::stringstream tree_print;
    auto              root_view = interpreter.root();
    ASSERT_FALSE(root_view.is_null());
    root_view.paths(tree_print);
    std::stringstream expected_tree;
    expected_tree <<
        R"INPUT(/
/GlobalParams
/GlobalParams/[ ([)
/GlobalParams/decl (GlobalParams)
/GlobalParams/] (])
/GlobalParams/disp_x
/GlobalParams/disp_x/decl (disp_x)
/GlobalParams/disp_x/= (=)
/GlobalParams/disp_x/value (disp_x)
/GlobalParams/disp_y
/GlobalParams/disp_y/decl (disp_y)
/GlobalParams/disp_y/= (=)
/GlobalParams/disp_y/value (disp_y)
/GlobalParams/disp_z
/GlobalParams/disp_z/decl (disp_z)
/GlobalParams/disp_z/= (=)
/GlobalParams/disp_z/value (disp_z)
/GlobalParams/term ([../])
)INPUT";
    ASSERT_EQ(expected_tree.str(), tree_print.str());
}

TEST(HITInterpreter, simple)
{
    std::stringstream input;
    // Simple parse
    // document
    //  |_key
    //    |_ decl 'key'
    //    |_ = '='
    //    |_ value '3.421'
    input << "key =  3.421";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());

    std::vector<std::string> token_data      = {"key", "=", "3.421"};
    std::vector<std::string> leaf_node_names = {"decl", "=", "value"};
    std::vector<wasp::NODE>  leaf_node_types = {wasp::DECL, wasp::ASSIGN,
                                               wasp::VALUE};
    for (std::size_t i = 0; i < 3; ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(token_data[i], interpreter.data(i));
            ASSERT_EQ(leaf_node_types[i], interpreter.type(i));
            ASSERT_EQ(leaf_node_names[i], interpreter.name(i));
        }
    }
    std::vector<std::size_t> parent_indices = {3, 3, 3, 4, 5};
    std::vector<std::size_t> child_count    = {0, 0, 0, 3, 1};
    std::vector<std::size_t> column         = {1, 5, 8, 1, 1};
    std::vector<wasp::NODE> node_types = {wasp::DECL, wasp::ASSIGN, wasp::VALUE,
                                          wasp::KEYED_VALUE,
                                          wasp::DOCUMENT_ROOT};
    std::vector<std::string> node_names = {"decl", "=", "value", "key",
                                           "document"};
    ASSERT_EQ(node_types.size(), node_names.size());
    ASSERT_EQ(node_types.size(), parent_indices.size());
    ASSERT_EQ(node_types.size(), child_count.size());
    ASSERT_EQ(node_types.size(), column.size());
    for (std::size_t i = 0; i < interpreter.node_count(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(node_types[i], interpreter.type(i));
            ASSERT_EQ(node_names[i], interpreter.name(i));
            ASSERT_EQ(parent_indices[i], interpreter.parent_node_index(i));
            ASSERT_EQ(child_count[i], interpreter.child_count(i));
            ASSERT_EQ(child_count[i], interpreter.child_count(i));
            ASSERT_EQ(column[i], interpreter.column(i));
            ASSERT_EQ(1, interpreter.line(i));
        }
    }
}

TEST(HITInterpreter, empty_object)
{
    std::stringstream input;
    // Simple parse
    // document
    //  |_object
    //    |_ decl 'ted'
    //      |_ [ '['
    //      |_ string 'ted'
    //      |_ ] ']'
    //    |_ object_term '[]'

    input << "[ted][]";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(6, interpreter.node_count());

    std::vector<std::string> token_data      = {"[", "ted", "]"};
    std::vector<std::string> leaf_node_names = {"[", "decl", "]"};
    std::vector<wasp::NODE>  leaf_node_types = {wasp::LBRACKET, wasp::DECL,
                                               wasp::RBRACKET};
    for (std::size_t i = 0; i < 3; ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(token_data[i], interpreter.data(i));
            ASSERT_EQ(leaf_node_types[i], interpreter.type(i));
            ASSERT_EQ(leaf_node_names[i], interpreter.name(i));
        }
    }
    std::vector<wasp::NODE> node_types = {wasp::LBRACKET, wasp::DECL,
                                          wasp::RBRACKET, wasp::OBJECT_TERM,
                                          wasp::OBJECT,   wasp::DOCUMENT_ROOT};
    std::vector<std::string> node_names = {"[",  "decl", "]",
                                           "term", "ted",  "document"};
    ASSERT_EQ(node_types.size(), node_names.size());
    for (std::size_t i = 0; i < interpreter.node_count(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(node_types[i], interpreter.type(i));
            ASSERT_EQ(node_names[i], interpreter.name(i));
        }
    }
}

TEST(HITInterpreter, simple_object)
{
    std::stringstream input;
    // Simple parse
    // document
    //  |_object
    //    |_ object_decl 'ted'
    //      |_ [ '['
    //      |_ string 'ted'
    //      |_ ] ']'
    //    |_key
    //      |_ decl 'key'
    //      |_ = '='
    //      |_ value '3.421'
    //    |_ object_term '[]'

    input << "[ted]key = 3.421[]";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(10, interpreter.node_count());

    // object decl
    std::vector<wasp::NODE> node_types = {wasp::LBRACKET,
                                          wasp::DECL,
                                          wasp::RBRACKET
                                          // keyed value
                                          ,
                                          wasp::DECL,
                                          wasp::ASSIGN,
                                          wasp::VALUE,
                                          wasp::KEYED_VALUE
                                          // object term
                                          ,
                                          wasp::OBJECT_TERM
                                          // object
                                          ,
                                          wasp::OBJECT
                                          // document root
                                          ,
                                          wasp::DOCUMENT_ROOT};
    std::vector<std::string> node_names = {
        "[", "decl", "]", "decl", "=", "value", "key", "term", "ted", "document"};
    ASSERT_EQ(node_types.size(), node_names.size());
    for (std::size_t i = 0; i < interpreter.node_count(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(node_types[i], interpreter.type(i));
            ASSERT_EQ(node_names[i], interpreter.name(i));
        }
    }
}

TEST(HITInterpreter, less_simple_object)
{
    std::stringstream input;
    // Simple parse
    // document
    //  |_object
    //    |_ object_decl 'ted'
    //      |_ [ '['
    //      |_ string 'ted'
    //      |_ ] ']'
    //    |_boo
    //      |_ decl 'boo'
    //      |_ = '='
    //      |_ value 'foo'
    //    |_comment
    //    |_fred
    //      |_ decl 'fred'
    //      |_ = '='
    //      |_ value '1'
    //    |_key
    //      |_ decl 'key'
    //      |_ = '='
    //      |_ value '3.421'
    //    |_ object_term '[]'

    input << R"INPUT([ted]
     boo = foo # halloween
     fred = 1
     key = 3.421
 [])INPUT";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(19, interpreter.node_count());

    // object decl
    std::vector<wasp::NODE> node_types = {wasp::LBRACKET,
                                          wasp::DECL,
                                          wasp::RBRACKET
                                          // keyed value boo
                                          ,
                                          wasp::DECL,
                                          wasp::ASSIGN,
                                          wasp::VALUE,
                                          wasp::KEYED_VALUE
                                          // comment
                                          ,
                                          wasp::COMMENT
                                          // keyed value fred
                                          ,
                                          wasp::DECL,
                                          wasp::ASSIGN,
                                          wasp::VALUE,
                                          wasp::KEYED_VALUE
                                          // keyed value key
                                          ,
                                          wasp::DECL,
                                          wasp::ASSIGN,
                                          wasp::VALUE,
                                          wasp::KEYED_VALUE
                                          // object term
                                          ,
                                          wasp::OBJECT_TERM
                                          // object
                                          ,
                                          wasp::OBJECT
                                          // document root
                                          ,
                                          wasp::DOCUMENT_ROOT};
    std::vector<std::string> node_names = {"[",
                                           "decl",
                                           "]",
                                           "decl",
                                           "=",
                                           "value"  // boo
                                           ,
                                           "boo",
                                           "comment"  // halloween
                                           ,
                                           "decl",
                                           "=",
                                           "value"  // fred
                                           ,
                                           "fred",
                                           "decl",
                                           "=",
                                           "value"  // key
                                           ,
                                           "key",
                                           "term",
                                           "ted",
                                           "document"};
    ASSERT_EQ(node_types.size(), node_names.size());
    NodeView fred;
    for (std::size_t i = 0; i < interpreter.node_count(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(node_types[i], interpreter.type(i));
            ASSERT_EQ(node_names[i], interpreter.name(i));
            if (std::strcmp("fred", interpreter.name(i)) == 0) fred = interpreter.node_at(i);
        }
    }
    NodeView fred_value = fred.first_child_by_name("value");
    ASSERT_EQ("1", fred_value.data());
    fred_value.set_data("1\n2\n3\n1\n2\n3\n1\n2\n3\n1\n2\n3");
    ASSERT_EQ("fred = 1\n2\n3\n1\n2\n3\n1\n2\n3\n1\n2\n3", fred.data());
    // Notice that the additional byte offset introduced by the longer token data
    // absorbes the whitespace prefix on the following line 'key'
    std::string expected_value = R"INPUT([ted]
     boo = foo # halloween
     fred = 1
2
3
1
2
3
1
2
3
1
2
3 key = 3.421
 [])INPUT";
    ASSERT_EQ(expected_value.data(), interpreter.root().data());
}

TEST(HITInterpreter, object_array)
{
    std::stringstream input;
    // Simple parse
    // document
    //  |_object
    //    |_ object_decl 'ted'
    //      |_ [ '['
    //      |_ string 'ted'
    //      |_ ] ']'
    //    |_array 'data'
    //      |_ decl 'data'
    //      |_ = '='
    //      |_ ' '''
    //      |_ value 'basic'
    //      |_ value '201'
    //      |_ value 'lu'
    //      |_ ' '''
    //    |_ object_term '[]'

    input << "[ted]data='basic 201 lu'[]";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(14, interpreter.node_count());

    std::vector<wasp::NODE> node_types = {wasp::LBRACKET,
                                          wasp::DECL,
                                          wasp::RBRACKET
                                          // keyed value array data
                                          ,
                                          wasp::DECL,
                                          wasp::ASSIGN,
                                          wasp::QUOTE,
                                          wasp::VALUE,
                                          wasp::VALUE,
                                          wasp::VALUE,
                                          wasp::QUOTE,
                                          wasp::ARRAY,
                                          wasp::OBJECT_TERM,
                                          wasp::OBJECT,
                                          wasp::DOCUMENT_ROOT};
    std::vector<std::string> node_names = {"[",
                                           "decl",
                                           "]",
                                           "decl",
                                           "="  // data array
                                           ,
                                           "'"  // start
                                           ,
                                           "value"  // basic
                                           ,
                                           "value"  // 201
                                           ,
                                           "value"  // lu
                                           ,
                                           "'"  // terminator
                                           ,
                                           "data",
                                           "term",
                                           "ted",
                                           "document"};
    ASSERT_EQ(node_types.size(), node_names.size());
    for (std::size_t i = 0; i < interpreter.node_count(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(node_types[i], interpreter.type(i));
            ASSERT_EQ(node_names[i], interpreter.name(i));
        }
    }
}

TEST(HITInterpreter, double_quoted_value)
{
    std::stringstream input;
    // Simple parse
    // document
    //  |_object
    //    |_ object_decl 'ted'
    //      |_ [ '['
    //      |_ string 'ted'
    //      |_ ] ']'
    //    |_key
    //      |_ decl 'key'
    //      |_ = '='
    //      |_ value '"quotedvalue"'
    //    |_ object_term '[]'

    input << "[ted]key = \"quotedvalue\"[]";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(10, interpreter.node_count());

    // object decl
    std::vector<wasp::NODE> node_types = {wasp::LBRACKET,
                                          wasp::DECL,
                                          wasp::RBRACKET
                                          // keyed value
                                          ,
                                          wasp::DECL,
                                          wasp::ASSIGN,
                                          wasp::VALUE,
                                          wasp::KEYED_VALUE
                                          // object term
                                          ,
                                          wasp::OBJECT_TERM
                                          // object
                                          ,
                                          wasp::OBJECT
                                          // document root
                                          ,
                                          wasp::DOCUMENT_ROOT};
    std::vector<std::string> node_names = {
        "[", "decl", "]", "decl", "=", "value", "key", "term", "ted", "document"};
    ASSERT_EQ(node_types.size(), node_names.size());
    for (std::size_t i = 0; i < interpreter.node_count(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(node_types[i], interpreter.type(i));
            ASSERT_EQ(node_names[i], interpreter.name(i));
        }
    }
}

TEST(HITInterpreter, object_array_semicolons)
{
    std::stringstream input;
    // Simple parse
    // document
    //  |_object
    //    |_ object_decl 'ted'
    //      |_ [ '['
    //      |_ string 'ted'
    //      |_ ] ']'
    //    |_array 'data'
    //      |_ decl 'data'
    //      |_ = '='
    //      |_ ' '''
    //      |_ value basic
    //      |_ , semicolon
    //      |_ value 201
    //      |_ , semicolon
    //      |_ value again
    //      |_ , semicolon
    //      |_ value here
    //      |_ , semicolon
    //      |_ value close
    //      |_ , semicolon
    //      |_ value far
    //      |_ value lu
    //      |_ ' '''
    //    |_ object_term '[]'

    input << "[ted]data='basic;201 ; again;here;close ; far lu'[]";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(23, interpreter.node_count());

    std::vector<wasp::NODE> node_types = {wasp::LBRACKET,
                                          wasp::DECL,
                                          wasp::RBRACKET
                                          // keyed value array data
                                          ,
                                          wasp::DECL,
                                          wasp::ASSIGN,
                                          wasp::QUOTE,
                                          wasp::VALUE,
                                          wasp::SEMICOLON,
                                          wasp::VALUE,
                                          wasp::SEMICOLON,
                                          wasp::VALUE,
                                          wasp::SEMICOLON,
                                          wasp::VALUE,
                                          wasp::SEMICOLON,
                                          wasp::VALUE,
                                          wasp::SEMICOLON,
                                          wasp::VALUE,
                                          wasp::VALUE,
                                          wasp::QUOTE,
                                          wasp::ARRAY,
                                          wasp::OBJECT_TERM,
                                          wasp::OBJECT,
                                          wasp::DOCUMENT_ROOT};
    std::vector<std::string> node_names = {"[",
                                           "decl",
                                           "]",
                                           "decl",
                                           "="  // data array
                                           ,
                                           "'"  // start
                                           ,
                                           "value"  // basic
                                           ,
                                           ";"  // semicolon
                                           ,
                                           "value"  // 201
                                           ,
                                           ";"  // semicolon
                                           ,
                                           "value"  // again
                                           ,
                                           ";"  // semicolon
                                           ,
                                           "value"  // here
                                           ,
                                           ";"  // semicolon
                                           ,
                                           "value"  // close
                                           ,
                                           ";"  // semicolon
                                           ,
                                           "value"  // far
                                           ,
                                           "value"  // lu
                                           ,
                                           "'"  // terminator
                                           ,
                                           "data",
                                           "term",
                                           "ted",
                                           "document"};
    ASSERT_EQ(node_types.size(), node_names.size());
    for (std::size_t i = 0; i < interpreter.node_count(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(node_types[i], interpreter.type(i));
            ASSERT_EQ(node_names[i], interpreter.name(i));
        }
    }
}

TEST(HITInterpreter, object_empty_subobject)
{
    std::stringstream input;
    // Simple parse
    // document
    //  |_object
    //    |_ object_decl 'ted'
    //      |_ [ '['
    //      |_ string 'ted'
    //      |_ ] ']'
    //      |_ sub_object 'fred'
    //        |_ sub_object_decl 'fred'
    //          |_ [ '['
    //          |_ ./ './'
    //          |_ string 'fred'
    //          |_ ] ']'
    //        |_ object_term '[../]'
    //    |_ object_term '[]'

    input << "[ted][./fred][../][]";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(12, interpreter.node_count());

    std::vector<wasp::NODE> node_types = {wasp::LBRACKET,
                                          wasp::DECL,
                                          wasp::RBRACKET
                                          // sub object
                                          ,
                                          wasp::LBRACKET,
                                          wasp::DOT_SLASH,
                                          wasp::DECL,
                                          wasp::RBRACKET,
                                          wasp::OBJECT_TERM,
                                          wasp::OBJECT,
                                          wasp::OBJECT_TERM,
                                          wasp::OBJECT,
                                          wasp::DOCUMENT_ROOT};
    std::vector<std::string> node_names = {"[",    "decl", "]",   "[",
                                           "./",   "decl", "]",   "term",
                                           "fred", "term",   "ted", "document"};
    ASSERT_EQ(node_types.size(), node_names.size());
    for (std::size_t i = 0; i < interpreter.node_count(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(node_types[i], interpreter.type(i));
            ASSERT_EQ(node_names[i], interpreter.name(i));
        }
    }
}

TEST(HITInterpreter, object_subobject)
{
    std::stringstream input;
    // Simple parse
    // document
    //  |_object
    //    |_ object_decl 'ted'
    //      |_ [ '['
    //      |_ string 'ted'
    //      |_ ] ']'
    //      |_ sub_object 'fred'
    //        |_ sub_object_decl 'fred'
    //          |_ [ '['
    //          |_ ./ './'
    //          |_ string 'fred'
    //          |_ ] ']'
    //          |_key
    //            |_ decl 'key'
    //            |_ = '='
    //            |_ value '3.421'
    //              |_ object_term '[../]'
    //    |_ object_term '[]'

    input << "[ted][./fred]key=3.4321[../][]";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(16, interpreter.node_count());

    std::vector<wasp::NODE> node_types = {wasp::LBRACKET,
                                          wasp::DECL,
                                          wasp::RBRACKET
                                          // sub object
                                          ,
                                          wasp::LBRACKET,
                                          wasp::DOT_SLASH,
                                          wasp::DECL,
                                          wasp::RBRACKET
                                          // keyed value key
                                          ,
                                          wasp::DECL,
                                          wasp::ASSIGN,
                                          wasp::VALUE,
                                          wasp::KEYED_VALUE,
                                          wasp::OBJECT_TERM,
                                          wasp::OBJECT,
                                          wasp::OBJECT_TERM,
                                          wasp::OBJECT,
                                          wasp::DOCUMENT_ROOT};
    std::vector<std::string> node_names = {"[",
                                           "decl",
                                           "]",
                                           "[",
                                           "./",
                                           "decl",
                                           "]",
                                           "decl",
                                           "=",
                                           "value"  // key
                                           ,
                                           "key",
                                           "term",
                                           "fred",
                                           "term",
                                           "ted",
                                           "document"};
    ASSERT_EQ(node_types.size(), node_names.size());
    for (std::size_t i = 0; i < interpreter.node_count(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(node_types[i], interpreter.type(i));
            ASSERT_EQ(node_names[i], interpreter.name(i));
        }
    }
}

TEST(HITInterpreter, simple_view)
{
    std::stringstream input;
    // Simple parse
    // document
    //  |_key
    //    |_ decl 'key'
    //    |_ = '='
    //    |_ value '3.421'
    input << "key =  3.421";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());

    std::vector<wasp::NODE> node_types = {wasp::DECL, wasp::ASSIGN, wasp::VALUE,
                                          wasp::KEYED_VALUE,
                                          wasp::DOCUMENT_ROOT};
    std::vector<std::string> node_names = {"decl", "=", "value", "key",
                                           "document"};
    std::vector<std::string> node_paths = {"/key/decl", "/key/=", "/key/value",
                                           "/key", "/"};
    std::vector<std::size_t> node_columns = {1, 5, 8, 1, 1};
    ASSERT_EQ(node_types.size(), node_names.size());
    ASSERT_EQ(node_types.size(), node_columns.size());
    ASSERT_EQ(node_types.size(), node_paths.size());
    auto document = interpreter.root();
    // the root of the document
    ASSERT_EQ(1, document.child_count());
    ASSERT_EQ(node_names.back(), document.name());
    ASSERT_EQ(wasp::DOCUMENT_ROOT, document.type());
    ASSERT_EQ(1, document.line());
    ASSERT_EQ(1, document.column());
    ASSERT_TRUE(document.equal(document));
    ASSERT_TRUE(false == document.has_parent());
    ASSERT_EQ(node_paths.back(), document.path());
    auto key = document.child_at(0);
    ASSERT_EQ(3, key.child_count());
    ASSERT_TRUE(key.has_parent());
    ASSERT_TRUE(document.equal(key.parent()));
    ASSERT_EQ(wasp::KEYED_VALUE, key.type());
    ASSERT_EQ(node_names[3], key.name());
    ASSERT_EQ(1, key.line());
    ASSERT_EQ(1, key.column());
    ASSERT_FALSE(document.equal(key));
    ASSERT_EQ(node_paths[3], key.path());
    // TODO add test on data (type, line, col, etc).
    for (std::size_t i = 0, child_count = key.child_count(); i < child_count;
         ++i)
    {
        {
            SCOPED_TRACE(i);
            auto child = key.child_at(i);
            ASSERT_EQ(node_types[i], child.type());
            ASSERT_EQ(0, child.child_count());
            ASSERT_EQ(node_types[i], child.type());
            ASSERT_EQ(node_names[i], child.name());
            ASSERT_EQ(1, child.line());
            ASSERT_EQ(node_columns[i], child.column());
            ASSERT_TRUE(child.has_parent());
            ASSERT_TRUE(key.equal(child.parent()));
            ASSERT_EQ(node_paths[i], child.path());
        }
    }
    std::string expected_paths =
        "/\n/key\n/key/decl (key)\n/key/= (=)\n/key/value (3.421)\n";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
}

TEST(HITInterpreter, paths)
{
    std::stringstream input;
    input << R"INPUT([Problem]
  coord_type = RZ
  [./child][../]
[])INPUT";

    std::string expected_paths =
        R"INPUT(/
/Problem
/Problem/[ ([)
/Problem/decl (Problem)
/Problem/] (])
/Problem/coord_type
/Problem/coord_type/decl (coord_type)
/Problem/coord_type/= (=)
/Problem/coord_type/value (RZ)
/Problem/child
/Problem/child/[ ([)
/Problem/child/./ (./)
/Problem/child/decl (child)
/Problem/child/] (])
/Problem/child/term ([../])
/Problem/term ([])
)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(16, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
}

/**
 * @brief TEST hit doesn't support expressions natively
 * It just grabs them as strings
 */
TEST(HITInterpreter, expression)
{
    std::stringstream input;
    input << R"INPUT(
function = 'A*c^2*(1-c)^2+B*(c^2+6*(1-c),(gr0^2+gr1^2+gr2^2+gr3^2)
             -4*(2-c)*(gr0^3+gr1^3+gr2^3+gr3^3)
             +3*(gr0^2+gr1^2,gr2^2+gr3^2)^2)'
)INPUT";
    std::vector<std::string> data = {
        "function",
        "=",
        "'",
        "A*c^2*(1-c)^2+B*(c^2+6*(1-c),(gr0^2+gr1^2+gr2^2+gr3^2)",
        "-4*(2-c)*(gr0^3+gr1^3+gr2^3+gr3^3)",
        "+3*(gr0^2+gr1^2,gr2^2+gr3^2)^2)",
        "'"};

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(9, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto fnode = document.child_at(0);
    ASSERT_EQ(data.size(), fnode.child_count());
    for (std::size_t i = 0; i < fnode.child_count(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(data[i], fnode.child_at(i).data());
        }
    }
}
/**
 *  @brief TEST comments that are empty and with content
 */
TEST(HITInterpreter, comments)
{
    std::stringstream input;
    input << R"INPUT(#
# comment with content
#)INPUT";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(4, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(3, document.child_count());
    std::vector<std::string> data = {"#", "# comment with content", "#"};
    ASSERT_EQ(data.size(), document.child_count());
    for (std::size_t i = 0; i < document.child_count(); ++i)
    {
        {
            SCOPED_TRACE(i);
            ASSERT_EQ(data[i], document.child_at(i).data());
        }
    }
}

/**
 * @brief TEST nested subblocks (subblocks within subblocks)
 */
TEST(HITInterpreter, nested_subblocks)
{
    std::stringstream input;
    input << R"INPUT([block]
  [./subblock]
    [./nested_subblock]
    [../]
  [../]
[])INPUT";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(18, interpreter.node_count());
    auto document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    auto block = document.child_at(0);
    ASSERT_EQ("block", std::string(block.name()));
    ASSERT_EQ(wasp::OBJECT, block.type());
    ASSERT_EQ(5, block.child_count());
    auto subblock = block.child_at(3);
    ASSERT_EQ("subblock", std::string(subblock.name()));
    ASSERT_EQ(wasp::OBJECT, subblock.type());
    ASSERT_EQ(6, subblock.child_count());
    auto nestedsubblock = subblock.child_at(4);
    ASSERT_EQ("nested_subblock", std::string(nestedsubblock.name()));
    ASSERT_EQ(wasp::OBJECT, nestedsubblock.type());
    ASSERT_EQ(3, nestedsubblock.line());
    ASSERT_EQ(5, nestedsubblock.column());
}

TEST(HITInterpreter, multiple_objects)
{
    std::stringstream input;
    input << R"INPUT([Problem]
    # Specify coordinate system type
    coord_type = RZ
[]

[Mesh]
    file = pelletcladmergedfine10_rz.e
    displacements = 'disp_x disp_y'
    patch_size = 1000 # For contact algorithm
[]
[1]
    some = thing
    [./2]
        another=value
        sliceheight = ${/ ${fuelheight} ${numslices}}
    [../]
[])INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(55, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(3, document.child_count());  // problem and mesh
    ASSERT_EQ(3, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));
    std::string       expected_paths = R"INPUT(/
/Problem
/Problem/[ ([)
/Problem/decl (Problem)
/Problem/] (])
/Problem/comment (# Specify coordinate system type)
/Problem/coord_type
/Problem/coord_type/decl (coord_type)
/Problem/coord_type/= (=)
/Problem/coord_type/value (RZ)
/Problem/term ([])
/Mesh
/Mesh/[ ([)
/Mesh/decl (Mesh)
/Mesh/] (])
/Mesh/file
/Mesh/file/decl (file)
/Mesh/file/= (=)
/Mesh/file/value (pelletcladmergedfine10_rz.e)
/Mesh/displacements
/Mesh/displacements/decl (displacements)
/Mesh/displacements/= (=)
/Mesh/displacements/' (')
/Mesh/displacements/value (disp_x)
/Mesh/displacements/value (disp_y)
/Mesh/displacements/' (')
/Mesh/patch_size
/Mesh/patch_size/decl (patch_size)
/Mesh/patch_size/= (=)
/Mesh/patch_size/value (1000)
/Mesh/comment (# For contact algorithm)
/Mesh/term ([])
/1
/1/[ ([)
/1/decl (1)
/1/] (])
/1/some
/1/some/decl (some)
/1/some/= (=)
/1/some/value (thing)
/1/2
/1/2/[ ([)
/1/2/./ (./)
/1/2/decl (2)
/1/2/] (])
/1/2/another
/1/2/another/decl (another)
/1/2/another/= (=)
/1/2/another/value (value)
/1/2/sliceheight
/1/2/sliceheight/decl (sliceheight)
/1/2/sliceheight/= (=)
/1/2/sliceheight/value (${/ ${fuelheight} ${numslices}})
/1/2/term ([../])
/1/term ([])
)INPUT";
    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
    auto mesh_view = document.first_child_by_name("Mesh");
    ASSERT_FALSE(mesh_view.is_null());
    ASSERT_FALSE(mesh_view.is_decorative());
    ASSERT_EQ(wasp::OBJECT, mesh_view.type());
    ASSERT_EQ(8, mesh_view.child_count());
    ASSERT_EQ(3, mesh_view.non_decorative_children_count());
    auto disp_view = mesh_view.first_child_by_name("displacements");
    ASSERT_FALSE(disp_view.is_null());
    ASSERT_EQ(wasp::ARRAY, disp_view.type());
    ASSERT_EQ(2, disp_view.non_decorative_children_count());
    auto values = disp_view.non_decorative_children();
    ASSERT_EQ(2, values.size());
    for (auto v : values)
    {
        ASSERT_EQ("value", std::string(v.name()));
        ASSERT_EQ("disp", v.to_string().substr(0, 4));
    }
}

/**
 * @brief TEST hit type promotion
 * When a block/subblock has a type field,
 * the block/subblock name becomes the type field's value
 * with "_type" appended.
 *
 * Test each scenario for block and subblock
 * 1. block with only type = value
 * 2. block with type = value and other stuff
 * 3. subblock with only type = value
 * 4. subblock with type = value and other stuff
 * 5. subblock with type = value
 * 6. block with type = value
 */
TEST(HITInterpreter, type_promotion)
{
    std::stringstream input;
    input << R"INPUT([Problem]
    # Specify coordinate system type
    type = ted
    [./s1] type = fred [../]
    [./s2] type = x  y=1 [../]
[]
[p1]
    type = t1
[]
[p2]
   type = t2
    [./s3]
        type = us3
    [../]
[]
)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(63, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(3, document.child_count());  // problem and mesh
    ASSERT_EQ(3, interpreter.child_count(document.node_index()));

    std::string expected_paths;
#if DISABLE_HIT_TYPE_PROMOTION
    expected_paths = R"INPUT(/
/Problem
/Problem/[ ([)
/Problem/decl (Problem)
/Problem/] (])
/Problem/comment (# Specify coordinate system type)
/Problem/type
/Problem/type/decl (type)
/Problem/type/= (=)
/Problem/type/value (ted)
/Problem/s1
/Problem/s1/[ ([)
/Problem/s1/./ (./)
/Problem/s1/decl (s1)
/Problem/s1/] (])
/Problem/s1/type
/Problem/s1/type/decl (type)
/Problem/s1/type/= (=)
/Problem/s1/type/value (fred)
/Problem/s1/term ([../])
/Problem/s2
/Problem/s2/[ ([)
/Problem/s2/./ (./)
/Problem/s2/decl (s2)
/Problem/s2/] (])
/Problem/s2/type
/Problem/s2/type/decl (type)
/Problem/s2/type/= (=)
/Problem/s2/type/value (x)
/Problem/s2/y
/Problem/s2/y/decl (y)
/Problem/s2/y/= (=)
/Problem/s2/y/value (1)
/Problem/s2/term ([../])
/Problem/term ([])
/p1
/p1/[ ([)
/p1/decl (p1)
/p1/] (])
/p1/type
/p1/type/decl (type)
/p1/type/= (=)
/p1/type/value (t1)
/p1/term ([])
/p2
/p2/[ ([)
/p2/decl (p2)
/p2/] (])
/p2/type
/p2/type/decl (type)
/p2/type/= (=)
/p2/type/value (t2)
/p2/s3
/p2/s3/[ ([)
/p2/s3/./ (./)
/p2/s3/decl (s3)
/p2/s3/] (])
/p2/s3/type
/p2/s3/type/decl (type)
/p2/s3/type/= (=)
/p2/s3/type/value (us3)
/p2/s3/term ([../])
/p2/term ([])
)INPUT";
#else
    expected_paths = R"INPUT(/
/ted_type
/ted_type/[ ([)
/ted_type/decl (Problem)
/ted_type/] (])
/ted_type/comment (# Specify coordinate system type)
/ted_type/type
/ted_type/type/decl (type)
/ted_type/type/= (=)
/ted_type/type/value (ted)
/ted_type/fred_type
/ted_type/fred_type/[ ([)
/ted_type/fred_type/./ (./)
/ted_type/fred_type/decl (s1)
/ted_type/fred_type/] (])
/ted_type/fred_type/type
/ted_type/fred_type/type/decl (type)
/ted_type/fred_type/type/= (=)
/ted_type/fred_type/type/value (fred)
/ted_type/fred_type/term ([../])
/ted_type/x_type
/ted_type/x_type/[ ([)
/ted_type/x_type/./ (./)
/ted_type/x_type/decl (s2)
/ted_type/x_type/] (])
/ted_type/x_type/type
/ted_type/x_type/type/decl (type)
/ted_type/x_type/type/= (=)
/ted_type/x_type/type/value (x)
/ted_type/x_type/y
/ted_type/x_type/y/decl (y)
/ted_type/x_type/y/= (=)
/ted_type/x_type/y/value (1)
/ted_type/x_type/term ([../])
/ted_type/term ([])
/t1_type
/t1_type/[ ([)
/t1_type/decl (p1)
/t1_type/] (])
/t1_type/type
/t1_type/type/decl (type)
/t1_type/type/= (=)
/t1_type/type/value (t1)
/t1_type/term ([])
/t2_type
/t2_type/[ ([)
/t2_type/decl (p2)
/t2_type/] (])
/t2_type/type
/t2_type/type/decl (type)
/t2_type/type/= (=)
/t2_type/type/value (t2)
/t2_type/us3_type
/t2_type/us3_type/[ ([)
/t2_type/us3_type/./ (./)
/t2_type/us3_type/decl (s3)
/t2_type/us3_type/] (])
/t2_type/us3_type/type
/t2_type/us3_type/type/decl (type)
/t2_type/us3_type/type/= (=)
/t2_type/us3_type/type/value (us3)
/t2_type/us3_type/term ([../])
/t2_type/term ([])
)INPUT";
#endif

    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());

    std::string first_block_name;
#if DISABLE_HIT_TYPE_PROMOTION
    first_block_name = "Problem";
#else
    first_block_name = "ted_type";
#endif
    auto first_block = document.first_child_by_name(first_block_name);
    ASSERT_FALSE(first_block.is_null());
    ASSERT_FALSE(first_block.is_decorative());
    ASSERT_EQ(wasp::OBJECT, first_block.type());
    ASSERT_EQ(8, first_block.child_count());
    ASSERT_EQ(3, first_block.non_decorative_children_count());
}

/**
 * @brief Test HIT syntax
 *
 * MOOSE's new HIT parser relaxes the declarator and terminator syntax
 * for blocks and subblocks and all blocks are considered the same type
 *
 * Blocks and subblocks may be started with:
 *  - Block declarator syntax    ( i.e. "[blockname]")
 *  - Subblock declarator syntax ( i.e. "[./blockname]")
 *
 * Blocks and subblocks may be terminated with:
 *  - Block terminator syntax    ( i.e. "[]")
 *  - Subblock terminator syntax ( i.e. "[../]")
 */
TEST(HITInterpreter, hit_syntax)
{
    std::stringstream input;
    input << R"INPUT(
[TopLevelBlock]

  [./sub-decl-sub-term-no-type]
    flag01  = 1.1
    array01 = 'name11 name12 name13'
  [../]

  [./sub-decl-top-term-no-type]
    flag02  = 2.2
    array02 = 'name21 name22 name23'
  []

  [top-decl-sub-term-no-type]
    flag03  = 3.3
    array03 = 'name31 name32 name33'
  [../]

  [top-decl-top-term-no-type]
    flag04  = 4.4
    array04 = 'name41 name42 name43'
  []

  [./sub-decl-sub-term-wt-type]
    type    = SubDeclSubTerm
    flag05  = 5.5
    array05 = 'name51 name42 name53'
  [../]

  [./sub-decl-top-term-wt-type]
    type    = SubDeclTopTerm
    flag06  = 6.6
    array06 = 'name61 name62 name63'
  []

  [top-decl-sub-term-wt-type]
    type    = TopDeclSubTerm
    flag07  = 7.7
    array07 = 'name71 name72 name73'
  [../]

  [top-decl-top-term-wt-type]
    type    = TopDeclTopTerm
    flag08  = 8.8
    array08 = 'name81 name82 name83'
  []

[]
)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(162, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    ASSERT_EQ(1, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths;
#if DISABLE_HIT_TYPE_PROMOTION
    expected_paths = R"INPUT(/
/TopLevelBlock
/TopLevelBlock/[ ([)
/TopLevelBlock/decl (TopLevelBlock)
/TopLevelBlock/] (])
/TopLevelBlock/sub-decl-sub-term-no-type
/TopLevelBlock/sub-decl-sub-term-no-type/[ ([)
/TopLevelBlock/sub-decl-sub-term-no-type/./ (./)
/TopLevelBlock/sub-decl-sub-term-no-type/decl (sub-decl-sub-term-no-type)
/TopLevelBlock/sub-decl-sub-term-no-type/] (])
/TopLevelBlock/sub-decl-sub-term-no-type/flag01
/TopLevelBlock/sub-decl-sub-term-no-type/flag01/decl (flag01)
/TopLevelBlock/sub-decl-sub-term-no-type/flag01/= (=)
/TopLevelBlock/sub-decl-sub-term-no-type/flag01/value (1.1)
/TopLevelBlock/sub-decl-sub-term-no-type/array01
/TopLevelBlock/sub-decl-sub-term-no-type/array01/decl (array01)
/TopLevelBlock/sub-decl-sub-term-no-type/array01/= (=)
/TopLevelBlock/sub-decl-sub-term-no-type/array01/' (')
/TopLevelBlock/sub-decl-sub-term-no-type/array01/value (name11)
/TopLevelBlock/sub-decl-sub-term-no-type/array01/value (name12)
/TopLevelBlock/sub-decl-sub-term-no-type/array01/value (name13)
/TopLevelBlock/sub-decl-sub-term-no-type/array01/' (')
/TopLevelBlock/sub-decl-sub-term-no-type/term ([../])
/TopLevelBlock/sub-decl-top-term-no-type
/TopLevelBlock/sub-decl-top-term-no-type/[ ([)
/TopLevelBlock/sub-decl-top-term-no-type/./ (./)
/TopLevelBlock/sub-decl-top-term-no-type/decl (sub-decl-top-term-no-type)
/TopLevelBlock/sub-decl-top-term-no-type/] (])
/TopLevelBlock/sub-decl-top-term-no-type/flag02
/TopLevelBlock/sub-decl-top-term-no-type/flag02/decl (flag02)
/TopLevelBlock/sub-decl-top-term-no-type/flag02/= (=)
/TopLevelBlock/sub-decl-top-term-no-type/flag02/value (2.2)
/TopLevelBlock/sub-decl-top-term-no-type/array02
/TopLevelBlock/sub-decl-top-term-no-type/array02/decl (array02)
/TopLevelBlock/sub-decl-top-term-no-type/array02/= (=)
/TopLevelBlock/sub-decl-top-term-no-type/array02/' (')
/TopLevelBlock/sub-decl-top-term-no-type/array02/value (name21)
/TopLevelBlock/sub-decl-top-term-no-type/array02/value (name22)
/TopLevelBlock/sub-decl-top-term-no-type/array02/value (name23)
/TopLevelBlock/sub-decl-top-term-no-type/array02/' (')
/TopLevelBlock/sub-decl-top-term-no-type/term ([])
/TopLevelBlock/top-decl-sub-term-no-type
/TopLevelBlock/top-decl-sub-term-no-type/[ ([)
/TopLevelBlock/top-decl-sub-term-no-type/decl (top-decl-sub-term-no-type)
/TopLevelBlock/top-decl-sub-term-no-type/] (])
/TopLevelBlock/top-decl-sub-term-no-type/flag03
/TopLevelBlock/top-decl-sub-term-no-type/flag03/decl (flag03)
/TopLevelBlock/top-decl-sub-term-no-type/flag03/= (=)
/TopLevelBlock/top-decl-sub-term-no-type/flag03/value (3.3)
/TopLevelBlock/top-decl-sub-term-no-type/array03
/TopLevelBlock/top-decl-sub-term-no-type/array03/decl (array03)
/TopLevelBlock/top-decl-sub-term-no-type/array03/= (=)
/TopLevelBlock/top-decl-sub-term-no-type/array03/' (')
/TopLevelBlock/top-decl-sub-term-no-type/array03/value (name31)
/TopLevelBlock/top-decl-sub-term-no-type/array03/value (name32)
/TopLevelBlock/top-decl-sub-term-no-type/array03/value (name33)
/TopLevelBlock/top-decl-sub-term-no-type/array03/' (')
/TopLevelBlock/top-decl-sub-term-no-type/term ([../])
/TopLevelBlock/top-decl-top-term-no-type
/TopLevelBlock/top-decl-top-term-no-type/[ ([)
/TopLevelBlock/top-decl-top-term-no-type/decl (top-decl-top-term-no-type)
/TopLevelBlock/top-decl-top-term-no-type/] (])
/TopLevelBlock/top-decl-top-term-no-type/flag04
/TopLevelBlock/top-decl-top-term-no-type/flag04/decl (flag04)
/TopLevelBlock/top-decl-top-term-no-type/flag04/= (=)
/TopLevelBlock/top-decl-top-term-no-type/flag04/value (4.4)
/TopLevelBlock/top-decl-top-term-no-type/array04
/TopLevelBlock/top-decl-top-term-no-type/array04/decl (array04)
/TopLevelBlock/top-decl-top-term-no-type/array04/= (=)
/TopLevelBlock/top-decl-top-term-no-type/array04/' (')
/TopLevelBlock/top-decl-top-term-no-type/array04/value (name41)
/TopLevelBlock/top-decl-top-term-no-type/array04/value (name42)
/TopLevelBlock/top-decl-top-term-no-type/array04/value (name43)
/TopLevelBlock/top-decl-top-term-no-type/array04/' (')
/TopLevelBlock/top-decl-top-term-no-type/term ([])
/TopLevelBlock/sub-decl-sub-term-wt-type
/TopLevelBlock/sub-decl-sub-term-wt-type/[ ([)
/TopLevelBlock/sub-decl-sub-term-wt-type/./ (./)
/TopLevelBlock/sub-decl-sub-term-wt-type/decl (sub-decl-sub-term-wt-type)
/TopLevelBlock/sub-decl-sub-term-wt-type/] (])
/TopLevelBlock/sub-decl-sub-term-wt-type/type
/TopLevelBlock/sub-decl-sub-term-wt-type/type/decl (type)
/TopLevelBlock/sub-decl-sub-term-wt-type/type/= (=)
/TopLevelBlock/sub-decl-sub-term-wt-type/type/value (SubDeclSubTerm)
/TopLevelBlock/sub-decl-sub-term-wt-type/flag05
/TopLevelBlock/sub-decl-sub-term-wt-type/flag05/decl (flag05)
/TopLevelBlock/sub-decl-sub-term-wt-type/flag05/= (=)
/TopLevelBlock/sub-decl-sub-term-wt-type/flag05/value (5.5)
/TopLevelBlock/sub-decl-sub-term-wt-type/array05
/TopLevelBlock/sub-decl-sub-term-wt-type/array05/decl (array05)
/TopLevelBlock/sub-decl-sub-term-wt-type/array05/= (=)
/TopLevelBlock/sub-decl-sub-term-wt-type/array05/' (')
/TopLevelBlock/sub-decl-sub-term-wt-type/array05/value (name51)
/TopLevelBlock/sub-decl-sub-term-wt-type/array05/value (name42)
/TopLevelBlock/sub-decl-sub-term-wt-type/array05/value (name53)
/TopLevelBlock/sub-decl-sub-term-wt-type/array05/' (')
/TopLevelBlock/sub-decl-sub-term-wt-type/term ([../])
/TopLevelBlock/sub-decl-top-term-wt-type
/TopLevelBlock/sub-decl-top-term-wt-type/[ ([)
/TopLevelBlock/sub-decl-top-term-wt-type/./ (./)
/TopLevelBlock/sub-decl-top-term-wt-type/decl (sub-decl-top-term-wt-type)
/TopLevelBlock/sub-decl-top-term-wt-type/] (])
/TopLevelBlock/sub-decl-top-term-wt-type/type
/TopLevelBlock/sub-decl-top-term-wt-type/type/decl (type)
/TopLevelBlock/sub-decl-top-term-wt-type/type/= (=)
/TopLevelBlock/sub-decl-top-term-wt-type/type/value (SubDeclTopTerm)
/TopLevelBlock/sub-decl-top-term-wt-type/flag06
/TopLevelBlock/sub-decl-top-term-wt-type/flag06/decl (flag06)
/TopLevelBlock/sub-decl-top-term-wt-type/flag06/= (=)
/TopLevelBlock/sub-decl-top-term-wt-type/flag06/value (6.6)
/TopLevelBlock/sub-decl-top-term-wt-type/array06
/TopLevelBlock/sub-decl-top-term-wt-type/array06/decl (array06)
/TopLevelBlock/sub-decl-top-term-wt-type/array06/= (=)
/TopLevelBlock/sub-decl-top-term-wt-type/array06/' (')
/TopLevelBlock/sub-decl-top-term-wt-type/array06/value (name61)
/TopLevelBlock/sub-decl-top-term-wt-type/array06/value (name62)
/TopLevelBlock/sub-decl-top-term-wt-type/array06/value (name63)
/TopLevelBlock/sub-decl-top-term-wt-type/array06/' (')
/TopLevelBlock/sub-decl-top-term-wt-type/term ([])
/TopLevelBlock/top-decl-sub-term-wt-type
/TopLevelBlock/top-decl-sub-term-wt-type/[ ([)
/TopLevelBlock/top-decl-sub-term-wt-type/decl (top-decl-sub-term-wt-type)
/TopLevelBlock/top-decl-sub-term-wt-type/] (])
/TopLevelBlock/top-decl-sub-term-wt-type/type
/TopLevelBlock/top-decl-sub-term-wt-type/type/decl (type)
/TopLevelBlock/top-decl-sub-term-wt-type/type/= (=)
/TopLevelBlock/top-decl-sub-term-wt-type/type/value (TopDeclSubTerm)
/TopLevelBlock/top-decl-sub-term-wt-type/flag07
/TopLevelBlock/top-decl-sub-term-wt-type/flag07/decl (flag07)
/TopLevelBlock/top-decl-sub-term-wt-type/flag07/= (=)
/TopLevelBlock/top-decl-sub-term-wt-type/flag07/value (7.7)
/TopLevelBlock/top-decl-sub-term-wt-type/array07
/TopLevelBlock/top-decl-sub-term-wt-type/array07/decl (array07)
/TopLevelBlock/top-decl-sub-term-wt-type/array07/= (=)
/TopLevelBlock/top-decl-sub-term-wt-type/array07/' (')
/TopLevelBlock/top-decl-sub-term-wt-type/array07/value (name71)
/TopLevelBlock/top-decl-sub-term-wt-type/array07/value (name72)
/TopLevelBlock/top-decl-sub-term-wt-type/array07/value (name73)
/TopLevelBlock/top-decl-sub-term-wt-type/array07/' (')
/TopLevelBlock/top-decl-sub-term-wt-type/term ([../])
/TopLevelBlock/top-decl-top-term-wt-type
/TopLevelBlock/top-decl-top-term-wt-type/[ ([)
/TopLevelBlock/top-decl-top-term-wt-type/decl (top-decl-top-term-wt-type)
/TopLevelBlock/top-decl-top-term-wt-type/] (])
/TopLevelBlock/top-decl-top-term-wt-type/type
/TopLevelBlock/top-decl-top-term-wt-type/type/decl (type)
/TopLevelBlock/top-decl-top-term-wt-type/type/= (=)
/TopLevelBlock/top-decl-top-term-wt-type/type/value (TopDeclTopTerm)
/TopLevelBlock/top-decl-top-term-wt-type/flag08
/TopLevelBlock/top-decl-top-term-wt-type/flag08/decl (flag08)
/TopLevelBlock/top-decl-top-term-wt-type/flag08/= (=)
/TopLevelBlock/top-decl-top-term-wt-type/flag08/value (8.8)
/TopLevelBlock/top-decl-top-term-wt-type/array08
/TopLevelBlock/top-decl-top-term-wt-type/array08/decl (array08)
/TopLevelBlock/top-decl-top-term-wt-type/array08/= (=)
/TopLevelBlock/top-decl-top-term-wt-type/array08/' (')
/TopLevelBlock/top-decl-top-term-wt-type/array08/value (name81)
/TopLevelBlock/top-decl-top-term-wt-type/array08/value (name82)
/TopLevelBlock/top-decl-top-term-wt-type/array08/value (name83)
/TopLevelBlock/top-decl-top-term-wt-type/array08/' (')
/TopLevelBlock/top-decl-top-term-wt-type/term ([])
/TopLevelBlock/term ([])
)INPUT";
#else
    expected_paths = R"INPUT(/
/TopLevelBlock
/TopLevelBlock/[ ([)
/TopLevelBlock/decl (TopLevelBlock)
/TopLevelBlock/] (])
/TopLevelBlock/sub-decl-sub-term-no-type
/TopLevelBlock/sub-decl-sub-term-no-type/[ ([)
/TopLevelBlock/sub-decl-sub-term-no-type/./ (./)
/TopLevelBlock/sub-decl-sub-term-no-type/decl (sub-decl-sub-term-no-type)
/TopLevelBlock/sub-decl-sub-term-no-type/] (])
/TopLevelBlock/sub-decl-sub-term-no-type/flag01
/TopLevelBlock/sub-decl-sub-term-no-type/flag01/decl (flag01)
/TopLevelBlock/sub-decl-sub-term-no-type/flag01/= (=)
/TopLevelBlock/sub-decl-sub-term-no-type/flag01/value (1.1)
/TopLevelBlock/sub-decl-sub-term-no-type/array01
/TopLevelBlock/sub-decl-sub-term-no-type/array01/decl (array01)
/TopLevelBlock/sub-decl-sub-term-no-type/array01/= (=)
/TopLevelBlock/sub-decl-sub-term-no-type/array01/' (')
/TopLevelBlock/sub-decl-sub-term-no-type/array01/value (name11)
/TopLevelBlock/sub-decl-sub-term-no-type/array01/value (name12)
/TopLevelBlock/sub-decl-sub-term-no-type/array01/value (name13)
/TopLevelBlock/sub-decl-sub-term-no-type/array01/' (')
/TopLevelBlock/sub-decl-sub-term-no-type/term ([../])
/TopLevelBlock/sub-decl-top-term-no-type
/TopLevelBlock/sub-decl-top-term-no-type/[ ([)
/TopLevelBlock/sub-decl-top-term-no-type/./ (./)
/TopLevelBlock/sub-decl-top-term-no-type/decl (sub-decl-top-term-no-type)
/TopLevelBlock/sub-decl-top-term-no-type/] (])
/TopLevelBlock/sub-decl-top-term-no-type/flag02
/TopLevelBlock/sub-decl-top-term-no-type/flag02/decl (flag02)
/TopLevelBlock/sub-decl-top-term-no-type/flag02/= (=)
/TopLevelBlock/sub-decl-top-term-no-type/flag02/value (2.2)
/TopLevelBlock/sub-decl-top-term-no-type/array02
/TopLevelBlock/sub-decl-top-term-no-type/array02/decl (array02)
/TopLevelBlock/sub-decl-top-term-no-type/array02/= (=)
/TopLevelBlock/sub-decl-top-term-no-type/array02/' (')
/TopLevelBlock/sub-decl-top-term-no-type/array02/value (name21)
/TopLevelBlock/sub-decl-top-term-no-type/array02/value (name22)
/TopLevelBlock/sub-decl-top-term-no-type/array02/value (name23)
/TopLevelBlock/sub-decl-top-term-no-type/array02/' (')
/TopLevelBlock/sub-decl-top-term-no-type/term ([])
/TopLevelBlock/top-decl-sub-term-no-type
/TopLevelBlock/top-decl-sub-term-no-type/[ ([)
/TopLevelBlock/top-decl-sub-term-no-type/decl (top-decl-sub-term-no-type)
/TopLevelBlock/top-decl-sub-term-no-type/] (])
/TopLevelBlock/top-decl-sub-term-no-type/flag03
/TopLevelBlock/top-decl-sub-term-no-type/flag03/decl (flag03)
/TopLevelBlock/top-decl-sub-term-no-type/flag03/= (=)
/TopLevelBlock/top-decl-sub-term-no-type/flag03/value (3.3)
/TopLevelBlock/top-decl-sub-term-no-type/array03
/TopLevelBlock/top-decl-sub-term-no-type/array03/decl (array03)
/TopLevelBlock/top-decl-sub-term-no-type/array03/= (=)
/TopLevelBlock/top-decl-sub-term-no-type/array03/' (')
/TopLevelBlock/top-decl-sub-term-no-type/array03/value (name31)
/TopLevelBlock/top-decl-sub-term-no-type/array03/value (name32)
/TopLevelBlock/top-decl-sub-term-no-type/array03/value (name33)
/TopLevelBlock/top-decl-sub-term-no-type/array03/' (')
/TopLevelBlock/top-decl-sub-term-no-type/term ([../])
/TopLevelBlock/top-decl-top-term-no-type
/TopLevelBlock/top-decl-top-term-no-type/[ ([)
/TopLevelBlock/top-decl-top-term-no-type/decl (top-decl-top-term-no-type)
/TopLevelBlock/top-decl-top-term-no-type/] (])
/TopLevelBlock/top-decl-top-term-no-type/flag04
/TopLevelBlock/top-decl-top-term-no-type/flag04/decl (flag04)
/TopLevelBlock/top-decl-top-term-no-type/flag04/= (=)
/TopLevelBlock/top-decl-top-term-no-type/flag04/value (4.4)
/TopLevelBlock/top-decl-top-term-no-type/array04
/TopLevelBlock/top-decl-top-term-no-type/array04/decl (array04)
/TopLevelBlock/top-decl-top-term-no-type/array04/= (=)
/TopLevelBlock/top-decl-top-term-no-type/array04/' (')
/TopLevelBlock/top-decl-top-term-no-type/array04/value (name41)
/TopLevelBlock/top-decl-top-term-no-type/array04/value (name42)
/TopLevelBlock/top-decl-top-term-no-type/array04/value (name43)
/TopLevelBlock/top-decl-top-term-no-type/array04/' (')
/TopLevelBlock/top-decl-top-term-no-type/term ([])
/TopLevelBlock/SubDeclSubTerm_type
/TopLevelBlock/SubDeclSubTerm_type/[ ([)
/TopLevelBlock/SubDeclSubTerm_type/./ (./)
/TopLevelBlock/SubDeclSubTerm_type/decl (sub-decl-sub-term-wt-type)
/TopLevelBlock/SubDeclSubTerm_type/] (])
/TopLevelBlock/SubDeclSubTerm_type/type
/TopLevelBlock/SubDeclSubTerm_type/type/decl (type)
/TopLevelBlock/SubDeclSubTerm_type/type/= (=)
/TopLevelBlock/SubDeclSubTerm_type/type/value (SubDeclSubTerm)
/TopLevelBlock/SubDeclSubTerm_type/flag05
/TopLevelBlock/SubDeclSubTerm_type/flag05/decl (flag05)
/TopLevelBlock/SubDeclSubTerm_type/flag05/= (=)
/TopLevelBlock/SubDeclSubTerm_type/flag05/value (5.5)
/TopLevelBlock/SubDeclSubTerm_type/array05
/TopLevelBlock/SubDeclSubTerm_type/array05/decl (array05)
/TopLevelBlock/SubDeclSubTerm_type/array05/= (=)
/TopLevelBlock/SubDeclSubTerm_type/array05/' (')
/TopLevelBlock/SubDeclSubTerm_type/array05/value (name51)
/TopLevelBlock/SubDeclSubTerm_type/array05/value (name42)
/TopLevelBlock/SubDeclSubTerm_type/array05/value (name53)
/TopLevelBlock/SubDeclSubTerm_type/array05/' (')
/TopLevelBlock/SubDeclSubTerm_type/term ([../])
/TopLevelBlock/SubDeclTopTerm_type
/TopLevelBlock/SubDeclTopTerm_type/[ ([)
/TopLevelBlock/SubDeclTopTerm_type/./ (./)
/TopLevelBlock/SubDeclTopTerm_type/decl (sub-decl-top-term-wt-type)
/TopLevelBlock/SubDeclTopTerm_type/] (])
/TopLevelBlock/SubDeclTopTerm_type/type
/TopLevelBlock/SubDeclTopTerm_type/type/decl (type)
/TopLevelBlock/SubDeclTopTerm_type/type/= (=)
/TopLevelBlock/SubDeclTopTerm_type/type/value (SubDeclTopTerm)
/TopLevelBlock/SubDeclTopTerm_type/flag06
/TopLevelBlock/SubDeclTopTerm_type/flag06/decl (flag06)
/TopLevelBlock/SubDeclTopTerm_type/flag06/= (=)
/TopLevelBlock/SubDeclTopTerm_type/flag06/value (6.6)
/TopLevelBlock/SubDeclTopTerm_type/array06
/TopLevelBlock/SubDeclTopTerm_type/array06/decl (array06)
/TopLevelBlock/SubDeclTopTerm_type/array06/= (=)
/TopLevelBlock/SubDeclTopTerm_type/array06/' (')
/TopLevelBlock/SubDeclTopTerm_type/array06/value (name61)
/TopLevelBlock/SubDeclTopTerm_type/array06/value (name62)
/TopLevelBlock/SubDeclTopTerm_type/array06/value (name63)
/TopLevelBlock/SubDeclTopTerm_type/array06/' (')
/TopLevelBlock/SubDeclTopTerm_type/term ([])
/TopLevelBlock/TopDeclSubTerm_type
/TopLevelBlock/TopDeclSubTerm_type/[ ([)
/TopLevelBlock/TopDeclSubTerm_type/decl (top-decl-sub-term-wt-type)
/TopLevelBlock/TopDeclSubTerm_type/] (])
/TopLevelBlock/TopDeclSubTerm_type/type
/TopLevelBlock/TopDeclSubTerm_type/type/decl (type)
/TopLevelBlock/TopDeclSubTerm_type/type/= (=)
/TopLevelBlock/TopDeclSubTerm_type/type/value (TopDeclSubTerm)
/TopLevelBlock/TopDeclSubTerm_type/flag07
/TopLevelBlock/TopDeclSubTerm_type/flag07/decl (flag07)
/TopLevelBlock/TopDeclSubTerm_type/flag07/= (=)
/TopLevelBlock/TopDeclSubTerm_type/flag07/value (7.7)
/TopLevelBlock/TopDeclSubTerm_type/array07
/TopLevelBlock/TopDeclSubTerm_type/array07/decl (array07)
/TopLevelBlock/TopDeclSubTerm_type/array07/= (=)
/TopLevelBlock/TopDeclSubTerm_type/array07/' (')
/TopLevelBlock/TopDeclSubTerm_type/array07/value (name71)
/TopLevelBlock/TopDeclSubTerm_type/array07/value (name72)
/TopLevelBlock/TopDeclSubTerm_type/array07/value (name73)
/TopLevelBlock/TopDeclSubTerm_type/array07/' (')
/TopLevelBlock/TopDeclSubTerm_type/term ([../])
/TopLevelBlock/TopDeclTopTerm_type
/TopLevelBlock/TopDeclTopTerm_type/[ ([)
/TopLevelBlock/TopDeclTopTerm_type/decl (top-decl-top-term-wt-type)
/TopLevelBlock/TopDeclTopTerm_type/] (])
/TopLevelBlock/TopDeclTopTerm_type/type
/TopLevelBlock/TopDeclTopTerm_type/type/decl (type)
/TopLevelBlock/TopDeclTopTerm_type/type/= (=)
/TopLevelBlock/TopDeclTopTerm_type/type/value (TopDeclTopTerm)
/TopLevelBlock/TopDeclTopTerm_type/flag08
/TopLevelBlock/TopDeclTopTerm_type/flag08/decl (flag08)
/TopLevelBlock/TopDeclTopTerm_type/flag08/= (=)
/TopLevelBlock/TopDeclTopTerm_type/flag08/value (8.8)
/TopLevelBlock/TopDeclTopTerm_type/array08
/TopLevelBlock/TopDeclTopTerm_type/array08/decl (array08)
/TopLevelBlock/TopDeclTopTerm_type/array08/= (=)
/TopLevelBlock/TopDeclTopTerm_type/array08/' (')
/TopLevelBlock/TopDeclTopTerm_type/array08/value (name81)
/TopLevelBlock/TopDeclTopTerm_type/array08/value (name82)
/TopLevelBlock/TopDeclTopTerm_type/array08/value (name83)
/TopLevelBlock/TopDeclTopTerm_type/array08/' (')
/TopLevelBlock/TopDeclTopTerm_type/term ([])
/TopLevelBlock/term ([])
)INPUT";
#endif

    std::stringstream paths;
    document.paths(paths);
    ASSERT_EQ(expected_paths, paths.str());
}

/**
 * @brief Test HIT syntax - period in block declarators
 */
TEST(HITInterpreter, period_in_block_declarators)
{
    std::stringstream input;
    input << R"INPUT(
[TopBlock]
  [child.01.period.no.type]
    var01 = val01
  [../]
  [./child.02.period.no.type]
    var02 = val02
  []
[]
)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(25, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    ASSERT_EQ(1, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/TopBlock
/TopBlock/[ ([)
/TopBlock/decl (TopBlock)
/TopBlock/] (])
/TopBlock/child.01.period.no.type
/TopBlock/child.01.period.no.type/[ ([)
/TopBlock/child.01.period.no.type/decl (child.01.period.no.type)
/TopBlock/child.01.period.no.type/] (])
/TopBlock/child.01.period.no.type/var01
/TopBlock/child.01.period.no.type/var01/decl (var01)
/TopBlock/child.01.period.no.type/var01/= (=)
/TopBlock/child.01.period.no.type/var01/value (val01)
/TopBlock/child.01.period.no.type/term ([../])
/TopBlock/child.02.period.no.type
/TopBlock/child.02.period.no.type/[ ([)
/TopBlock/child.02.period.no.type/./ (./)
/TopBlock/child.02.period.no.type/decl (child.02.period.no.type)
/TopBlock/child.02.period.no.type/] (])
/TopBlock/child.02.period.no.type/var02
/TopBlock/child.02.period.no.type/var02/decl (var02)
/TopBlock/child.02.period.no.type/var02/= (=)
/TopBlock/child.02.period.no.type/var02/value (val02)
/TopBlock/child.02.period.no.type/term ([])
/TopBlock/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - equals, brackets, and double quotes in arrays
 */
TEST(HITInterpreter, equals_brackets_quotes_in_arrays)
{
    std::stringstream input;
    input << R"INPUT(
array01 = 'd2F1:=D[F1,eta1,eta1] d2F2:=D[F2,eta1,eta1]'
[BlockContainer]
  array02 = ' 08.4;; ; -5;;123;abc:=t[e_x]t;;-02.85;  '
  array03 = 'path/to/real=1.01;path/to/int=10;path/to/string=one;path/to/blank=" "
             path/to/real=2.02;path/to/int=20;path/to/string=two;path/to/blank=""'
[]
)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(53, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(2, document.child_count());
    ASSERT_EQ(2, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/array01
/array01/decl (array01)
/array01/= (=)
/array01/' (')
/array01/value (d2F1:=D[F1,eta1,eta1])
/array01/value (d2F2:=D[F2,eta1,eta1])
/array01/' (')
/BlockContainer
/BlockContainer/[ ([)
/BlockContainer/decl (BlockContainer)
/BlockContainer/] (])
/BlockContainer/array02
/BlockContainer/array02/decl (array02)
/BlockContainer/array02/= (=)
/BlockContainer/array02/' (')
/BlockContainer/array02/value (08.4)
/BlockContainer/array02/; (;)
/BlockContainer/array02/; (;)
/BlockContainer/array02/; (;)
/BlockContainer/array02/value (-5)
/BlockContainer/array02/; (;)
/BlockContainer/array02/; (;)
/BlockContainer/array02/value (123)
/BlockContainer/array02/; (;)
/BlockContainer/array02/value (abc:=t[e_x]t)
/BlockContainer/array02/; (;)
/BlockContainer/array02/; (;)
/BlockContainer/array02/value (-02.85)
/BlockContainer/array02/; (;)
/BlockContainer/array02/' (')
/BlockContainer/array03
/BlockContainer/array03/decl (array03)
/BlockContainer/array03/= (=)
/BlockContainer/array03/' (')
/BlockContainer/array03/value (path/to/real=1.01)
/BlockContainer/array03/; (;)
/BlockContainer/array03/value (path/to/int=10)
/BlockContainer/array03/; (;)
/BlockContainer/array03/value (path/to/string=one)
/BlockContainer/array03/; (;)
/BlockContainer/array03/value (path/to/blank=)
/BlockContainer/array03/value (" ")
/BlockContainer/array03/value (path/to/real=2.02)
/BlockContainer/array03/; (;)
/BlockContainer/array03/value (path/to/int=20)
/BlockContainer/array03/; (;)
/BlockContainer/array03/value (path/to/string=two)
/BlockContainer/array03/; (;)
/BlockContainer/array03/value (path/to/blank=)
/BlockContainer/array03/value ("")
/BlockContainer/array03/' (')
/BlockContainer/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - double quotes in arrays
 */
TEST(HITInterpreter, double_quotes_in_arrays)
{
    std::stringstream input;
    input << R"INPUT(
[DoubleQuotesInArrays]
  array01 = 'one two="foo bar "'
  array02 = 'one two="foo bar;"'
  array03 = 'one two"foo bar "'
  array04 = 'one two"foo bar;"'
  array05 = 'one two "foo bar "'
  array06 = 'one two "foo bar;"'
[]
)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(54, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    ASSERT_EQ(1, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/DoubleQuotesInArrays
/DoubleQuotesInArrays/[ ([)
/DoubleQuotesInArrays/decl (DoubleQuotesInArrays)
/DoubleQuotesInArrays/] (])
/DoubleQuotesInArrays/array01
/DoubleQuotesInArrays/array01/decl (array01)
/DoubleQuotesInArrays/array01/= (=)
/DoubleQuotesInArrays/array01/' (')
/DoubleQuotesInArrays/array01/value (one)
/DoubleQuotesInArrays/array01/value (two=)
/DoubleQuotesInArrays/array01/value ("foo bar ")
/DoubleQuotesInArrays/array01/' (')
/DoubleQuotesInArrays/array02
/DoubleQuotesInArrays/array02/decl (array02)
/DoubleQuotesInArrays/array02/= (=)
/DoubleQuotesInArrays/array02/' (')
/DoubleQuotesInArrays/array02/value (one)
/DoubleQuotesInArrays/array02/value (two=)
/DoubleQuotesInArrays/array02/value ("foo bar;")
/DoubleQuotesInArrays/array02/' (')
/DoubleQuotesInArrays/array03
/DoubleQuotesInArrays/array03/decl (array03)
/DoubleQuotesInArrays/array03/= (=)
/DoubleQuotesInArrays/array03/' (')
/DoubleQuotesInArrays/array03/value (one)
/DoubleQuotesInArrays/array03/value (two)
/DoubleQuotesInArrays/array03/value ("foo bar ")
/DoubleQuotesInArrays/array03/' (')
/DoubleQuotesInArrays/array04
/DoubleQuotesInArrays/array04/decl (array04)
/DoubleQuotesInArrays/array04/= (=)
/DoubleQuotesInArrays/array04/' (')
/DoubleQuotesInArrays/array04/value (one)
/DoubleQuotesInArrays/array04/value (two)
/DoubleQuotesInArrays/array04/value ("foo bar;")
/DoubleQuotesInArrays/array04/' (')
/DoubleQuotesInArrays/array05
/DoubleQuotesInArrays/array05/decl (array05)
/DoubleQuotesInArrays/array05/= (=)
/DoubleQuotesInArrays/array05/' (')
/DoubleQuotesInArrays/array05/value (one)
/DoubleQuotesInArrays/array05/value (two)
/DoubleQuotesInArrays/array05/value ("foo bar ")
/DoubleQuotesInArrays/array05/' (')
/DoubleQuotesInArrays/array06
/DoubleQuotesInArrays/array06/decl (array06)
/DoubleQuotesInArrays/array06/= (=)
/DoubleQuotesInArrays/array06/' (')
/DoubleQuotesInArrays/array06/value (one)
/DoubleQuotesInArrays/array06/value (two)
/DoubleQuotesInArrays/array06/value ("foo bar;")
/DoubleQuotesInArrays/array06/' (')
/DoubleQuotesInArrays/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - non-ascii unicode in comments
 */
TEST(HITInterpreter, non_ascii_unicode_in_comments)
{
    std::stringstream input;
    input << R"INPUT(
######################################################################
##                                                                  ##
## Lead off comment with non-ascii unicode degree symbol 600°C      ##
## Lead off comment with non-ascii unicode “L&R double quotes”      ##
## Lead off comment with non-ascii unicode ‘L&R single quotes’      ##
## Lead off comment with non-ascii unicode en dash – character      ##
##                                                                  ##
######################################################################

[ObjName]
  Param01 = val01 # trailing comment with non-ascii unicode degree symbol 600°C
  Param02 = val02 # trailing comment with non-ascii unicode “L&R double quotes”
  Param03 = val03 # trailing comment with non-ascii unicode ‘L&R single quotes’
  Param04 = val04 # trailing comment with non-ascii unicode en dash – character
[]
)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(34, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(9, document.child_count());
    ASSERT_EQ(9, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/comment (######################################################################)
/comment (##                                                                  ##)
/comment (## Lead off comment with non-ascii unicode degree symbol 600°C      ##)
/comment (## Lead off comment with non-ascii unicode “L&R double quotes”      ##)
/comment (## Lead off comment with non-ascii unicode ‘L&R single quotes’      ##)
/comment (## Lead off comment with non-ascii unicode en dash – character      ##)
/comment (##                                                                  ##)
/comment (######################################################################)
/ObjName
/ObjName/[ ([)
/ObjName/decl (ObjName)
/ObjName/] (])
/ObjName/Param01
/ObjName/Param01/decl (Param01)
/ObjName/Param01/= (=)
/ObjName/Param01/value (val01)
/ObjName/comment (# trailing comment with non-ascii unicode degree symbol 600°C)
/ObjName/Param02
/ObjName/Param02/decl (Param02)
/ObjName/Param02/= (=)
/ObjName/Param02/value (val02)
/ObjName/comment (# trailing comment with non-ascii unicode “L&R double quotes”)
/ObjName/Param03
/ObjName/Param03/decl (Param03)
/ObjName/Param03/= (=)
/ObjName/Param03/value (val03)
/ObjName/comment (# trailing comment with non-ascii unicode ‘L&R single quotes’)
/ObjName/Param04
/ObjName/Param04/decl (Param04)
/ObjName/Param04/= (=)
/ObjName/Param04/value (val04)
/ObjName/comment (# trailing comment with non-ascii unicode en dash – character)
/ObjName/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - multi-line and multi-group arrays
 */
TEST(HITInterpreter, multi_line_multi_group_arrays)
{
    std::stringstream input;
    input << R"INPUT(
[Params]

  array01 = '-flag_01 -flag_02 -flag_03   '
             '  -flag_04'
             ' -flag_05
               -flag_06' '-flag_07'

  array02 = '01 02.4 15.6 strain_xx strain_yy strain_zz '
                           '  19.8 max_principal_stress  96 ' ' more 8.3 vals'
            ' group_04 line_03 values '

  array03 = '   vonmises_stress hydrostatic_stress stress_xx stress_yy
       stress_zz '

                           'strain_xx strain_yy strain_zz'
[]
)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(65, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    ASSERT_EQ(1, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/Params
/Params/[ ([)
/Params/decl (Params)
/Params/] (])
/Params/array01
/Params/array01/decl (array01)
/Params/array01/= (=)
/Params/array01/' (')
/Params/array01/value (-flag_01)
/Params/array01/value (-flag_02)
/Params/array01/value (-flag_03)
/Params/array01/' (')
/Params/array01/' (')
/Params/array01/value (-flag_04)
/Params/array01/' (')
/Params/array01/' (')
/Params/array01/value (-flag_05)
/Params/array01/value (-flag_06)
/Params/array01/' (')
/Params/array01/' (')
/Params/array01/value (-flag_07)
/Params/array01/' (')
/Params/array02
/Params/array02/decl (array02)
/Params/array02/= (=)
/Params/array02/' (')
/Params/array02/value (01)
/Params/array02/value (02.4)
/Params/array02/value (15.6)
/Params/array02/value (strain_xx)
/Params/array02/value (strain_yy)
/Params/array02/value (strain_zz)
/Params/array02/' (')
/Params/array02/' (')
/Params/array02/value (19.8)
/Params/array02/value (max_principal_stress)
/Params/array02/value (96)
/Params/array02/' (')
/Params/array02/' (')
/Params/array02/value (more)
/Params/array02/value (8.3)
/Params/array02/value (vals)
/Params/array02/' (')
/Params/array02/' (')
/Params/array02/value (group_04)
/Params/array02/value (line_03)
/Params/array02/value (values)
/Params/array02/' (')
/Params/array03
/Params/array03/decl (array03)
/Params/array03/= (=)
/Params/array03/' (')
/Params/array03/value (vonmises_stress)
/Params/array03/value (hydrostatic_stress)
/Params/array03/value (stress_xx)
/Params/array03/value (stress_yy)
/Params/array03/value (stress_zz)
/Params/array03/' (')
/Params/array03/' (')
/Params/array03/value (strain_xx)
/Params/array03/value (strain_yy)
/Params/array03/value (strain_zz)
/Params/array03/' (')
/Params/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - equals and double quotes after slash in value
 */
TEST(HITInterpreter, equals_and_quotes_after_slash)
{
    std::stringstream input;
    input << R"INPUT(
[SlashesInValues]
  [input01]
    directory = some/directory/path01
    inputfile = file01.i
    parameter = path/to/a/param01=one
  []
  [input02]
    directory = some/directory/path02/
    inputfile = file02.i
    parameter = path/to/a/param02/="two"
  []
  [input03]
    directory = /some/directory/path03
    inputfile = file03.i
    parameter = /path/to/a/param03=3.3
  []
  [input04]
    directory = /some/directory/path04/
    inputfile = file04.i
    parameter = /path/to/a/param04/=444
  []
[]
)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(74, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    ASSERT_EQ(1, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/SlashesInValues
/SlashesInValues/[ ([)
/SlashesInValues/decl (SlashesInValues)
/SlashesInValues/] (])
/SlashesInValues/input01
/SlashesInValues/input01/[ ([)
/SlashesInValues/input01/decl (input01)
/SlashesInValues/input01/] (])
/SlashesInValues/input01/directory
/SlashesInValues/input01/directory/decl (directory)
/SlashesInValues/input01/directory/= (=)
/SlashesInValues/input01/directory/value (some/directory/path01)
/SlashesInValues/input01/inputfile
/SlashesInValues/input01/inputfile/decl (inputfile)
/SlashesInValues/input01/inputfile/= (=)
/SlashesInValues/input01/inputfile/value (file01.i)
/SlashesInValues/input01/parameter
/SlashesInValues/input01/parameter/decl (parameter)
/SlashesInValues/input01/parameter/= (=)
/SlashesInValues/input01/parameter/value (path/to/a/param01=one)
/SlashesInValues/input01/term ([])
/SlashesInValues/input02
/SlashesInValues/input02/[ ([)
/SlashesInValues/input02/decl (input02)
/SlashesInValues/input02/] (])
/SlashesInValues/input02/directory
/SlashesInValues/input02/directory/decl (directory)
/SlashesInValues/input02/directory/= (=)
/SlashesInValues/input02/directory/value (some/directory/path02/)
/SlashesInValues/input02/inputfile
/SlashesInValues/input02/inputfile/decl (inputfile)
/SlashesInValues/input02/inputfile/= (=)
/SlashesInValues/input02/inputfile/value (file02.i)
/SlashesInValues/input02/parameter
/SlashesInValues/input02/parameter/decl (parameter)
/SlashesInValues/input02/parameter/= (=)
/SlashesInValues/input02/parameter/value (path/to/a/param02/="two")
/SlashesInValues/input02/term ([])
/SlashesInValues/input03
/SlashesInValues/input03/[ ([)
/SlashesInValues/input03/decl (input03)
/SlashesInValues/input03/] (])
/SlashesInValues/input03/directory
/SlashesInValues/input03/directory/decl (directory)
/SlashesInValues/input03/directory/= (=)
/SlashesInValues/input03/directory/value (/some/directory/path03)
/SlashesInValues/input03/inputfile
/SlashesInValues/input03/inputfile/decl (inputfile)
/SlashesInValues/input03/inputfile/= (=)
/SlashesInValues/input03/inputfile/value (file03.i)
/SlashesInValues/input03/parameter
/SlashesInValues/input03/parameter/decl (parameter)
/SlashesInValues/input03/parameter/= (=)
/SlashesInValues/input03/parameter/value (/path/to/a/param03=3.3)
/SlashesInValues/input03/term ([])
/SlashesInValues/input04
/SlashesInValues/input04/[ ([)
/SlashesInValues/input04/decl (input04)
/SlashesInValues/input04/] (])
/SlashesInValues/input04/directory
/SlashesInValues/input04/directory/decl (directory)
/SlashesInValues/input04/directory/= (=)
/SlashesInValues/input04/directory/value (/some/directory/path04/)
/SlashesInValues/input04/inputfile
/SlashesInValues/input04/inputfile/decl (inputfile)
/SlashesInValues/input04/inputfile/= (=)
/SlashesInValues/input04/inputfile/value (file04.i)
/SlashesInValues/input04/parameter
/SlashesInValues/input04/parameter/decl (parameter)
/SlashesInValues/input04/parameter/= (=)
/SlashesInValues/input04/parameter/value (/path/to/a/param04/=444)
/SlashesInValues/input04/term ([])
/SlashesInValues/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - shorthand path naming for regular and type blocks
 */
TEST(HITInterpreter, shorthand_path_naming_blocks)
{
    std::stringstream input;
    input << R"INPUT(

[a/b/c]
  param_outer = bar_outer
  [x/y/z]
    param_inner = bar_inner
  []
[]

[a/b/c]
  type        = foo_outer
  param_outer = bar_outer
  [x/y/z]
    type        = foo_inner
    param_inner = bar_inner
  []
[]

[/a/b///c//]
  param = bar
[]

)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(64, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(3, document.child_count());
    ASSERT_EQ(3, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths;
#if DISABLE_HIT_TYPE_PROMOTION
    expected_paths = R"INPUT(/
/a
/a/b
/a/b/c
/a/b/c/[ ([)
/a/b/c/decl (a/b/c)
/a/b/c/] (])
/a/b/c/param_outer
/a/b/c/param_outer/decl (param_outer)
/a/b/c/param_outer/= (=)
/a/b/c/param_outer/value (bar_outer)
/a/b/c/x
/a/b/c/x/y
/a/b/c/x/y/z
/a/b/c/x/y/z/[ ([)
/a/b/c/x/y/z/decl (x/y/z)
/a/b/c/x/y/z/] (])
/a/b/c/x/y/z/param_inner
/a/b/c/x/y/z/param_inner/decl (param_inner)
/a/b/c/x/y/z/param_inner/= (=)
/a/b/c/x/y/z/param_inner/value (bar_inner)
/a/b/c/x/y/z/term ([])
/a/b/c/term ([])
/a
/a/b
/a/b/c
/a/b/c/[ ([)
/a/b/c/decl (a/b/c)
/a/b/c/] (])
/a/b/c/type
/a/b/c/type/decl (type)
/a/b/c/type/= (=)
/a/b/c/type/value (foo_outer)
/a/b/c/param_outer
/a/b/c/param_outer/decl (param_outer)
/a/b/c/param_outer/= (=)
/a/b/c/param_outer/value (bar_outer)
/a/b/c/x
/a/b/c/x/y
/a/b/c/x/y/z
/a/b/c/x/y/z/[ ([)
/a/b/c/x/y/z/decl (x/y/z)
/a/b/c/x/y/z/] (])
/a/b/c/x/y/z/type
/a/b/c/x/y/z/type/decl (type)
/a/b/c/x/y/z/type/= (=)
/a/b/c/x/y/z/type/value (foo_inner)
/a/b/c/x/y/z/param_inner
/a/b/c/x/y/z/param_inner/decl (param_inner)
/a/b/c/x/y/z/param_inner/= (=)
/a/b/c/x/y/z/param_inner/value (bar_inner)
/a/b/c/x/y/z/term ([])
/a/b/c/term ([])
/a
/a/b
/a/b/c
/a/b/c/[ ([)
/a/b/c/decl (/a/b///c//)
/a/b/c/] (])
/a/b/c/param
/a/b/c/param/decl (param)
/a/b/c/param/= (=)
/a/b/c/param/value (bar)
/a/b/c/term ([])
)INPUT";
#else
    expected_paths = R"INPUT(/
/a
/a/b
/a/b/c
/a/b/c/[ ([)
/a/b/c/decl (a/b/c)
/a/b/c/] (])
/a/b/c/param_outer
/a/b/c/param_outer/decl (param_outer)
/a/b/c/param_outer/= (=)
/a/b/c/param_outer/value (bar_outer)
/a/b/c/x
/a/b/c/x/y
/a/b/c/x/y/z
/a/b/c/x/y/z/[ ([)
/a/b/c/x/y/z/decl (x/y/z)
/a/b/c/x/y/z/] (])
/a/b/c/x/y/z/param_inner
/a/b/c/x/y/z/param_inner/decl (param_inner)
/a/b/c/x/y/z/param_inner/= (=)
/a/b/c/x/y/z/param_inner/value (bar_inner)
/a/b/c/x/y/z/term ([])
/a/b/c/term ([])
/a
/a/b
/a/b/foo_outer_type
/a/b/foo_outer_type/[ ([)
/a/b/foo_outer_type/decl (a/b/c)
/a/b/foo_outer_type/] (])
/a/b/foo_outer_type/type
/a/b/foo_outer_type/type/decl (type)
/a/b/foo_outer_type/type/= (=)
/a/b/foo_outer_type/type/value (foo_outer)
/a/b/foo_outer_type/param_outer
/a/b/foo_outer_type/param_outer/decl (param_outer)
/a/b/foo_outer_type/param_outer/= (=)
/a/b/foo_outer_type/param_outer/value (bar_outer)
/a/b/foo_outer_type/x
/a/b/foo_outer_type/x/y
/a/b/foo_outer_type/x/y/foo_inner_type
/a/b/foo_outer_type/x/y/foo_inner_type/[ ([)
/a/b/foo_outer_type/x/y/foo_inner_type/decl (x/y/z)
/a/b/foo_outer_type/x/y/foo_inner_type/] (])
/a/b/foo_outer_type/x/y/foo_inner_type/type
/a/b/foo_outer_type/x/y/foo_inner_type/type/decl (type)
/a/b/foo_outer_type/x/y/foo_inner_type/type/= (=)
/a/b/foo_outer_type/x/y/foo_inner_type/type/value (foo_inner)
/a/b/foo_outer_type/x/y/foo_inner_type/param_inner
/a/b/foo_outer_type/x/y/foo_inner_type/param_inner/decl (param_inner)
/a/b/foo_outer_type/x/y/foo_inner_type/param_inner/= (=)
/a/b/foo_outer_type/x/y/foo_inner_type/param_inner/value (bar_inner)
/a/b/foo_outer_type/x/y/foo_inner_type/term ([])
/a/b/foo_outer_type/term ([])
/a
/a/b
/a/b/c
/a/b/c/[ ([)
/a/b/c/decl (/a/b///c//)
/a/b/c/] (])
/a/b/c/param
/a/b/c/param/decl (param)
/a/b/c/param/= (=)
/a/b/c/param/value (bar)
/a/b/c/term ([])
)INPUT";
#endif

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - shorthand path naming keyed value parameters
 */
TEST(HITInterpreter, shorthand_path_naming_keyed_values)
{
    std::stringstream input;
    input << R"INPUT(

path/to/param/inner/01 = "one"

[//path/to///object/]

  ///path/to///param/inner/02// = 2.2

[]

)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(24, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(2, document.child_count());
    ASSERT_EQ(2, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/path
/path/to
/path/to/param
/path/to/param/inner
/path/to/param/inner/01
/path/to/param/inner/01/decl (path/to/param/inner/01)
/path/to/param/inner/01/= (=)
/path/to/param/inner/01/value ("one")
/path
/path/to
/path/to/object
/path/to/object/[ ([)
/path/to/object/decl (//path/to///object/)
/path/to/object/] (])
/path/to/object/path
/path/to/object/path/to
/path/to/object/path/to/param
/path/to/object/path/to/param/inner
/path/to/object/path/to/param/inner/02
/path/to/object/path/to/param/inner/02/decl (///path/to///param/inner/02//)
/path/to/object/path/to/param/inner/02/= (=)
/path/to/object/path/to/param/inner/02/value (2.2)
/path/to/object/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - shorthand path naming keyed array parameters
 */
TEST(HITInterpreter, shorthand_path_naming_keyed_arrays)
{
    std::stringstream input;
    input << R"INPUT(

path/to/array/inner/01 = '"eleven" "twelve" "thirteen"'

[//path/to///object/]

  ///path/to///array/inner/02// = '2.2 3.3 4.4'

[]

)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(32, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(2, document.child_count());
    ASSERT_EQ(2, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/path
/path/to
/path/to/array
/path/to/array/inner
/path/to/array/inner/01
/path/to/array/inner/01/decl (path/to/array/inner/01)
/path/to/array/inner/01/= (=)
/path/to/array/inner/01/' (')
/path/to/array/inner/01/value ("eleven")
/path/to/array/inner/01/value ("twelve")
/path/to/array/inner/01/value ("thirteen")
/path/to/array/inner/01/' (')
/path
/path/to
/path/to/object
/path/to/object/[ ([)
/path/to/object/decl (//path/to///object/)
/path/to/object/] (])
/path/to/object/path
/path/to/object/path/to
/path/to/object/path/to/array
/path/to/object/path/to/array/inner
/path/to/object/path/to/array/inner/02
/path/to/object/path/to/array/inner/02/decl (///path/to///array/inner/02//)
/path/to/object/path/to/array/inner/02/= (=)
/path/to/object/path/to/array/inner/02/' (')
/path/to/object/path/to/array/inner/02/value (2.2)
/path/to/object/path/to/array/inner/02/value (3.3)
/path/to/object/path/to/array/inner/02/value (4.4)
/path/to/object/path/to/array/inner/02/' (')
/path/to/object/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - keyed-keyed-value parameters
 */
TEST(HITInterpreter, keyed_keyed_values)
{
    std::stringstream input;
    input << R"INPUT(

param_outer_01 = path/to/param/inner/01="one"

[//path/to///object/]

  param_outer_02=///path/to///param/inner/02//=2.2

[]

)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(16, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(2, document.child_count());
    ASSERT_EQ(2, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/param_outer_01
/param_outer_01/decl (param_outer_01)
/param_outer_01/= (=)
/param_outer_01/value (path/to/param/inner/01="one")
/path
/path/to
/path/to/object
/path/to/object/[ ([)
/path/to/object/decl (//path/to///object/)
/path/to/object/] (])
/path/to/object/param_outer_02
/path/to/object/param_outer_02/decl (param_outer_02)
/path/to/object/param_outer_02/= (=)
/path/to/object/param_outer_02/value (///path/to///param/inner/02//=2.2)
/path/to/object/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - keyed-keyed-array parameters
 */
TEST(HITInterpreter, keyed_keyed_arrays)
{
    std::stringstream input;
    input << R"INPUT(

array_outer_01 = path/to/array/inner/01='"eleven";"twelve";"thirteen"'

[//path/to///object/]

  array_outer_02 = ///path/to///array/inner/02//='2.2;3.3;4.4'

[]

)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(16, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(2, document.child_count());
    ASSERT_EQ(2, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/array_outer_01
/array_outer_01/decl (array_outer_01)
/array_outer_01/= (=)
/array_outer_01/value (path/to/array/inner/01='"eleven";"twelve";"thirteen"')
/path
/path/to
/path/to/object
/path/to/object/[ ([)
/path/to/object/decl (//path/to///object/)
/path/to/object/] (])
/path/to/object/array_outer_02
/path/to/object/array_outer_02/decl (array_outer_02)
/path/to/object/array_outer_02/= (=)
/path/to/object/array_outer_02/value (///path/to///array/inner/02//='2.2;3.3;4.4')
/path/to/object/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - shorthand path naming keyed-keyed-value parameters
 */
TEST(HITInterpreter, shorthand_path_naming_keyed_keyed_values)
{
    std::stringstream input;
    input << R"INPUT(

path/to/param/outer/01 = path/to/param/inner/01="one"

[//path/to///object/]

  ///path/to///param/outer/02// = ///path/to///param/inner/02//=2.2

[]

)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(24, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(2, document.child_count());
    ASSERT_EQ(2, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/path
/path/to
/path/to/param
/path/to/param/outer
/path/to/param/outer/01
/path/to/param/outer/01/decl (path/to/param/outer/01)
/path/to/param/outer/01/= (=)
/path/to/param/outer/01/value (path/to/param/inner/01="one")
/path
/path/to
/path/to/object
/path/to/object/[ ([)
/path/to/object/decl (//path/to///object/)
/path/to/object/] (])
/path/to/object/path
/path/to/object/path/to
/path/to/object/path/to/param
/path/to/object/path/to/param/outer
/path/to/object/path/to/param/outer/02
/path/to/object/path/to/param/outer/02/decl (///path/to///param/outer/02//)
/path/to/object/path/to/param/outer/02/= (=)
/path/to/object/path/to/param/outer/02/value (///path/to///param/inner/02//=2.2)
/path/to/object/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - shorthand path naming keyed-keyed-array parameters
 */
TEST(HITInterpreter, shorthand_path_naming_keyed_keyed_arrays)
{
    std::stringstream input;
    input << R"INPUT(

path/to/array/outer/01 = path/to/array/inner/01='"eleven";"twelve";"thirteen"'

[//path/to///object/]

  ///path/to///array/outer/02// = ///path/to///array/inner/02//='2.2;3.3;4.4'

[]

)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(24, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(2, document.child_count());
    ASSERT_EQ(2, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/path
/path/to
/path/to/array
/path/to/array/outer
/path/to/array/outer/01
/path/to/array/outer/01/decl (path/to/array/outer/01)
/path/to/array/outer/01/= (=)
/path/to/array/outer/01/value (path/to/array/inner/01='"eleven";"twelve";"thirteen"')
/path
/path/to
/path/to/object
/path/to/object/[ ([)
/path/to/object/decl (//path/to///object/)
/path/to/object/] (])
/path/to/object/path
/path/to/object/path/to
/path/to/object/path/to/array
/path/to/object/path/to/array/outer
/path/to/object/path/to/array/outer/02
/path/to/object/path/to/array/outer/02/decl (///path/to///array/outer/02//)
/path/to/object/path/to/array/outer/02/= (=)
/path/to/object/path/to/array/outer/02/value (///path/to///array/inner/02//='2.2;3.3;4.4')
/path/to/object/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - array values that are keyed values
 */
TEST(HITInterpreter, arrays_containing_keyed_values)
{
    std::stringstream input;
    input << R"INPUT(

array_outer_01 = 'param_inner_01 = 100 param_inner_02 = 200 param_inner_03 = 300'

[//path/to///object/]

  path/to/array//outer/02/ = 'param/inner/01 = 100 param/inner/02 = 200 param/inner/03 = 300'

[]

///path//to/array/outer/03 = array_outer_01='param_inner_01=100;param_inner_02=200;param_inner_03=300'

[//path/to///object/]

  path/to/array/outer//04 = path/to/array//outer/02/='param/inner/01=100;param/inner/02=200;param/inner/03=300'

[]

)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(63, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(4, document.child_count());
    ASSERT_EQ(4, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/array_outer_01
/array_outer_01/decl (array_outer_01)
/array_outer_01/= (=)
/array_outer_01/' (')
/array_outer_01/value (param_inner_01)
/array_outer_01/value (=)
/array_outer_01/value (100)
/array_outer_01/value (param_inner_02)
/array_outer_01/value (=)
/array_outer_01/value (200)
/array_outer_01/value (param_inner_03)
/array_outer_01/value (=)
/array_outer_01/value (300)
/array_outer_01/' (')
/path
/path/to
/path/to/object
/path/to/object/[ ([)
/path/to/object/decl (//path/to///object/)
/path/to/object/] (])
/path/to/object/path
/path/to/object/path/to
/path/to/object/path/to/array
/path/to/object/path/to/array/outer
/path/to/object/path/to/array/outer/02
/path/to/object/path/to/array/outer/02/decl (path/to/array//outer/02/)
/path/to/object/path/to/array/outer/02/= (=)
/path/to/object/path/to/array/outer/02/' (')
/path/to/object/path/to/array/outer/02/value (param/inner/01)
/path/to/object/path/to/array/outer/02/value (=)
/path/to/object/path/to/array/outer/02/value (100)
/path/to/object/path/to/array/outer/02/value (param/inner/02)
/path/to/object/path/to/array/outer/02/value (=)
/path/to/object/path/to/array/outer/02/value (200)
/path/to/object/path/to/array/outer/02/value (param/inner/03)
/path/to/object/path/to/array/outer/02/value (=)
/path/to/object/path/to/array/outer/02/value (300)
/path/to/object/path/to/array/outer/02/' (')
/path/to/object/term ([])
/path
/path/to
/path/to/array
/path/to/array/outer
/path/to/array/outer/03
/path/to/array/outer/03/decl (///path//to/array/outer/03)
/path/to/array/outer/03/= (=)
/path/to/array/outer/03/value (array_outer_01='param_inner_01=100;param_inner_02=200;param_inner_03=300')
/path
/path/to
/path/to/object
/path/to/object/[ ([)
/path/to/object/decl (//path/to///object/)
/path/to/object/] (])
/path/to/object/path
/path/to/object/path/to
/path/to/object/path/to/array
/path/to/object/path/to/array/outer
/path/to/object/path/to/array/outer/04
/path/to/object/path/to/array/outer/04/decl (path/to/array/outer//04)
/path/to/object/path/to/array/outer/04/= (=)
/path/to/object/path/to/array/outer/04/value (path/to/array//outer/02/='param/inner/01=100;param/inner/02=200;param/inner/03=300')
/path/to/object/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief standalone file include not found error
 */
TEST(HITInterpreter, only_include_not_found)
{
    std::stringstream input;
    input << "\n\n\n      " << R"I(!include block_missing.i)I" << std::endl;
    std::stringstream errors;
    DefaultHITInterpreter interpreter(errors);
    ASSERT_FALSE(interpreter.parse(input));

    std::stringstream expected_errors;
    expected_errors << "stream input:4.7: could not find 'block_missing.i'" << std::endl;

    ASSERT_EQ(expected_errors.str(), errors.str());
    ASSERT_EQ(expected_errors.str(), interpreter.error_diagnostics().front().str());
    ASSERT_DIAGNOSTICS(interpreter, expected_errors);
    // Double check the contents

    // check that pieces of diagnostic object are values expected for error
    const auto diagnostic_error = interpreter.error_diagnostics().front();
    ASSERT_EQ("stream input", diagnostic_error.filename());
    ASSERT_EQ(4, diagnostic_error.start_line());
    ASSERT_EQ(7, diagnostic_error.start_column());
    ASSERT_EQ(4, diagnostic_error.end_line());
    ASSERT_EQ(7, diagnostic_error.end_column());
    ASSERT_EQ("could not find 'block_missing.i'", diagnostic_error.message());
}

/**
 * @brief standalone file include
 */
TEST(HITInterpreter, only_include)
{
    { // Scope for file buffer to be flushed before reading
    std::ofstream block_file("block.i");
    block_file << "[block] " <<std::endl
               << "  key = 3"<<std::endl
               << "[]"<<std::endl;
    block_file.close();
    }

    std::stringstream input;
    input << R"I(!include block.i)I" << std::endl;

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::string expected_paths = R"INPUT(/
/incl
/incl/decl (!include)
/incl/path (block.i)
)INPUT";

    std::stringstream actual_paths;
    interpreter.root().paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief file include within a block
 */
TEST(HITInterpreter, include_block)
{
    { // Scope for file buffer to be flushed before reading
    std::ofstream block_file("nested_block.i");
    block_file << "[nested_block] " <<std::endl
               << "  key = 3"<<std::endl
               << "[]"<<std::endl;
    block_file.close();
    }

    std::stringstream input;
    input << R"I([block]!include nested_block.i[])I" << std::endl;

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::string expected_paths = R"INPUT(/
/block
/block/[ ([)
/block/decl (block)
/block/] (])
/block/incl
/block/incl/decl (!include)
/block/incl/path (nested_block.i)
/block/term ([])
)INPUT";

    std::stringstream actual_paths;
    interpreter.root().paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());

    // Obtain the nested content
    HITNodeView document = interpreter.root();
    HITNodeView block = document.child_at(0);
    ASSERT_EQ(std::string("block"), block.name());
    HITNodeView::Collection children = block.non_decorative_children();
    ASSERT_EQ(1, children.size());
    ASSERT_EQ(std::string("nested_block"), children[0].name());

    // Ensure the path produces from a nested document's node includes
    // the context of the parent document's nodes
    auto key_node = children[0].first_child_by_name("key");
    ASSERT_FALSE(key_node.is_null());
    ASSERT_EQ("/block/nested_block/key", key_node.path());

    // Ensure the 'first_child_by_name' properly traverses file boundaries
    auto nested_block = block.first_child_by_name("nested_block");
    ASSERT_EQ(children[0], nested_block);
    ASSERT_EQ(children[0], block.child_by_name("nested_block")[0]);
}


/**
 * @brief Test FilePush iterator when iterating
 * Should iterate over variables a,b,c, where b
 * is actually obtained from an included file
 */
TEST(HITInterpreter, iterating_include)
{
    { // Scope for file buffer to be flushed before reading
    std::ofstream block_file("b.i");
    block_file << "b = 2" <<std::endl;
    block_file.close();
    }

    std::stringstream input;
    input << R"I(a = 1
!include b.i
c = 3)I" << std::endl;

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));

    // Obtain the nested content
    HITNodeView document = interpreter.root();

    std::vector<std::string> expected_names = {"a", "b", "c"};
    std::vector<std::string> expected_data = {"1", "2", "3"};
    std::vector<std::string> expected_paths = {"stream input", "./b.i", "stream input"};
    size_t index = 0;
    for (const auto & node : document)
    {
        SCOPED_TRACE(index);
        ASSERT_EQ(expected_names[index], node.name());
        ASSERT_EQ(expected_data[index], node.to_string());
        ASSERT_EQ(expected_paths[index], node.node_pool()->stream_name());
        ++index;
    }

}

/**
 * @brief is_nested_file - function that checks for node being file include
 */
TEST(HITInterpreter, is_nested_file)
{
   std::stringstream input_base;
   input_base << R"INPUT(
param_01 = 10
!include input_incl.i
param_03 = 30
)INPUT";

   std::ofstream input_incl("input_incl.i");
   input_incl << R"INPUT(
# if is_nested_file() is not checked in findChild the '=' below is returned
param_02 = 20
)INPUT";
   input_incl.close();

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parseStream(input_base, "input_base"));
    HITNodeView document = interpreter.root();
    ASSERT_EQ(3, document.child_count());

    HITNodeView child_01 = document.child_at(0); // param_01 = 10
    HITNodeView child_02 = document.child_at(1); // !include input_incl.i
    HITNodeView child_03 = document.child_at(2); // param_03 = 30

    // verify convenience function reports that param_01 is not nested file
    ASSERT_FALSE(child_01.is_null());
    ASSERT_EQ(std::string("param_01"), child_01.name());
    ASSERT_FALSE(wasp::is_nested_file(child_01));

    // verify convenience function reports that param_02 is an include file
    ASSERT_FALSE(child_02.is_null());
    ASSERT_EQ(std::string("incl"), child_02.name());
    ASSERT_TRUE(wasp::is_nested_file(child_02));

    // verify convenience function reports that param_03 is not nested file
    ASSERT_FALSE(child_03.is_null());
    ASSERT_EQ(std::string("param_03"), child_03.name());
    ASSERT_FALSE(wasp::is_nested_file(child_03));

    // verify param_03 still reports not nested after changing type to file
    ASSERT_EQ(wasp::KEYED_VALUE, child_03.type());
    child_03.set_type(wasp::FILE);
    ASSERT_EQ(wasp::FILE, child_03.type());
    ASSERT_FALSE(wasp::is_nested_file(child_03));

    // verify is_nested_file works as expected with findNodeUnderLineColumn
    HITNodeView found_node = wasp::findNodeUnderLineColumn(document, 3, 10);
    ASSERT_FALSE(found_node.is_null());
    ASSERT_EQ(std::string("incl"), found_node.name());
    ASSERT_TRUE(wasp::is_nested_file(found_node));
    ASSERT_EQ("input_base", found_node.node_pool()->stream_name());
    ASSERT_EQ(3, found_node.line());
    ASSERT_EQ(1, found_node.column());
    ASSERT_EQ(3, found_node.last_line());
    ASSERT_EQ(21, found_node.last_column());
    ASSERT_EQ("!include input_incl.i", found_node.to_string());
    ASSERT_EQ("!include input_incl.i", found_node.data());
    ASSERT_EQ(child_02, found_node);
}

/**
 * @brief Test HIT syntax error - file include loop creates circular reference
 */
TEST(HITInterpreter, file_include_circular_loop)
{
    std::stringstream input01;
    input01 << R"INPUT(
[Block01]
  !include input02.i
[]
)INPUT";

    std::ofstream input02("input02.i");
    input02 << R"INPUT(
[Block02]
  !include input03.i
[]
)INPUT";
    input02.close();

    std::ofstream input03("input03.i");
    input03 << R"INPUT(
[Block03]
  !include input02.i
[]
)INPUT";
    input03.close();

    std::stringstream actual_errors;
    DefaultHITInterpreter interpreter(actual_errors);
    ASSERT_FALSE(interpreter.parse(input01));

    std::stringstream expected_errors;
    expected_errors << "./input03.i:3.3: file include would create circular reference 'input02.i'"
                  << std::endl;

    ASSERT_EQ(expected_errors.str(), actual_errors.str());
    ASSERT_DIAGNOSTICS(interpreter, expected_errors);
    std::string expected_paths;
    expected_paths = R"INPUT(/
/Block01
/Block01/[ ([)
/Block01/decl (Block01)
/Block01/] (])
/Block01/incl
/Block01/incl/decl (!include)
/Block01/incl/path (input02.i)
/Block01/term ([])
)INPUT";
    std::stringstream actual_paths;
    interpreter.root().paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax error - file include self creates circular reference
 */
TEST(HITInterpreter, file_include_circular_self)
{
    std::stringstream input01;
    input01 << R"INPUT(
[Block01]
  !include input02.i
[]
)INPUT";

    std::ofstream input02("input02.i");
    input02 << R"INPUT(
[Block02]
  !include input02.i
[]
)INPUT";
    input02.close();

    std::stringstream actual_errors;
    DefaultHITInterpreter interpreter(actual_errors);
    ASSERT_FALSE(interpreter.parse(input01));

    std::stringstream expected_errors;
    expected_errors << "./input02.i:3.3: file include would create circular reference 'input02.i'"
                  << std::endl;
    ASSERT_EQ(expected_errors.str(), actual_errors.str());
    ASSERT_DIAGNOSTICS(interpreter, expected_errors);

    std::string expected_paths;
    expected_paths = R"INPUT(/
/Block01
/Block01/[ ([)
/Block01/decl (Block01)
/Block01/] (])
/Block01/incl
/Block01/incl/decl (!include)
/Block01/incl/path (input02.i)
/Block01/term ([])
)INPUT";
    std::stringstream actual_paths;
    interpreter.root().paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax error - missing object terminators
 */
TEST(HITInterpreter, missing_object_terminator)
{
    std::stringstream input;
    input << R"INPUT([GlobalParams]
[one]
  [one_sub]
    one_sub_param=100
[]
[two]
  two_param=200
[]
)INPUT";

    std::stringstream actual_errors;
    DefaultHITInterpreter interpreter(actual_errors);
    ASSERT_FALSE(interpreter.parse(input));

    std::stringstream expected_errors;
    expected_errors << "stream input:2.1: syntax error, unexpected end of file, expecting block terminator" << std::endl
                  << "stream input:1.1: syntax error, unexpected end of file, expecting block terminator" << std::endl;

    ASSERT_EQ(expected_errors.str(), actual_errors.str());
    ASSERT_DIAGNOSTICS(interpreter, expected_errors);
    std::string expected_paths;
    expected_paths = R"INPUT(/
/GlobalParams
/GlobalParams/[ ([)
/GlobalParams/decl (GlobalParams)
/GlobalParams/] (])
/GlobalParams/one
/GlobalParams/one/[ ([)
/GlobalParams/one/decl (one)
/GlobalParams/one/] (])
/GlobalParams/one/one_sub
/GlobalParams/one/one_sub/[ ([)
/GlobalParams/one/one_sub/decl (one_sub)
/GlobalParams/one/one_sub/] (])
/GlobalParams/one/one_sub/one_sub_param
/GlobalParams/one/one_sub/one_sub_param/decl (one_sub_param)
/GlobalParams/one/one_sub/one_sub_param/= (=)
/GlobalParams/one/one_sub/one_sub_param/value (100)
/GlobalParams/one/one_sub/term ([])
/GlobalParams/one/two
/GlobalParams/one/two/[ ([)
/GlobalParams/one/two/decl (two)
/GlobalParams/one/two/] (])
/GlobalParams/one/two/two_param
/GlobalParams/one/two/two_param/decl (two_param)
/GlobalParams/one/two/two_param/= (=)
/GlobalParams/one/two/two_param/value (200)
/GlobalParams/one/two/term ([])
)INPUT"; // TODO 
    std::stringstream actual_paths;
    interpreter.root().paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax error - object name with invalid character '#'
 */
TEST(HITInterpreter, object_name_with_invalid_pound)
{
    std::stringstream input;
    input << R"INPUT(
[#objectname] []
)INPUT";

    std::stringstream actual_errors;
    DefaultHITInterpreter interpreter(actual_errors);
    ASSERT_FALSE(interpreter.parse(input));

    std::stringstream expected_errors;
    expected_errors << "stream input:2.2: syntax error, unexpected invalid token"
                  << std::endl;

    ASSERT_EQ(expected_errors.str(), actual_errors.str());
    ASSERT_DIAGNOSTICS(interpreter, expected_errors);
}

/**
 * @brief Test HIT syntax error - object name with invalid character '='
 */
TEST(HITInterpreter, object_name_with_invalid_equals)
{
    std::stringstream input;
    input << R"INPUT(
[object=name] []
)INPUT";

    std::stringstream actual_errors;
    DefaultHITInterpreter interpreter(actual_errors);
    ASSERT_FALSE(interpreter.parse(input));

    std::stringstream expected_errors;
    expected_errors << "stream input:2.8: syntax error, unexpected invalid token, expecting ]"
                  << std::endl;

        std::string expect_paths_and_types = R"(
/                  )" + std::to_string(wasp::DOCUMENT_ROOT) + R"(
/object           )"  + std::to_string(wasp::OBJECT)        + R"(
/object/[         )"  + std::to_string(wasp::LBRACKET)      + R"( ([)
/object/decl       )" + std::to_string(wasp::DECL)          + R"( (object)
/object/]         )"  + std::to_string(wasp::RBRACKET)      + R"( (])
/object/term      )"  + std::to_string(wasp::OBJECT_TERM)   + R"( ([])
)";

    // Check parse failure, error message, non-null root, paths, and types
    std::stringstream actual_paths_and_types;
    ASSERT_EQ(expected_errors.str(), actual_errors.str());
    ASSERT_DIAGNOSTICS(interpreter, expected_errors);
    ASSERT_FALSE(interpreter.root().is_null());
    wasp::node_paths_and_types(interpreter.root(), actual_paths_and_types);
    ASSERT_EQ(expect_paths_and_types, "\n" + actual_paths_and_types.str());
}

/**
 * @brief Test HIT syntax error - object name with invalid character '&'
 */
TEST(HITInterpreter, object_name_with_invalid_ampersand)
{
    std::stringstream input;
    input << R"INPUT(
[objectname&] []
)INPUT";

    std::stringstream actual_errors;
    DefaultHITInterpreter interpreter(actual_errors);
    ASSERT_FALSE(interpreter.parse(input));

    std::stringstream expected_errors;
    expected_errors << "stream input:2.12: syntax error, unexpected invalid token, expecting ]"
                  << std::endl;

    std::string expect_paths_and_types = R"(
/                  )" + std::to_string(wasp::DOCUMENT_ROOT) + R"(
/objectname       )"  + std::to_string(wasp::OBJECT)        + R"(
/objectname/[     )"  + std::to_string(wasp::LBRACKET)      + R"( ([)
/objectname/decl   )" + std::to_string(wasp::DECL)          + R"( (objectname)
/objectname/]     )"  + std::to_string(wasp::RBRACKET)      + R"( (])
/objectname/term  )"  + std::to_string(wasp::OBJECT_TERM)   + R"( ([])
)";

    // Check parse failure, error message, non-null root, paths, and types
    std::stringstream actual_paths_and_types;
    ASSERT_EQ(expected_errors.str(), actual_errors.str());
    ASSERT_DIAGNOSTICS(interpreter, expected_errors);
    ASSERT_FALSE(interpreter.root().is_null());
    wasp::node_paths_and_types(interpreter.root(), actual_paths_and_types);
    ASSERT_EQ(expect_paths_and_types, "\n" + actual_paths_and_types.str());
}

/**
 * @brief Test HIT syntax error - parameter name with invalid character '&'
 */
TEST(HITInterpreter, parameter_name_with_invalid_ampersand)
{
    std::stringstream input;
    input << R"INPUT(
param&name=10
)INPUT";

    std::stringstream actual_errors;
    DefaultHITInterpreter interpreter(actual_errors);
    ASSERT_FALSE(interpreter.parse(input));

    std::stringstream expected_errors;
    expected_errors << "stream input:2.6: syntax error, unexpected invalid token, expecting = or :="
                  << std::endl;

    ASSERT_EQ(expected_errors.str(), actual_errors.str());
    ASSERT_DIAGNOSTICS(interpreter, expected_errors);
}

/**
 * @brief Test HIT syntax error - invalid object terminator
 */
TEST(HITInterpreter, invalid_object_terminator)
{
    std::stringstream input;
    input << R"INPUT(
[object_name][./]
)INPUT";

    std::stringstream actual_errors;
    DefaultHITInterpreter interpreter(actual_errors);
    ASSERT_FALSE(interpreter.parse(input));

    std::stringstream expected_errors;
    expected_errors << "stream input:2.17: syntax error, unexpected invalid token, expecting integer or object name"
                  << std::endl;

    ASSERT_EQ(expected_errors.str(), actual_errors.str());
    ASSERT_DIAGNOSTICS(interpreter, expected_errors);
}

/**
 * @brief Test HIT syntax - brace expression spans multiple lines
 */
TEST(HITInterpreter, brace_expression_spans_multiple_lines)
{
    std::stringstream input;
    input << R"INPUT(
param_name=${raw 4
                 2
            }
)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(5, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    ASSERT_EQ(1, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/param_name
/param_name/decl (param_name)
/param_name/= (=)
/param_name/value (${raw 4
                 2
            })
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - quote chaining scenarios
 */
TEST(HITInterpreter, quote_chaining_scenarios)
{
    std::stringstream input;
    input << R"INPUT(
[quotes]
    [singles]
        field = ' "    mix1a mix1b "   "mix1c"    mix1d 1.2

                  "mix\"1e "  mix1f "    mix1g" 34  "mix\'1h "
                ' 'mix\'2a'
                '"mix\3a"' '    '
    []
    [doubles]
        field = " '    mix1a mix1b '   'mix1c'    mix1d 1.2

                  'mix\'1e '  mix1f '    mix1g' 34  'mix\"1h '
                " "mix\"2a"
                "'mix\3a'" "    "
    []
[]
)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(45, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(1, document.child_count());
    ASSERT_EQ(1, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/quotes
/quotes/[ ([)
/quotes/decl (quotes)
/quotes/] (])
/quotes/singles
/quotes/singles/[ ([)
/quotes/singles/decl (singles)
/quotes/singles/] (])
/quotes/singles/field
/quotes/singles/field/decl (field)
/quotes/singles/field/= (=)
/quotes/singles/field/' (')
/quotes/singles/field/value ("    mix1a mix1b ")
/quotes/singles/field/value ("mix1c")
/quotes/singles/field/value (mix1d)
/quotes/singles/field/value (1.2)
/quotes/singles/field/value ("mix\"1e ")
/quotes/singles/field/value (mix1f)
/quotes/singles/field/value ("    mix1g")
/quotes/singles/field/value (34)
/quotes/singles/field/value ("mix\'1h ")
/quotes/singles/field/' (')
/quotes/singles/field/' (')
/quotes/singles/field/value (mix\'2a)
/quotes/singles/field/' (')
/quotes/singles/field/' (')
/quotes/singles/field/value ("mix\3a")
/quotes/singles/field/' (')
/quotes/singles/field/' (')
/quotes/singles/field/' (')
/quotes/singles/term ([])
/quotes/doubles
/quotes/doubles/[ ([)
/quotes/doubles/decl (doubles)
/quotes/doubles/] (])
/quotes/doubles/field
/quotes/doubles/field/decl (field)
/quotes/doubles/field/= (=)
/quotes/doubles/field/value (" '    mix1a mix1b '   'mix1c'    mix1d 1.2

                  'mix\'1e '  mix1f '    mix1g' 34  'mix\"1h '
                ")
/quotes/doubles/field/value ("mix\"2a")
/quotes/doubles/field/value ("'mix\3a'")
/quotes/doubles/field/value ("    ")
/quotes/doubles/term ([])
/quotes/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - ampersands and semicolons in values
 */
TEST(HITInterpreter, ampersands_and_semicolons_in_values)
{
    std::stringstream input;
    input << R"INPUT(

value_wt_quotes_ampersand_beg = '&if((x<1)(y>0.5),1E7,-1E7)'
value_wt_quotes_ampersand_mid = 'if((x<1)&(y>0.5),1E7,-1E7)'
value_wt_quotes_ampersand_end = 'if((x<1)(y>0.5),1E7,-1E7)&'

value_no_quotes_ampersand_beg = &if((x<1)(y>0.5),1E7,-1E7)
value_no_quotes_ampersand_mid = if((x<1)&(y>0.5),1E7,-1E7)
value_no_quotes_ampersand_end = if((x<1)(y>0.5),1E7,-1E7)&

value_no_quotes_semicolon_beg = ;if((x<1)(y>0.5),1E7,-1E7)
value_no_quotes_semicolon_mid = if((x<1);(y>0.5),1E7,-1E7)
value_no_quotes_semicolon_end = if((x<1)(y>0.5),1E7,-1E7);

)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(43, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(9, document.child_count());
    ASSERT_EQ(9, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/value_wt_quotes_ampersand_beg
/value_wt_quotes_ampersand_beg/decl (value_wt_quotes_ampersand_beg)
/value_wt_quotes_ampersand_beg/= (=)
/value_wt_quotes_ampersand_beg/' (')
/value_wt_quotes_ampersand_beg/value (&if((x<1)(y>0.5),1E7,-1E7))
/value_wt_quotes_ampersand_beg/' (')
/value_wt_quotes_ampersand_mid
/value_wt_quotes_ampersand_mid/decl (value_wt_quotes_ampersand_mid)
/value_wt_quotes_ampersand_mid/= (=)
/value_wt_quotes_ampersand_mid/' (')
/value_wt_quotes_ampersand_mid/value (if((x<1)&(y>0.5),1E7,-1E7))
/value_wt_quotes_ampersand_mid/' (')
/value_wt_quotes_ampersand_end
/value_wt_quotes_ampersand_end/decl (value_wt_quotes_ampersand_end)
/value_wt_quotes_ampersand_end/= (=)
/value_wt_quotes_ampersand_end/' (')
/value_wt_quotes_ampersand_end/value (if((x<1)(y>0.5),1E7,-1E7)&)
/value_wt_quotes_ampersand_end/' (')
/value_no_quotes_ampersand_beg
/value_no_quotes_ampersand_beg/decl (value_no_quotes_ampersand_beg)
/value_no_quotes_ampersand_beg/= (=)
/value_no_quotes_ampersand_beg/value (&if((x<1)(y>0.5),1E7,-1E7))
/value_no_quotes_ampersand_mid
/value_no_quotes_ampersand_mid/decl (value_no_quotes_ampersand_mid)
/value_no_quotes_ampersand_mid/= (=)
/value_no_quotes_ampersand_mid/value (if((x<1)&(y>0.5),1E7,-1E7))
/value_no_quotes_ampersand_end
/value_no_quotes_ampersand_end/decl (value_no_quotes_ampersand_end)
/value_no_quotes_ampersand_end/= (=)
/value_no_quotes_ampersand_end/value (if((x<1)(y>0.5),1E7,-1E7)&)
/value_no_quotes_semicolon_beg
/value_no_quotes_semicolon_beg/decl (value_no_quotes_semicolon_beg)
/value_no_quotes_semicolon_beg/= (=)
/value_no_quotes_semicolon_beg/value (;if((x<1)(y>0.5),1E7,-1E7))
/value_no_quotes_semicolon_mid
/value_no_quotes_semicolon_mid/decl (value_no_quotes_semicolon_mid)
/value_no_quotes_semicolon_mid/= (=)
/value_no_quotes_semicolon_mid/value (if((x<1);(y>0.5),1E7,-1E7))
/value_no_quotes_semicolon_end
/value_no_quotes_semicolon_end/decl (value_no_quotes_semicolon_end)
/value_no_quotes_semicolon_end/= (=)
/value_no_quotes_semicolon_end/value (if((x<1)(y>0.5),1E7,-1E7);)
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - period starts block names and paths
 */
TEST(HITInterpreter, period_starts_block_names_and_paths)
{
    std::stringstream input;
    input << R"INPUT(

[Block_Names_Begin_With_Letter]

  [child_1a_no_type]
    var_1a = val_1a
  []

[]

[Block_Names_Begin_With_Period]

  [.child_2a_no_type]
    var_2a = val_2a
  []

[]

[Block_Paths_Begin_With_Period]

  [.child/3a/no/type]
    var_3a = val_3a
  []

[]

)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(46, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(3, document.child_count());
    ASSERT_EQ(3, interpreter.child_count(document.node_index()));
    ASSERT_EQ(document.child_at(0).node_index(),
              interpreter.child_index_at(document.node_index(), 0));

    std::string expected_paths = R"INPUT(/
/Block_Names_Begin_With_Letter
/Block_Names_Begin_With_Letter/[ ([)
/Block_Names_Begin_With_Letter/decl (Block_Names_Begin_With_Letter)
/Block_Names_Begin_With_Letter/] (])
/Block_Names_Begin_With_Letter/child_1a_no_type
/Block_Names_Begin_With_Letter/child_1a_no_type/[ ([)
/Block_Names_Begin_With_Letter/child_1a_no_type/decl (child_1a_no_type)
/Block_Names_Begin_With_Letter/child_1a_no_type/] (])
/Block_Names_Begin_With_Letter/child_1a_no_type/var_1a
/Block_Names_Begin_With_Letter/child_1a_no_type/var_1a/decl (var_1a)
/Block_Names_Begin_With_Letter/child_1a_no_type/var_1a/= (=)
/Block_Names_Begin_With_Letter/child_1a_no_type/var_1a/value (val_1a)
/Block_Names_Begin_With_Letter/child_1a_no_type/term ([])
/Block_Names_Begin_With_Letter/term ([])
/Block_Names_Begin_With_Period
/Block_Names_Begin_With_Period/[ ([)
/Block_Names_Begin_With_Period/decl (Block_Names_Begin_With_Period)
/Block_Names_Begin_With_Period/] (])
/Block_Names_Begin_With_Period/.child_2a_no_type
/Block_Names_Begin_With_Period/.child_2a_no_type/[ ([)
/Block_Names_Begin_With_Period/.child_2a_no_type/decl (.child_2a_no_type)
/Block_Names_Begin_With_Period/.child_2a_no_type/] (])
/Block_Names_Begin_With_Period/.child_2a_no_type/var_2a
/Block_Names_Begin_With_Period/.child_2a_no_type/var_2a/decl (var_2a)
/Block_Names_Begin_With_Period/.child_2a_no_type/var_2a/= (=)
/Block_Names_Begin_With_Period/.child_2a_no_type/var_2a/value (val_2a)
/Block_Names_Begin_With_Period/.child_2a_no_type/term ([])
/Block_Names_Begin_With_Period/term ([])
/Block_Paths_Begin_With_Period
/Block_Paths_Begin_With_Period/[ ([)
/Block_Paths_Begin_With_Period/decl (Block_Paths_Begin_With_Period)
/Block_Paths_Begin_With_Period/] (])
/Block_Paths_Begin_With_Period/.child
/Block_Paths_Begin_With_Period/.child/3a
/Block_Paths_Begin_With_Period/.child/3a/no
/Block_Paths_Begin_With_Period/.child/3a/no/type
/Block_Paths_Begin_With_Period/.child/3a/no/type/[ ([)
/Block_Paths_Begin_With_Period/.child/3a/no/type/decl (.child/3a/no/type)
/Block_Paths_Begin_With_Period/.child/3a/no/type/] (])
/Block_Paths_Begin_With_Period/.child/3a/no/type/var_3a
/Block_Paths_Begin_With_Period/.child/3a/no/type/var_3a/decl (var_3a)
/Block_Paths_Begin_With_Period/.child/3a/no/type/var_3a/= (=)
/Block_Paths_Begin_With_Period/.child/3a/no/type/var_3a/value (val_3a)
/Block_Paths_Begin_With_Period/.child/3a/no/type/term ([])
/Block_Paths_Begin_With_Period/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - comment delimiters within quotes
 */
TEST(HITInterpreter, comment_delimiters_within_quotes)
{
    std::stringstream input;
    input << R"INPUT(
[SingleQuotedCommentDelims]
  # comment for single quote block
  one = '#123'
  two = '#456 #789'
[]
[DoubleQuotedCommentDelims]
  # comment for double quote block
  one = "#123"
  two = "#456 #789"
[]
)INPUT";

    // Check parse success, count of total nodes, count of children of root
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(34, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(2, document.child_count());

    // SingleQuotedCommentDelims - block: ([)(decl)(])(comment)(one)(two)(term)
    auto block_01 = document.child_at(0);
    ASSERT_FALSE(block_01.is_null());
    ASSERT_FALSE(block_01.is_decorative());
    ASSERT_EQ(std::string("SingleQuotedCommentDelims"), block_01.name());
    ASSERT_EQ(wasp::OBJECT, block_01.type());
    ASSERT_EQ(7, block_01.child_count());
    ASSERT_EQ(2, block_01.non_decorative_children_count());

    // SingleQuotedCommentDelims/one/comment - # comment for single quote block
    auto block_01_comment = block_01.first_child_by_name("comment");
    ASSERT_FALSE(block_01_comment.is_null());
    ASSERT_TRUE(block_01_comment.is_decorative());
    ASSERT_EQ(wasp::COMMENT, block_01_comment.type());
    ASSERT_EQ("# comment for single quote block", block_01_comment.data());

    // SingleQuotedCommentDelims/one - key-array: (decl)(=)(')(value)(')
    auto block_01_param_01 = block_01.child_at(4);
    ASSERT_FALSE(block_01_param_01.is_null());
    ASSERT_FALSE(block_01_param_01.is_decorative());
    ASSERT_EQ(std::string("one"), block_01_param_01.name());
    ASSERT_EQ(wasp::ARRAY, block_01_param_01.type());
    ASSERT_EQ(5, block_01_param_01.child_count());
    ASSERT_EQ(1, block_01_param_01.non_decorative_children_count());

    // SingleQuotedCommentDelims/one/value - number of values is one
    auto block_01_param_01_values = block_01_param_01.child_by_name("value");
    ASSERT_EQ(1, block_01_param_01_values.size());

    // SingleQuotedCommentDelims/one/value - value number 01 is #123
    auto block_01_param_01_value_01 = block_01_param_01_values[0];
    ASSERT_FALSE(block_01_param_01_value_01.is_null());
    ASSERT_FALSE(block_01_param_01_value_01.is_decorative());
    ASSERT_EQ(wasp::VALUE, block_01_param_01_value_01.type());
    ASSERT_EQ(std::string("#123"), block_01_param_01_value_01.data());

    // SingleQuotedCommentDelims/two - key-array: (decl)(=)(')(value)(value)(')
    auto block_01_param_02 = block_01.child_at(5);
    ASSERT_FALSE(block_01_param_02.is_null());
    ASSERT_FALSE(block_01_param_02.is_decorative());
    ASSERT_EQ(std::string("two"), block_01_param_02.name());
    ASSERT_EQ(wasp::ARRAY, block_01_param_02.type());
    ASSERT_EQ(6, block_01_param_02.child_count());
    ASSERT_EQ(2, block_01_param_02.non_decorative_children_count());

    // SingleQuotedCommentDelims/two/value - number of values is two
    auto block_01_param_02_values = block_01_param_02.child_by_name("value");
    ASSERT_EQ(2, block_01_param_02_values.size());

    // SingleQuotedCommentDelims/two/value - value number 01 is #456
    auto block_01_param_02_value_01 = block_01_param_02_values[0];
    ASSERT_FALSE(block_01_param_02_value_01.is_null());
    ASSERT_FALSE(block_01_param_02_value_01.is_decorative());
    ASSERT_EQ(wasp::VALUE, block_01_param_02_value_01.type());
    ASSERT_EQ(std::string("#456"), block_01_param_02_value_01.data());

    // SingleQuotedCommentDelims/two/value - value number 02 is #789
    auto block_01_param_02_value_02 = block_01_param_02_values[1];
    ASSERT_FALSE(block_01_param_02_value_02.is_null());
    ASSERT_FALSE(block_01_param_02_value_02.is_decorative());
    ASSERT_EQ(wasp::VALUE, block_01_param_02_value_02.type());
    ASSERT_EQ(std::string("#789"), block_01_param_02_value_02.data());

    // DoubleQuotedCommentDelims - block: ([)(decl)(])(comment)(one)(two)(term)
    auto block_02 = document.child_at(1);
    ASSERT_FALSE(block_02.is_null());
    ASSERT_FALSE(block_02.is_decorative());
    ASSERT_EQ(std::string("DoubleQuotedCommentDelims"), block_02.name());
    ASSERT_EQ(wasp::OBJECT, block_02.type());
    ASSERT_EQ(7, block_02.child_count());
    ASSERT_EQ(2, block_02.non_decorative_children_count());

    // DoubleQuotedCommentDelims/one/comment - # comment for double quote block
    auto block_02_comment = block_02.first_child_by_name("comment");
    ASSERT_FALSE(block_02_comment.is_null());
    ASSERT_TRUE(block_02_comment.is_decorative());
    ASSERT_EQ(wasp::COMMENT, block_02_comment.type());
    ASSERT_EQ("# comment for double quote block", block_02_comment.data());

    // DoubleQuotedCommentDelims/one - key-value: (decl)(=)(value)
    auto block_02_param_01 = block_02.child_at(4);
    ASSERT_FALSE(block_02_param_01.is_null());
    ASSERT_FALSE(block_02_param_01.is_decorative());
    ASSERT_EQ(std::string("one"), block_02_param_01.name());
    ASSERT_EQ(wasp::KEYED_VALUE, block_02_param_01.type());
    ASSERT_EQ(3, block_02_param_01.child_count());
    ASSERT_EQ(1, block_02_param_01.non_decorative_children_count());

    // DoubleQuotedCommentDelims/one/value - number of values is one
    auto block_02_param_01_values = block_02_param_01.child_by_name("value");
    ASSERT_EQ(1, block_02_param_01_values.size());

    // DoubleQuotedCommentDelims/one/value - value number 01 is "#123"
    auto block_02_param_01_value_01 = block_02_param_01_values[0];
    ASSERT_FALSE(block_02_param_01_value_01.is_null());
    ASSERT_FALSE(block_02_param_01_value_01.is_decorative());
    ASSERT_EQ(wasp::VALUE, block_02_param_01_value_01.type());
    ASSERT_EQ(std::string("\"#123\""), block_02_param_01_value_01.data());

    // DoubleQuotedCommentDelims/two - key-value: (decl)(=)(value)
    auto block_02_param_02 = block_02.child_at(5);
    ASSERT_FALSE(block_02_param_02.is_null());
    ASSERT_FALSE(block_02_param_02.is_decorative());
    ASSERT_EQ(std::string("two"), block_02_param_02.name());
    ASSERT_EQ(wasp::KEYED_VALUE, block_02_param_02.type());
    ASSERT_EQ(3, block_02_param_02.child_count());
    ASSERT_EQ(1, block_02_param_02.non_decorative_children_count());

    // DoubleQuotedCommentDelims/two/value - number of values is one
    auto block_02_param_02_values = block_02_param_02.child_by_name("value");
    ASSERT_EQ(1, block_02_param_02_values.size());

    // DoubleQuotedCommentDelims/two/value - value number 01 is "#456 #789"
    auto block_02_param_02_value_01 = block_02_param_02_values[0];
    ASSERT_FALSE(block_02_param_02_value_01.is_null());
    ASSERT_FALSE(block_02_param_02_value_01.is_decorative());
    ASSERT_EQ(wasp::VALUE, block_02_param_02_value_01.type());
    ASSERT_EQ(std::string("\"#456 #789\""), block_02_param_02_value_01.data());

    // Check full set of document paths from the root of the parse tree
    std::string expected_paths = R"INPUT(/
/SingleQuotedCommentDelims
/SingleQuotedCommentDelims/[ ([)
/SingleQuotedCommentDelims/decl (SingleQuotedCommentDelims)
/SingleQuotedCommentDelims/] (])
/SingleQuotedCommentDelims/comment (# comment for single quote block)
/SingleQuotedCommentDelims/one
/SingleQuotedCommentDelims/one/decl (one)
/SingleQuotedCommentDelims/one/= (=)
/SingleQuotedCommentDelims/one/' (')
/SingleQuotedCommentDelims/one/value (#123)
/SingleQuotedCommentDelims/one/' (')
/SingleQuotedCommentDelims/two
/SingleQuotedCommentDelims/two/decl (two)
/SingleQuotedCommentDelims/two/= (=)
/SingleQuotedCommentDelims/two/' (')
/SingleQuotedCommentDelims/two/value (#456)
/SingleQuotedCommentDelims/two/value (#789)
/SingleQuotedCommentDelims/two/' (')
/SingleQuotedCommentDelims/term ([])
/DoubleQuotedCommentDelims
/DoubleQuotedCommentDelims/[ ([)
/DoubleQuotedCommentDelims/decl (DoubleQuotedCommentDelims)
/DoubleQuotedCommentDelims/] (])
/DoubleQuotedCommentDelims/comment (# comment for double quote block)
/DoubleQuotedCommentDelims/one
/DoubleQuotedCommentDelims/one/decl (one)
/DoubleQuotedCommentDelims/one/= (=)
/DoubleQuotedCommentDelims/one/value ("#123")
/DoubleQuotedCommentDelims/two
/DoubleQuotedCommentDelims/two/decl (two)
/DoubleQuotedCommentDelims/two/= (=)
/DoubleQuotedCommentDelims/two/value ("#456 #789")
/DoubleQuotedCommentDelims/term ([])
)INPUT";

    std::stringstream actual_paths;
    document.paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}


/**
 * @brief Test HIT Expression syntax processing 
 */
TEST(HITInterpreter, expression_syntax)
{
    std::stringstream input;
    input << R"INPUT(
[Simple]
  foo01=${fparse 42}
  foo02='${fparse 42}'
[]

[Multiple]
  foo01=${fparse 42} bar01=${fparse foo01}
  foo02='${fparse 42}' bar02='${fparse foo02}'
[]

[Fparse]
  pp_ini_bc = ${fparse if(depth<=2000, 1000*9.8*depth, 1000*9.8*depth+(depth-2000)*9.8*1000)}
[]

[Examples]
  sliceheight = ${ ${fuelheight} ${numslices}}
  file_base01 = ${raw ${postprocessor_type_02} _fv}
  file_base02 = '${raw ${postprocessor_type_01} _fv}'
[]

[NoQuotedNestings]
  slice_01 = ${/ ${fuel ${numslices} height} }
  slice_02 = ${${fuel ${numslices} height}}
  slice_03 = ${${${numslices} height}}
  slice_04 = ${${${numslices}}}
  slice_05 = ${raw ${postprocessor ${numslices} _01} _fv}
[]

[SingleQuotedArrays]
  ndice_01 = '${/ ${fuel ${numslices} height} }'
  ndice_02 = '${${fuel ${numslices} height}}'
  ndice_03 = '${${${numslices} height}}'
  ndice_04 = '${${${numslices}}}'
  ndice_05 = '${raw ${postprocessor ${numslices} _01} _fv}'
[]

[DoubleQuotedStrings]
  nchop_01 = "${/ ${fuel ${numslices} height} }"
  nchop_02 = "${${fuel ${numslices} height}}"
  nchop_03 = "${${${numslices} height}}"
  nchop_04 = "${${${numslices}}}"
  nchop_05 = "${raw ${postprocessor ${numslices} _01} _fv}"
[]
)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::string expected_paths = R"INPUT(/
/Simple
/Simple/[ ([)
/Simple/decl (Simple)
/Simple/] (])
/Simple/foo01
/Simple/foo01/decl (foo01)
/Simple/foo01/= (=)
/Simple/foo01/value (${fparse 42})
/Simple/foo02
/Simple/foo02/decl (foo02)
/Simple/foo02/= (=)
/Simple/foo02/' (')
/Simple/foo02/value (${fparse 42})
/Simple/foo02/' (')
/Simple/term ([])
/Multiple
/Multiple/[ ([)
/Multiple/decl (Multiple)
/Multiple/] (])
/Multiple/foo01
/Multiple/foo01/decl (foo01)
/Multiple/foo01/= (=)
/Multiple/foo01/value (${fparse 42})
/Multiple/bar01
/Multiple/bar01/decl (bar01)
/Multiple/bar01/= (=)
/Multiple/bar01/value (${fparse foo01})
/Multiple/foo02
/Multiple/foo02/decl (foo02)
/Multiple/foo02/= (=)
/Multiple/foo02/' (')
/Multiple/foo02/value (${fparse 42})
/Multiple/foo02/' (')
/Multiple/bar02
/Multiple/bar02/decl (bar02)
/Multiple/bar02/= (=)
/Multiple/bar02/' (')
/Multiple/bar02/value (${fparse foo02})
/Multiple/bar02/' (')
/Multiple/term ([])
/Fparse
/Fparse/[ ([)
/Fparse/decl (Fparse)
/Fparse/] (])
/Fparse/pp_ini_bc
/Fparse/pp_ini_bc/decl (pp_ini_bc)
/Fparse/pp_ini_bc/= (=)
/Fparse/pp_ini_bc/value (${fparse if(depth<=2000, 1000*9.8*depth, 1000*9.8*depth+(depth-2000)*9.8*1000)})
/Fparse/term ([])
/Examples
/Examples/[ ([)
/Examples/decl (Examples)
/Examples/] (])
/Examples/sliceheight
/Examples/sliceheight/decl (sliceheight)
/Examples/sliceheight/= (=)
/Examples/sliceheight/value (${ ${fuelheight} ${numslices}})
/Examples/file_base01
/Examples/file_base01/decl (file_base01)
/Examples/file_base01/= (=)
/Examples/file_base01/value (${raw ${postprocessor_type_02} _fv})
/Examples/file_base02
/Examples/file_base02/decl (file_base02)
/Examples/file_base02/= (=)
/Examples/file_base02/' (')
/Examples/file_base02/value (${raw ${postprocessor_type_01} _fv})
/Examples/file_base02/' (')
/Examples/term ([])
/NoQuotedNestings
/NoQuotedNestings/[ ([)
/NoQuotedNestings/decl (NoQuotedNestings)
/NoQuotedNestings/] (])
/NoQuotedNestings/slice_01
/NoQuotedNestings/slice_01/decl (slice_01)
/NoQuotedNestings/slice_01/= (=)
/NoQuotedNestings/slice_01/value (${/ ${fuel ${numslices} height} })
/NoQuotedNestings/slice_02
/NoQuotedNestings/slice_02/decl (slice_02)
/NoQuotedNestings/slice_02/= (=)
/NoQuotedNestings/slice_02/value (${${fuel ${numslices} height}})
/NoQuotedNestings/slice_03
/NoQuotedNestings/slice_03/decl (slice_03)
/NoQuotedNestings/slice_03/= (=)
/NoQuotedNestings/slice_03/value (${${${numslices} height}})
/NoQuotedNestings/slice_04
/NoQuotedNestings/slice_04/decl (slice_04)
/NoQuotedNestings/slice_04/= (=)
/NoQuotedNestings/slice_04/value (${${${numslices}}})
/NoQuotedNestings/slice_05
/NoQuotedNestings/slice_05/decl (slice_05)
/NoQuotedNestings/slice_05/= (=)
/NoQuotedNestings/slice_05/value (${raw ${postprocessor ${numslices} _01} _fv})
/NoQuotedNestings/term ([])
/SingleQuotedArrays
/SingleQuotedArrays/[ ([)
/SingleQuotedArrays/decl (SingleQuotedArrays)
/SingleQuotedArrays/] (])
/SingleQuotedArrays/ndice_01
/SingleQuotedArrays/ndice_01/decl (ndice_01)
/SingleQuotedArrays/ndice_01/= (=)
/SingleQuotedArrays/ndice_01/' (')
/SingleQuotedArrays/ndice_01/value (${/ ${fuel ${numslices} height} })
/SingleQuotedArrays/ndice_01/' (')
/SingleQuotedArrays/ndice_02
/SingleQuotedArrays/ndice_02/decl (ndice_02)
/SingleQuotedArrays/ndice_02/= (=)
/SingleQuotedArrays/ndice_02/' (')
/SingleQuotedArrays/ndice_02/value (${${fuel ${numslices} height}})
/SingleQuotedArrays/ndice_02/' (')
/SingleQuotedArrays/ndice_03
/SingleQuotedArrays/ndice_03/decl (ndice_03)
/SingleQuotedArrays/ndice_03/= (=)
/SingleQuotedArrays/ndice_03/' (')
/SingleQuotedArrays/ndice_03/value (${${${numslices} height}})
/SingleQuotedArrays/ndice_03/' (')
/SingleQuotedArrays/ndice_04
/SingleQuotedArrays/ndice_04/decl (ndice_04)
/SingleQuotedArrays/ndice_04/= (=)
/SingleQuotedArrays/ndice_04/' (')
/SingleQuotedArrays/ndice_04/value (${${${numslices}}})
/SingleQuotedArrays/ndice_04/' (')
/SingleQuotedArrays/ndice_05
/SingleQuotedArrays/ndice_05/decl (ndice_05)
/SingleQuotedArrays/ndice_05/= (=)
/SingleQuotedArrays/ndice_05/' (')
/SingleQuotedArrays/ndice_05/value (${raw ${postprocessor ${numslices} _01} _fv})
/SingleQuotedArrays/ndice_05/' (')
/SingleQuotedArrays/term ([])
/DoubleQuotedStrings
/DoubleQuotedStrings/[ ([)
/DoubleQuotedStrings/decl (DoubleQuotedStrings)
/DoubleQuotedStrings/] (])
/DoubleQuotedStrings/nchop_01
/DoubleQuotedStrings/nchop_01/decl (nchop_01)
/DoubleQuotedStrings/nchop_01/= (=)
/DoubleQuotedStrings/nchop_01/value ("${/ ${fuel ${numslices} height} }")
/DoubleQuotedStrings/nchop_02
/DoubleQuotedStrings/nchop_02/decl (nchop_02)
/DoubleQuotedStrings/nchop_02/= (=)
/DoubleQuotedStrings/nchop_02/value ("${${fuel ${numslices} height}}")
/DoubleQuotedStrings/nchop_03
/DoubleQuotedStrings/nchop_03/decl (nchop_03)
/DoubleQuotedStrings/nchop_03/= (=)
/DoubleQuotedStrings/nchop_03/value ("${${${numslices} height}}")
/DoubleQuotedStrings/nchop_04
/DoubleQuotedStrings/nchop_04/decl (nchop_04)
/DoubleQuotedStrings/nchop_04/= (=)
/DoubleQuotedStrings/nchop_04/value ("${${${numslices}}}")
/DoubleQuotedStrings/nchop_05
/DoubleQuotedStrings/nchop_05/decl (nchop_05)
/DoubleQuotedStrings/nchop_05/= (=)
/DoubleQuotedStrings/nchop_05/value ("${raw ${postprocessor ${numslices} _01} _fv}")
/DoubleQuotedStrings/term ([])
)INPUT";
    std::stringstream actual_paths;
    interpreter.root().paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());

    // Check count of total nodes in tree and count of children of root
    ASSERT_EQ(154, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(7, document.child_count());

    // SingleQuotedArrays - block: ([) (decl) (]) (key-array)x5 (term)
    auto block_06 = document.child_at(5);
    ASSERT_FALSE(block_06.is_null());
    ASSERT_FALSE(block_06.is_decorative());
    ASSERT_EQ(std::string("SingleQuotedArrays"), block_06.name());
    ASSERT_EQ(wasp::OBJECT, block_06.type());
    ASSERT_EQ(9, block_06.child_count());
    ASSERT_EQ(5, block_06.non_decorative_children_count());
    ASSERT_EQ(30, block_06.line());
    ASSERT_EQ(1, block_06.column());
    ASSERT_EQ(36, block_06.last_line());
    ASSERT_EQ(2, block_06.last_column());
    std::string expected_block_06_data = R"INPUT(
[SingleQuotedArrays]
  ndice_01 = '${/ ${fuel ${numslices} height} }'
  ndice_02 = '${${fuel ${numslices} height}}'
  ndice_03 = '${${${numslices} height}}'
  ndice_04 = '${${${numslices}}}'
  ndice_05 = '${raw ${postprocessor ${numslices} _01} _fv}'
[]
)INPUT";
    ASSERT_EQ(expected_block_06_data, "\n" + block_06.data() + "\n");

    // SingleQuotedArrays/ndice_03 - key-array: (decl) (=) (') (value) (')
    auto block_06_param_03 = block_06.child_at(5);
    ASSERT_FALSE(block_06_param_03.is_null());
    ASSERT_FALSE(block_06_param_03.is_decorative());
    ASSERT_EQ(std::string("ndice_03"), block_06_param_03.name());
    ASSERT_EQ(wasp::ARRAY, block_06_param_03.type());
    ASSERT_EQ(5, block_06_param_03.child_count());
    ASSERT_EQ(1, block_06_param_03.non_decorative_children_count());
    ASSERT_EQ(33, block_06_param_03.line());
    ASSERT_EQ(3, block_06_param_03.column());
    ASSERT_EQ(33, block_06_param_03.last_line());
    ASSERT_EQ(40, block_06_param_03.last_column());
    std::string expected_block_06_param_03_data = R"INPUT(
ndice_03 = '${${${numslices} height}}'
)INPUT";
    ASSERT_EQ(expected_block_06_param_03_data, "\n" + block_06_param_03.data() + "\n");

    // SingleQuotedArrays/ndice_03/value - value: ${${${numslices} height}}
    auto block_06_param_03_value = block_06_param_03.first_child_by_name("value");
    ASSERT_FALSE(block_06_param_03_value.is_null());
    ASSERT_FALSE(block_06_param_03_value.is_decorative());
    ASSERT_EQ(wasp::VALUE, block_06_param_03_value.type());
    ASSERT_EQ(0, block_06_param_03_value.child_count());
    ASSERT_EQ(0, block_06_param_03_value.non_decorative_children_count());
    ASSERT_EQ(33, block_06_param_03_value.line());
    ASSERT_EQ(15, block_06_param_03_value.column());
    ASSERT_EQ(33, block_06_param_03_value.last_line());
    ASSERT_EQ(39, block_06_param_03_value.last_column());
    std::string expected_block_06_param_03_value_data = R"INPUT(
${${${numslices} height}}
)INPUT";
    ASSERT_EQ(expected_block_06_param_03_value_data, "\n" + block_06_param_03_value.data() + "\n");
    ASSERT_EQ("${${${numslices} height}}", block_06_param_03_value.to_string());
}

/**
 * @brief Test HIT Expression syntax error processing (unterminated expression)
 * Looking for the matching closing curly brace should result in a 
 * error reaching the end of file
 */
TEST(HITInterpreter, expression_syntax_error)
{
    std::stringstream input;
    input << R"INPUT(
[Simple]
  foo01=${fparse 42
  foo02='${fparse 42}'
[]
)INPUT";

    std::stringstream     error;
    DefaultHITInterpreter interpreter(error);
    ASSERT_FALSE(interpreter.parse(input));
    std::string expected = "stream input:3.9: syntax error, unexpected end of file\nstream input:3.8: syntax error, 'foo01' has a missing or malformed value\nstream input:2.1: syntax error, unexpected end of file, expecting block terminator\n";
    ASSERT_EQ(expected, error.str());
}

/**
 * @brief Test HIT syntax - dollar sign in parameter values and array values
 */
TEST(HITInterpreter, dollar_in_param_and_array_values)
{
    std::stringstream input;
    input << R"INPUT(
[DollarSingleValueParams]
  param01 = $misc-\w+
  param02 = misc$-\w+
  param03 = $mis-\w+$
  param04 = misc-\w+$

  param05 = {misc-\w+
  param06 = misc{-\w+
  param07 = m`i$.{c}*
  param08 = {isc-\w+{

  param09 = }misc-\w+
  param10 = misc}-\w+
  param11 = $}sc-\w+}
  param12 = misc-\w+}

  param13 = trailing-single-space$ 
  param14 = trailing-comment-char$# comment here
[]
[DollarSingleValueArrays]
  array01 = '$misc-\w+'
  array02 = 'misc$-\w+'
  array03 = '$ms-\w+$ '
  array04 = 'misc-\w+$'

  array05 = '{misc-\w+'
  array06 = 'misc{-\w+'
  array07 = 'm`i$.{c}*'
  array08 = '{isc-\w+{'

  array09 = '}misc-\w+'
  array10 = 'misc}-\w+'
  array11 = '$}sc-\w+}'
  array12 = 'misc-\w+}'

  array13 = 'trailing-single-space$ '
  array14 = 'trailing-comment-char$# comment here'
[]
[DollarMultiValuesArrays]
  array01 = '$misc-\w+ misc$-\w+ $mis-\w+$ misc-\w+$'
  array02 = '{misc-\w+ misc{-\w+ misc-{+ $ {isc-\w+}'
  array03 = '}misc-\w+ misc}-\w+ $}sc-\w+} misc-\w+}'
  command = 'for bin in `ls ../../../dir/app_exe-*`
             do
             cat dir$
             echo
             $env
             cd ..
             ln -sf $bin .
             done'
[]
)INPUT";

    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::string expected_paths = R"INPUT(/
/DollarSingleValueParams
/DollarSingleValueParams/[ ([)
/DollarSingleValueParams/decl (DollarSingleValueParams)
/DollarSingleValueParams/] (])
/DollarSingleValueParams/param01
/DollarSingleValueParams/param01/decl (param01)
/DollarSingleValueParams/param01/= (=)
/DollarSingleValueParams/param01/value ($misc-\w+)
/DollarSingleValueParams/param02
/DollarSingleValueParams/param02/decl (param02)
/DollarSingleValueParams/param02/= (=)
/DollarSingleValueParams/param02/value (misc$-\w+)
/DollarSingleValueParams/param03
/DollarSingleValueParams/param03/decl (param03)
/DollarSingleValueParams/param03/= (=)
/DollarSingleValueParams/param03/value ($mis-\w+$)
/DollarSingleValueParams/param04
/DollarSingleValueParams/param04/decl (param04)
/DollarSingleValueParams/param04/= (=)
/DollarSingleValueParams/param04/value (misc-\w+$)
/DollarSingleValueParams/param05
/DollarSingleValueParams/param05/decl (param05)
/DollarSingleValueParams/param05/= (=)
/DollarSingleValueParams/param05/value ({misc-\w+)
/DollarSingleValueParams/param06
/DollarSingleValueParams/param06/decl (param06)
/DollarSingleValueParams/param06/= (=)
/DollarSingleValueParams/param06/value (misc{-\w+)
/DollarSingleValueParams/param07
/DollarSingleValueParams/param07/decl (param07)
/DollarSingleValueParams/param07/= (=)
/DollarSingleValueParams/param07/value (m`i$.{c}*)
/DollarSingleValueParams/param08
/DollarSingleValueParams/param08/decl (param08)
/DollarSingleValueParams/param08/= (=)
/DollarSingleValueParams/param08/value ({isc-\w+{)
/DollarSingleValueParams/param09
/DollarSingleValueParams/param09/decl (param09)
/DollarSingleValueParams/param09/= (=)
/DollarSingleValueParams/param09/value (}misc-\w+)
/DollarSingleValueParams/param10
/DollarSingleValueParams/param10/decl (param10)
/DollarSingleValueParams/param10/= (=)
/DollarSingleValueParams/param10/value (misc}-\w+)
/DollarSingleValueParams/param11
/DollarSingleValueParams/param11/decl (param11)
/DollarSingleValueParams/param11/= (=)
/DollarSingleValueParams/param11/value ($}sc-\w+})
/DollarSingleValueParams/param12
/DollarSingleValueParams/param12/decl (param12)
/DollarSingleValueParams/param12/= (=)
/DollarSingleValueParams/param12/value (misc-\w+})
/DollarSingleValueParams/param13
/DollarSingleValueParams/param13/decl (param13)
/DollarSingleValueParams/param13/= (=)
/DollarSingleValueParams/param13/value (trailing-single-space$)
/DollarSingleValueParams/param14
/DollarSingleValueParams/param14/decl (param14)
/DollarSingleValueParams/param14/= (=)
/DollarSingleValueParams/param14/value (trailing-comment-char$)
/DollarSingleValueParams/comment (# comment here)
/DollarSingleValueParams/term ([])
/DollarSingleValueArrays
/DollarSingleValueArrays/[ ([)
/DollarSingleValueArrays/decl (DollarSingleValueArrays)
/DollarSingleValueArrays/] (])
/DollarSingleValueArrays/array01
/DollarSingleValueArrays/array01/decl (array01)
/DollarSingleValueArrays/array01/= (=)
/DollarSingleValueArrays/array01/' (')
/DollarSingleValueArrays/array01/value ($misc-\w+)
/DollarSingleValueArrays/array01/' (')
/DollarSingleValueArrays/array02
/DollarSingleValueArrays/array02/decl (array02)
/DollarSingleValueArrays/array02/= (=)
/DollarSingleValueArrays/array02/' (')
/DollarSingleValueArrays/array02/value (misc$-\w+)
/DollarSingleValueArrays/array02/' (')
/DollarSingleValueArrays/array03
/DollarSingleValueArrays/array03/decl (array03)
/DollarSingleValueArrays/array03/= (=)
/DollarSingleValueArrays/array03/' (')
/DollarSingleValueArrays/array03/value ($ms-\w+$)
/DollarSingleValueArrays/array03/' (')
/DollarSingleValueArrays/array04
/DollarSingleValueArrays/array04/decl (array04)
/DollarSingleValueArrays/array04/= (=)
/DollarSingleValueArrays/array04/' (')
/DollarSingleValueArrays/array04/value (misc-\w+$)
/DollarSingleValueArrays/array04/' (')
/DollarSingleValueArrays/array05
/DollarSingleValueArrays/array05/decl (array05)
/DollarSingleValueArrays/array05/= (=)
/DollarSingleValueArrays/array05/' (')
/DollarSingleValueArrays/array05/value ({misc-\w+)
/DollarSingleValueArrays/array05/' (')
/DollarSingleValueArrays/array06
/DollarSingleValueArrays/array06/decl (array06)
/DollarSingleValueArrays/array06/= (=)
/DollarSingleValueArrays/array06/' (')
/DollarSingleValueArrays/array06/value (misc{-\w+)
/DollarSingleValueArrays/array06/' (')
/DollarSingleValueArrays/array07
/DollarSingleValueArrays/array07/decl (array07)
/DollarSingleValueArrays/array07/= (=)
/DollarSingleValueArrays/array07/' (')
/DollarSingleValueArrays/array07/value (m`i$.{c}*)
/DollarSingleValueArrays/array07/' (')
/DollarSingleValueArrays/array08
/DollarSingleValueArrays/array08/decl (array08)
/DollarSingleValueArrays/array08/= (=)
/DollarSingleValueArrays/array08/' (')
/DollarSingleValueArrays/array08/value ({isc-\w+{)
/DollarSingleValueArrays/array08/' (')
/DollarSingleValueArrays/array09
/DollarSingleValueArrays/array09/decl (array09)
/DollarSingleValueArrays/array09/= (=)
/DollarSingleValueArrays/array09/' (')
/DollarSingleValueArrays/array09/value (}misc-\w+)
/DollarSingleValueArrays/array09/' (')
/DollarSingleValueArrays/array10
/DollarSingleValueArrays/array10/decl (array10)
/DollarSingleValueArrays/array10/= (=)
/DollarSingleValueArrays/array10/' (')
/DollarSingleValueArrays/array10/value (misc}-\w+)
/DollarSingleValueArrays/array10/' (')
/DollarSingleValueArrays/array11
/DollarSingleValueArrays/array11/decl (array11)
/DollarSingleValueArrays/array11/= (=)
/DollarSingleValueArrays/array11/' (')
/DollarSingleValueArrays/array11/value ($}sc-\w+})
/DollarSingleValueArrays/array11/' (')
/DollarSingleValueArrays/array12
/DollarSingleValueArrays/array12/decl (array12)
/DollarSingleValueArrays/array12/= (=)
/DollarSingleValueArrays/array12/' (')
/DollarSingleValueArrays/array12/value (misc-\w+})
/DollarSingleValueArrays/array12/' (')
/DollarSingleValueArrays/array13
/DollarSingleValueArrays/array13/decl (array13)
/DollarSingleValueArrays/array13/= (=)
/DollarSingleValueArrays/array13/' (')
/DollarSingleValueArrays/array13/value (trailing-single-space$)
/DollarSingleValueArrays/array13/' (')
/DollarSingleValueArrays/array14
/DollarSingleValueArrays/array14/decl (array14)
/DollarSingleValueArrays/array14/= (=)
/DollarSingleValueArrays/array14/' (')
/DollarSingleValueArrays/array14/value (trailing-comment-char$#)
/DollarSingleValueArrays/array14/value (comment)
/DollarSingleValueArrays/array14/value (here)
/DollarSingleValueArrays/array14/' (')
/DollarSingleValueArrays/term ([])
/DollarMultiValuesArrays
/DollarMultiValuesArrays/[ ([)
/DollarMultiValuesArrays/decl (DollarMultiValuesArrays)
/DollarMultiValuesArrays/] (])
/DollarMultiValuesArrays/array01
/DollarMultiValuesArrays/array01/decl (array01)
/DollarMultiValuesArrays/array01/= (=)
/DollarMultiValuesArrays/array01/' (')
/DollarMultiValuesArrays/array01/value ($misc-\w+)
/DollarMultiValuesArrays/array01/value (misc$-\w+)
/DollarMultiValuesArrays/array01/value ($mis-\w+$ misc-\w+$)
/DollarMultiValuesArrays/array01/' (')
/DollarMultiValuesArrays/array02
/DollarMultiValuesArrays/array02/decl (array02)
/DollarMultiValuesArrays/array02/= (=)
/DollarMultiValuesArrays/array02/' (')
/DollarMultiValuesArrays/array02/value ({misc-\w+)
/DollarMultiValuesArrays/array02/value (misc{-\w+)
/DollarMultiValuesArrays/array02/value (misc-{+)
/DollarMultiValuesArrays/array02/value ($ {isc-\w+})
/DollarMultiValuesArrays/array02/' (')
/DollarMultiValuesArrays/array03
/DollarMultiValuesArrays/array03/decl (array03)
/DollarMultiValuesArrays/array03/= (=)
/DollarMultiValuesArrays/array03/' (')
/DollarMultiValuesArrays/array03/value (}misc-\w+)
/DollarMultiValuesArrays/array03/value (misc}-\w+)
/DollarMultiValuesArrays/array03/value ($}sc-\w+})
/DollarMultiValuesArrays/array03/value (misc-\w+})
/DollarMultiValuesArrays/array03/' (')
/DollarMultiValuesArrays/command
/DollarMultiValuesArrays/command/decl (command)
/DollarMultiValuesArrays/command/= (=)
/DollarMultiValuesArrays/command/' (')
/DollarMultiValuesArrays/command/value (for)
/DollarMultiValuesArrays/command/value (bin)
/DollarMultiValuesArrays/command/value (in)
/DollarMultiValuesArrays/command/value (`ls)
/DollarMultiValuesArrays/command/value (../../../dir/app_exe-*`)
/DollarMultiValuesArrays/command/value (do)
/DollarMultiValuesArrays/command/value (cat)
/DollarMultiValuesArrays/command/value (dir$)
/DollarMultiValuesArrays/command/value (echo)
/DollarMultiValuesArrays/command/value ($env)
/DollarMultiValuesArrays/command/value (cd)
/DollarMultiValuesArrays/command/value (..)
/DollarMultiValuesArrays/command/value (ln)
/DollarMultiValuesArrays/command/value (-sf)
/DollarMultiValuesArrays/command/value ($bin)
/DollarMultiValuesArrays/command/value (.)
/DollarMultiValuesArrays/command/value (done)
/DollarMultiValuesArrays/command/' (')
/DollarMultiValuesArrays/term ([])
)INPUT";
    std::stringstream actual_paths;
    interpreter.root().paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());

    // Check count of total nodes in tree and count of children of root
    ASSERT_EQ(207, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(3, document.child_count());

    // Remove end space from line to match expectation and check round trip
    std::string input_string = input.str();
    std::string line_with_end_space = "param13 = trailing-single-space$ \n";
    std::string line_sans_end_space = "param13 = trailing-single-space$\n";
    input_string.replace(input_string.find(line_with_end_space),
                         line_with_end_space.size(), line_sans_end_space);
    ASSERT_EQ(input_string, "\n" + document.data() + "\n");
}

/**
 * @brief Test HIT syntax - exp
 */
TEST(HITInterpreter, expression_w_prefix_and_suffix)
{
    std::stringstream input; 
    input << "key = 'x_${y=a b c}_z'";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::string expected_paths;
    expected_paths = R"INPUT(/
/key
/key/decl (key)
/key/= (=)
/key/' (')
/key/value (x_${y=a b c}_z)
/key/' (')
)INPUT";
    std::stringstream actual_paths;
    interpreter.root().paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - exp
 */
TEST(HITInterpreter, expression_w_prefix)
{
    std::stringstream input; 
    input << "key = 'x_${y=a b c}'";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::string expected_paths;
    expected_paths = R"INPUT(/
/key
/key/decl (key)
/key/= (=)
/key/' (')
/key/value (x_${y=a b c})
/key/' (')
)INPUT";
    std::stringstream actual_paths;
    interpreter.root().paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - exp
 */
TEST(HITInterpreter, expression_w_suffix)
{
    std::stringstream input; 
    input << "key = '${y=a b c}_z'";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::string expected_paths;
    expected_paths = R"INPUT(/
/key
/key/decl (key)
/key/= (=)
/key/' (')
/key/value (${y=a b c}_z)
/key/' (')
)INPUT";
    std::stringstream actual_paths;
    interpreter.root().paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - exp
 */
TEST(HITInterpreter, expression_unquoted_w_suffix)
{
    std::stringstream input; 
    input << "key = ${y=a b c}_z";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::string expected_paths;
    expected_paths = R"INPUT(/
/key
/key/decl (key)
/key/= (=)
/key/value (${y=a b c}_z)
)INPUT";
    std::stringstream actual_paths;
    interpreter.root().paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - exp
 */
TEST(HITInterpreter, expression_unquoted_w_prefix)
{
    std::stringstream input; 
    input << "key = x_${y=a b c}";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::string expected_paths;
    expected_paths = R"INPUT(/
/key
/key/decl (key)
/key/= (=)
/key/value (x_${y=a b c})
)INPUT";
    std::stringstream actual_paths;
    interpreter.root().paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HIT syntax - exp
 */
TEST(HITInterpreter, expression_variant)
{
    std::stringstream input; 
    input << "G2 = ${fparse ${units 505.97 J/(kg*K) -> eV/(g*K)}*ru0*(Temp_-Theta_e_mb)}";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::string expected_paths;
    expected_paths = R"INPUT(/
/G2
/G2/decl (G2)
/G2/= (=)
/G2/value (${fparse ${units 505.97 J/(kg*K) -> eV/(g*K)}*ru0*(Temp_-Theta_e_mb)})
)INPUT";
    std::stringstream actual_paths;
    interpreter.root().paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}


/**
 * @brief Test HIT syntax - exp
 */
TEST(HITInterpreter, expression_sandwich_variant)
{
    std::stringstream input; 
    input << R"INPUT(k1 = x_${a}_y_${b}_z
k2 = ${a}_y_${b}
k3 = ${a}_y_${b}${c}
k4 = ${a}_y_${b}${c}_z
value = (1.0/2.0)*(eps*eps*(-grad_grad_p_x+${gx}*grad_rho_x)+2.0*eps*grad_eps*(-grad_p_x+rho*${gx}sdfs))
)INPUT";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    std::string expected_paths;
    expected_paths = R"INPUT(/
/k1
/k1/decl (k1)
/k1/= (=)
/k1/value (x_${a}_y_${b}_z)
/k2
/k2/decl (k2)
/k2/= (=)
/k2/value (${a}_y_${b})
/k3
/k3/decl (k3)
/k3/= (=)
/k3/value (${a}_y_${b}${c})
/k4
/k4/decl (k4)
/k4/= (=)
/k4/value (${a}_y_${b}${c}_z)
/value
/value/decl (value)
/value/= (=)
/value/value ((1.0/2.0)*(eps*eps*(-grad_grad_p_x+${gx}*grad_rho_x)+2.0*eps*grad_eps*(-grad_p_x+rho*${gx}sdfs)))
)INPUT";
    std::stringstream actual_paths;
    interpreter.root().paths(actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test HITNodeView set_type - change type of parent node and leaf node
 */
TEST(HITInterpreter, set_type)
{
    std::stringstream input;
    input << R"INPUT(
[BlockObject]
  # blankline
  a = 100.001
[]
)INPUT";

    // Check parse success, total node count, child count of document root
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_EQ(11, interpreter.node_count());
    HITNodeView document = interpreter.root();
    ASSERT_EQ(1, document.child_count());

    // BlockObject - parent node with original parsed type of wasp::OBJECT
    // children - (LBRACKET,DECL,RBRACKET,COMMENT,KEYED_VALUE,OBJECT_TERM)
    HITNodeView object_node = document.child_at(0);
    ASSERT_EQ(std::string("BlockObject"), object_node.name());
    ASSERT_FALSE(object_node.is_null());
    ASSERT_EQ(wasp::OBJECT, object_node.type());
    ASSERT_FALSE(object_node.is_decorative());
    ASSERT_FALSE(object_node.is_leaf());

    // Change non-decorative wasp::OBJECT into non-decorative wasp::PARENT
    object_node.set_type(wasp::PARENT);
    ASSERT_EQ(wasp::PARENT, object_node.type());
    ASSERT_FALSE(object_node.is_decorative());
    ASSERT_FALSE(object_node.is_leaf());

    // Check object non-decorative children count pre child types changing
    ASSERT_EQ(6, object_node.child_count());
    ASSERT_EQ(1, object_node.non_decorative_children_count());

    // BlockObject/comment - leaf node with original type of wasp::COMMENT
    HITNodeView comment_node = object_node.child_at(3);
    ASSERT_EQ(std::string("comment"), comment_node.name());
    ASSERT_FALSE(comment_node.is_null());
    ASSERT_EQ(wasp::COMMENT, comment_node.type());
    ASSERT_TRUE(comment_node.is_decorative());
    ASSERT_TRUE(comment_node.is_leaf());
    ASSERT_EQ("# blankline", comment_node.last_as_string());

    // Change from decorative wasp::COMMENT to decorative wasp::BLANK_LINE
    comment_node.set_type(wasp::BLANK_LINE);
    ASSERT_EQ(wasp::BLANK_LINE, comment_node.type());
    ASSERT_TRUE(comment_node.is_decorative());
    ASSERT_TRUE(comment_node.is_leaf());
    ASSERT_EQ("# blankline", comment_node.last_as_string());

    // BlockObject/a - parent node with original type of wasp::KEYED_VALUE
    HITNodeView keyval_node = object_node.child_at(4);
    ASSERT_EQ(std::string("a"), keyval_node.name());
    ASSERT_FALSE(keyval_node.is_null());
    ASSERT_EQ(wasp::KEYED_VALUE, keyval_node.type());
    ASSERT_FALSE(keyval_node.is_decorative());
    ASSERT_FALSE(keyval_node.is_leaf());
    ASSERT_EQ("100.001", keyval_node.last_as_string());

    // Change non-decorative wasp::KEYED_VALUE to decorative wasp::COMMENT
    keyval_node.set_type(wasp::COMMENT);
    ASSERT_EQ(wasp::COMMENT, keyval_node.type());
    ASSERT_TRUE(keyval_node.is_decorative());
    ASSERT_FALSE(keyval_node.is_leaf());
    ASSERT_EQ("100.001", keyval_node.last_as_string());

    // Check object non-decorative children count post child types changes
    ASSERT_EQ(6, object_node.child_count());
    ASSERT_EQ(0, object_node.non_decorative_children_count());
}


/**
 * @brief Test HIT file include scenario involving the Iterator class
 */
TEST(HITInterpreter, include_scenario_abc)
{

    std::string root_file = test_dir+"/inputs/fileA.i";
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parseFile(root_file));
    HITNodeView root = interpreter.root();
    std::stringstream actual_list;
    // list the tree using node iterators
    // that incorporate the file include logic
    wasp::tree_list(root, actual_list);
    std::stringstream expected_list;
    // Expected is that 
    // fileA.i includes fileB.i on line 1, nested on line 5
    // and trailing on line 8
    // where fileB.i includes a simple C variable.
    expected_list << R"INPUT(/
/C
/C/decl (C)
/C/= (=)
/C/value (5.0)
/B
/B/[ ([)
/B/decl (B)
/B/] (])
/B/test_b
/B/test_b/[ ([)
/B/test_b/decl (test_b)
/B/test_b/] (])
/B/test_b/value
/B/test_b/value/decl (value)
/B/test_b/value/= (=)
/B/test_b/value/value (${C})
/B/test_b/term ([])
/B/term ([])
/A
/A/[ ([)
/A/decl (A)
/A/] (])
/A/comment (# nested)
/A/C
/A/C/decl (C)
/A/C/= (=)
/A/C/value (5.0)
/A/B
/A/B/[ ([)
/A/B/decl (B)
/A/B/] (])
/A/B/test_b
/A/B/test_b/[ ([)
/A/B/test_b/decl (test_b)
/A/B/test_b/] (])
/A/B/test_b/value
/A/B/test_b/value/decl (value)
/A/B/test_b/value/= (=)
/A/B/test_b/value/value (${C})
/A/B/test_b/term ([])
/A/B/term ([])
/A/term ([])
/C
/C/decl (C)
/C/= (=)
/C/value (5.0)
/B
/B/[ ([)
/B/decl (B)
/B/] (])
/B/test_b
/B/test_b/[ ([)
/B/test_b/decl (test_b)
/B/test_b/] (])
/B/test_b/value
/B/test_b/value/decl (value)
/B/test_b/value/= (=)
/B/test_b/value/value (${C})
/B/test_b/term ([])
/B/term ([])
)INPUT";
    ASSERT_EQ(expected_list.str(), actual_list.str());
}


/**
 * @brief Test missing value
 */
TEST(HITInterpreter, missing_value)
{
    std::stringstream input;
    input << "key1 =";

    // Check parse success, total node count, child count of document root
    std::stringstream errors;
    DefaultHITInterpreter interpreter(errors);
    ASSERT_FALSE(interpreter.parse(input));
    ASSERT_EQ("stream input:1.7: syntax error, unexpected end of file\nstream input:1.6: syntax error, 'key1' has a missing or malformed value\n", errors.str());
    HITNodeView document = interpreter.root();
    ASSERT_FALSE(document.is_null());

    // Ensure all syntax is captured in the tree
    std::string expected_paths;
    expected_paths = R"(
/                  )" + std::to_string(wasp::DOCUMENT_ROOT) + R"(
/key1             )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/key1/decl         )" + std::to_string(wasp::DECL)          + R"( (key1)
/key1/=            )" + std::to_string(wasp::ASSIGN)        + R"( (=)
)";
    expected_paths.erase(expected_paths.begin());
    std::stringstream actual_paths;
    wasp::node_paths_and_types(document, actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str()); 
}

/**
 * @brief Test missing value
 */
TEST(HITInterpreter, recovery_missing_value_in_block)
{
    std::stringstream input;
    input << R"INPUT(
[block]
   key1 =
[])INPUT";

    // Check parse success, total node count, child count of document root
    std::stringstream errors;
    DefaultHITInterpreter interpreter(errors);
    ASSERT_FALSE(interpreter.parse(input));
    ASSERT_EQ("stream input:4.1: syntax error, unexpected end of line\nstream input:3.9: syntax error, 'key1' has a missing or malformed value\n", errors.str());
    HITNodeView document = interpreter.root();
    ASSERT_FALSE(document.is_null());

    // Ensure all syntax is captured in the tree
    std::string expected_paths;
    expected_paths = R"(
/                  )" + std::to_string(wasp::DOCUMENT_ROOT) + R"(
/block            )"  + std::to_string(wasp::OBJECT)        + R"(
/block/[          )"  + std::to_string(wasp::LBRACKET)      + R"( ([)
/block/decl        )" + std::to_string(wasp::DECL)          + R"( (block)
/block/]          )"  + std::to_string(wasp::RBRACKET)      + R"( (])
/block/key1       )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/block/key1/decl   )" + std::to_string(wasp::DECL)          + R"( (key1)
/block/key1/=      )" + std::to_string(wasp::ASSIGN)        + R"( (=)
/block/term       )"  + std::to_string(wasp::OBJECT_TERM)   + R"( ([])
)";
    expected_paths.erase(expected_paths.begin());
    std::stringstream actual_paths;
    wasp::node_paths_and_types(document, actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());    
}


/**
 * @brief Test recovery from missing value following a correct key=value 
 * This scenario includes object terminator
 */
TEST(HITInterpreter, recovery_missing_value_in_block2)
{
    std::stringstream input;
    input << R"INPUT(
[block]
   key1 = 3.14159
   key2 = [])INPUT";

    // Check parse success, total node count, child count of document root
    std::stringstream errors;
    DefaultHITInterpreter interpreter(errors);
    ASSERT_FALSE(interpreter.parse(input));
    ASSERT_EQ("stream input:4.11: syntax error, unexpected [\nstream input:4.9: syntax error, 'key2' has a missing or malformed value\n", errors.str());
    HITNodeView document = interpreter.root();
    ASSERT_FALSE(document.is_null());

        std::string expected_paths;
    expected_paths = R"(
/                  )" + std::to_string(wasp::DOCUMENT_ROOT) + R"(
/block            )"  + std::to_string(wasp::OBJECT)        + R"(
/block/[          )"  + std::to_string(wasp::LBRACKET)      + R"( ([)
/block/decl        )" + std::to_string(wasp::DECL)          + R"( (block)
/block/]          )"  + std::to_string(wasp::RBRACKET)      + R"( (])
/block/key1       )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/block/key1/decl   )" + std::to_string(wasp::DECL)          + R"( (key1)
/block/key1/=      )" + std::to_string(wasp::ASSIGN)        + R"( (=)
/block/key1/value )"  + std::to_string(wasp::VALUE)         + R"( (3.14159)
/block/key2       )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/block/key2/decl   )" + std::to_string(wasp::DECL)          + R"( (key2)
/block/key2/=      )" + std::to_string(wasp::ASSIGN)        + R"( (=)
/block/term       )"  + std::to_string(wasp::OBJECT_TERM)   + R"( ([])
)";
    expected_paths.erase(expected_paths.begin());
    std::stringstream actual_paths;
    wasp::node_paths_and_types(document, actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test recovery from missing value followed by a correct key=value
 */
TEST(HITInterpreter, recovery_missing_value_in_block3)
{
    std::stringstream input;
    input << R"INPUT(
[block]
   key1 = 
   key2 = 3.14159  
[])INPUT";

    // Check parse success, total node count, child count of document root
    std::stringstream errors;
    DefaultHITInterpreter interpreter(errors);
    ASSERT_FALSE(interpreter.parse(input));
    ASSERT_EQ("stream input:4.1: syntax error, unexpected end of line\nstream input:3.9: syntax error, 'key1' has a missing or malformed value\n", errors.str());
    HITNodeView document = interpreter.root();
    ASSERT_FALSE(document.is_null());

        std::string expected_paths;
    expected_paths = R"(
/                  )" + std::to_string(wasp::DOCUMENT_ROOT) + R"(
/block            )"  + std::to_string(wasp::OBJECT)        + R"(
/block/[          )"  + std::to_string(wasp::LBRACKET)      + R"( ([)
/block/decl        )" + std::to_string(wasp::DECL)          + R"( (block)
/block/]          )"  + std::to_string(wasp::RBRACKET)      + R"( (])
/block/key1       )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/block/key1/decl   )" + std::to_string(wasp::DECL)          + R"( (key1)
/block/key1/=      )" + std::to_string(wasp::ASSIGN)        + R"( (=)
/block/key2       )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/block/key2/decl   )" + std::to_string(wasp::DECL)          + R"( (key2)
/block/key2/=      )" + std::to_string(wasp::ASSIGN)        + R"( (=)
/block/key2/value )"  + std::to_string(wasp::VALUE)         + R"( (3.14159)
/block/term       )"  + std::to_string(wasp::OBJECT_TERM)   + R"( ([])
)";
    expected_paths.erase(expected_paths.begin());
    std::stringstream actual_paths;
    wasp::node_paths_and_types(document, actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

 /* @brief Test recovery from missing value followed by a correct key=value
 */
TEST(HITInterpreter, recovery_repeated_missing_value_in_block)
{
    std::stringstream input;
    input << R"INPUT(
[block]
   key1 = 
   key2 = 
[])INPUT";

    // Check parse success, total node count, child count of document root
    std::stringstream errors;
    DefaultHITInterpreter interpreter(errors);
    ASSERT_FALSE(interpreter.parse(input));
    ASSERT_EQ("stream input:4.1: syntax error, unexpected end of line\nstream input:3.9: syntax error, 'key1' has a missing or malformed value\nstream input:4.9: syntax error, 'key2' has a missing or malformed value\n", errors.str());
    HITNodeView document = interpreter.root();
    ASSERT_FALSE(document.is_null());

        std::string expected_paths;
    expected_paths = R"(
/                  )" + std::to_string(wasp::DOCUMENT_ROOT) + R"(
/block            )"  + std::to_string(wasp::OBJECT)        + R"(
/block/[          )"  + std::to_string(wasp::LBRACKET)      + R"( ([)
/block/decl        )" + std::to_string(wasp::DECL)          + R"( (block)
/block/]          )"  + std::to_string(wasp::RBRACKET)      + R"( (])
/block/key1       )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/block/key1/decl   )" + std::to_string(wasp::DECL)          + R"( (key1)
/block/key1/=      )" + std::to_string(wasp::ASSIGN)        + R"( (=)
/block/key2       )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/block/key2/decl   )" + std::to_string(wasp::DECL)          + R"( (key2)
/block/key2/=      )" + std::to_string(wasp::ASSIGN)        + R"( (=)
/block/term       )"  + std::to_string(wasp::OBJECT_TERM)   + R"( ([])
)";
    expected_paths.erase(expected_paths.begin());
    std::stringstream actual_paths;
    wasp::node_paths_and_types(document, actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

 /* @brief Test recovery from missing value followed by a correct key=value
 */
TEST(HITInterpreter, recovery_repeated_missing_value_in_block_with_following)
{
    std::stringstream input;
    input << R"INPUT(
[block]
   key1 = 
[]
key2 = 
)INPUT";

    // Check parse success, total node count, child count of document root
    std::stringstream errors;
    DefaultHITInterpreter interpreter(errors);
    ASSERT_FALSE(interpreter.parse(input));
    ASSERT_EQ("stream input:4.1: syntax error, unexpected end of line\nstream input:3.9: syntax error, 'key1' has a missing or malformed value\nstream input:6.1: syntax error, unexpected end of line\nstream input:5.6: syntax error, 'key2' has a missing or malformed value\n", errors.str());
    HITNodeView document = interpreter.root();
    ASSERT_FALSE(document.is_null());

        std::string expected_paths;
    expected_paths = R"(
/                  )" + std::to_string(wasp::DOCUMENT_ROOT) + R"(
/block            )"  + std::to_string(wasp::OBJECT)        + R"(
/block/[          )"  + std::to_string(wasp::LBRACKET)      + R"( ([)
/block/decl        )" + std::to_string(wasp::DECL)          + R"( (block)
/block/]          )"  + std::to_string(wasp::RBRACKET)      + R"( (])
/block/key1       )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/block/key1/decl   )" + std::to_string(wasp::DECL)          + R"( (key1)
/block/key1/=      )" + std::to_string(wasp::ASSIGN)        + R"( (=)
/block/term       )"  + std::to_string(wasp::OBJECT_TERM)   + R"( ([])
/key2             )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/key2/decl         )" + std::to_string(wasp::DECL)          + R"( (key2)
/key2/=            )" + std::to_string(wasp::ASSIGN)        + R"( (=)
)";
    expected_paths.erase(expected_paths.begin());
    std::stringstream actual_paths;
    wasp::node_paths_and_types(document, actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/* @brief Test recovery from missing value followed by a correct key=value
 */
TEST(HITInterpreter, recovery_repeated_missing_value_at_root)
{
    std::stringstream input;
    input << R"INPUT(
   key1 = 
   key2 = 
)INPUT";

    // Check parse success, total node count, child count of document root
    std::stringstream errors;
    DefaultHITInterpreter interpreter(errors);
    ASSERT_FALSE(interpreter.parse(input));
    ASSERT_EQ("stream input:3.1: syntax error, unexpected end of line\nstream input:2.9: syntax error, 'key1' has a missing or malformed value\nstream input:3.9: syntax error, 'key2' has a missing or malformed value\n", errors.str());
    HITNodeView document = interpreter.root();
    ASSERT_FALSE(document.is_null());

        std::string expected_paths;
    expected_paths = R"(
/                  )" + std::to_string(wasp::DOCUMENT_ROOT) + R"(
/key1             )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/key1/decl         )" + std::to_string(wasp::DECL)          + R"( (key1)
/key1/=            )" + std::to_string(wasp::ASSIGN)        + R"( (=)
/key2             )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/key2/decl         )" + std::to_string(wasp::DECL)          + R"( (key2)
/key2/=            )" + std::to_string(wasp::ASSIGN)        + R"( (=)
)";
    expected_paths.erase(expected_paths.begin());
    std::stringstream actual_paths;
    wasp::node_paths_and_types(document, actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/* @brief Test recovery from missing value followed by a correct key=value
 */
TEST(HITInterpreter, recovery_missing_value_at_root_w_comment)
{
    std::stringstream input;
    input << R"INPUT(
   key1 = # a comment instead of expected value
   key2 = 3.1415
)INPUT";

    // Check parse success, total node count, child count of document root
    std::stringstream errors;
    DefaultHITInterpreter interpreter(errors);
    ASSERT_FALSE(interpreter.parse(input));
    ASSERT_EQ("stream input:2.11-47: syntax error, unexpected comment\nstream input:2.9: syntax error, 'key1' has a missing or malformed value\n", errors.str());
    HITNodeView document = interpreter.root();
    ASSERT_FALSE(document.is_null());

        std::string expected_paths;
    expected_paths = R"(
/                  )" + std::to_string(wasp::DOCUMENT_ROOT) + R"(
/key1             )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/key1/decl         )" + std::to_string(wasp::DECL)          + R"( (key1)
/key1/=            )" + std::to_string(wasp::ASSIGN)        + R"( (=)
/comment          )"  + std::to_string(wasp::COMMENT)       + R"( (# a comment instead of expected value)
/key2             )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/key2/decl         )" + std::to_string(wasp::DECL)          + R"( (key2)
/key2/=            )" + std::to_string(wasp::ASSIGN)        + R"( (=)
/key2/value       )"  + std::to_string(wasp::VALUE)         + R"( (3.1415)
)";
    expected_paths.erase(expected_paths.begin());
    std::stringstream actual_paths;
    wasp::node_paths_and_types(document, actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/* @brief Test recovery from missing value in a block followed by a correct key=value
 */
TEST(HITInterpreter, recovery_missing_value_in_block_w_comment)
{
    std::stringstream input;
    input << R"INPUT([block]        
   key1 = # a comment instead of expected value
   key2 = 3.1415
[]
)INPUT";

    // Check parse success, total node count, child count of document root
    std::stringstream errors;
    DefaultHITInterpreter interpreter(errors);
    ASSERT_FALSE(interpreter.parse(input));
    ASSERT_EQ("stream input:2.11-47: syntax error, unexpected comment\nstream input:2.9: syntax error, 'key1' has a missing or malformed value\n", errors.str());
    HITNodeView document = interpreter.root();
    ASSERT_FALSE(document.is_null());

        std::string expected_paths;
    expected_paths = R"(
/                  )" + std::to_string(wasp::DOCUMENT_ROOT) + R"(
/block            )"  + std::to_string(wasp::OBJECT)        + R"(
/block/[          )"  + std::to_string(wasp::LBRACKET)      + R"( ([)
/block/decl        )" + std::to_string(wasp::DECL)          + R"( (block)
/block/]          )"  + std::to_string(wasp::RBRACKET)      + R"( (])
/block/key1       )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/block/key1/decl   )" + std::to_string(wasp::DECL)          + R"( (key1)
/block/key1/=      )" + std::to_string(wasp::ASSIGN)        + R"( (=)
/block/comment    )"  + std::to_string(wasp::COMMENT)       + R"( (# a comment instead of expected value)
/block/key2       )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/block/key2/decl   )" + std::to_string(wasp::DECL)          + R"( (key2)
/block/key2/=      )" + std::to_string(wasp::ASSIGN)        + R"( (=)
/block/key2/value )"  + std::to_string(wasp::VALUE)         + R"( (3.1415)
/block/term       )"  + std::to_string(wasp::OBJECT_TERM)   + R"( ([])
)";
    expected_paths.erase(expected_paths.begin());
    std::stringstream actual_paths;
    wasp::node_paths_and_types(document, actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}


/* @brief Test recovery from missing terminator for a block
 */
TEST(HITInterpreter, recovery_unclosed_block_without_content)
{
    std::stringstream input;
    input << R"INPUT([block]
)INPUT";

    // Check parse success, total node count, child count of document root
    std::stringstream errors;
    DefaultHITInterpreter interpreter(errors);
    ASSERT_FALSE(interpreter.parse(input));
    ASSERT_EQ("stream input:2.1: syntax error, unexpected end of file\n", errors.str());
    HITNodeView document = interpreter.root();
    ASSERT_FALSE(document.is_null());

        std::string expected_paths;
    expected_paths = R"(
/                  )" + std::to_string(wasp::DOCUMENT_ROOT) + R"(
/block            )"  + std::to_string(wasp::OBJECT)        + R"(
/block/[          )"  + std::to_string(wasp::LBRACKET)      + R"( ([)
/block/decl        )" + std::to_string(wasp::DECL)          + R"( (block)
/block/]          )"  + std::to_string(wasp::RBRACKET)      + R"( (])
)";
    expected_paths.erase(expected_paths.begin());
    std::stringstream actual_paths;
    wasp::node_paths_and_types(document, actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}
/* @brief Test recovery from missing terminator for a block
 */
TEST(HITInterpreter, recovery_unclosed_block_with_content)
{
    std::stringstream input;
    input << R"INPUT([block]        
   x=y
)INPUT";

    // Check parse success, total node count, child count of document root
    std::stringstream errors;
    DefaultHITInterpreter interpreter(errors);
    ASSERT_FALSE(interpreter.parse(input));
    ASSERT_EQ("stream input:1.1: syntax error, unexpected end of file, expecting block terminator\n", errors.str());
    HITNodeView document = interpreter.root();
    ASSERT_FALSE(document.is_null());

        std::string expected_paths;
    expected_paths = R"(
/                  )" + std::to_string(wasp::DOCUMENT_ROOT) + R"(
/block            )"  + std::to_string(wasp::OBJECT)        + R"(
/block/[          )"  + std::to_string(wasp::LBRACKET)      + R"( ([)
/block/decl        )" + std::to_string(wasp::DECL)          + R"( (block)
/block/]          )"  + std::to_string(wasp::RBRACKET)      + R"( (])
/block/x          )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/block/x/decl      )" + std::to_string(wasp::DECL)          + R"( (x)
/block/x/=         )" + std::to_string(wasp::ASSIGN)        + R"( (=)
/block/x/value    )"  + std::to_string(wasp::VALUE)         + R"( (y)
)";
    expected_paths.erase(expected_paths.begin());
    std::stringstream actual_paths;
    wasp::node_paths_and_types(document, actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/* @brief Test recovery from missing assign in a block
 */
TEST(HITInterpreter, recovery_missing_assign_in_block)
{
    std::stringstream input;
    input << R"INPUT([block]
   x
)INPUT";

    // Check parse success, total node count, child count of document root
    std::stringstream errors;
    DefaultHITInterpreter interpreter(errors);
    ASSERT_FALSE(interpreter.parse(input));
    ASSERT_EQ("stream input:3.1: syntax error, unexpected end of line, expecting = or :=\nstream input:1.1: syntax error, unexpected end of file, expecting block terminator\n", errors.str());
    HITNodeView document = interpreter.root();
    ASSERT_FALSE(document.is_null());

        std::string expected_paths;
    expected_paths = R"(
/                  )" + std::to_string(wasp::DOCUMENT_ROOT) + R"(
/block            )"  + std::to_string(wasp::OBJECT)        + R"(
/block/[          )"  + std::to_string(wasp::LBRACKET)      + R"( ([)
/block/decl        )" + std::to_string(wasp::DECL)          + R"( (block)
/block/]          )"  + std::to_string(wasp::RBRACKET)      + R"( (])
/block/x          )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/block/x/decl      )" + std::to_string(wasp::DECL)          + R"( (x)
)";
    expected_paths.erase(expected_paths.begin());
    std::stringstream actual_paths;
    wasp::node_paths_and_types(document, actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}
/* @brief Test recovery from missing assign in a block with no newline, just EOF
 */
TEST(HITInterpreter, recovery_missing_assign_in_block_w_eof)
{
    std::stringstream input;
    input << R"INPUT([block]
   x)INPUT";

    // Check parse success, total node count, child count of document root
    std::stringstream errors;
    DefaultHITInterpreter interpreter(errors);
    ASSERT_FALSE(interpreter.parse(input));
    ASSERT_EQ("stream input:2.5: syntax error, unexpected end of file, expecting = or :=\nstream input:1.1: syntax error, unexpected end of file, expecting block terminator\n", errors.str());
    HITNodeView document = interpreter.root();
    ASSERT_FALSE(document.is_null());

    std::string expected_paths;
    expected_paths = R"(
/                  )" + std::to_string(wasp::DOCUMENT_ROOT) + R"(
/block            )"  + std::to_string(wasp::OBJECT)        + R"(
/block/[          )"  + std::to_string(wasp::LBRACKET)      + R"( ([)
/block/decl        )" + std::to_string(wasp::DECL)          + R"( (block)
/block/]          )"  + std::to_string(wasp::RBRACKET)      + R"( (])
/block/x          )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/block/x/decl      )" + std::to_string(wasp::DECL)          + R"( (x)
)";
    expected_paths.erase(expected_paths.begin());
    std::stringstream actual_paths;
    wasp::node_paths_and_types(document, actual_paths);
    ASSERT_EQ(expected_paths, actual_paths.str());
}

/**
 * @brief Test recovery from missing value in a block without a terminator
 */
TEST(HITInterpreter, recovery_missing_value_in_unclosed_block)
{
    std::stringstream input;
    input << R"INPUT([block]
  param =
)INPUT";

    std::string expect_error = R"INPUT(
stream input:3.1: syntax error, unexpected end of line
stream input:2.9: syntax error, 'param' has a missing or malformed value
stream input:1.1: syntax error, unexpected end of file, expecting block terminator
)INPUT";

    std::string expect_paths_and_types = R"(
/                  )" + std::to_string(wasp::DOCUMENT_ROOT) + R"(
/block            )"  + std::to_string(wasp::OBJECT)        + R"(
/block/[          )"  + std::to_string(wasp::LBRACKET)      + R"( ([)
/block/decl        )" + std::to_string(wasp::DECL)          + R"( (block)
/block/]          )"  + std::to_string(wasp::RBRACKET)      + R"( (])
/block/param      )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/block/param/decl  )" + std::to_string(wasp::DECL)          + R"( (param)
/block/param/=     )" + std::to_string(wasp::ASSIGN)        + R"( (=)
)";

    // Check parse failure, error message, non-null root, paths, and types
    std::stringstream actual_error, actual_paths_and_types;
    DefaultHITInterpreter interpreter(actual_error);
    ASSERT_FALSE(interpreter.parse(input));
    ASSERT_EQ(expect_error, "\n" + actual_error.str());
    ASSERT_FALSE(interpreter.root().is_null());
    wasp::node_paths_and_types(interpreter.root(), actual_paths_and_types);
    ASSERT_EQ(expect_paths_and_types, "\n" + actual_paths_and_types.str());
}

/**
 * @brief Test findNodeUnderLineColumn when column is past end of document
 */
TEST(HITInterpreter, find_node_column_past_document_end)
{
    std::stringstream input;
    input << R"INPUT(

[block]
  param =

)INPUT";

    // Check parse fails but document root is not null from error recovery
    std::stringstream error;
    DefaultHITInterpreter interpreter(error);
    ASSERT_FALSE(interpreter.parse(input));
    HITNodeView document_root = interpreter.root();
    ASSERT_FALSE(document_root.is_null());

    // Check line and column range that is spanned by entire document root
    ASSERT_EQ(3, document_root.line());
    ASSERT_EQ(1, document_root.column());
    ASSERT_EQ(4, document_root.last_line());
    ASSERT_EQ(9, document_root.last_column());

    // Check findNodeUnderLineColumn with column past end of document root
    HITNodeView found = wasp::findNodeUnderLineColumn(document_root, 4, 11);
    ASSERT_FALSE(found.is_null());
    ASSERT_EQ(document_root, found);
}


/**
 * @brief Test recovery from missing terminator block with unexpected EOF 
 */
TEST(HITInterpreter, recovery_missing_value_in_unclosed_block_eof)
{
    std::stringstream input;
    input << R"INPUT([block]
  param = true)INPUT";

    std::string expect_error = R"INPUT(
stream input:1.1: syntax error, unexpected end of file, expecting block terminator
)INPUT";

    std::string expect_paths_and_types = R"(
/                  )" + std::to_string(wasp::DOCUMENT_ROOT) + R"(
/block            )"  + std::to_string(wasp::OBJECT)        + R"(
/block/[          )"  + std::to_string(wasp::LBRACKET)      + R"( ([)
/block/decl        )" + std::to_string(wasp::DECL)          + R"( (block)
/block/]          )"  + std::to_string(wasp::RBRACKET)      + R"( (])
/block/param      )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/block/param/decl  )" + std::to_string(wasp::DECL)          + R"( (param)
/block/param/=     )" + std::to_string(wasp::ASSIGN)        + R"( (=)
/block/param/value )" + std::to_string(wasp::VALUE)        + R"( (true)
)";

    // Check parse failure, error message, non-null root, paths, and types
    std::stringstream actual_error, actual_paths_and_types;
    DefaultHITInterpreter interpreter(actual_error);
    ASSERT_FALSE(interpreter.parse(input));
    ASSERT_EQ(expect_error, "\n" + actual_error.str());
    ASSERT_FALSE(interpreter.root().is_null());
    wasp::node_paths_and_types(interpreter.root(), actual_paths_and_types);
    ASSERT_EQ(expect_paths_and_types, "\n" + actual_paths_and_types.str());
}
/**
 * @brief Test recovery from missing value in a block without a terminator 
 */
TEST(HITInterpreter, recovery_missing_value_eof)
{
    std::stringstream input;
    input << R"INPUT([block]
  param =)INPUT";

    std::string expect_error = R"INPUT(
stream input:2.10: syntax error, unexpected end of file
stream input:2.9: syntax error, 'param' has a missing or malformed value
stream input:1.1: syntax error, unexpected end of file, expecting block terminator
)INPUT";

    std::string expect_paths_and_types = R"(
/                  )" + std::to_string(wasp::DOCUMENT_ROOT) + R"(
/block            )"  + std::to_string(wasp::OBJECT)        + R"(
/block/[          )"  + std::to_string(wasp::LBRACKET)      + R"( ([)
/block/decl        )" + std::to_string(wasp::DECL)          + R"( (block)
/block/]          )"  + std::to_string(wasp::RBRACKET)      + R"( (])
/block/param      )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/block/param/decl  )" + std::to_string(wasp::DECL)          + R"( (param)
/block/param/=     )" + std::to_string(wasp::ASSIGN)        + R"( (=)
)";

    // Check parse failure, error message, non-null root, paths, and types
    std::stringstream actual_error, actual_paths_and_types;
    DefaultHITInterpreter interpreter(actual_error);
    ASSERT_FALSE(interpreter.parse(input));
    ASSERT_EQ(expect_error, "\n" + actual_error.str());
    ASSERT_FALSE(interpreter.root().is_null());
    wasp::node_paths_and_types(interpreter.root(), actual_paths_and_types);
    ASSERT_EQ(expect_paths_and_types, "\n" + actual_paths_and_types.str());
}

/**
 * @brief Test recovery from missing assign in a block without a terminator 
 */
TEST(HITInterpreter, recovery_missing_assign_eof)
{
    std::stringstream input;
    input << R"INPUT([block]
  param)INPUT";

    std::string expect_error = R"INPUT(
stream input:2.8: syntax error, unexpected end of file, expecting = or :=
stream input:1.1: syntax error, unexpected end of file, expecting block terminator
)INPUT";

    std::string expect_paths_and_types = R"(
/                  )" + std::to_string(wasp::DOCUMENT_ROOT) + R"(
/block            )"  + std::to_string(wasp::OBJECT)        + R"(
/block/[          )"  + std::to_string(wasp::LBRACKET)      + R"( ([)
/block/decl        )" + std::to_string(wasp::DECL)          + R"( (block)
/block/]          )"  + std::to_string(wasp::RBRACKET)      + R"( (])
/block/param      )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/block/param/decl  )" + std::to_string(wasp::DECL)          + R"( (param)
)";

    // Check parse failure, error message, non-null root, paths, and types
    std::stringstream actual_error, actual_paths_and_types;
    DefaultHITInterpreter interpreter(actual_error);
    ASSERT_FALSE(interpreter.parse(input));
    ASSERT_EQ(expect_error, "\n" + actual_error.str());
    ASSERT_FALSE(interpreter.root().is_null());
    wasp::node_paths_and_types(interpreter.root(), actual_paths_and_types);
    ASSERT_EQ(expect_paths_and_types, "\n" + actual_paths_and_types.str());
}

/**
 * @brief Test recovery from missing assign in a block without a terminator 
 */
TEST(HITInterpreter, recovery_partial_block_decl)
{
    std::stringstream input;
    input << R"INPUT([blo)INPUT";

    std::string expect_error = R"INPUT(
stream input:1.5: syntax error, unexpected end of file, expecting ]
)INPUT";

    std::string expect_paths_and_types = R"(
/                  )" + std::to_string(wasp::DOCUMENT_ROOT) + R"(
/blo              )"  + std::to_string(wasp::OBJECT)        + R"(
/blo/[            )"  + std::to_string(wasp::LBRACKET)      + R"( ([)
/blo/decl          )" + std::to_string(wasp::DECL)          + R"( (blo)
)";

    // Check parse failure, error message, non-null root, paths, and types
    std::stringstream actual_error, actual_paths_and_types;
    DefaultHITInterpreter interpreter(actual_error);
    ASSERT_FALSE(interpreter.parse(input));
    ASSERT_EQ(expect_error, "\n" + actual_error.str());
    ASSERT_FALSE(interpreter.root().is_null());
    wasp::node_paths_and_types(interpreter.root(), actual_paths_and_types);
    ASSERT_EQ(expect_paths_and_types, "\n" + actual_paths_and_types.str());
}
/**
 * @brief Test recovery from missing assign in a block without a terminator 
 */
TEST(HITInterpreter, recovery_partial_block_decl_w_nl)
{
    std::stringstream input;
    input << R"INPUT([blo
)INPUT";

    std::string expect_error = R"INPUT(
stream input:2.1: syntax error, unexpected end of line, expecting ]
)INPUT";

    std::string expect_paths_and_types = R"(
/                  )" + std::to_string(wasp::DOCUMENT_ROOT) + R"(
/blo              )"  + std::to_string(wasp::OBJECT)        + R"(
/blo/[            )"  + std::to_string(wasp::LBRACKET)      + R"( ([)
/blo/decl          )" + std::to_string(wasp::DECL)          + R"( (blo)
)";

    // Check parse failure, error message, non-null root, paths, and types
    std::stringstream actual_error, actual_paths_and_types;
    DefaultHITInterpreter interpreter(actual_error);
    ASSERT_FALSE(interpreter.parse(input));
    ASSERT_EQ(expect_error, "\n" + actual_error.str());
    ASSERT_FALSE(interpreter.root().is_null());
    wasp::node_paths_and_types(interpreter.root(), actual_paths_and_types);
    ASSERT_EQ(expect_paths_and_types, "\n" + actual_paths_and_types.str());
}
/**
 * @brief Test recovery from missing assign in a block without a terminator 
 */
TEST(HITInterpreter, recovery_partial_block_decl_w_more_data)
{
    std::stringstream input;
    input << R"INPUT([block1]
  foo1 = bar1
  foo2 = bar2
[]

[block2]
  foo3 = bar3
  [block3]
    foo4 = bar4
    [blo

)INPUT";

    std::string expect_error = R"INPUT(
stream input:11.1: syntax error, unexpected end of line, expecting ]
stream input:8.3: syntax error, unexpected end of file, expecting block terminator
stream input:6.1: syntax error, unexpected end of file, expecting block terminator
)INPUT";

    std::string expect_paths_and_types = R"(
/                  1
/block1           15
/block1/[         42 ([)
/block1/decl       2 (block1)
/block1/]         43 (])
/block1/foo1      13
/block1/foo1/decl  2 (foo1)
/block1/foo1/=     7 (=)
/block1/foo1/value 11 (bar1)
/block1/foo2      13
/block1/foo2/decl  2 (foo2)
/block1/foo2/=     7 (=)
/block1/foo2/value 11 (bar2)
/block1/term      14 ([])
/block2           15
/block2/[         42 ([)
/block2/decl       2 (block2)
/block2/]         43 (])
/block2/foo3      13
/block2/foo3/decl  2 (foo3)
/block2/foo3/=     7 (=)
/block2/foo3/value 11 (bar3)
/block2/block3    15
/block2/block3/[  42 ([)
/block2/block3/decl  2 (block3)
/block2/block3/]  43 (])
/block2/block3/foo4 13
/block2/block3/foo4/decl  2 (foo4)
/block2/block3/foo4/=  7 (=)
/block2/block3/foo4/value 11 (bar4)
/block2/block3/blo 15
/block2/block3/blo/[ 42 ([)
/block2/block3/blo/decl  2 (blo)
)";

    // Check parse failure, error message, non-null root, paths, and types
    std::stringstream actual_error, actual_paths_and_types;
    DefaultHITInterpreter interpreter(actual_error);
    ASSERT_FALSE(interpreter.parse(input));
    ASSERT_EQ(expect_error, "\n" + actual_error.str());
    ASSERT_FALSE(interpreter.root().is_null());
    wasp::node_paths_and_types(interpreter.root(), actual_paths_and_types);
    ASSERT_EQ(expect_paths_and_types, "\n" + actual_paths_and_types.str());
}
/**
 * @brief Test recovery from partial block decl without a terminator 
 */
TEST(HITInterpreter, recovery_partial_block_decl_w_data)
{
    std::stringstream input;
    input << R"INPUT([blo
    foo=bar
[])INPUT";

    std::string expect_error = R"INPUT(
stream input:2.1: syntax error, unexpected end of line, expecting ]
)INPUT";

    std::string expect_paths_and_types = R"(
/                  )" + std::to_string(wasp::DOCUMENT_ROOT) + R"(
/blo              )"  + std::to_string(wasp::OBJECT)        + R"(
/blo/[            )"  + std::to_string(wasp::LBRACKET)      + R"( ([)
/blo/decl          )" + std::to_string(wasp::DECL)          + R"( (blo)
/blo/foo          )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/blo/foo/decl      )" + std::to_string(wasp::DECL)          + R"( (foo)
/blo/foo/=         )" + std::to_string(wasp::ASSIGN)        + R"( (=)
/blo/foo/value    )"  + std::to_string(wasp::VALUE)         + R"( (bar)
/blo/term         )"  + std::to_string(wasp::OBJECT_TERM)          + R"( ([])
)";

    // Check parse failure, error message, non-null root, paths, and types
    std::stringstream actual_error, actual_paths_and_types;
    DefaultHITInterpreter interpreter(actual_error);
    ASSERT_FALSE(interpreter.parse(input));
    ASSERT_EQ(expect_error, "\n" + actual_error.str());
    ASSERT_FALSE(interpreter.root().is_null());
    wasp::node_paths_and_types(interpreter.root(), actual_paths_and_types);
    ASSERT_EQ(expect_paths_and_types, "\n" + actual_paths_and_types.str());
}

/**
 * @brief Test recovery from missing assign in a block without a terminator 
 */
TEST(HITInterpreter, recovery_partial_block_decl_w_moredata_missing_terminator)
{
    std::stringstream input;
    input << R"INPUT([blo
  foo = bar
)INPUT";

    std::string expect_error = R"INPUT(
stream input:2.1: syntax error, unexpected end of line, expecting ]
stream input:1.1: syntax error, unexpected end of file, expecting block terminator
)INPUT";

    std::string expect_paths_and_types = R"(
/                  )" + std::to_string(wasp::DOCUMENT_ROOT) + R"(
/blo              )"  + std::to_string(wasp::OBJECT)        + R"(
/blo/[            )"  + std::to_string(wasp::LBRACKET)      + R"( ([)
/blo/decl          )" + std::to_string(wasp::DECL)          + R"( (blo)
/blo/foo          )"  + std::to_string(wasp::KEYED_VALUE)   + R"(
/blo/foo/decl      )" + std::to_string(wasp::DECL)          + R"( (foo)
/blo/foo/=         )" + std::to_string(wasp::ASSIGN)        + R"( (=)
/blo/foo/value    )"  + std::to_string(wasp::VALUE)         + R"( (bar)
)";

    // Check parse failure, error message, non-null root, paths, and types
    std::stringstream actual_error, actual_paths_and_types;
    DefaultHITInterpreter interpreter(actual_error);
    ASSERT_FALSE(interpreter.parse(input));
    ASSERT_EQ(expect_error, "\n" + actual_error.str());
    ASSERT_FALSE(interpreter.root().is_null());
    wasp::node_paths_and_types(interpreter.root(), actual_paths_and_types);
    ASSERT_EQ(expect_paths_and_types, "\n" + actual_paths_and_types.str());
}

/**
 * @brief Test recovery from missing block name
 */
TEST(HITInterpreter, recovery_missing_block_name)
{
    std::stringstream input;
    input << R"INPUT(
  [
    foo = bar
  []
)INPUT";

    std::string expect_error = R"INPUT(
stream input:3.1: syntax error, unexpected end of line, expecting object name
)INPUT";

    std::string expect_paths_and_types = R"(
/                  1
/                 15
//[               42 ([)
//decl             2 ()
//foo             13
//foo/decl         2 (foo)
//foo/=            7 (=)
//foo/value       11 (bar)
//term            14 ([])
)";

    // Check parse failure, error message, non-null root, paths, and types
    std::stringstream actual_error, actual_paths_and_types;
    DefaultHITInterpreter interpreter(actual_error);
    ASSERT_FALSE(interpreter.parse(input));
    ASSERT_EQ(expect_error, "\n" + actual_error.str());
    ASSERT_FALSE(interpreter.root().is_null());
    wasp::node_paths_and_types(interpreter.root(), actual_paths_and_types);
    ASSERT_EQ(expect_paths_and_types, "\n" + actual_paths_and_types.str());

    // Check line and column range for missing block declarator empty node
    HITNodeView empty_node = interpreter.root().child_at(0).child_at(1);
    ASSERT_FALSE(empty_node.is_null());
    EXPECT_TRUE(empty_node.data().empty());
    EXPECT_EQ(std::string("decl"), empty_node.name());
    EXPECT_EQ(2, empty_node.line());
    EXPECT_EQ(3, empty_node.column());
    EXPECT_EQ(2, empty_node.last_line());
    EXPECT_EQ(3, empty_node.last_column());
}

/**
 * @brief Test recovery from missing block immediately followed by EOF
 */
TEST(HITInterpreter, recovery_missing_block_name_EOF)
{
    std::stringstream input;
    input << R"INPUT([
  foo = bar
[]
[)INPUT";

    std::string expect_error = R"INPUT(
stream input:2.1: syntax error, unexpected end of line, expecting object name
stream input:4.2: syntax error, unexpected end of file, expecting object name
)INPUT";

    std::string expect_paths_and_types = R"(
/                  1
/                 15
//[               42 ([)
//decl             2 ()
//foo             13
//foo/decl         2 (foo)
//foo/=            7 (=)
//foo/value       11 (bar)
//term            14 ([])
/                 15
//[               42 ([)
//decl             2 ()
)";

    // Check parse failure, error message, non-null root, paths, and types
    std::stringstream actual_error, actual_paths_and_types;
    DefaultHITInterpreter interpreter(actual_error);
    ASSERT_FALSE(interpreter.parse(input));
    ASSERT_EQ(expect_error, "\n" + actual_error.str());
    ASSERT_FALSE(interpreter.root().is_null());
    wasp::node_paths_and_types(interpreter.root(), actual_paths_and_types);
    ASSERT_EQ(expect_paths_and_types, "\n" + actual_paths_and_types.str());

    // Check line and column range for first missing declarator empty node
    HITNodeView empty_node_01 = interpreter.root().child_at(0).child_at(1);
    ASSERT_FALSE(empty_node_01.is_null());
    EXPECT_TRUE(empty_node_01.data().empty());
    EXPECT_EQ(std::string("decl"), empty_node_01.name());
    EXPECT_EQ(1, empty_node_01.line());
    EXPECT_EQ(1, empty_node_01.column());
    EXPECT_EQ(1, empty_node_01.last_line());
    EXPECT_EQ(1, empty_node_01.last_column());

    // Check line and column range for other missing declarator empty node
    HITNodeView empty_node_02 = interpreter.root().child_at(1).child_at(1);
    ASSERT_FALSE(empty_node_02.is_null());
    EXPECT_TRUE(empty_node_02.data().empty());
    EXPECT_EQ(std::string("decl"), empty_node_02.name());
    EXPECT_EQ(4, empty_node_02.line());
    EXPECT_EQ(1, empty_node_02.column());
    EXPECT_EQ(4, empty_node_02.last_line());
    EXPECT_EQ(1, empty_node_02.last_column());
}

/**
 * @brief Test scenarios where a single quote occurs 
 */
TEST(HITInterpreter, test_quote_after_newline)
{
    std::stringstream input;
    input << R"INPUT(
  foo = 
  'bar')INPUT";

    std::string expect_paths_and_types = R"(
/                  1
/foo              50
/foo/decl          2 (foo)
/foo/=             7 (=)
/foo/'             8 (')
/foo/value        11 (bar)
/foo/'             8 (')
)";

    
    std::stringstream actual_paths_and_types;
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_FALSE(interpreter.root().is_null());
    wasp::node_paths_and_types(interpreter.root(), actual_paths_and_types);
    ASSERT_EQ(expect_paths_and_types, "\n" + actual_paths_and_types.str());

    ASSERT_EQ("foo =\n  'bar'", interpreter.root().data());
}


/**
 * @brief Test scenarios where a double quote occurs 
 */
TEST(HITInterpreter, test_dquote_after_newline)
{
    std::stringstream input;
    input << R"INPUT(
  foo = 
       "bar")INPUT";

    std::string expect_paths_and_types = R"(
/                  1
/foo              13
/foo/decl          2 (foo)
/foo/=             7 (=)
/foo/value        11 ("bar")
)";

    
    std::stringstream actual_paths_and_types;
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input));
    ASSERT_FALSE(interpreter.root().is_null());
    wasp::node_paths_and_types(interpreter.root(), actual_paths_and_types);
    ASSERT_EQ(expect_paths_and_types, "\n" + actual_paths_and_types.str());

    ASSERT_EQ("foo =\n       \"bar\"", interpreter.root().data());
}

/**
 * @brief Test short and verbose override assign for key values and arrays
 */
TEST(HITInterpreter, override_assigns)
{
    std::stringstream input_stream;
    input_stream << R"INPUT(
[obj]
  param1 = 10
  param2 := 20
  param3 :override= 30
  array1 = '11 12 13'
  array2 := '21 22 23'
  array3 :override= '31 32 33'
  param4:=40
  array4:='41 42 43'
  foo1::param5 := 50
  foo2::array5 := '51 52 53'
  bar1::param6:=60
  bar2::array6:='61 62 63'
[]
)INPUT";

    std::string expect_paths_and_types = R"(
/                  )" + std::to_string(wasp::DOCUMENT_ROOT)   + R"(
/obj              )"  + std::to_string(wasp::OBJECT)          + R"(
/obj/[            )"  + std::to_string(wasp::LBRACKET)        + R"( ([)
/obj/decl          )" + std::to_string(wasp::DECL)            + R"( (obj)
/obj/]            )"  + std::to_string(wasp::RBRACKET)        + R"( (])
/obj/param1       )"  + std::to_string(wasp::KEYED_VALUE)     + R"( OVERRIDE=OFF
/obj/param1/decl   )" + std::to_string(wasp::DECL)            + R"( (param1)
/obj/param1/=      )" + std::to_string(wasp::ASSIGN)          + R"( (=)
/obj/param1/value )"  + std::to_string(wasp::VALUE)           + R"( (10)
/obj/param2       )"  + std::to_string(wasp::KEYED_VALUE)     + R"( OVERRIDE=ON
/obj/param2/decl   )" + std::to_string(wasp::DECL)            + R"( (param2)
/obj/param2/:=    )"  + std::to_string(wasp::OVERRIDE_ASSIGN) + R"( (:=)
/obj/param2/value )"  + std::to_string(wasp::VALUE)           + R"( (20)
/obj/param3       )"  + std::to_string(wasp::KEYED_VALUE)     + R"( OVERRIDE=ON
/obj/param3/decl   )" + std::to_string(wasp::DECL)            + R"( (param3)
/obj/param3/:=    )"  + std::to_string(wasp::OVERRIDE_ASSIGN) + R"( (:override=)
/obj/param3/value )"  + std::to_string(wasp::VALUE)           + R"( (30)
/obj/array1       )"  + std::to_string(wasp::ARRAY)           + R"( OVERRIDE=OFF
/obj/array1/decl   )" + std::to_string(wasp::DECL)            + R"( (array1)
/obj/array1/=      )" + std::to_string(wasp::ASSIGN)          + R"( (=)
/obj/array1/'      )" + std::to_string(wasp::QUOTE)           + R"( (')
/obj/array1/value )"  + std::to_string(wasp::VALUE)           + R"( (11)
/obj/array1/value )"  + std::to_string(wasp::VALUE)           + R"( (12)
/obj/array1/value )"  + std::to_string(wasp::VALUE)           + R"( (13)
/obj/array1/'      )" + std::to_string(wasp::QUOTE)           + R"( (')
/obj/array2       )"  + std::to_string(wasp::ARRAY)           + R"( OVERRIDE=ON
/obj/array2/decl   )" + std::to_string(wasp::DECL)            + R"( (array2)
/obj/array2/:=    )"  + std::to_string(wasp::OVERRIDE_ASSIGN) + R"( (:=)
/obj/array2/'      )" + std::to_string(wasp::QUOTE)           + R"( (')
/obj/array2/value )"  + std::to_string(wasp::VALUE)           + R"( (21)
/obj/array2/value )"  + std::to_string(wasp::VALUE)           + R"( (22)
/obj/array2/value )"  + std::to_string(wasp::VALUE)           + R"( (23)
/obj/array2/'      )" + std::to_string(wasp::QUOTE)           + R"( (')
/obj/array3       )"  + std::to_string(wasp::ARRAY)           + R"( OVERRIDE=ON
/obj/array3/decl   )" + std::to_string(wasp::DECL)            + R"( (array3)
/obj/array3/:=    )"  + std::to_string(wasp::OVERRIDE_ASSIGN) + R"( (:override=)
/obj/array3/'      )" + std::to_string(wasp::QUOTE)           + R"( (')
/obj/array3/value )"  + std::to_string(wasp::VALUE)           + R"( (31)
/obj/array3/value )"  + std::to_string(wasp::VALUE)           + R"( (32)
/obj/array3/value )"  + std::to_string(wasp::VALUE)           + R"( (33)
/obj/array3/'      )" + std::to_string(wasp::QUOTE)           + R"( (')
/obj/param4       )"  + std::to_string(wasp::KEYED_VALUE)     + R"( OVERRIDE=ON
/obj/param4/decl   )" + std::to_string(wasp::DECL)            + R"( (param4)
/obj/param4/:=    )"  + std::to_string(wasp::OVERRIDE_ASSIGN) + R"( (:=)
/obj/param4/value )"  + std::to_string(wasp::VALUE)           + R"( (40)
/obj/array4       )"  + std::to_string(wasp::ARRAY)           + R"( OVERRIDE=ON
/obj/array4/decl   )" + std::to_string(wasp::DECL)            + R"( (array4)
/obj/array4/:=    )"  + std::to_string(wasp::OVERRIDE_ASSIGN) + R"( (:=)
/obj/array4/'      )" + std::to_string(wasp::QUOTE)           + R"( (')
/obj/array4/value )"  + std::to_string(wasp::VALUE)           + R"( (41)
/obj/array4/value )"  + std::to_string(wasp::VALUE)           + R"( (42)
/obj/array4/value )"  + std::to_string(wasp::VALUE)           + R"( (43)
/obj/array4/'      )" + std::to_string(wasp::QUOTE)           + R"( (')
/obj/foo1::param5 )"       + std::to_string(wasp::KEYED_VALUE)     + R"( OVERRIDE=ON
/obj/foo1::param5/decl  )" + std::to_string(wasp::DECL)            + R"( (foo1::param5)
/obj/foo1::param5/:= )"    + std::to_string(wasp::OVERRIDE_ASSIGN) + R"( (:=)
/obj/foo1::param5/value )" + std::to_string(wasp::VALUE)           + R"( (50)
/obj/foo2::array5 )"       + std::to_string(wasp::ARRAY)           + R"( OVERRIDE=ON
/obj/foo2::array5/decl  )" + std::to_string(wasp::DECL)            + R"( (foo2::array5)
/obj/foo2::array5/:= )"    + std::to_string(wasp::OVERRIDE_ASSIGN) + R"( (:=)
/obj/foo2::array5/'  )"    + std::to_string(wasp::QUOTE)           + R"( (')
/obj/foo2::array5/value )" + std::to_string(wasp::VALUE)           + R"( (51)
/obj/foo2::array5/value )" + std::to_string(wasp::VALUE)           + R"( (52)
/obj/foo2::array5/value )" + std::to_string(wasp::VALUE)           + R"( (53)
/obj/foo2::array5/'  )"    + std::to_string(wasp::QUOTE)           + R"( (')
/obj/bar1::param6 )"       + std::to_string(wasp::KEYED_VALUE)     + R"( OVERRIDE=ON
/obj/bar1::param6/decl  )" + std::to_string(wasp::DECL)            + R"( (bar1::param6)
/obj/bar1::param6/:= )"    + std::to_string(wasp::OVERRIDE_ASSIGN) + R"( (:=)
/obj/bar1::param6/value )" + std::to_string(wasp::VALUE)           + R"( (60)
/obj/bar2::array6 )"       + std::to_string(wasp::ARRAY)           + R"( OVERRIDE=ON
/obj/bar2::array6/decl  )" + std::to_string(wasp::DECL)            + R"( (bar2::array6)
/obj/bar2::array6/:= )"    + std::to_string(wasp::OVERRIDE_ASSIGN) + R"( (:=)
/obj/bar2::array6/'  )"    + std::to_string(wasp::QUOTE)           + R"( (')
/obj/bar2::array6/value )" + std::to_string(wasp::VALUE)           + R"( (61)
/obj/bar2::array6/value )" + std::to_string(wasp::VALUE)           + R"( (62)
/obj/bar2::array6/value )" + std::to_string(wasp::VALUE)           + R"( (63)
/obj/bar2::array6/'  )"    + std::to_string(wasp::QUOTE)           + R"( (')
/obj/term         )"  + std::to_string(wasp::OBJECT_TERM)     + R"( ([])
)";

    // Check successful parse, paths, types, override info, and round trip
    std::stringstream actual_paths_and_types;
    DefaultHITInterpreter interpreter;
    ASSERT_TRUE(interpreter.parse(input_stream));
    ASSERT_FALSE(interpreter.root().is_null());
    wasp::node_paths_and_types(interpreter.root(), actual_paths_and_types, true);
    ASSERT_EQ(expect_paths_and_types, "\n" + actual_paths_and_types.str());
    ASSERT_EQ(input_stream.str(), "\n" + interpreter.root().data() + "\n");
}
