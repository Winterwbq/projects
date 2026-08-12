#ifndef WASP_SnippetINTERPRETER_I_H
#define WASP_SnippetINTERPRETER_I_H

#include "waspcore/wasp_bug.h"

template<class S>
SnippetInterpreter<S>::SnippetInterpreter()
    : Interpreter<S>()
    , traceLexing(false)
    , traceParsing(false)
{
}
template<class S>
SnippetInterpreter<S>::SnippetInterpreter(std::ostream& err)
    : Interpreter<S>(err)
    , traceLexing(false)
    , traceParsing(false)
{
}
template<class S>
SnippetInterpreter<S>::~SnippetInterpreter()
{
}
template<class S>
bool SnippetInterpreter<S>::parse(std::istream& in,
                              size_t        startLine,
                              size_t        startColumn)
{
    return parseStream(
        in,
        "snippet",
        startLine, startColumn);
}
template<class S>
bool SnippetInterpreter<S>::parseStream(std::istream&      in,
                                    const std::string& sname,
                                    size_t             start_line,
                                    size_t             start_column)
{
    return Interpreter<S>::template parse_impl<SnippetParser>(in, sname, start_line,
                                                          start_column);
}
template<class S>
bool SnippetInterpreter<S>::parseFile(const std::string& filename, size_t line)
{
    std::ifstream in(filename.c_str());
    if (!in.good())
    {
        Interpreter<S>::error_diagnostic()
            << position(&filename)
            << " is either inaccessible or doesn't exist! Unable to read."
            << std::endl;
        return false;
    }
    return parseStream(in, filename, line);
}
template<class S>
bool SnippetInterpreter<S>::parseString(const std::string& input,
                                    const std::string& sname,
                                    size_t             startLine,
                                    size_t             startColumn)
{
    std::istringstream iss(input);
    return parseStream(iss, sname, startLine, startColumn);
}

#endif
