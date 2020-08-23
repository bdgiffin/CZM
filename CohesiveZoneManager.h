#ifndef COHESIVE_ZONE_MANAGER_H
#define COHESIVE_ZONE_MANAGER_H

#include "Materials.h"
#include "Grid.h"
#include "Blocks.h"
#include "CohesiveZone.h"
#include <vector>
#include <map>

class CohesiveZoneManager {
public:

  void initialize(Grid& grid) {

    // delete existing cohesive zones
    for (auto cohesiveZone : cohesiveZones) delete cohesiveZone.second;
    cohesiveZones.clear();

    // loop over all x-faces
    for (int j = 0; j < grid.Ny; j++) {
      for (int i = 1; i < grid.Nx; i++) {
	int left  = grid.blockIDs[grid.Nx*j+i-1];
	int right = grid.blockIDs[grid.Nx*j+i];
	if ((left >= 0) && (right >= 0)) {
	  std::pair<Material*,Material*> key;
	  if (grid.cells[grid.Nx*j+i-1] < grid.cells[grid.Nx*j+i]) {
	    key = std::pair<Material*,Material*>(grid.cells[grid.Nx*j+i-1], grid.cells[grid.Nx*j+i]);
	  } else {
	    key = std::pair<Material*,Material*>(grid.cells[grid.Nx*j+i], grid.cells[grid.Nx*j+i-1]);
	  }
	  if (cohesiveZones[key] == NULL) cohesiveZones[key] = instantiateCohesiveZoneModel(key.first, key.second);
	  cohesiveZones[key]->insertFaceX(left,right);
	} // if ((left >= 0) && (right >= 0))
      } // for i = ...
    } // for j = ...

    // loop over all y-faces
    for (int i = 0; i < grid.Nx; i++) {
      for (int j = 1; j < grid.Ny; j++) {
	int lower = grid.blockIDs[grid.Nx*(j-1)+i];
	int upper = grid.blockIDs[grid.Nx*j+i];
	if ((lower >= 0) && (upper >= 0)) {
	  std::pair<Material*,Material*> key;
	  if (grid.cells[grid.Nx*(j-1)+i] < grid.cells[grid.Nx*j+i]) {
	    key = std::pair<Material*,Material*>(grid.cells[grid.Nx*(j-1)+i], grid.cells[grid.Nx*j+i]);
	  } else {
	    key = std::pair<Material*,Material*>(grid.cells[grid.Nx*j+i], grid.cells[grid.Nx*(j-1)+i]);
	  }
	  if (cohesiveZones[key] == NULL) cohesiveZones[key] = instantiateCohesiveZoneModel(key.first, key.second);
	  cohesiveZones[key]->insertFaceY(lower,upper);
	} // if ((lower >= 0) && (upper >= 0))
      } // for j = ...
    } // for i = ...

    // initialize newly generated cohesive zones
    for (auto cohesiveZone : cohesiveZones) cohesiveZone.second->initialize();

  } // initialize()

  void applyCohesiveForces(Blocks& blocks) {
    // compute forces for each instantiated CZ type
    for (auto cohesiveZone : cohesiveZones) cohesiveZone.second->applyForces(blocks);
  } // applyCohesiveForces()

