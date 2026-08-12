#ifndef WASP_MCNP_INTERPRETER_I_H
#define WASP_MCNP_INTERPRETER_I_H

template<class S>
MCNPInterpreter<S>::MCNPInterpreter()
    : Interpreter<S>()
    , traceLexing(false)
    , traceParsing(false)
    , singleParse(false)
    , mHasFile(false)
{
}
template<class S>
MCNPInterpreter<S>::MCNPInterpreter(std::ostream& err)
    : Interpreter<S>(err)
    , traceLexing(false)
    , traceParsing(false)
    , singleParse(false)
    , mHasFile(false)
{
}
template<class S>
MCNPInterpreter<S>::~MCNPInterpreter()
{
}
template<class S>
bool MCNPInterpreter<S>::parse(std::istream& in,
                             size_t        startLine,
                             size_t        startColumn)
{
    std::string name = Interpreter<S>::stream_name();
    return parseStream(
        in, name,
        startLine, startColumn);
}
template<class S>
bool MCNPInterpreter<S>::parseStream(std::istream&      in,
                                   const std::string& sname,
                                   size_t             start_line,
                                   size_t             start_column)
{
    return Interpreter<S>::template parse_impl<MCNPParser>(in, sname, start_line,
                                                          start_column);
}
template<class S>
bool MCNPInterpreter<S>::parseFile(const std::string& filename, size_t line)
{
    std::string name = filename;
    std::ifstream in(name.c_str());
    if (!in.good())
    {
        Interpreter<S>::error_stream()
            << "file '" << filename
            << "' is either inaccessible or doesn't exist! Unable to read."
            << std::endl;
        return false;
    }
    setStreamName(name, true);
    return parseStream(in, name, line);
}
template<class S>
bool MCNPInterpreter<S>::parseString(const std::string& input,
                                   const std::string& sname,
                                   size_t             startLine,
                                   size_t             startColumn)
{
    std::istringstream iss(input);
    return parseStream(iss, sname, startLine, startColumn);
}

#endif
