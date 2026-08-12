
#include "gtest/gtest.h"

#include "wasplsp/SnippetInterpreter.h"
#include <thread>
#include <vector>
#include <sstream>
#include <memory>

namespace wasp {
namespace lsp{
TEST(snippet, text)
{
    // Note the escaped '$' ',' '\'
    std::stringstream data(R"I(`1234567890-= qwertyuiop[]\\asdfghjkl;'zxcvbnm\,
./~!@#\$%^&*()_+QWERTYUIOP{\}|ASDFGHJKL:"ZXCVBNM<>?)I");
    DefaultSnippetInterpreter interp;
    ASSERT_TRUE(interp.parse(data));
    // document
    // |_ text (`123...<>?)
    ASSERT_EQ(2, interp.node_count());
    auto txt = interp.root().child_at(0);
    ASSERT_EQ(SnippetType::TEXT, txt.type());
    // TODO - do not account for the escape characters
    ASSERT_EQ(100, txt.to_string().size());
}

TEST(snippet, simple_tabstop)
{
    std::stringstream data("$1");
    DefaultSnippetInterpreter interp;
    ASSERT_TRUE(interp.parse(data));
    // document
    // |_ tabstop
    //    |_ decl
    //    |_ integer
    ASSERT_EQ(4, interp.node_count());
    auto ts = interp.root().child_at(0);
    ASSERT_EQ(SnippetType::TABSTOP, ts.type());
    ASSERT_EQ(1, ts.child_at(1).to_int());
}
TEST(snippet, multiple_simple_tabstop)
{
    std::stringstream data("$1$2");
    DefaultSnippetInterpreter interp;
    ASSERT_TRUE(interp.parse(data));
    // document
    // |_ tabstop
    //    |_ decl
    //    |_ integer
    // |_ tabstop
    //    |_ decl
    //    |_ integer
    ASSERT_EQ(7, interp.node_count());
    auto ts = interp.root().child_at(0);
    ASSERT_EQ(SnippetType::TABSTOP, ts.type());
    ASSERT_EQ(1, ts.child_at(1).to_int());
    ts = interp.root().child_at(1);
    ASSERT_EQ(SnippetType::TABSTOP, ts.type());
    ASSERT_EQ(2, ts.child_at(1).to_int());
}
TEST(snippet, multi_tabstop)
{
    std::stringstream data("${1:ph}$2");
    DefaultSnippetInterpreter interp;
    ASSERT_TRUE(interp.parse(data));
    // document
    // |_ placeholder
    //    |_ decl
    //    |_ integer
    //    |_ text
    // |_ tabstop
    //    |_ decl
    //    |_ integer
    ASSERT_EQ(8, interp.node_count());
    auto ts = interp.root().child_at(0);
    ASSERT_EQ(SnippetType::PLACEHOLDER, ts.type());
    ASSERT_EQ(1, ts.child_at(1).to_int());
    ASSERT_EQ("ph", ts.child_at(2).to_string());
    ts = interp.root().child_at(1);
    ASSERT_EQ(SnippetType::TABSTOP, ts.type());
    ASSERT_EQ(2, ts.child_at(1).to_int());
}
TEST(snippet, multi_tabstop_w_text)
{
    std::stringstream data("something${1}here$2");
    DefaultSnippetInterpreter interp;
    ASSERT_TRUE(interp.parse(data));
    // document
    // |_ text
    // |_ tabstop
    //    |_ decl
    //    |_ integer
    // |_ text
    // |_ tabstop
    //    |_ decl
    //    |_ integer
    ASSERT_EQ(9, interp.node_count());
    auto ts = interp.root().child_at(1);
    ASSERT_EQ(SnippetType::TABSTOP, ts.type());
    ASSERT_EQ(1, ts.child_at(1).to_int());
    ts = interp.root().child_at(3);
    ASSERT_EQ(SnippetType::TABSTOP, ts.type());
    ASSERT_EQ(2, ts.child_at(1).to_int());
}

TEST(snippet, simple_curlied_tabstop)
{
    std::stringstream data("${22}");
    DefaultSnippetInterpreter interp;
    ASSERT_TRUE(interp.parse(data));
    // document
    // |_ tabstop
    //    |_ decl ($)
    //    |_ integer
    ASSERT_EQ(4, interp.node_count());
    auto ts = interp.root().child_at(0);
    ASSERT_EQ(SnippetType::TABSTOP, ts.type());
    ASSERT_EQ(22, ts.child_at(1).to_int());
}

TEST(snippet, simple_placeholder)
{
    std::stringstream data("${1:true}");
    DefaultSnippetInterpreter interp;
    ASSERT_TRUE(interp.parse(data));
    // document
    // |_ placeholder
    //    |_ decl ($)
    //    |_ integer
    //    |_ text
    ASSERT_EQ(5, interp.node_count());
    auto ph = interp.root().child_at(0);
    ASSERT_EQ(SnippetType::PLACEHOLDER, ph.type());
    auto php = ph.child_at(1);
    ASSERT_EQ(1, php.to_int());
    auto pht = ph.child_at(2);
    ASSERT_EQ("true", pht.to_string());
}
TEST(snippet, simple_placeholder_withescape)
{
    std::stringstream data("${1:true\\}\\\\$}");
    DefaultSnippetInterpreter interp;
    ASSERT_TRUE(interp.parse(data));
    // document
    // |_ placeholder
    //    |_ decl ($)
    //    |_ integer
    //    |_ text
    ASSERT_EQ(5, interp.node_count());
    auto ph = interp.root().child_at(0);
    ASSERT_EQ(SnippetType::PLACEHOLDER, ph.type());
    auto php = ph.child_at(1);
    ASSERT_EQ(1, php.to_int());
    auto pht = ph.child_at(2);
    ASSERT_EQ("true}\\$", unescape_snippet(pht.to_string()));
}

TEST(snippet, empty_tabstop)
{
    std::stringstream data("${1:}");
    DefaultSnippetInterpreter interp;
    ASSERT_TRUE(interp.parse(data));
    // document
    // |_ tabstop
    //    |_ decl
    //    |_ integer
    ASSERT_EQ(4, interp.node_count());
    auto ts = interp.root().child_at(0);
    ASSERT_EQ(SnippetType::TABSTOP, ts.type());
    ASSERT_EQ(1, ts.child_at(1).to_int());
}

TEST(snippet, line_embedded_nakedtabstop)
{
    std::stringstream data("fooo$33 boo");
    DefaultSnippetInterpreter interp;
    ASSERT_TRUE(interp.parse(data));
    // document
    // |_ text (fooo)
    // |_ tabstop
    //    |_ decl ($)
    //    |_ integer
    // |_ text ( boo)
    ASSERT_EQ(6, interp.node_count());
    auto ts = interp.root().child_at(1);
    ASSERT_EQ(SnippetType::TABSTOP, ts.type());
    ASSERT_EQ(33, ts.child_at(1).to_int());
    ASSERT_EQ(" boo", interp.root().child_at(2).to_string());
}

TEST(snippet, line_embedded_nakedtabstop2)
{
    std::stringstream data("fooo$33boo");
    DefaultSnippetInterpreter interp;
    ASSERT_TRUE(interp.parse(data));
    // document
    // |_ text (fooo)
    // |_ tabstop
    //    |_ decl ($)
    //    |_ integer
    // |_ text (boo)
    ASSERT_EQ(6, interp.node_count());
    auto ts = interp.root().child_at(1);
    ASSERT_EQ(SnippetType::TABSTOP, ts.type());
    ASSERT_EQ(33, ts.child_at(1).to_int());
    ASSERT_EQ("boo", interp.root().child_at(2).to_string());
}
TEST(snippet, multipeline_embedded_nakedtabstop)
{
    std::stringstream data;
    data << "some declarator" << std::endl
         << "    $0" << std::endl
         << "terminator text " << std::endl;
    DefaultSnippetInterpreter interp;
    ASSERT_TRUE(interp.parse(data));
    // document
    // |_ text (some dec)
    // |_ tabstop
    //    |_ decl ($)
    //    |_ integer
    // |_ text (termina...)
    ASSERT_EQ(6, interp.node_count());
    ASSERT_EQ("some declarator\n    ", interp.root().child_at(0).to_string());
    auto ts = interp.root().child_at(1);
    ASSERT_EQ(SnippetType::TABSTOP, ts.type());
    ASSERT_EQ(0, ts.child_at(1).to_int());
    ASSERT_EQ("\nterminator text \n", interp.root().child_at(2).to_string());
}

TEST(snippet, multipeline_embedded_placeholder)
{
    std::stringstream data;
    data << "some declarator" << std::endl
         << "    ${1:something}" << std::endl
         << "terminator text " << std::endl;
    DefaultSnippetInterpreter interp;
    ASSERT_TRUE(interp.parse(data));
    // document
    // |_ text (some dec)
    // |_ placeholder
    //    |_ decl ($)
    //    |_ integer
    //    |_ text (something)
    // |_ text (termina...)
    ASSERT_EQ(7, interp.node_count());
    ASSERT_EQ("some declarator\n    ", interp.root().child_at(0).to_string());
    auto ph = interp.root().child_at(1);
    ASSERT_EQ(SnippetType::PLACEHOLDER, ph.type());
    ASSERT_EQ(1, ph.child_at(1).to_int());
    ASSERT_EQ(2, ph.line());
    ASSERT_EQ(5, ph.column());
    ASSERT_EQ("something", ph.child_at(2).to_string());
    ASSERT_EQ("\nterminator text \n", interp.root().child_at(2).to_string());
}
TEST(snippet, multipeline_embedded_escaped_placeholder)
{
    std::stringstream data;
    data << "some declarator" << std::endl
         << "\\$\\\\}${1:something}" << std::endl
         << "terminator text " << std::endl;
    DefaultSnippetInterpreter interp;
    ASSERT_TRUE(interp.parse(data));
    // document
    // |_ text (some dec...)
    // |_ placeholder
    //    |_ decl ($)
    //    |_ integer
    //    |_ text (something)
    // |_ text (termina...)
    ASSERT_EQ(7, interp.node_count());
    ASSERT_EQ("some declarator\n\\$\\\\}", interp.root().child_at(0).to_string());
    ASSERT_EQ("some declarator\n$\\}", unescape_snippet(interp.root().child_at(0).to_string()));
    auto ph = interp.root().child_at(1);
    ASSERT_EQ(SnippetType::PLACEHOLDER, ph.type());
    ASSERT_EQ(1, ph.child_at(1).to_int());
    ASSERT_EQ(2, ph.line());
    // TODO - identify best path for dealing with actual column
    // offset as a function of unescaped characters, as would be visible to
    // a user editing the snippet in a text editor
    ASSERT_EQ(6, ph.column());
    ASSERT_EQ("something", ph.child_at(2).to_string());
    ASSERT_EQ("\nterminator text \n", interp.root().child_at(2).to_string());
    ASSERT_EQ(3, interp.root().child_at(2).line());
    // This is a special case where the first character is a newline
    // TokenPool::column logic computes column a token_offset - prior_newline_offset = 0
    // TODO - determine if this should be expected.
    // Most lanugages do not track newlines so the column logic may need to be
    // updated to account for this scenario
    ASSERT_EQ(0, interp.root().child_at(2).column());
}

TEST(snippet, son_snippet)
{
    std::stringstream data;
    data << "object{key = ${1:something}}";
    DefaultSnippetInterpreter interp;
    ASSERT_TRUE(interp.parse(data));
    // document
    // |_ text (object{key = )
    // |_ placeholder
    //    |_ decl ($)
    //    |_ integer
    //    |_ text (something)
    // |_ text (})
    ASSERT_EQ(7, interp.node_count());
    ASSERT_EQ("object{key = ", interp.root().child_at(0).to_string());
    ASSERT_EQ("object{key = ", unescape_snippet(interp.root().child_at(0).to_string()));
    auto ph = interp.root().child_at(1);
    ASSERT_EQ(SnippetType::PLACEHOLDER, ph.type());
    ASSERT_EQ(1, ph.child_at(1).to_int());
    ASSERT_EQ(1, ph.line());
    ASSERT_EQ("}", interp.root().child_at(2).to_string());
    ASSERT_EQ("}", unescape_snippet(interp.root().child_at(2).to_string()));
}

TEST(snippet, son_snippet_addl_trailing)
{
    std::stringstream data;
    data << "object{key = ${1:something}} something trailing after { after }  " << std::endl
         << " with other {stuff} following newline";
    DefaultSnippetInterpreter interp;
    ASSERT_TRUE(interp.parse(data));
    // document
    // |_ text (object{key = )
    // |_ placeholder
    //    |_ decl ($)
    //    |_ integer
    //    |_ text (something)
    // |_ text (} something trailing ...)

    ASSERT_EQ(7, interp.node_count());
    ASSERT_EQ("object{key = ", interp.root().child_at(0).to_string());
    ASSERT_EQ("object{key = ", unescape_snippet(interp.root().child_at(0).to_string()));
    auto ph = interp.root().child_at(1);
    ASSERT_EQ(SnippetType::PLACEHOLDER, ph.type());
    ASSERT_EQ(1, ph.child_at(1).to_int());
    ASSERT_EQ(1, ph.line());
    ASSERT_EQ("} something trailing after { after }  \n with other {stuff} following newline", interp.root().child_at(2).to_string());
    ASSERT_EQ("} something trailing after { after }  \n with other {stuff} following newline", unescape_snippet(interp.root().child_at(2).to_string()));
}
} // namespace lsp
} // namespace wasp
