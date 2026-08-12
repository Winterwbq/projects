flex -Caf --8bit -v TOML.lex
sed -i '' 's/#include <FlexLexer.h>/#include \"waspcore\/FlexLexer.h\"/' TOMLLexer.cpp
bison --version
bison TOML.bison --report=state --report=lookahead #-Wcounterexamples
sed -i '' 's/class TOMLParser/class WASP_PUBLIC TOMLParser/' TOMLParser.hpp
#bison TOML.bison --report=state --report=lookahead --report=solved  -g
