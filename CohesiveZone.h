#ifndef COHESIVE_ZONE_H
#define COHESIVE_ZONE_H

#include "Materials.h"
#include "Blocks.h"
#include <vector>
#include <cmath>

enum Orientation { X, Y };

class CohesiveZone {
public:
  
  void insertFaceX(int left, int right) {
    xFaceIDs.push_back(std::pair<int,int>(left,right));
  } // insertFaceX()

  void insertFaceY(int lower, int upper) {
    yFaceIDs.push_back(std::pair<int,int>(lower,upper));
  } // insertFaceY()

  virtual void initialize(void) = 0;

  virtual void computeTraction(float* ux, float* uy, float* vx, float* vy, float* nx, float* ny, float* tx, float* ty, float divdx, Orientation dir, int size) = 0;

  void applyForces(Blocks& blocks) {
    // declare local workspace arrays
    int MAX_SIZE = 128;
    float nx[MAX_SIZE];
    float ny[MAX_SIZE];
    float ux[MAX_SIZE];
    float uy[MAX_SIZE];
    float vx[MAX_SIZE];
    float vy[MAX_SIZE];
    float tx[MAX_SIZE];
    float ty[MAX_SIZE];
    float rxm[MAX_SIZE];
    float rym[MAX_SIZE];
    float rxp[MAX_SIZE];
    float ryp[MAX_SIZE];

    // define local constants
    float dx = blocks.L;
    float divdx = 1.0/dx;
    float halfdx = 0.5*dx;
    float divsqrt3 = 1.0/sqrt(3.0);

    // loop over all x-faces
    for (int i = 0; i < xFaceIDs.size(); i++) {
      // get the global block ids of the current x-face
      int left  = xFaceIDs[i].first;
      int right = xFaceIDs[i].second;

      // -----o . . . . . o-----
      // #####|   2 x     |#####
      //  left|     :-> N |right
      // #####|   1 x     |#####
      // -----o . . . . . o-----

      // compute the sin and cos of the left- and right- block rotations
      float sinl_halfdx = sin(blocks.rz[left])*halfdx;
      float cosl_halfdx = cos(blocks.rz[left])*halfdx;
      float sinr_halfdx = sin(blocks.rz[right])*halfdx;
      float cosr_halfdx = cos(blocks.rz[right])*halfdx;
      
      // compute the average x-face normal direction
      float rzavg = 0.5*(blocks.rz[left]+blocks.rz[right]);
      nx[2*i]   = cos(rzavg);
      ny[2*i]   = sin(rzavg);
      nx[2*i+1] = nx[2*i];
      ny[2*i+1] = ny[2*i];

      // compute the quadrature point relative displacements
      float ux0 = blocks.px[right] - blocks.px[left] - cosr_halfdx - cosl_halfdx;
      float uy0 = blocks.py[right] - blocks.py[left] - sinr_halfdx - sinl_halfdx;
      float diff_sin_halfdx = (sinr_halfdx - sinl_halfdx)*divsqrt3;
      float diff_cos_halfdx = (cosr_halfdx - cosl_halfdx)*divsqrt3;
      ux[2*i]   = ux0 + diff_sin_halfdx;
      uy[2*i]   = uy0 - diff_cos_halfdx;
      ux[2*i+1] = ux0 - diff_sin_halfdx;
      uy[2*i+1] = uy0 + diff_cos_halfdx;

      // compute the time-rates for sin and cos of the left- and right- block rotations
      float dsinl_halfdx = +cosl_halfdx*blocks.wz[left];
      float dcosl_halfdx = -sinl_halfdx*blocks.wz[left];
      float dsinr_halfdx = +cosr_halfdx*blocks.wz[right];
      float dcosr_halfdx = -sinr_halfdx*blocks.wz[right];

      // compute the quadrature point relative velocities
      float vx0 = blocks.vx[right] - blocks.vx[left] - dcosr_halfdx - dcosl_halfdx;
      float vy0 = blocks.vy[right] - blocks.vy[left] - dsinr_halfdx - dsinl_halfdx;
      float diff_dsin_halfdx = (dsinr_halfdx - dsinl_halfdx)*divsqrt3;
      float diff_dcos_halfdx = (dcosr_halfdx - dcosl_halfdx)*divsqrt3;
      vx[2*i]   = vx0 + diff_dsin_halfdx;
      vy[2*i]   = vy0 - diff_dcos_halfdx;
      vx[2*i+1] = vx0 - diff_dsin_halfdx;
      vy[2*i+1] = vy0 + diff_dcos_halfdx;

      // compute the moment arms relative to the left- and right- blocks
      rxm[2*i]   = + cosl_halfdx + sinl_halfdx*divsqrt3 + 0.5*ux[2*i];
      rym[2*i]   = + sinl_halfdx - cosl_halfdx*divsqrt3 + 0.5*uy[2*i];
      rxm[2*i+1] = + cosl_halfdx - sinl_halfdx*divsqrt3 + 0.5*ux[2*i+1];
      rym[2*i+1] = + sinl_halfdx + cosl_halfdx*divsqrt3 + 0.5*uy[2*i+1];
      rxp[2*i]   = - cosr_halfdx + sinr_halfdx*divsqrt3 - 0.5*ux[2*i];
      ryp[2*i]   = - sinr_halfdx - cosr_halfdx*divsqrt3 - 0.5*uy[2*i];
      rxp[2*i+1] = - cosr_halfdx - sinr_halfdx*divsqrt3 - 0.5*ux[2*i+1];
      ryp[2*i+1] = - sinr_halfdx + cosr_halfdx*divsqrt3 - 0.5*uy[2*i+1];
    } // for i = ...

    // compute the cohesive traction vectors at each quadrature point
    computeTraction(ux,uy,vx,vy,nx,ny,tx,ty,divdx,Orientation::X,2*xFaceIDs.size());

    // loop over all x-faces
    for (int i = 0; i < xFaceIDs.size(); i++) {
      // get the global block ids of the current x-face
      int left  = xFaceIDs[i].first;
      int right = xFaceIDs[i].second;

      // sum the cohesive tractions to the applied block forces
      float fx = (tx[2*i] + tx[2*i+1])*dx;
      float fy = (ty[2*i] + ty[2*i+1])*dx;
      blocks.fx[left]  += fx;
      blocks.fy[left]  += fy;
      blocks.fx[right] -= fx;
      blocks.fy[right] -= fy;
      blocks.mz[left]  += (rxm[2*i]*ty[2*i] - rym[2*i]*tx[2*i] + rxm[2*i+1]*ty[2*i+1] - rym[2*i+1]*tx[2*i+1])*halfdx;
      blocks.mz[right] -= (rxp[2*i]*ty[2*i] - ryp[2*i]*tx[2*i] + rxp[2*i+1]*ty[2*i+1] - ryp[2*i+1]*tx[2*i+1])*halfdx;
    } // for i = ...

    // loop over all y-faces
    for (int i = 0; i < yFaceIDs.size(); i++) {
      // get the global block ids of the current y-face
      int lower = yFaceIDs[i].first;
      int upper = yFaceIDs[i].second;

      // -----o . . . . . o-----
      // #####|   2 x     |#####
      // lower|     :-> N |upper
      // #####|   1 x     |#####
      // -----o . . . . . o-----

      // compute the sin and cos of the lower- and upper- block rotations
      float sinl_halfdx = sin(blocks.rz[lower])*halfdx;
      float cosl_halfdx = cos(blocks.rz[lower])*halfdx;
      float sinu_halfdx = sin(blocks.rz[upper])*halfdx;
      float cosu_halfdx = cos(blocks.rz[upper])*halfdx;

      // compute the average y-face normal direction
      float rzavg = 0.5*(blocks.rz[lower]+blocks.rz[upper]);
      nx[2*i]   =-sin(rzavg);
      ny[2*i]   = cos(rzavg);
      nx[2*i+1] = nx[2*i];
      ny[2*i+1] = ny[2*i];

      // compute the quadrature point relative displacements
      float ux0 = blocks.px[upper] - blocks.px[lower] + sinu_halfdx + sinl_halfdx;
      float uy0 = blocks.py[upper] - blocks.py[lower] - cosu_halfdx - cosl_halfdx;
      float diff_sin_halfdx = (sinu_halfdx - sinl_halfdx)*divsqrt3;
      float diff_cos_halfdx = (cosu_halfdx - cosl_halfdx)*divsqrt3;
      ux[2*i]   = ux0 + diff_cos_halfdx;
      uy[2*i]   = uy0 + diff_sin_halfdx;
      ux[2*i+1] = ux0 - diff_cos_halfdx;
      uy[2*i+1] = uy0 - diff_sin_halfdx;

      // compute the time-rates for sin and cos of the lower- and upper- block rotations
      float dsinl_halfdx = +cosl_halfdx*blocks.wz[lower];
      float dcosl_halfdx = -sinl_halfdx*blocks.wz[lower];
      float dsinu_halfdx = +cosu_halfdx*blocks.wz[upper];
      float dcosu_halfdx = -sinu_halfdx*blocks.wz[upper];

      // compute the quadrature point relative velocities
      float vx0 = blocks.vx[upper] - blocks.vx[lower] + dsinu_halfdx + dsinl_halfdx;
      float vy0 = blocks.vy[upper] - blocks.vy[lower] - dcosu_halfdx - dcosl_halfdx;
      float diff_dsin_halfdx = (dsinu_halfdx - dsinl_halfdx)*divsqrt3;
      float diff_dcos_halfdx = (dcosu_halfdx - dcosl_halfdx)*divsqrt3;
      vx[2*i]   = vx0 + diff_dcos_halfdx;
      vy[2*i]   = vy0 + diff_dsin_halfdx;
      vx[2*i+1] = vx0 - diff_dcos_halfdx;
      vy[2*i+1] = vy0 - diff_dsin_halfdx;

      // compute the moment arms relative to the lower- and upper- blocks
      rxm[2*i]   = - sinl_halfdx + cosl_halfdx*divsqrt3 + 0.5*ux[2*i];
      rym[2*i]   = + cosl_halfdx + sinl_halfdx*divsqrt3 + 0.5*uy[2*i];
      rxm[2*i+1] = - sinl_halfdx - cosl_halfdx*divsqrt3 + 0.5*ux[2*i+1];
      rym[2*i+1] = + cosl_halfdx - sinl_halfdx*divsqrt3 + 0.5*uy[2*i+1];
      rxp[2*i]   = + sinu_halfdx + cosu_halfdx*divsqrt3 - 0.5*ux[2*i];
      ryp[2*i]   = - cosu_halfdx + sinu_halfdx*divsqrt3 - 0.5*uy[2*i];
      rxp[2*i+1] = + sinu_halfdx - cosu_halfdx*divsqrt3 - 0.5*ux[2*i+1];
      ryp[2*i+1] = - cosu_halfdx - sinu_halfdx*divsqrt3 - 0.5*uy[2*i+1];
    } // for i = ...

    // compute the cohesive traction vectors at each quadrature point
    computeTraction(ux,uy,vx,vy,nx,ny,tx,ty,divdx,Orientation::Y,2*yFaceIDs.size());

    // loop over all y-faces
    for (int i = 0; i < yFaceIDs.size(); i++) {
      // get the global block ids of the current y-face
      int lower = yFaceIDs[i].first;
      int upper = yFaceIDs[i].second;

      // and sum the cohesive tractions to the applied block forces
      float fx = (tx[2*i] + tx[2*i+1])*dx;
      float fy = (ty[2*i] + ty[2*i+1])*dx;
      blocks.fx[lower] += fx;
      blocks.fy[lower] += fy;
      blocks.fx[upper] -= fx;
      blocks.fy[upper] -= fy;
      blocks.mz[lower] += (rxm[2*i]*ty[2*i] - rym[2*i]*tx[2*i] + rxm[2*i+1]*ty[2*i+1] - rym[2*i+1]*tx[2*i+1])*halfdx;
      blocks.mz[upper] -= (rxp[2*i]*ty[2*i] - ryp[2*i]*tx[2*i] + rxp[2*i+1]*ty[2*i+1] - ryp[2*i+1]*tx[2*i+1])*halfdx;
    } // for i = ...

  } // applyForces()
  
