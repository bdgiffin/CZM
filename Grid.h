#ifndef GRID_H
#define GRID_H

#if __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "Materials.h"
#include <vector>
#include <cmath>

// forward declarations
void GridSwipeAdd(int x, int y);
void GridSwipeRemove(int x, int y);
void GridHighlight(int x, int y);

class Grid {
public:

  Grid() {
    Nx = 0;
    Ny = 0;
    dx = 0.0;
    width = 0.0;
    height = 0.0;
    brushColor = nullptr;
  } // Grid()
  
  void initialize(int xcells, int ycells, float xsize, float ysize) {
    Nx = xcells;
    Ny = ycells;
    width = xsize;
    height = ysize;
    dx = width / Nx;
    cells.resize(Nx*Ny);
    blockIDs.resize(Nx*Ny);
    std::fill(blockIDs.begin(), blockIDs.end(), -1);
  } // initialize()

  void reset() {
    for (int i = 0; i < Nx; i++) {
      for (int j = 0; j < Ny; j++) {
	// remove old material
	if (cells[Nx*j+i] != nullptr) cells[Nx*j+i]->quantity++;
	// replace with null material
	cells[Nx*j+i] = nullptr;
      } // for j = ...
    } // for i = ...
    std::fill(blockIDs.begin(), blockIDs.end(), -1);
  } // initialize()

  void edit() {
    std::fill(blockIDs.begin(), blockIDs.end(), -1);
  } // edit()

  void swipeAdd(int x, int y) {
    int i = std::min(std::max(int(floor(x / dx)),0),Nx-1);
    int j = std::min(std::max(Ny-1-int(floor(y / dx)),0),Ny-1);
    int cell_id = Nx*j+i;
    // check available quantity of new material
    if (brushColor->quantity > 0) {
      // remove old material
      if (cells[cell_id] != nullptr) cells[cell_id]->quantity++;
      // replace with new material
      cells[cell_id] = brushColor;
      brushColor->quantity--;
    }
  } // swipeAdd()

  void swipeRemove(int x, int y) {
    int i = std::min(std::max(int(floor(x / dx)),0),Nx-1);
    int j = std::min(std::max(Ny-1-int(floor(y / dx)),0),Ny-1);
    int cell_id = Nx*j+i;
    // remove old material
    if (cells[cell_id] != nullptr) cells[cell_id]->quantity++;
    // replace with null material
    cells[cell_id] = nullptr;
  } // swipeRemove()

  void update_cursor(int x, int y) {
    current_ij[0] = std::min(std::max(int(floor(x / dx)),0),Nx-1);
    current_ij[1] = std::min(std::max(Ny-1-int(floor(y / dx)),0),Ny-1);
  } // update_cursor()

  void modify(int button, int state, int x, int y) {
    int i = std::min(std::max(int(floor(x / dx)),0),Nx-1);
    int j = std::min(std::max(Ny-1-int(floor(y / dx)),0),Ny-1);
    int cell_id = Nx*j+i;
    if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
      // check available quantity of new material
      if (brushColor->quantity > 0) {
	// remove old material
	if (cells[cell_id] != nullptr) cells[cell_id]->quantity++;
	// replace with new material
	cells[cell_id] = brushColor;
	brushColor->quantity--;
      }
      glutMotionFunc(GridSwipeAdd);
    } else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)) {
      // remove old material
      if (cells[cell_id] != nullptr) cells[cell_id]->quantity++;
      // replace with null material
      cells[cell_id] = nullptr;
      glutMotionFunc(GridSwipeRemove);
    }
  } // modify()

  void selectBrushColor(int button, int dir, int x, int y) {
    if (dir > 0) {
      // Cycle up
      brushColor = brushColor->next;
    } else {
      // Cycle down
      brushColor = brushColor->prev;
    }
  } // selectBrush()

  void render(void) {

    // load material textures
    glBindTexture(GL_TEXTURE_2D, Material::textures);
    glEnable(GL_TEXTURE_2D);

    // draw highlighted cell
    if (brushColor->quantity > 0) {
      glBegin(GL_QUADS);
      int i = current_ij[0];
      int j = current_ij[1];
      float color[3] = { 1.0f, 1.0f, 1.0f };
      float* coords = brushColor->coord;
      glColor4f(color[0], color[1], color[2], 0.2);
      glTexCoord2f(coords[0], coords[1]); glVertex2f(dx*i,dx*j);
      glColor4f(color[0], color[1], color[2], 0.2);
      glTexCoord2f(coords[2], coords[1]); glVertex2f(dx*(i+1),dx*j);
      glColor4f(color[0], color[1], color[2], 0.2);
      glTexCoord2f(coords[2], coords[3]); glVertex2f(dx*(i+1),dx*(j+1));
      glColor4f(color[0], color[1], color[2], 0.2);
      glTexCoord2f(coords[0], coords[3]); glVertex2f(dx*i,dx*(j+1));
      glEnd();
    }

    // draw grid cells
    glBegin(GL_QUADS);
    for (int i = 0; i < Nx; i++) {
      for (int j = 0; j < Ny; j++) {
	if (cells[Nx*j+i] != nullptr) {
	  float color[3] = { 1.0f, 1.0f, 1.0f };
	  float* coords = cells[Nx*j+i]->coord;
	  glColor4f(color[0], color[1], color[2], 1);
	  glTexCoord2f(coords[0], coords[1]); glVertex2f(dx*i,dx*j);
	  glColor4f(color[0], color[1], color[2], 1);
	  glTexCoord2f(coords[2], coords[1]); glVertex2f(dx*(i+1),dx*j);
	  glColor4f(color[0], color[1], color[2], 1);
	  glTexCoord2f(coords[2], coords[3]); glVertex2f(dx*(i+1),dx*(j+1));
	  glColor4f(color[0], color[1], color[2], 1);
	  glTexCoord2f(coords[0], coords[3]); glVertex2f(dx*i,dx*(j+1));
	} // if (cells[Nx*j+i] != nullptr)
      } // for j = ...
    } // for i = ...
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // draw grid lines
    glBegin(GL_LINES);
    for (int i = 0; i < Nx; i++) {
      glColor4f(0.0f, 0.0f, 0.0f, 0.125f);
      glVertex2f(dx*i,0.0f);
      glColor4f(0.0f, 0.0f, 0.0f, 0.125f);
      glVertex2f(dx*i,dx*Ny);
    } // for i = ...
    for (int j = 0; j < Ny; j++) {
      glColor4f(0.0f, 0.0f, 0.0f, 0.125f);
      glVertex2f(0.0f,dx*j);
      glColor4f(0.0f, 0.0f, 0.0f, 0.125f);
      glVertex2f(dx*Nx,dx*j);
    } // for j = ...
    glEnd();

  } // render()

  int Nx;
  int Ny;
  int current_ij[2];
  float dx;
  float width;
  float height;
  std::vector<Material*> cells;
  std::vector<int> blockIDs;
  Material* brushColor;
}; // Grid

#endif // GRID_H
