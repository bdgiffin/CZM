#ifndef BLOCKS_H
#define BLOCKS_H

#if __APPLE__
#include <GLUT/freeglut.h>
#else
#include <GL/freeglut.h>
#endif

#include "Materials.h"
#include "Grid.h"
#include <vector>
#include <math.h>

class Blocks {
public:
  
  void clear(void) {
    h = 0.0;
    L = 1.0; // [1 meter]
    dhdL = 0.0;
    Nblocks = 0;
    mat.clear();
    mass.clear();
    imass.clear();
    iinertia.clear();
    px.clear();
    py.clear();
    rz.clear();
    vx.clear();
    vy.clear();
    wz.clear();
    fx.clear();
    fy.clear();
    mz.clear();
    fixity.clear();

    player_mat = NULL;
    player_mass = 0.0;
    player_px = 0.0;
    player_py = 0.0;
    player_vx = 0.0;
    player_vy = 0.0;
    player_fx = 0.0;
    player_fy = 0.0;
  } // clear()

  void initialize(Grid& grid) {
    clear();
    h = grid.dx;
    L = 1.0; // [1 meter]
    dhdL = h / L;
    for (int i = 0; i < grid.Nx; i++) {
      for (int j = 0; j < grid.Ny; j++) {
	if (grid.cells[grid.Nx*j+i] != NULL) {
	  float area = L*L;
	  float value = grid.cells[grid.Nx*j+i]->density * area;
	  if (grid.cells[grid.Nx*j+i]->name == "Player") {
	    player_mat = grid.cells[grid.Nx*j+i];
	    player_mass = value;
	    player_px = L*(i+0.5);
	    player_py = L*(j+0.5);
	  } else {
	    mat.push_back(grid.cells[grid.Nx*j+i]);
	    mass.push_back(value);
	    value = 1.0 / value;
	    imass.push_back(value);
	    iinertia.push_back(6.0*value/area);
	    px.push_back(L*(i+0.5));
	    py.push_back(L*(j+0.5));
	    rz.push_back(0.0);
	    vx.push_back(0.0);
	    vy.push_back(0.0);
	    wz.push_back(0.0);
	    fx.push_back(0.0);
	    fy.push_back(0.0);
	    mz.push_back(0.0);
	    fixity.push_back(1.0);
	    if ((i == 0) || (j == 0) || (i == grid.Nx-1) || (j == grid.Ny-1)) fixity[Nblocks] = 0.0;
	    grid.blockIDs[grid.Nx*j+i] = Nblocks;
	    Nblocks++;
	  } // if (grid.cells[grid.Nx*j+i].name == "Player")
	} // if (grid.cells[grid.Nx*j+i] != NULL)
      } // for j = ...
    } // for i = ...
  } // initialize()

  void zeroForces() {
    std::fill(fx.begin(), fx.end(), 0.0);
    std::fill(fy.begin(), fy.end(), 0.0);
    std::fill(mz.begin(), mz.end(), 0.0);
    player_fx = 0.0;
    player_fy = 0.0;
  } // zeroForces()

  void applyIncrementalDisplacements(float ux, float uy, float dt) {
    float duxdt = ux / dt;
    float duydt = uy / dt;
    for (int i = 0; i < Nblocks; i++) {
      if (fixity[i] == 0.0) {
	px[i] += ux;
	py[i] += uy;
	vx[i] = duxdt;
	vy[i] = duydt;
      } // if (fixity[i] == 0.0)
    } // for i = ...
  } // applyBodyForce()

  void applyBodyForce(float bx, float by) {
    for (int i = 0; i < Nblocks; i++) {
      fx[i] += mass[i] * bx;
      fy[i] += mass[i] * by;
    } // for i = ...

    // apply body force to player
    if (player_mat != NULL) {
      player_fx += player_mass * bx;
      player_fy += player_mass * by;
    } // if (player_mat != NULL)
  } // applyBodyForce()

