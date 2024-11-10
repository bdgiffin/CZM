#ifndef GROUND_MOTION_H
#define GROUND_MOTION_H

#include <vector>
#include <cmath>

class GroundMotion {
public:

  void evaluate(float time, float& uxt, float& uyt) {
    float frac = time / dt;
    int n = floor(frac);
    if ((n+1) < ux.size()) {
      frac -= n;
      uxt = scale*(frac*ux[n+1] + (1.0-frac)*ux[n]);
      uyt = 0.0*(frac*uy[n+1] + (1.0-frac)*uy[n]);
    } else {
      uxt = 0.0;
      uyt = 0.0;
    }
  } // evaluate

  std::vector<float> ux;
  std::vector<float> uy;
  float dt;
  float scale;
}; // GroundMotion

#endif // GROUND_MOTION_H