  std::vector<std::pair<int,int> > xFaceIDs;
  std::vector<std::pair<int,int> > yFaceIDs;
}; // CohesiveZone

class KelvinVoigt : public CohesiveZone {
public:
  
  KelvinVoigt(float newStiffness, float newViscosity) {
    stiffness = newStiffness;
    viscosity = newViscosity;
  } // KelvinVoigt()

  virtual void initialize(void) { } // initialize()

  virtual void computeTraction(float* ux, float* uy, float* vx, float* vy, float* nx, float* ny, float* tx, float* ty, float divdx, Orientation dir, int size) {
    // pre-compute material constants, adjusted by length scale (and possibly initial orientation)
    float Edivdx   = stiffness*divdx;
    float etadivdx = viscosity*divdx;

    // loop over all quadrature points
    for (int i = 0; i < size; i++) {
      tx[i] = Edivdx*ux[i] + etadivdx*vx[i];
      ty[i] = Edivdx*uy[i] + etadivdx*vy[i];
    } // for i = ...
  } // computeTraction()
  
  float stiffness;
  float viscosity;
}; // KelvinVoigt

class BrittleDamage : public KelvinVoigt {
public:
  
  BrittleDamage(float newFailureStress, float newStiffness, float newViscosity) : KelvinVoigt(newStiffness,newViscosity) {
    failureStress = newFailureStress;
  } // BrittleDamage()

