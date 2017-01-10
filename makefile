FILES        = file_converter.cpp serial_reader.cpp serial_viewer.cpp 
LIB_FILES    = sharedmem.cpp draw.cpp form.cpp Frame.cpp serial_tools.cpp
EXE          = $(addprefix bin/, $(addsuffix .exe, $(basename $(FILES)))) 
LIB          = $(addprefix obj/, $(addsuffix .o, $(basename $(LIB_FILES))))
OPT_CXX      = -std=c++11
LINKED_LIBS  = -lfltk -lfltk_gl -lglut 

MY_OS := $(shell uname -s)
ifeq ($(MY_OS),Darwin)
LINKED_LIBS += -lboost_chrono-mt -lboost_system-mt -lboost_filesystem-mt -lboost_regex-mt -lboost_serialization-mt -lboost_date_time-mt -lboost_thread-mt -lpthread
LINKED_LIBS += -framework OpenGL
else
LINKED_LIBS += -lboost_chrono -lboost_system -lboost_filesystem -lboost_regex -lboost_serialization -lboost_date_time -lboost_thread -lpthread
LINKED_LIBS += -lGL -lGLU
endif

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
