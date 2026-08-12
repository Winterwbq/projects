#include "SnippetManager.h"
#include "SnippetInterpreter.h"


namespace wasp {
namespace lsp  {

bool SnippetManager::load(std::istream& snippet_data, std::ostream& errors)
{
  wasp_require(m_snippets.empty() && m_tabstops.empty());

  DefaultSnippetInterpreter interp(errors);
  if (!interp.parse(snippet_data))
  {
    return false;
  }

  // construct snippets from snippet parse tree
  auto root = interp.root();
  // No content in a snippet is not an error
  // return success
  if (root.is_null())
  {
    return true;
  }
  for (size_t i = 0; i < root.child_count(); ++i)
  {
    auto child = root.child_at(i);
    m_snippets.push_back(Snippet());
    auto& snippet = m_snippets.back();
    if (child.type() == SnippetType::TEXT)
    {
      snippet.text = unescape_snippet(child.data());
    }
    else if (child.type() == SnippetType::TABSTOP)
    {
      // Tabstop parse tree is: '$' int
      // Obtain the int as the tabstop
      snippet.tabstop = child.child_at(1).to_int();
      m_tabstops.push_back(m_snippets.size()-1);
    }
    else if (child.type() == SnippetType::PLACEHOLDER)
    {
      // Placeholder parse tree is: '$' int text
      // Obtain the int as the tabstop
      // obtain the text as the snippet text
      snippet.tabstop = child.child_at(1).to_int();
      snippet.text = unescape_snippet(child.child_at(2).data());
      m_tabstops.push_back(m_snippets.size()-1);
    }
    else
    {
      wasp_not_implemented("snippet type" << child.type() );
    }

    snippet.line_offset = child.line()-1;
    auto column = child.column()-1;
    // correct column to account for prior snippet syntax ($, ${}, placeholder, etc.)
    // I.e., column is just accumulation of last column + text, if any 
    // This snippet is already on the stack, need to check prior (-2) on the same line
    if (m_snippets.size() > 2 && m_snippets[m_snippets.size()-2].line_offset == snippet.line_offset)
    {
      auto last_column = m_snippets[m_snippets.size()-2].column_offset;
      auto text_size = m_snippets[m_snippets.size()-2].text.size();
      column = last_column + text_size;
    }
    snippet.column_offset = column;
  }

  if (!m_tabstops.empty()){
    // Order tab stops according to $1...$n, then $0
    auto comparator = [&](size_t snippet_index_a, size_t snippet_index_b) -> bool
    {
      auto ts_a = -1 * m_snippets[snippet_index_a].tabstop;
      auto ts_b = -1 * m_snippets[snippet_index_b].tabstop;

      return ts_b < ts_a;
    };
    std::sort(m_tabstops.begin(), m_tabstops.end(), comparator);
    m_current_tabstop = 0;

  }
  return true;
}

std::string SnippetManager::text() const
{
  std::stringstream s;

  for (const Snippet& snip : m_snippets)
  {
    s << snip.text;
  }
  return s.str();
}

std::string SnippetManager::tabstop_text() const
{
  if (!valid_tabstop()) return "";

  auto snippet_index = m_tabstops[m_current_tabstop];
  return m_snippets[snippet_index].text;
}

int SnippetManager::line_offset() const
{
  wasp_require(valid_tabstop());
  auto snippet_index = m_tabstops[m_current_tabstop];
  return m_snippets[snippet_index].line_offset;
}
int SnippetManager::column_offset() const
{
  wasp_require(valid_tabstop());
  auto snippet_index = m_tabstops[m_current_tabstop];
  return m_snippets[snippet_index].column_offset;
}
int SnippetManager::tabstop() const
{
  wasp_require(valid_tabstop());
  auto snippet_index = m_tabstops[m_current_tabstop];
  return m_snippets[snippet_index].tabstop;
}

} // namespace lsp
} // namespace wasp

