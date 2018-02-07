SRC= \
  src/*.cpp

CFLAGS=/DGLEW_STATIC /DFREEGLUT_LIB_PRAGMAS=0 /MD /Iinclude
CXXFLAGS=/EHsc
LDFLAGS=opengl32.lib lib\freeglut.lib lib\glew.lib lib\SOIL.lib

all: snes.exe

snes.exe: $(SRC)
	cl $(CFLAGS) $(CXXFLAGS) $(SRC) $(LDFLAGS) /Fesnes.exe

clean:
	del snes.exe
	del *.obj
