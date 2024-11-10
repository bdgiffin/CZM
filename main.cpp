#if __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <string>
#include <iostream>

//#include <SOIL/SOIL.h>
#include "Materials.h"
#include "Grid.h"

using namespace std;

MaterialInventory inventory;
Grid grid;

// rendering projection parameters
const static int WINDOW_WIDTH  = 1600;
const static int WINDOW_HEIGHT = 1000;
const static double VIEW_SCALE = 1.0;
const static double VIEW_WIDTH = VIEW_SCALE*WINDOW_WIDTH;
const static double VIEW_HEIGHT = VIEW_SCALE*WINDOW_HEIGHT;

const static float DT = 0.4f; // integration timestep

void InitGrid(void) {

  // create blank grid
  grid.initialize(WINDOW_WIDTH/50, WINDOW_HEIGHT/50, VIEW_WIDTH, VIEW_HEIGHT);

} // InitGrid()

void Update(void) {
  glutPostRedisplay();
} // Update()

void InitGL(void) {
  glClearColor(0.8f,0.8f,1.0f,1);
  glEnable(GL_POINT_SMOOTH);
  glMatrixMode(GL_PROJECTION);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
} // InitGL()

void drawText(float x, float y, char* text) {
  /* FONTS:
     GLUT_BITMAP_8_BY_13
     GLUT_BITMAP_9_BY_15
     GLUT_BITMAP_TIMES_ROMAN_10
     GLUT_BITMAP_TIMES_ROMAN_24
     GLUT_BITMAP_HELVETICA_10
     GLUT_BITMAP_HELVETICA_12
     GLUT_BITMAP_HELVETICA_18 
  */
  glColor3f(grid.brushColor->color[0], grid.brushColor->color[1], grid.brushColor->color[2]);
  glRasterPos2f(x, y);
  for (char* c = text; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);  // Updates the position
  }
} // drawText()

GLuint LoadTexture( const char * filename ) {
  //GLuint texture = SOIL_load_OGL_texture(filename,SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

  // Typical Texture Generation Using Data From The Bitmap
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glBindTexture (GL_TEXTURE_2D, 0);

  return texture;
} // LoadTexture()

void Render(void) {
  glClear(GL_COLOR_BUFFER_BIT);
	
  glLoadIdentity();
  glOrtho(0, VIEW_WIDTH, 0, VIEW_HEIGHT, 0, 1);

  grid.render();
  
  char buffer[16];
  sprintf (buffer, "%d", grid.brushColor->quantity);
  drawText(10, WINDOW_HEIGHT-28, buffer);

  glutSwapBuffers();
} // Render()

void Keyboard(unsigned char c, __attribute__((unused)) int x, __attribute__((unused)) int y) {   
  switch(c) {
  case 'r': 
  case 'R':  
    grid.reset(); 
    break;
  }
} // Keyboard()

void MouseWheel(int button, int dir, int x, int y) {
  grid.selectBrushColor(button, dir, x, y);
} // MouseWheel()

void Mouse(int button, int state, int x, int y) {
  if ((button == 3) || (button == 4)) { // It's a wheel event
      // Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
      if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
      MouseWheel(button, (button == 3) ? +1 : -1, x, y);
  } else {
    grid.modify(button, state, x, y);
  }
} // Mouse()

void GridSwipeAdd(int x, int y) {
  grid.swipeAdd(x, y);
} // GridSwipeAdd()

void GridSwipeRemove(int x, int y) {
  grid.swipeRemove(x, y);
} // GridSwipeRemove()

int main(int argc, char** argv) {
  glutInitWindowSize(WINDOW_WIDTH,WINDOW_HEIGHT);
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutCreateWindow("Grid Demo");
  glutDisplayFunc(Render);
  glutIdleFunc(Update);
  glutMouseFunc(Mouse);
  glutKeyboardFunc(Keyboard);

  InitGL();

  inventory.insertMaterial(new Material("Rock",      3500.0,      16, 0.38, 0.38, 0.38, LoadTexture("bitmaps/rock.png")));
  inventory.insertMaterial(new Material("Soil",      1500.0,      64, 0.25, 0.50, 0.25, LoadTexture("bitmaps/soil.png")));
  inventory.insertMaterial(new Material("Concrete",  2400.0,      32, 0.75, 0.75, 0.75, LoadTexture("bitmaps/concrete.png")));
  inventory.insertMaterial(new Material("Wood",       600.0,      32, 0.80, 0.45, 0.10, LoadTexture("bitmaps/wood.png")));
  inventory.insertMaterial(new Material("Steel",     8050.0,      16, 0.50, 0.63, 0.70, LoadTexture("bitmaps/steel.png")));
  inventory.insertMaterial(new Material("Water",     1000.0,      32, 0.29, 0.22, 1.00, LoadTexture("bitmaps/water.png")));
  grid.brushColor = inventory.getFirstMaterial();
  InitGrid();

  glutMainLoop();
  return 0;
} // main()