  void applyContactForces(void) {
    // apply contact force to player
    if (player_mat != NULL) {
      float halfL = 0.51*L;
      float halfB = 0.5*L;
      float maxContactDistanceSquared = 2.0*L*L;
      float contactStiffness = 1.0e+4;
      float contactViscosity = 1.0e+3;
      float nodex = 0.0;
      float nodey = 0.0;
      float fcx = 0.0;
      float fcy = 0.0;
      for (int i = 0; i < Nblocks; i++) {
	float distX = player_px - px[i];
	float distY = player_py - py[i];
	if ((distX*distX+distY*distY) < maxContactDistanceSquared) {
	  float s = sin(rz[i]);
	  float c = cos(rz[i]);
	  float drx = (c-s)*halfB;
	  float dry = (c+s)*halfB;

	  nodex = px[i]-drx;
	  nodey = py[i]-dry;
	  distX = player_px - nodex;
	  distY = player_py - nodey;
	  if ((fabs(distX) < halfL) && (fabs(distY) < halfL)) {
	    if (fabs(distX) > fabs(distY)) {
	      if (distX > 0.0) {
		// contact on left player face
		fcx = -contactStiffness*(distX-halfL);
		fcy = 0.0;
	      } else {
		// contact on right player face
		fcx = -contactStiffness*(distX+halfL);
		fcy = 0.0;
	      }
	    } else {
	      if (distY > 0.0) {
		// contact on bottom player face
		fcy = 0.0;
		fcx = -contactStiffness*(distY-halfL);
	      } else {
		// contact on top player face
		fcy = 0.0;
		fcx = -contactStiffness*(distY+halfL);
	      }
	    }
	  } else {
	    fcx = 0.0;
	    fcy = 0.0;
	  } // if ((fabs(distX) < halfL) && (fabs(distY) < halfL))
	  fx[i] -= fcx;
	  fy[i] -= fcy;
	  player_fx += fcx;
	  player_fy += fcy;

	  nodex = px[i]+dry;
	  nodey = py[i]-drx;
	  distX = player_px - nodex;
	  distY = player_py - nodey;
	  if ((fabs(distX) < halfL) && (fabs(distY) < halfL)) {
	    if (fabs(distX) > fabs(distY)) {
	      if (distX > 0.0) {
		// contact on left player face
		fcx = -contactStiffness*(distX-halfL);
		fcy = 0.0;
	      } else {
		// contact on right player face
		fcx = -contactStiffness*(distX+halfL);
		fcy = 0.0;
	      }
	    } else {
	      if (distY > 0.0) {
		// contact on bottom player face
		fcy = 0.0;
		fcx = -contactStiffness*(distY-halfL);
	      } else {
		// contact on top player face
		fcy = 0.0;
		fcx = -contactStiffness*(distY+halfL);
	      }
	    }
	  } else {
	    fcx = 0.0;
	    fcy = 0.0;
	  } // if ((fabs(distX) < halfL) && (fabs(distY) < halfL))
	  fx[i] -= fcx;
	  fy[i] -= fcy;
	  player_fx += fcx;
	  player_fy += fcy;

	  nodex = px[i]+drx;
	  nodey = py[i]+dry;
	  distX = player_px - nodex;
	  distY = player_py - nodey;
	  if ((fabs(distX) < halfL) && (fabs(distY) < halfL)) {
	    if (fabs(distX) > fabs(distY)) {
	      if (distX > 0.0) {
		// contact on left player face
		fcx = -contactStiffness*(distX-halfL);
		fcy = 0.0;
	      } else {
		// contact on right player face
		fcx = -contactStiffness*(distX+halfL);
		fcy = 0.0;
	      }
	    } else {
	      if (distY > 0.0) {
		// contact on bottom player face
		fcy = 0.0;
		fcx = -contactStiffness*(distY-halfL);
	      } else {
		// contact on top player face
		fcy = 0.0;
		fcx = -contactStiffness*(distY+halfL);
	      }
	    }
	  } else {
	    fcx = 0.0;
	    fcy = 0.0;
	  } // if ((fabs(distX) < halfL) && (fabs(distY) < halfL))
	  fx[i] -= fcx;
	  fy[i] -= fcy;
	  player_fx += fcx;
	  player_fy += fcy;

	  nodex = px[i]-dry;
	  nodey = py[i]+drx;
	  distX = player_px - nodex;
	  distY = player_py - nodey;
	  if ((fabs(distX) < halfL) && (fabs(distY) < halfL)) {
	    if (fabs(distX) > fabs(distY)) {
	      if (distX > 0.0) {
		// contact on left player face
		fcx = -contactStiffness*(distX-halfL);
		fcy = 0.0;
	      } else {
		// contact on right player face
		fcx = -contactStiffness*(distX+halfL);
		fcy = 0.0;
	      }
	    } else {
	      if (distY > 0.0) {
		// contact on bottom player face
		fcy = 0.0;
		fcx = -contactStiffness*(distY-halfL);
	      } else {
		// contact on top player face
		fcy = 0.0;
		fcx = -contactStiffness*(distY+halfL);
	      }
	    }
	  } else {
	    fcx = 0.0;
	    fcy = 0.0;
	  } // if ((fabs(distX) < halfL) && (fabs(distY) < halfL))
	  fx[i] -= fcx;
	  fy[i] -= fcy;
	  player_fx += fcx;
	  player_fy += fcy;
	  
	} // if ((distX*distX+distY*distY) < maxContactDistanceSquared)
      } // for i = ...
    } // if (player_mat != NULL)
  } // applyContactForces()

