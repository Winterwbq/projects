
#ifndef WASP_SnippetINTERPRETER_H
#define WASP_SnippetINTERPRETER_H

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <fstream>
#include <istream>
#include <ostream>
#include <iostream>
#include <vector>
#include <map>
#include <memory>

#include "waspcore/TreeNodePool.h"
#include "waspcore/TokenPool.h"

#include "wasplsp/SnippetParser.hpp"
#include "waspcore/Interpreter.h"

#include "waspcore/decl.h"

/** The wasp namespace is used to encapsulate the three parser classes
 * wasp::SnippetParser, wasp::SnippetLexerImpl and wasp::SnippetInterpreter */
namespace wasp
{
  /**
   * The SnippetType as
   */
  enum SnippetType{
    TEXT = wasp::STRING,
    TABSTOP = wasp::DECL,
    PLACEHOLDER = wasp::OBJECT
  };

  /**
   * Substitute escape sequences for the character being escaped.
   * Specifically, \* where * can be '$', '}'.
   *
   * Additionally, if within_choice is provided, '|' and ',' are also unescaped, if necessary
   */
  inline std::string unescape_snippet(std::string snippet, bool within_choice=false)
  {
    std::string unescaped;
    static const std::string chars = "$\\,}";
    static const std::string within_choice_chars = "|,";
    for( size_t i = 0; i < snippet.size(); ++i)
    {
      // It isn't an escape, capture it and move on
      if (snippet[i] != '\\')
      {
        unescaped.push_back(snippet[i]);
        continue;
      }
      // It is an escape, but is there isn't an escaped character,
      if (i+1 >= snippet[i]) break;
      // It is an escape and there is an escaped character
      // is it a reserved snippet character
      else if (chars.find_first_of(snippet[i+1]) != std::string::npos
        || within_choice && within_choice_chars.find_first_of(snippet[i+1]) != std::string::npos)
      {
        ++i;
      }
      // index i has been adjusted accordingly to append to the unescaped string
      unescaped.push_back(snippet[i]);
    }
    return unescaped;
  }

// How many input node type's (section, value, etc.) in a Snippet file
typedef std::uint8_t SnippetNodeType_t;

// How many input token type's (word, int, real, comma, etc.) in a Snippet file
typedef std::uint8_t SnippetTokenType_t;

// How many bytes in a file
typedef std::uint8_t  SnippetTinyFileSize_t;
typedef std::uint16_t SnippetMediumFileSize_t;
typedef std::uint32_t SnippetFileSize_t;
typedef std::uint64_t SnippetLargeFileSize_t;

// How many tokens in a file (5 reals, 100 ints, 50 words, etc.)
typedef std::uint8_t  SnippetTinyTokenSize_t;
typedef std::uint16_t SnippetMediumTokenSize_t;
typedef std::uint32_t SnippetTokenSize_t;
typedef std::uint64_t SnippetLargeTokenSize_t;

// Tiny file TokenPool (less than 256 bytes)
typedef TokenPool<
    // Token type
    SnippetTokenType_t,
    // Max number of token
    SnippetTinyTokenSize_t,
    // Max number of bytes in the file
    SnippetTinyFileSize_t>
    SnippetTinyTokenPool;

// Medium file TokenPool (less than 65k bytes)
typedef TokenPool<
    // Token type
    SnippetTokenType_t,
    // Max number of token
    SnippetMediumTokenSize_t,
    // Max number of bytes in the file
    SnippetMediumFileSize_t>
    SnippetMediumTokenPool;

// Regular file TokenPool (less than 4b bytes)
typedef TokenPool<
    // Token type
    SnippetTokenType_t,
    // Max number of token
    SnippetTokenSize_t,
    // Max number of bytes in the file
    SnippetFileSize_t>
    SnippetTokenPool;

// Large file TokenPool (greater than 4b bytes)
typedef TokenPool<
    // Token type
    SnippetTokenType_t,
    // Max number of token
    SnippetLargeTokenSize_t,
    // Max number of bytes in the file
    SnippetLargeFileSize_t>
    SnippetLargeTokenPool;

// Regular NodePool storage
typedef TreeNodePool<
    // Node type
    SnippetNodeType_t,
    // Max number of nodes in the file
    std::uint32_t,
    // Regular TokenPool
    SnippetTokenPool>
    SnippetNodePool;

/** The SnippetInterpreter class brings together all components. It creates an
 * instance of
 * the SnippetParser and SnippetLexerImpl classes and connects them. Then the input
 * stream is
 * fed into the lexer object and the parser gets it's token
 * sequence. Furthermore the driver object is available in the grammar rules as
 * a parameter. Therefore the driver class contains a reference to the
 * structure into which the parsed data is saved.
 *
 * @brief Interpret Language Server Protocol Snippet Syntax (SnippetInterpreter) file/stream
 * SnippetInterpreter currently supports the following grammar:
 *
 * start       ::= any*
 * any         ::= tabstop | placeholder | text
 * tabstop     ::= '$' int | '${' int '}'
 * placeholder ::= '${' int ':' text '}'
 * int         ::= [0-9]+
 * text        ::= .*
 *
 */
template<class S = SnippetNodePool>
class WASP_PUBLIC SnippetInterpreter : public Interpreter<S>
{
  public:
    typedef S                               Storage_type;
    typedef std::shared_ptr<SnippetInterpreter> SharedPtr;
    typedef Interpreter<S>                  Super;
    SnippetInterpreter();
    SnippetInterpreter(std::ostream& err);
    virtual ~SnippetInterpreter();

    /** Invoke the lexer and parser for a stream.
     * @param in        input stream
     * @param sname     stream name for error messages
     * @return          true if successfully parsed
     */
    bool parseStream(std::istream&      in,
                     const std::string& sname       = "stream input",
                     size_t             startLine   = 1u,
                     size_t             startColumn = 1u);
    bool
    parse(std::istream& input, size_t startLine = 1u, size_t startColumn = 1u);
    /** Invoke the lexer and parser on an input string.
     * @param input     input string
     * @param sname     stream name for error messages
     * @return          true if successfully parsed
     */
    bool parseString(const std::string& input,
                     const std::string& sname       = "string stream",
                     size_t             startLine   = 1u,
                     size_t             startColumn = 1u);
    /** Invoke the lexer and parser on a file. Use parse_stream with a
     * std::ifstream if detection of file reading errors is required.
     * @param filename  input file name
     * @return          true if successfully parsed
     */
    bool parseFile(const std::string& filename, size_t startLine = 1u);

  public:  // public variables

    /**
     * @brief traceLexing - variable available for verbosely debugging lexing
     * logic
     * @note '%debug' must be specified in the '.lex' grammar file
     */
    bool traceLexing;
    /**
     * @brief traceParsing - variable available for verbosely debugging parsing
     * logic
     * @note '%debug' must be specified in the '.biSnippet' grammar file
     */
    bool traceParsing;

  private:  // private variables

  private:  // private methods

};  // end of SnippetInterpreter class
#include "wasplsp/SnippetInterpreter.i.h"

typedef SnippetInterpreter<> DefaultSnippetInterpreter;
}  // namespace wasp
#endif  // WASPSnippetINTERPRETER_H
