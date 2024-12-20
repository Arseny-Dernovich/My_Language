TARGET = read.exe

CXX = g++

CXXSTD = -std=c++11

CXXFLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline \
           -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum \
           -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -pipe -fexceptions -Wcast-qual \
           -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 \
           -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor \
           -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing \
           -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla

SRCS = Read.cpp Syntax_Analis.cpp DOT.cpp Main.cpp Simply.cpp Back_end.cpp Build_Table_Name_Func.cpp
OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXSTD) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)