  void applyAcceleration(float ax, float ay) {
    for (int i = 0; i < Nblocks; i++) {
      fx[i] += ax;
      fy[i] += ay;
    } // for i = ...

    // apply acceleration to player
    if (player_mat != NULL) {
      player_fx += ax;
      player_fy += ay;
    } // if (player_mat != NULL)
  } // applyAcceleration()

  void applyDragForce(float drag_coefficient) {
    for (int i = 0; i < Nblocks; i++) {
      float drag_force = drag_coefficient * (vx[i]*vx[i] + vy[i]*vy[i]);
      float drag_moment = drag_coefficient * (wz[i]*wz[i]);
      fx[i] -= drag_force * vx[i];
      fy[i] -= drag_force * vy[i];
      mz[i] -= drag_moment * wz[i];
    } // for i = ...

    // apply drag to player
    if (player_mat != NULL) {
      float drag_force = drag_coefficient * (player_vx*player_vx + player_vy*player_vy);
      player_fx -= drag_force * player_vx;
      player_fy -= drag_force * player_vy;
    } // if (player_mat != NULL)
  } // applyDragForce()

  void timeIntegrate(float dt) {
    // integrate block positions in time
    for (int i = 0; i < Nblocks; i++) {
      vx[i] += dt * imass[i] * fx[i];
      vy[i] += dt * imass[i] * fy[i];
      wz[i] += dt * iinertia[i] * mz[i];
      vx[i] *= fixity[i];
      vy[i] *= fixity[i];
      wz[i] *= fixity[i];
      px[i] += dt * vx[i];
      py[i] += dt * vy[i];
      rz[i] += dt * wz[i];
    } // for i = ...

    // integrate player position in time
    if (player_mat != NULL) {
      player_vx += dt * player_fx / player_mass;
      player_vy += dt * player_fy / player_mass;
      player_px += dt * player_vx;
      player_py += dt * player_vy;
    } // if (player_mat != NULL)
  } // timeIntegrate()

  void render(void) {
    
    // load material textures
    glBindTexture(GL_TEXTURE_2D, Material::textures);
    glEnable(GL_TEXTURE_2D);
    
    float halfh = 0.5*h;

    // draw blocks
    glBegin(GL_QUADS);
    glColor4f(1.0, 1.0, 1.0, 1);
    for (int i = 0; i < Nblocks; i++) {
      float s = sin(rz[i]);
      float c = cos(rz[i]);
      float drx = (c-s)*halfh;
      float dry = (c+s)*halfh;
      float* coords = mat[i]->coord;
      glTexCoord2f(coords[0], coords[1]); glVertex2f(dhdL*px[i]-drx,dhdL*py[i]-dry);
      glTexCoord2f(coords[2], coords[1]); glVertex2f(dhdL*px[i]+dry,dhdL*py[i]-drx);
      glTexCoord2f(coords[2], coords[3]); glVertex2f(dhdL*px[i]+drx,dhdL*py[i]+dry);
      glTexCoord2f(coords[0], coords[3]); glVertex2f(dhdL*px[i]-dry,dhdL*py[i]+drx);
    } // for i = ...
    // draw player
    float* coords = player_mat->coord;
    glTexCoord2f(coords[0], coords[1]); glVertex2f(dhdL*player_px-halfh,dhdL*player_py-halfh);
    glTexCoord2f(coords[2], coords[1]); glVertex2f(dhdL*player_px+halfh,dhdL*player_py-halfh);
    glTexCoord2f(coords[2], coords[3]); glVertex2f(dhdL*player_px+halfh,dhdL*player_py+halfh);
    glTexCoord2f(coords[0], coords[3]); glVertex2f(dhdL*player_px-halfh,dhdL*player_py+halfh);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
  } // render()

  int Nblocks;
  float h; // block height in pixels (used for drawing)
  float L; // characteristic block dimension (used for physical computations)
  float dhdL; // ratio of pixel dimension h to physical dimension L
  std::vector<Material*> mat;
  std::vector<float> mass;
  std::vector<float> imass;
  std::vector<float> iinertia;
  std::vector<float> px;
  std::vector<float> py;
  std::vector<float> rz;
  std::vector<float> vx;
  std::vector<float> vy;
  std::vector<float> wz;
  std::vector<float> fx;
  std::vector<float> fy;
  std::vector<float> mz;
  std::vector<float> fixity;
  
  Material* player_mat;
  float player_mass;
  float player_px;
  float player_py;
  float player_vx;
  float player_vy;
  float player_fx;
  float player_fy;
}; // Blocks

#endif // BLOCKS_H
