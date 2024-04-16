include_directories("include" "source" "scriptany" "scriptarray" "scriptdictionary" "scriptfile" "scriptstring")

file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE CPPS "*.cpp")

SOURCE_GROUP(Main REGULAR_EXPRESSION .*)
SOURCE_GROUP(ScriptAny REGULAR_EXPRESSION scriptany?.*)
SOURCE_GROUP(ScriptArray REGULAR_EXPRESSION scriptarray?.*)
SOURCE_GROUP(ScriptDictionary REGULAR_EXPRESSION scriptdictionary?.*)
SOURCE_GROUP(ScriptFile REGULAR_EXPRESSION scriptfile?.*)
SOURCE_GROUP(ScriptString REGULAR_EXPRESSION scriptstring?.*)

ADD_LIBRARY(AngelScript STATIC ${HEADERS} ${CPPS})