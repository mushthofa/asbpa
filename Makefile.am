bin_PROGRAMS = bpa
bpa_SOURCES = ASVerifier.cpp Atom.cpp AtomFactory.cpp AtomNode.cpp AtomSet.cpp \
	Bitmap.cpp BoostComponentFinder.cpp BoostHCFDetector.cpp Component.cpp \
	ComponentFinder.cpp DLVASPSolver.cpp DLVResultParser.cpp DependencyGraph.cpp Error.cpp \
	EvalComp.cpp Globals.cpp GraphBuilder.cpp GraphProcessor.cpp Literal.cpp \
	MinChecker.cpp ModelCheck.cpp ModelGenerator.cpp OutputBuilder.cpp PrintVisitor.cpp \
	ProcessBuff.cpp Program.cpp ProgramBuilder.cpp PrologProgramSubsetGenerator.cpp \
	PrologResultParser.cpp Registry.cpp Rule.cpp SafetyChecker.cpp SimpleMethodSelector.cpp \
	SpiritProgramParser.cpp Term.cpp XSBPrologEngine.cpp bpa.cpp

# set the include path found by configure
INCLUDES= $(all_includes)

# the library search path.
bpa_LDFLAGS = $(all_libraries) 
noinst_HEADERS = ASPResultParser.h ASPSolver.h ASVerifier.h Atom.h \
	AtomFactory.h AtomNode.h AtomSet.h BaseVisitor.h Bitmap.h BoostComponentFinder.h \
	BoostHCFDetector.h Component.h ComponentFinder.h DLVASPSolver.h DLVResultParser.h \
	DLVResult_grammar.h DependencyGraph.h Error.h EvalComp.h GetPot.h Globals.h GraphBuilder.h \
	GraphProcessor.h HCFDetector.h Literal.h MethodSelector.h MinChecker.h ModelCheck.h \
	ModelGenerator.h OutputBuilder.h PrintVisitor.h ProcessBuff.h Program.h ProgramBuilder.h \
	ProgramParser.h ProgramSubsetGenerator.h PrologEngine.h PrologProgramSubsetGenerator.h \
	PrologResultParser.h Registry.h Rule.h SafetyChecker.h SimpleMethodSelector.h \
	SpiritProgramParser.h Term.h XSBPrologEngine.h logic_grammar.h
