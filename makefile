FILES       = file_converter.cpp serial_reader.cpp serial_viewer.cpp 
LIB_FILES   = sharedmem.cpp draw.cpp form.cpp Frame.cpp serial_tools.cpp
EXE         = $(addprefix bin/, $(addsuffix .exe, $(basename $(FILES)))) 
LIB         = $(addprefix obj/, $(addsuffix .o, $(basename $(LIB_FILES))))
OPT_CXX     = -std=c++11
LINKED_LIBS = -lfltk -lfltk_gl -lGL -lGLU -lglut -lboost_chrono -lboost_system -lboost_filesystem -lboost_regex -lboost_serialization -lboost_date_time -lboost_thread -lpthread


all : dirtree $(EXE) $(LIB)

dirtree:
	@mkdir -p bin
	@mkdir -p obj

obj/%.o : src/%.cpp
	$(CXX) $(OPT_CXX) -c -o $@ $<

bin/%.exe : src/%.cpp $(LIB)
	$(CXX) $(OPT_CXX) -o $@ $< $(LIB) $(LINKED_LIBS)

clean :
	rm -f $(LIB) $(EXE)

cleanall :
	rm -rf bin/ obj/