  virtual void initialize(void) {
    
  } // initialize()

  virtual void computeTraction(float* ux, float* uy, float* vx, float* vy, float* nx, float* ny, float* tx, float* ty, float divdx, Orientation dir, int size) {
    // pre-compute material constants, adjusted by length scale (and possibly initial orientation)
    float etadivdx = viscosity*divdx;
    float Edivdx   = stiffness*divdx;

    // loop over all quadrature points
    for (int i = 0; i < size; i++) {
      tx[i] = Edivdx*ux[i] + etadivdx*vx[i];
      ty[i] = Edivdx*uy[i] + etadivdx*vy[i];
    } // for i = ...
  } // computeTraction()
  
  float failureStress;
  std::vector<bool> failed;
}; // BrittleDamage

class CohesiveDamage : public KelvinVoigt {
public:
  
  CohesiveDamage(float newFailureStress, float newFractureEnergy, float newStiffness, float newViscosity) : KelvinVoigt(newStiffness,newViscosity) {
    failureStress = newFailureStress;
    fractureEnergy = newFractureEnergy;
    failureStrain = failureStress / newStiffness;

    float maxStrain = 2.0*fractureEnergy/failureStress;
    if (maxStrain > failureStrain) {
      // cohesive failure, stable crack growth
      Esoftening = failureStress/(maxStrain-failureStrain);
    } else {
      // brittle failure, unstable crack growth
      Esoftening = failureStress/(1.0e-16);
    }
  } // BrittleDamage()

