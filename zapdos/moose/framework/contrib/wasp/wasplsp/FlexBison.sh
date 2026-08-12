flex -Caf --8bit -v Snippet.lex
sed -i '' 's/#include <FlexLexer.h>/#include \"waspcore\/FlexLexer.h\"/' SnippetLexer.cpp
bison --version
bison Snippet.bison --report=state --report=lookahead #-Wcounterexamples
sed -i '' 's/class SnippetParser/class WASP_PUBLIC SnippetParser/' SnippetParser.hpp

