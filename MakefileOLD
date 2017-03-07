TARGET = sph
OBJECTS = main.o src/ShaderProgram.o src/ShaderUtils.o src/OpenGLUtils.o

CXX    = C:/Rtools/mingw_64\bin\g++.exe
CFLAGS = -Wall -g

INCPATH += -IC:/sw/opengl/include -I./include
LIBPATH += -LC:/sw/opengl/lib -L./lib

DEL = rm

LIBS += -lopengl32 -lglut -lglu32 -lglew32

all: $(TARGET)

clean:
	$(DEL) -f $(OBJECTS) $(TARGET)

depend:
	$(DEL) -f Makefile.bak
	mv Makefile Makefile.bak
	sed '/^# DEPENDENCIES/,$$d' Makefile.bak > Makefile
	echo '# DEPENDENCIES' >> Makefile
	$(CXX) -MM *.cpp >> Makefile

.c.o: 	
	$(CXX) $(CFLAGS) $(INCPATH) -c -o $@ $<

.cc.o: 	
	$(CXX) $(CFLAGS) $(INCPATH) -c -o $@ $<

.cpp.o: 	
	$(CXX) $(CFLAGS) $(INCPATH) -c -o $@ $<

$(TARGET): $(OBJECTS)
	$(CXX) $(CFLAGS) $(INCPATH) -o $@ $^ $(LIBPATH) $(LIBS)

new: clean $(TARGET)

run: $(TARGET)
	sph.exe

debug: $(TARGET)
	C:\Strawberry\c\bin\gdb.exe sph.exe

# DEPENDENCIES
main.o: main.cpp