  virtual void initialize(void) {
    // allocate history variables for damaged interfaces
    xEdamaged.resize(2*xFaceIDs.size());
    yEdamaged.resize(2*yFaceIDs.size());
    xFailed.resize(2*xFaceIDs.size());
    yFailed.resize(2*yFaceIDs.size());
    std::fill(xEdamaged.begin(), xEdamaged.end(), stiffness);
    std::fill(yEdamaged.begin(), yEdamaged.end(), stiffness);
    std::fill(xFailed.begin(), xFailed.end(), 0);
    std::fill(yFailed.begin(), yFailed.end(), 0);
  } // initialize()

  virtual void computeTraction(float* ux, float* uy, float* vx, float* vy, float* nx, float* ny, float* tx, float* ty, float divdx, Orientation dir, int size) {
    // pre-compute material constants, adjusted by length scale (and possibly initial orientation)
    float divEdx = divdx/stiffness;
    float etadivEdx = divEdx*viscosity;

    // load appropriate history variables
    float* Edamaged;
    int*   failed;
    if (dir == Orientation::X) {
      Edamaged = xEdamaged.data();
      failed = xFailed.data();
    } else { // if (dir == Orientation::Y)
      Edamaged = yEdamaged.data();
      failed = yFailed.data();
    } // check X/Y-face orientation

    // loop over all quadrature points
    for (int i = 0; i < size; i++) {
      if (failed[i] == 0) {
	// transform relative displacement (normalized by element length) into relative coordinate system
	// with respect to the current face normal
	// { un } = [ +nx +ny ] { ux }
	// { ut } = [ -ny +nx ] { uy }
	float un = (+ nx[i]*ux[i] + ny[i]*uy[i])*divdx;
	float ut = (- ny[i]*ux[i] + nx[i]*uy[i])*divdx;
	float vn = (+ nx[i]*vx[i] + ny[i]*vy[i])*divdx;
	float vt = (- ny[i]*vx[i] + nx[i]*vy[i])*divdx;

	// compute the effective displacement
	float un_tensile = fmax(0.0,un);
	float un_compressive = un - un_tensile;
	float u = sqrt(un_tensile*un_tensile+ut*ut);

	// update the current damaged stiffness
	Edamaged[i] = fmax(0.0,fmin(Edamaged[i],(failureStress-Esoftening*(u-failureStrain))/fmax(u,failureStrain)));
	if (Edamaged[i] == 0.0) failed[i] = 1;

	float damaged_viscosity = etadivEdx*Edamaged[i];

	// compute the current traction in the relative coordinate system
	float tn = Edamaged[i]*un_tensile + damaged_viscosity*vn;
	float tt = Edamaged[i]*ut + damaged_viscosity*vt;
	if (un_tensile == 0.0) tn += stiffness*un_compressive + viscosity*vn;
      
	// rotate the traction into the global coordinate system
	// { tx } = [ +nx -ny ] { tn }
	// { ty } = [ +ny +nx ] { tt }
	tx[i] = + nx[i]*tn - ny[i]*tt;
	ty[i] = + ny[i]*tn + nx[i]*tt;
      } else {
	tx[i] = 0.0;
	ty[i] = 0.0;
      } // if (failed[i] == 0)
    } // for i = ...
  } // computeTraction()
  
  float failureStress;
  float fractureEnergy;
  float failureStrain;
  float Esoftening;
  std::vector<float> xEdamaged;
  std::vector<int>   xFailed;
  std::vector<float> yEdamaged;
  std::vector<int>   yFailed;
}; // CohesiveDamage

#endif // COHESIVE_ZONE_H
