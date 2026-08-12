
#include "gtest/gtest.h"

#include "wasplsp/SnippetManager.h"
#include <thread>
#include <vector>
#include <sstream>
#include <memory>

namespace wasp {
namespace lsp{
TEST(SnippetManager, empty)
{
    std::stringstream data(R"I()I");
    SnippetManager snippet;
    ASSERT_TRUE(snippet.load(data, std::cerr));

    ASSERT_EQ(0, snippet.tabstop_count());
    ASSERT_FALSE(snippet.valid_tabstop());
    ASSERT_EQ("", snippet.text());

}

TEST(SnippetManager, tabstop)
{
    std::stringstream data(R"I($0)I");
    SnippetManager snippet;
    ASSERT_TRUE(snippet.load(data, std::cerr));

    ASSERT_EQ(1, snippet.tabstop_count());
    ASSERT_TRUE(snippet.valid_tabstop());
    ASSERT_EQ("", snippet.text());
    ASSERT_EQ(0, snippet.current_tabstop());
    ASSERT_EQ(0, snippet.line_offset());
    ASSERT_EQ(0, snippet.column_offset());
}

TEST(SnippetManager, embeddedtabstop)
{
    std::stringstream data(R"I(something blah $0)I");
    SnippetManager snippet;
    ASSERT_TRUE(snippet.load(data, std::cerr));

    ASSERT_EQ(1, snippet.tabstop_count());
    ASSERT_EQ("something blah ", snippet.text());
    ASSERT_EQ(0, snippet.current_tabstop());
    ASSERT_EQ(0, snippet.line_offset());
    ASSERT_EQ(15, snippet.column_offset());
}

TEST(SnippetManager, embeddedtabstop2)
{
    std::stringstream data(R"I(something blah
     c$0
    doobop)I");
    SnippetManager snippet;
    ASSERT_TRUE(snippet.load(data, std::cerr));

    ASSERT_EQ(1, snippet.tabstop_count());
    ASSERT_TRUE(snippet.valid_tabstop());
    ASSERT_EQ("something blah\n\
     c\n\
    doobop", snippet.text());
    ASSERT_EQ(0, snippet.current_tabstop());
    ASSERT_EQ(1, snippet.line_offset());
    ASSERT_EQ(6, snippet.column_offset());
}

TEST(SnippetManager, placeholder)
{
    std::stringstream data(R"I(${0:something})I");
    SnippetManager snippet;
    ASSERT_TRUE(snippet.load(data, std::cerr));

    ASSERT_EQ(1, snippet.tabstop_count());
    ASSERT_TRUE(snippet.valid_tabstop());
    ASSERT_EQ("something", snippet.text());
    ASSERT_EQ(0, snippet.current_tabstop());
    ASSERT_EQ(0, snippet.line_offset());
    ASSERT_EQ(0, snippet.column_offset());
}


TEST(SnippetManager, embedded_placeholder)
{
    std::stringstream data(R"I(before${0:something}after)I");
    SnippetManager snippet;
    ASSERT_TRUE(snippet.load(data, std::cerr));

    ASSERT_EQ(1, snippet.tabstop_count());
    ASSERT_TRUE(snippet.valid_tabstop());
    ASSERT_EQ("beforesomethingafter", snippet.text());
    ASSERT_EQ(0, snippet.current_tabstop());
    ASSERT_EQ(0, snippet.line_offset());
    ASSERT_EQ(6, snippet.column_offset());
}

TEST(SnippetManager, syntax_error)
{
    std::stringstream data("${0");
    SnippetManager snippet;
    std::stringstream errors;
    ASSERT_FALSE(snippet.load(data, errors));

    ASSERT_EQ("snippet:1.4: syntax error, unexpected end of file, expecting '}' or ':'\n", errors.str());
    ASSERT_EQ(0, snippet.tabstop_count());
    ASSERT_FALSE(snippet.valid_tabstop());
    ASSERT_EQ("", snippet.text());
}

TEST(SnippetManager, multiline_embedded_placeholder)
{
    std::stringstream data(R"I(before
    c${0:something}
    after)I");
    SnippetManager snippet;
    ASSERT_TRUE(snippet.load(data, std::cerr));

    ASSERT_EQ(1, snippet.tabstop_count());
    ASSERT_TRUE(snippet.valid_tabstop());
    ASSERT_EQ("before\n\
    csomething\n\
    after", snippet.text());
    ASSERT_EQ(0, snippet.current_tabstop());
    ASSERT_EQ(1, snippet.line_offset());
    ASSERT_EQ(5, snippet.column_offset());
}

TEST(SnippetManager, multiline_tabstop_ordering)
{
    std::stringstream data(R"I(before$1
    c${0:something}
    after${2:else})I");
    SnippetManager snippet;
    ASSERT_TRUE(snippet.load(data, std::cerr));

    ASSERT_EQ(3, snippet.tabstop_count());
    ASSERT_TRUE(snippet.valid_tabstop());
    ASSERT_EQ("before\n\
    csomething\n\
    afterelse", snippet.text());

    // first ($1) tabstop is at line 1 column 7
    ASSERT_EQ(0, snippet.current_tabstop());
    ASSERT_EQ(0, snippet.line_offset());
    ASSERT_EQ(6, snippet.column_offset());

    // second ($2) tabstop is at line 3 column 10
    ASSERT_TRUE(snippet.next_tabstop());
    ASSERT_EQ(2, snippet.line_offset());
    ASSERT_EQ(9, snippet.column_offset());
    ASSERT_EQ("else", snippet.tabstop_text());

    // final ($0) tabstop is at line 2 column 6
    ASSERT_TRUE(snippet.next_tabstop());
    ASSERT_EQ(1, snippet.line_offset());
    ASSERT_EQ(5, snippet.column_offset());

    // next tabstop should be invalid
    ASSERT_FALSE(snippet.next_tabstop());
    ASSERT_FALSE(snippet.valid_tabstop());
}

/**
 * This tests the effects of tabstops/placeholders where the offset
 * is variable because of the tabstop/placeholder syntax.
 *
 * E.g., a editor will want to place the cursor at the tabstop offset
 * but what if there is a tabstop/placeholder prior to the tabstop?
 *
 * The offset, as observed in the snippet does not account for the non-visible
 * snippet syntax characters.
 */

TEST(SnippetManager, multicolumn_tabstop_ordering)
{
    std::stringstream data("before${0}something$1");
    SnippetManager snippet;
    ASSERT_TRUE(snippet.load(data, std::cerr));

    ASSERT_EQ(2, snippet.tabstop_count());
    ASSERT_TRUE(snippet.valid_tabstop());
    ASSERT_EQ("beforesomething", snippet.text());

    // first ($1) tabstop is at line 1 column 16 of offset 15?
    ASSERT_EQ(0, snippet.current_tabstop());
    ASSERT_EQ(0, snippet.line_offset());
    // should be column offset of 15 as the ${} should not be included
    ASSERT_EQ(15, snippet.column_offset());

    // final ($0) tabstop is at line 1 column 6
    ASSERT_TRUE(snippet.next_tabstop());
    ASSERT_EQ(0, snippet.line_offset());
    ASSERT_EQ(6, snippet.column_offset());
    ASSERT_EQ("", snippet.tabstop_text());

    // next tabstop should be invalid
    ASSERT_FALSE(snippet.next_tabstop());
    ASSERT_FALSE(snippet.valid_tabstop());
}

TEST(SnippetManager, son_snippet_addl_trailing)
{
    std::stringstream data;
    data << "object{key = ${1:something}} something trailing after { after }  $2" << std::endl
         << " with other {stuff} following newline";
    SnippetManager snippet;
    ASSERT_TRUE(snippet.load(data, std::cerr));

    ASSERT_EQ(2, snippet.tabstop_count());
    ASSERT_TRUE(snippet.valid_tabstop());
    ASSERT_EQ("object{key = something} something trailing after { after }  \n with other {stuff} following newline", snippet.text());

    // first ($1) tabstop is at line 1
    ASSERT_EQ(0, snippet.current_tabstop());
    ASSERT_EQ(0, snippet.line_offset());
    ASSERT_EQ(13, snippet.column_offset());

    
    ASSERT_TRUE(snippet.next_tabstop());
    ASSERT_TRUE(snippet.valid_tabstop());
    ASSERT_EQ(1, snippet.current_tabstop());
    ASSERT_EQ(0, snippet.line_offset());
    // be sure the 'something' in $1 is accounted for in $2 column offset
    ASSERT_EQ(60, snippet.column_offset());

    // next tabstop should be invalid
    ASSERT_FALSE(snippet.next_tabstop());
    ASSERT_FALSE(snippet.valid_tabstop());
}

} // namespace lsp
} // namespace wasp
