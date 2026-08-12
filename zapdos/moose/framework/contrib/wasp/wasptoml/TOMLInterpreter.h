#ifndef WASP_TOMLINTERPRETER_H
#define WASP_TOMLINTERPRETER_H
#include "waspcore/TreeNodePool.h"
#include "waspcore/Interpreter.h"
#include "waspcore/Object.h"

#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>

#include "wasptoml/TOMLParser.hpp"

namespace wasp
{
// How many input node type's (section, value, etc.) in a TOML file
typedef std::uint8_t TOMLNodeType_t;

// How many input token type's (word, int, real, comma, etc.) in a TOML file
typedef std::uint8_t TOMLTokenType_t;

// How many bytes in a file
typedef std::uint8_t  TOMLTinyFileSize_t;
typedef std::uint16_t TOMLMediumFileSize_t;
typedef std::uint32_t TOMLFileSize_t;
typedef std::uint64_t TOMLLargeFileSize_t;

// How many tokens in a file (5 reals, 100 ints, 50 words, etc.)
typedef std::uint8_t  TOMLTinyTokenSize_t;
typedef std::uint16_t TOMLMediumTokenSize_t;
typedef std::uint32_t TOMLTokenSize_t;
typedef std::uint64_t TOMLLargeTokenSize_t;

// Tiny file TokenPool (less than 256 bytes)
typedef TokenPool<
    // Token type
    TOMLTokenType_t,
    // Max number of token
    TOMLTinyTokenSize_t,
    // Max number of bytes in the file
    TOMLTinyFileSize_t>
    TOMLTinyTokenPool;

// Medium file TokenPool (less than 65k bytes)
typedef TokenPool<
    // Token type
    TOMLTokenType_t,
    // Max number of token
    TOMLMediumTokenSize_t,
    // Max number of bytes in the file
    TOMLMediumFileSize_t>
    TOMLMediumTokenPool;

// Regular file TokenPool (less than 4b bytes)
typedef TokenPool<
    // Token type
    TOMLTokenType_t,
    // Max number of token
    TOMLTokenSize_t,
    // Max number of bytes in the file
    TOMLFileSize_t>
    TOMLTokenPool;

// Large file TokenPool (greater than 4b bytes)
typedef TokenPool<
    // Token type
    TOMLTokenType_t,
    // Max number of token
    TOMLLargeTokenSize_t,
    // Max number of bytes in the file
    TOMLLargeFileSize_t>
    TOMLLargeTokenPool;

// Regular NodePool storage
typedef TreeNodePool<
    // Node type
    TOMLNodeType_t,
    // Max number of nodes in the file
    std::uint32_t,
    // Regular TokenPool
    TOMLTokenPool>
    TOMLNodePool;

template<class S = TOMLNodePool>
class WASP_PUBLIC TOMLInterpreter : public Interpreter<S>
{
  public:

    typedef Interpreter<S>                 Super;
    TOMLInterpreter(std::ostream& error_stream = std::cerr);
    virtual ~TOMLInterpreter();

    TOMLInterpreter* create_nested_interpreter(Super* parent);
    TOMLInterpreter* document_parent() const {return m_parent;}

    /**
     * @brief parse parser the given input stream
     * @param input the stream of the input data
     * @param startLine the first line of the input stream
     * @param startColumn the first column of the input stream
     * @return true, iff no input processing errors were encountered
     */
    bool parse(std::istream& input,
               size_t   start_line   = 1u,
               size_t   start_column = 1u);
    bool parseStream(std::istream& input,
               const std::string& name,
               std::size_t   start_line   = 1u,
               std::size_t   start_column = 1u)
    {
        Interpreter<S>::stream_name() = name;
        return parse(input, start_line, start_column);
    }
    bool parseFile(const std::string& filename, size_t startLine = 1u);

    /**
     * @brief setStreamName sets the name of this stream and indicates whether
     * the stream/name is a file[path]
     * @param name the name of the stream or file[path]
     * @param isFile [default=false] indicates whether the name is a file path
     */
    void setStreamName(const std::string& name, bool isFile = false)
    {
        Interpreter<S>::stream_name() = name;
        mHasFile = isFile;
    }

    bool hasFile() const {return mHasFile;}

    /**
     * Generate a DataObject given the current parse tree
     * A map of object to node will be generated
     * @return true, iff object was successfully generated
     * Diagnostics will be generated and accessible from interpreter::error_diagnostics
     */
    bool generate_object();
    DataObject* object() const{return m_object;}
private:

    bool build_object(DataObject& object, const NodeView& node);
    bool build_array(DataArray& array, const NodeView& node);
    bool assign_value(Value& v, const NodeView& node);
    TOMLInterpreter* m_parent;
    DataObject* m_object;
    bool mHasFile;
};
#include "wasptoml/TOMLInterpreter.i.h"
typedef TOMLInterpreter<> DefaultTOMLInterpreter;
}  // end of namespace
#endif
