
#ifndef WASP_MCNP_INTERPRETER_H
#define WASP_MCNP_INTERPRETER_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <fstream>
#include <istream>
#include <ostream>
#include <iostream>
#include <vector>
#include <memory>

#include "waspcore/TreeNodePool.h"
#include "waspcore/TokenPool.h"

#include "waspmcnpi/MCNPParser.hpp"
#include "waspcore/Interpreter.h"
#include "waspcore/Definition.h"

#include "waspcore/decl.h"

/** The wasp namespace is used to encapsulate the three parser classes
 * wasp::MCNPParser, wasp::MCNPLexerImpl and wasp::MCNPInterpreter */
namespace wasp
{
// How many input node type's (section, value, etc.) in a MCNP file
typedef std::uint8_t MCNPNodeType_t;

// How many input token type's (word, int, real, comma, etc.) in a MCNP file
typedef std::uint8_t MCNPTokenType_t;

// How many bytes in a file
typedef std::uint8_t  MCNPTinyFileSize_t;
typedef std::uint16_t MCNPMediumFileSize_t;
typedef std::uint32_t MCNPFileSize_t;
typedef std::uint64_t MCNPLargeFileSize_t;

// How many tokens in a file (5 reals, 100 ints, 50 words, etc.)
typedef std::uint8_t  MCNPTinyTokenSize_t;
typedef std::uint16_t MCNPMediumTokenSize_t;
typedef std::uint32_t MCNPTokenSize_t;
typedef std::uint64_t MCNPLargeTokenSize_t;

// Tiny file TokenPool (less than 256 bytes)
typedef TokenPool<
    // Token type
    MCNPTokenType_t,
    // Max number of token
    MCNPTinyTokenSize_t,
    // Max number of bytes in the file
    MCNPTinyFileSize_t>
    MCNPTinyTokenPool;

// Medium file TokenPool (less than 65k bytes)
typedef TokenPool<
    // Token type
    MCNPTokenType_t,
    // Max number of token
    MCNPMediumTokenSize_t,
    // Max number of bytes in the file
    MCNPMediumFileSize_t>
    MCNPMediumTokenPool;

// Regular file TokenPool (less than 4b bytes)
typedef TokenPool<
    // Token type
    MCNPTokenType_t,
    // Max number of token
    MCNPTokenSize_t,
    // Max number of bytes in the file
    MCNPFileSize_t>
    MCNPTokenPool;

// Large file TokenPool (greater than 4b bytes)
typedef TokenPool<
    // Token type
    MCNPTokenType_t,
    // Max number of token
    MCNPLargeTokenSize_t,
    // Max number of bytes in the file
    MCNPLargeFileSize_t>
    MCNPLargeTokenPool;

// Regular NodePool storage
typedef TreeNodePool<
    // Node type
    MCNPNodeType_t,
    // Max number of nodes in the file
    std::uint32_t,
    // Regular TokenPool
    MCNPTokenPool>
    MCNPNodePool;

template<class S = MCNPNodePool>
class WASP_PUBLIC MCNPInterpreter : public Interpreter<S>
{
  public:
    typedef S                              Storage_type;
    typedef std::shared_ptr<MCNPInterpreter> SharedPtr;
    MCNPInterpreter();
    MCNPInterpreter(std::ostream& err);
    virtual ~MCNPInterpreter();

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

    void setSingleParse(bool s) { singleParse = s; }
    bool                     single_parse() const { return singleParse; }

  public:  // public variables
    /**
     * @brief setStreamName sets the name of this stream and indicates whether
     * the stream/name is a file[path]
     * @param name the name of the stream or file[path]
     * @param isFile [default=false] indicates whether the name is a file path
     */
    void setStreamName(const std::string& name, bool isFile = false)
    {
        Interpreter<S>::stream_name() = name;
        mHasFile                      = isFile;
    }
    /**
     * @brief traceLexing - variable available for verbosely debugging lexing
     * logic
     * @note '%debug' must be specified in the '.lex' grammar file
     */
    bool traceLexing;
    /**
     * @brief traceParsing - variable available for verbosely debugging parsing
     * logic
     * @note '%debug' must be specified in the '.bison' grammar file
     */
    bool traceParsing;

    /**
     * @brief hasFile indicates whether this parser's stream name is a file path
     * @return
     */
    bool hasFile() const { return mHasFile; }

  private:  // private variables
    /**
     * @brief singleParse - indicates a single component is to be parsed
     * When true, only a single rule will be consumed
     * Default is false;
     */
    bool singleParse;

    /**
     * @brief mHasFile indicates whether this parser was instantiated via a file
     */
    bool mHasFile;
};  // end of MCNPInterpreter class
#include "waspmcnpi/MCNPInterpreter.i.h"

typedef MCNPInterpreter<> DefaultMCNPInterpreter;
}  // namespace wasp
#endif  // WASPMCNPINTERPRETER_H
