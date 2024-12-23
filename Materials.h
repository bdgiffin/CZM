#ifndef MATERIALS_H
#define MATERIALS_H

#include <string>
#include <iostream>

class Material {
public:
  
  Material(std::string newName, float newDensity, int newQuantity, float r, float g, float b, float x0, float y0, float x1, float y1) {
    prev = nullptr;
    next = nullptr;
    name = newName;
    density = newDensity;
    quantity = newQuantity;
    color[0] = r;
    color[1] = g;
    color[2] = b;
    coord[0] = x0;
    coord[1] = y0;
    coord[2] = x1;
    coord[3] = y1;
  } // Material()

  Material*    prev;
  Material*    next;
  std::string  name;
  float        density;
  int          quantity;
  float        color[3]; // text color
  float        coord[4]; // texture coordinates

  static unsigned int textures;
}; // Material

unsigned int Material::textures = 0;

class MaterialInventory {
public:

  MaterialInventory() {
    head = nullptr;
  } // MaterialInventory()

  Material* getFirstMaterial() {
    return head;
  } // getFirstMaterial()

  void insertMaterial(Material* newMaterial) {
    if (!head) {
      head = newMaterial;
      head->prev = newMaterial;
      head->next = newMaterial;
    } else {
      newMaterial->prev = head->prev;
      newMaterial->next = head;
      head->prev->next = newMaterial;
      head->prev = newMaterial;
      head = newMaterial;
    }
  } // insertMaterial()

protected:
  Material* head;
}; // MaterialInventory

#endif // MATERIALS_H
