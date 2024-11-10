#######################################################################################################

# Mac OS X
#INCLUDE_PATH      = -I/usr/local/include/
#LIBRARY_PATH      = -L/usr/local/lib/
#OPENGL_LIBS       = -framework OpenGL -framework GLUT

# # Linux
INCLUDE_PATH      = -I/usr/local/include/
LIBRARY_PATH      = -L/usr/local/lib/
OPENGL_LIBS       = -lglut -lGL

# # Windows / Cygwin
# INCLUDE_PATH      = -I/usr/include/opengl
# LIBRARY_PATH      = -L/usr/lib/w32api
# OPENGL_LIBS       = -lglut32 -lopengl32

#######################################################################################################

TARGET = czm_demo
CC = g++
LD = g++
CFLAGS = -std=c++11 -O3 -Wall -Wno-deprecated -pedantic $(INCLUDE_PATH) -I./include -I./src -DNDEBUG
LFLAGS = -std=c++11 -O3 -Wall -Wno-deprecated -Werror -pedantic $(LIBRARY_PATH) -DNDEBUG
LIBS = $(OPENGL_LIBS)

OBJS = czm_demo.o
HEADERS = 
WEBOBJS = index.js index.wasm index.html

default: $(TARGET)

.PHONY: web

all: clean $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LFLAGS) $(OBJS) $(LIBS) -o $(TARGET)

web:
	em++ -O3 -flto=full czm_demo.cpp -s WASM=0 -s LEGACY_GL_EMULATION=1 -s GL_FFP_ONLY=1 -s EXPORT_ALL=1 -o index.html --embed-file textures/ --embed-file ground_motions/

czm_demo.o: czm_demo.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c czm_demo.cpp -o czm_demo.o

clean:
	rm -f $(OBJS)
	rm -f $(TARGET)
	rm -f $(TARGET).exe
	rm -f $(WEBOBJS)
