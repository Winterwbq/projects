flex -Caf --8bit -v MCNPLexer.lex
sed -i '' 's/#include <FlexLexer.h>/#include \"waspcore\/FlexLexer.h\"/' MCNPLexer.cpp 
bison --version
bison MCNPParser.bison --report=state --report=lookahead --report=solved  -g #-Wcounterexamples
sed -i '' 's/class MCNPParser/class WASP_PUBLIC MCNPParser/' MCNPParser.hpp
