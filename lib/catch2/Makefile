ifdef X86
CXXFLAGS += -m32
LDFLAGS += -m32
endif

all: obj/catch_amalgamated.o
obj/catch_amalgamated.o: src/catch_amalgamated.cpp include/catch2/catch_amalgamated.hpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) -I include/catch2

$(shell mkdir -p obj)
