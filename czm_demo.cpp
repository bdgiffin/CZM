#if __APPLE__
#include <GLUT/freeglut.h>
#else
#include <GL/freeglut.h>
#endif

#include <string>
#include <iostream>
#include <fstream>

#include <SOIL/SOIL.h>
#include "CZM.h"

using namespace std;

CZM czm;

// rendering projection parameters
const static int WINDOW_WIDTH  = 1600;
const static int WINDOW_HEIGHT = 1000;
const static double VIEW_SCALE = 1.0;
const static double VIEW_WIDTH = VIEW_SCALE*WINDOW_WIDTH;
const static double VIEW_HEIGHT = VIEW_SCALE*WINDOW_HEIGHT;

const static float DT = 0.5f; // integration timestep

GLuint LoadTexture( const char * filename ) {
  GLuint texture = SOIL_load_OGL_texture(filename,SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

  // Typical Texture Generation Using Data From The Bitmap
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glBindTexture (GL_TEXTURE_2D, 0);

  return texture;
} // LoadTexture()

void InitCZM(void) {

  // create blank grid
  czm.initialize(WINDOW_WIDTH/50, WINDOW_HEIGHT/50, VIEW_WIDTH, VIEW_HEIGHT);

} // InitCZM()

void Update(void) {
  int Nsubincrements = 500;
  float ddt = DT / Nsubincrements;
  for (int i = 0; i < Nsubincrements; i++) {
    czm.timeIntegrate(ddt);
  } // for i = ...

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
  glColor3f(czm.grid.brushColor->color[0], czm.grid.brushColor->color[1], czm.grid.brushColor->color[2]);
  glRasterPos2f(x, y);
  for (char* c = text; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);  // Updates the position
  }
} // drawText()

void Render(void) {
  glClear(GL_COLOR_BUFFER_BIT);
	
  glLoadIdentity();
  glOrtho(0, VIEW_WIDTH, 0, VIEW_HEIGHT, 0, 1);

  czm.render();

  char buffer[16];
  sprintf (buffer, "%d", czm.grid.brushColor->quantity);
  drawText(10, WINDOW_HEIGHT-28, buffer);

  glutSwapBuffers();
}

void Keyboard(unsigned char c, __attribute__((unused)) int x, __attribute__((unused)) int y) {   
  switch(c) {
  case 'r': 
  case 'R':  
    czm.reset(); 
    break;
  case 'e': 
  case 'E':  
    czm.edit(); 
    break;
  case 's':
  case 'S':
    czm.simulation();
    break;
  }
}

void Arrows(int key, __attribute__((unused)) int x, __attribute__((unused)) int y) {   
  switch(key) {
  case GLUT_KEY_LEFT:  
    //czm.accelerateX = +1.0;
    break;
  case GLUT_KEY_RIGHT: 
    //czm.accelerateX = -1.0;
    break;
  case GLUT_KEY_UP:    
    //czm.accelerateY = -1.0;
    break;
  case GLUT_KEY_DOWN: 
    //czm.accelerateY = +1.0;
    break;
  }
} // Arrows()

void MouseWheel(int button, int dir, int x, int y) {
  czm.grid.selectBrushColor(button, dir, x, y);
} // MouseWheel()

void Mouse(int button, int state, int x, int y) {
  if ((button == 3) || (button == 4)) { // It's a wheel event
      // Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
      if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
      MouseWheel(button, (button == 3) ? +1 : -1, x, y);
  } else {
    czm.grid.modify(button, state, x, y);
  }
} // Mouse()

void GridSwipeAdd(int x, int y) {
  czm.grid.swipeAdd(x, y);
} // GridSwipeAdd()

void GridSwipeRemove(int x, int y) {
  czm.grid.swipeRemove(x, y);
} // GridSwipeRemove()

int main(int argc, char** argv) {
  glutInitWindowSize(WINDOW_WIDTH,WINDOW_HEIGHT);
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutCreateWindow("CZM Library Demo");
  glutDisplayFunc(Render);
  glutIdleFunc(Update);
  glutMouseFunc(Mouse);
  glutKeyboardFunc(Keyboard);
  glutSpecialFunc(Arrows);

  InitGL();
  InitCZM();

  // populate material inventory
  Material::textures = LoadTexture("textures/textures.png");
  czm.inventory.insertMaterial(new Material("Rock",      3500.0,      16, 0.38, 0.38, 0.38, 0.0, 0.0, 0.2, 0.2));
  czm.inventory.insertMaterial(new Material("Soil",      1500.0,      64, 0.25, 0.50, 0.25, 0.2, 0.0, 0.4, 0.2));
  czm.inventory.insertMaterial(new Material("Concrete",  2400.0,      32, 0.75, 0.75, 0.75, 0.4, 0.0, 0.6, 0.2));
  czm.inventory.insertMaterial(new Material("Wood",       600.0,      32, 0.80, 0.45, 0.10, 0.6, 0.0, 0.8, 0.2));
  czm.inventory.insertMaterial(new Material("Steel",     8050.0,      16, 0.50, 0.63, 0.70, 0.8, 0.0, 1.0, 0.2));
  czm.inventory.insertMaterial(new Material("Water",     1000.0,      32, 0.29, 0.22, 1.00, 0.0, 0.2, 0.2, 0.4));
  czm.inventory.insertMaterial(new Material("Player",    1000.0,       1, 1.00, 0.00, 0.50, 0.2, 0.2, 0.4, 0.4));

  // set default brush color
  czm.grid.brushColor = czm.inventory.getFirstMaterial();

  czm.dispTimeHistory.ux.clear();
  czm.dispTimeHistory.uy.clear();
  czm.dispTimeHistory.dt = 0.25; // 0.005 s
  czm.dispTimeHistory.scale = 10.0; // from cm to m
  
  string line;
  float value;
  std::ifstream file;
  file.open ("ground_motions/sanfran/RSN23_SANFRAN_GGP100.DT2");
  std::getline(file, line);
  std::cout << line << std::endl;
  std::getline(file, line);
  std::cout << line << std::endl;
  std::getline(file, line);
  std::cout << line << std::endl;
  std::getline(file, line);
  std::cout << line << std::endl;
  while (file >> value) czm.dispTimeHistory.ux.push_back(value);
  file.close();

  file.open ("ground_motions/sanfran/RSN23_SANFRAN_GGP-UP.DT2");
  std::getline(file, line);
  std::cout << line << std::endl;
  std::getline(file, line);
  std::cout << line << std::endl;
  std::getline(file, line);
  std::cout << line << std::endl;
  std::getline(file, line);
  std::cout << line << std::endl;
  while (file >> value) czm.dispTimeHistory.uy.push_back(value);
  file.close();

  glutMainLoop();
  return 0;
}