  CohesiveZone* instantiateCohesiveZoneModel(Material* firstMaterial, Material* secondMaterial) {
    float defaultStiffness = 200.0e+4;
    float defaultViscosity = 100.0e+3;
    float defaultFailureStrain = 5.0e-2;
    float defaultMaxStrain = 5.0e-1;
    float defaultFailureStress = defaultStiffness*defaultFailureStrain;
    float defaulFractureEnergy = 0.5*defaultFailureStress*defaultMaxStrain;
    return new CohesiveDamage(defaultFailureStress,defaulFractureEnergy,defaultStiffness,defaultViscosity);
    //return new KelvinVoigt(defaultStiffness,defaultViscosity);
    /*
    if        (firstMaterial->name == "Rock") {
      if        (secondMaterial->name == "Rock") {
	// Rock-Rock interface:
	return new BrittleDamage(stiffness,failureStress);
      } else if (secondMaterial->name == "Soil") {
	// Rock-Soil interface:
	return new CohesiveDamage(stiffness,failureStress,cohesiveEnergy);
      } else if (secondMaterial->name == "Concrete") {
	// Rock-Concrete interface:
	return new BrittleDamage(stiffness,failureStress);
      } else if (secondMaterial->name == "Wood") {
	// Rock-Wood interface:
	return new BrittleDamage(stiffness,failureStress);
      } else if (secondMaterial->name == "Steel") {
	// Rock-Steel interface:
	return new CohesiveDamage(stiffness,failureStress,cohesiveEnergy);
      } else if (secondMaterial->name == "Water") {
	// Rock-Water interface:
	return new ViscousFlow(viscosity);
      } else {
	std::cerr << "ERROR: Undefined material" << std::endl;
      } // if (secondMaterial->name == "...")
    } else if (firstMaterial->name == "Soil") {
      if        (secondMaterial->name == "Soil") {
	// Soil-Soil interface:
	return new CohesiveDamage(stiffness,failureStress,cohesiveEnergy);
      } else if (secondMaterial->name == "Concrete") {
	// Soil-Concrete interface:
	return new CohesiveDamage(stiffness,failureStress,cohesiveEnergy);
      } else if (secondMaterial->name == "Wood") {
	// Soil-Wood interface:
	return new CohesiveDamage(stiffness,failureStress,cohesiveEnergy);
      } else if (secondMaterial->name == "Steel") {
	// Soil-Steel interface:
	return new CohesiveDamage(stiffness,failureStress,cohesiveEnergy);
      } else if (secondMaterial->name == "Water") {
	// Soil-Water interface:
	return new ViscousFlow(viscosity);
      } else {
	return instantiateCohesiveZoneModel(secondMaterial,firstMaterial);
      } // if (secondMaterial->name == "...")
    } else if (firstMaterial->name == "Concrete") {
      if        (secondMaterial->name == "Concrete") {
	// Concrete-Concrete interface:
	return new CohesiveDamage(stiffness,failureStress,cohesiveEnergy);
      } else if (secondMaterial->name == "Wood") {
	// Concrete-Wood interface:
	return new CohesiveDamage(stiffness,failureStress,cohesiveEnergy);
      } else if (secondMaterial->name == "Steel") {
	// Concrete-Steel interface:
	return new CohesiveDamage(stiffness,failureStress,cohesiveEnergy);
      } else if (secondMaterial->name == "Water") {
	// Concrete-Water interface:
	return new ViscousFlow(viscosity);
      } else {
	return instantiateCohesiveZoneModel(secondMaterial,firstMaterial);
      } // if (secondMaterial->name == "...")
    } else if (firstMaterial->name == "Wood") {
      if        (secondMaterial->name == "Wood") {
	// Wood-Wood interface:
	return new CohesiveDamage(stiffness,failureStress,cohesiveEnergy);
      } else if (secondMaterial->name == "Steel") {
	// Wood-Steel interface:
	return new CohesiveDamage(stiffness,failureStress,cohesiveEnergy);
      } else if (secondMaterial->name == "Water") {
	// Wood-Water interface:
	return new ViscousFlow(viscosity);
      } else {
	return instantiateCohesiveZoneModel(secondMaterial,firstMaterial);
      } // if (secondMaterial->name == "...")
    } else if (firstMaterial->name == "Steel") {
      if        (secondMaterial->name == "Steel") {
	// Steel-Steel interface:
	return new Plastic(stiffness,failureStress,cohesiveEnergy);
      } else if (secondMaterial->name == "Water") {
	// Steel-Water interface:
	return new ViscousFlow(viscosity);
      } else {
	return instantiateCohesiveZoneModel(secondMaterial,firstMaterial);
      } // if (secondMaterial->name == "...")
    } else if (firstMaterial->name == "Water") {
      if        (secondMaterial->name == "Water") {
	// Water-Water interface:
	return new ViscousFlow(viscosity);
      } else {
	return instantiateCohesiveZoneModel(secondMaterial,firstMaterial);
      } // if (secondMaterial->name == "...")
    } // if (firstMaterial->name == "...")
    */
  } // instantiateCohesiveZoneModel()

  std::map<std::pair<Material*,Material*>,CohesiveZone*> cohesiveZones;
}; // CohesiveZoneManager

#endif // COHESIVE_ZONE_MANAGER_H
