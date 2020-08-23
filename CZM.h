#ifndef CZM_H
#define CZM_H

#if __APPLE__
#include <GLUT/freeglut.h>
#else
#include <GL/freeglut.h>
#endif

#include "Materials.h"
#include "Grid.h"
#include "Blocks.h"
#include "GroundMotion.h"
#include "CohesiveZoneManager.h"

#include <iostream>
#include <vector>

class CZM {
public:

  CZM() {
    simulate = false;
  } // CZM()

  void initialize(int xcells, int ycells, float xsize, float ysize) {
    simulate = false;
    grid.initialize(xcells, ycells, xsize, ysize);
  } // initializeGrid()

  void reset(void) {
    simulate = false;
    grid.reset();
  } // initializeGrid()

  void edit(void) {
    simulate = false;
  } // editGrid()

  void simulation() {
    simulate = true;
    time = 0.0;

    // initialize all blocks
    blocks.initialize(grid);
    
    // initialize all cohesive zones
    faces.initialize(grid);
  } // initializeSimulation()

  void timeIntegrate(float dt) {
    if (simulate) {
      // initialize forces
      blocks.zeroForces();

      // update acceleration time history
      float ux_old;
      float uy_old;
      dispTimeHistory.evaluate(time, ux_old, uy_old);
      time += dt;
      float ux;
      float uy;
      dispTimeHistory.evaluate(time, ux, uy);
      std::cout << ux << std::endl;

      // apply boundary conditions
      blocks.applyIncrementalDisplacements(ux-ux_old, uy-uy_old, dt);

      // apply body forces
      float gravity = 9.8*1.0e-2; // [m/s^2]
      blocks.applyBodyForce(0.0, -gravity);

      // apply drag forces
      float drag_coefficient = 0.0;
      blocks.applyDragForce(drag_coefficient);

      // apply cohesive forces
      faces.applyCohesiveForces(blocks);

      // apply contact forces
      blocks.applyContactForces();

      // integrate block positions in time
      blocks.timeIntegrate(dt);
    } // if (simulate)
  } // timeIntegrate()

  void render() {
    if (simulate) {
      // render dynamic blocks
      blocks.render();
    } else {
      // render static grid
      grid.render();
    }
  } // renderGrid()
  
  MaterialInventory inventory;
  Grid grid;
  Blocks blocks;
  CohesiveZoneManager faces;

  bool simulate = false;
  float time;

  GroundMotion dispTimeHistory;
}; // CZM

#endif // CZM_H
