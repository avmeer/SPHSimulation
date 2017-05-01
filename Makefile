########################################
## SETUP MAKEFILE
##      Set the appropriate TARGET (our
## executable name) and any OBJECT files
## we need to compile for this program.
##
## Next set the path to our local
## include/, lib/, and bin/ folders.
## (If you we are compiling in the lab,
## then you can ignore these values.
## They are only for if you are
## compiling on your personal machine.)
##
## Set if we are compiling in the lab
## environment or not.  Set to:
##    1 - if compiling in the Lab
##    0 - if compiling at home
##
## Finally, set the flags for which
## libraries are using and want to
## compile against.
########################################

TARGET = finalProject
OBJECTS = main.o src/OpenGLUtils.o src/ShaderProgram.o src/ShaderUtils.o

IN_LAB = 1

ifeq ($(IN_LAB), 1)
	CXX = C:/Rtools/mingw_64\bin\g++.exe
	INCPATH += -IC:/sw/opengl/include -I./include
	LIBPATH += -LC:/sw/opengl/lib -L./lib
endif

ifeq ($(OS), Windows_NT)
	CFLAGS = -Wall -g

	INCPATH += -I./include
	LIBPATH += -L./lib
endif

#############################
## SETUP GLFW
#############################
ifeq ($(OS), Windows_NT)
	LIBS += -lglfw3dll
else
	LIBS += -lglfw
endif

#############################
## SETUP OpenGL & GLUT 
#############################
ifeq ($(OS), Windows_NT)
	LIBS +=  -lopengl32 -lglut -lglu32 
else # Linux and all other builds
	LIBS += -lGL -lglut -lGLU
endif

#############################
## SETUP GLEW 
#############################
ifeq ($(OS), Windows_NT)
	LIBS += -lglew32
else
	LIBS += -lGLEW
endif



#############################
## COMPILATION INSTRUCTIONS 
#############################

all: $(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

.c.o: 	
	$(CXX) $(CFLAGS) $(INCPATH) -c -o $@ $<

.cc.o: 	
	$(CXX) $(CFLAGS) $(INCPATH) -c -o $@ $<

.cpp.o: 	
	$(CXX) $(CFLAGS) $(INCPATH) -c -o $@ $<

$(TARGET): $(OBJECTS) 
	$(CXX) $(CFLAGS) $(INCPATH) -o $@ $^ $(LIBPATH) $(LIBS)

run: $(TARGET)
	finalProject.exe

debug: $(TARGET)
	C:\Strawberry\c\bin\gdb.exe finalProject.exe
