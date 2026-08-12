#ifndef WASPLSP_SnippetManager_H
#define WASPLSP_SnippetManager_H

#include <istream>
#include <ostream>
#include <vector>
#include <string>

#include "waspcore/decl.h"

namespace wasp {
namespace lsp  {

/**
 * Manager for LSP Snippet interactions by a text editor
 *
 * Interprets and provides managed access snippets.
 *
 * 1. load snippet
 * 2. obtain text
 * 3. obtain current tabstop/placeholder and metadata
 * 4. interact
 *
 * Snippets are ordered according to tabstop number with
 * $0 being special representing the last tabstop.
 *
 * Supports following partial snippet syntax as prioritized by sponsor:
 *
 * any         ::= tabstop | placeholder | text
 * tabstop     ::= '$' int | '${' int '}'
 * placeholder ::= '${' int ':' text '}'
 * int         ::= [0-9]+
 * text        ::= .*
 *
 */
class WASP_PUBLIC SnippetManager
{
  public:

    /**
     * Load a snippet into his manager.
     * Errors related to snippet syntax issues are reported in errors stream
     * @brief snippet
     */
    bool load(std::istream& snippet, std::ostream& errors);
    /**
     * Obtain the snippet text with all defaults selected
     */
    std::string text() const;

    /**
     * Obtain the number of tabstops (tabstop or placeholders)
     */
    size_t tabstop_count() const {return m_tabstops.size();}

    int current_tabstop() const {return m_current_tabstop;}

    bool valid_tabstop() const {return m_current_tabstop >= 0 && m_current_tabstop < m_tabstops.size(); }

    bool next_tabstop() {m_current_tabstop++; return valid_tabstop();}
    bool prev_tabstop() {m_current_tabstop--; return valid_tabstop();}
    int line_offset() const;
    int column_offset() const;
    int tabstop() const;

    /**
     * Obtain the text for the current tabstop, if any
     */
    std::string tabstop_text() const;

  private:

    struct Snippet{
        size_t tabstop = 0; // only assigned if snippet index by m_tabstop
        int line_offset = 0;
        int column_offset = 0;
        std::string text;
    };

    std::vector<Snippet> m_snippets;
    std::vector<size_t> m_tabstops;
    // index into m_tabstops, initially invalid
    int m_current_tabstop = -1;
};

} // namespace lsp
} // namespace wasp

#endif // WASPLSP_SnippetManager_H
