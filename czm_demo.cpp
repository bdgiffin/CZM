#if __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#if __EMSCRIPTEN__
#include <GLES2/gl2.h>
#endif

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

//#define STB_VORBIS_HEADER_ONLY
//#include <stb/stb_vorbis.c> // Depends on stb_vorbis

#define STB_IMAGE_IMPLEMENTATION 
#include "stb/stb_image.h"
#include "stb/stb_easy_font.h"
#include "CZM.h"

// Sound libraries
#include "AudioFile/AudioFile.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

using namespace std;

CZM czm;

// sound fonts
ALuint building_music,shaking_music,pop,beep,boop;

// rendering projection parameters
const static int NX_CELLS = 32;
const static int NY_CELLS = 20;
const static int CELL_SIZE = 32;
const static int WINDOW_WIDTH  = NX_CELLS*CELL_SIZE; //1600;
const static int WINDOW_HEIGHT = NY_CELLS*CELL_SIZE; //1000;
const static double VIEW_SCALE = 1.0;
const static double VIEW_WIDTH = VIEW_SCALE*WINDOW_WIDTH;
const static double VIEW_HEIGHT = VIEW_SCALE*WINDOW_HEIGHT;

const static float DT = 0.5f; // integration timestep

GLuint LoadTexture(const char * filename) {
  // Create and bind new texture
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  // FOR WEBGL COMPATIBILITY: MAKE SURE TEXTURES ARE SIZED TO HAVE PIXEL DIMENSIONS AS A POWER OF 2!
  // Load texture using stb_image
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(1);
  unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 4);
  if (data) {
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Generate texture
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Free image data
    stbi_image_free(data);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }

  // unbind texture
  glBindTexture(GL_TEXTURE_2D, 0);

  return texture;
} // LoadTexture()

void InitAL(void) {
   ALCdevice *device;
   ALCcontext *context;
    
   // Open audio device and create context
   device = alcOpenDevice(NULL);
   if (!device) {
     fprintf(stderr, "Failed to open audio device\n");
   }
   context = alcCreateContext(device, NULL);
   if (!context) {
     fprintf(stderr, "Failed to create audio context\n");
   }
   alcMakeContextCurrent(context);

   // Clean up
   //alcMakeContextCurrent(NULL);
   //alcDestroyContext(context);
   //alcCloseDevice(device);

} // InitAL()

ALuint loadAudioFile(const char * filename, int looping) {
  AudioFile<ALshort> audioFile;
  audioFile.load(filename);
  
  ALint sampleRate = audioFile.getSampleRate();
  //int bitDepth = audioFile.getBitDepth();

  ALint numSamples = audioFile.getNumSamplesPerChannel();
  //double lengthInSeconds = audioFile.getLengthInSeconds();

  ALint numChannels = audioFile.getNumChannels();

  // or, just use this quick shortcut to print a summary to the console
  audioFile.printSummary();

  //bufferSize = stb_vorbis_decode_filename("sound.ogg", &channels, &sampleFrequency, &pcmData);

  // Generate a buffer and a source
  ALuint buffer, source;
  alGenBuffers(1, &buffer);
  alGenSources(1, &source);
  
  // Load data into buffer
  //numSamples = numSamples - numSamples%4;
  if (audioFile.isMono()) {
    ALint channel = 0;
    std::cout << "mono" << std::endl;
    alBufferData(buffer, AL_FORMAT_MONO16, &audioFile.samples[channel][0], 2*numSamples, sampleRate);
  } else if (audioFile.isStereo()) {
    ALint channel = 0;
    std::cout << "stereo" << std::endl;
    alBufferData(buffer, AL_FORMAT_STEREO16, &audioFile.samples[channel][0], numSamples, sampleRate);
  }

  // Configure the source
  alSourcei(source, AL_BUFFER, buffer);
  //alSourcef(source, AL_PITCH, 1.0f);
  //alSourcef(source, AL_GAIN, 1.0f);
  alSourcei(source, AL_LOOPING, looping);
  
  return source;

} // loadAudioFile()

ALuint loadAudio(float frequency) {
   ALuint buffer, source;
   ALint num = 1000;
   ALshort data[num]; // Sample audio data
   ALsizei size = sizeof(data);
   ALint channels = 1; // Mono
   ALint sampleRate = 44100;

   // Generate a buffer and a source
   alGenBuffers(1, &buffer);
   alGenSources(1, &source);

   // Fill sample data (replace with actual audio loading logic)
   for (int i = 0; i < num; ++i) {
     data[i] = (ALshort)(sin(2.0f * M_PI * frequency * i / sampleRate) * 32767);
   }

   // Load data into buffer
   alBufferData(buffer, AL_FORMAT_MONO16, data, size, sampleRate);

   // Configure the source
   alSourcei(source, AL_BUFFER, buffer);
   alSourcef(source, AL_PITCH, 1.0f);
   alSourcef(source, AL_GAIN, 1.0f);
   //alSourcei(source, AL_LOOPING, 1); // loop audio

   // Play the sound
   //alSourcePlay(source);
   return source;

   // Clean up
   //alDeleteSources(1, &source);
   //alDeleteBuffers(1, &buffer);

} // initializeAudio()

void InitCZM(void) {

  // create blank grid
  czm.initialize(NX_CELLS, NY_CELLS, VIEW_WIDTH, VIEW_HEIGHT);

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

void drawText(float x, float y, char* text, float* color) {
  static char buffer[99999]; // ~500 chars
  int num_quads;

  unsigned char rgba[4] = { (unsigned char)(255*color[0]), (unsigned char)(255*color[1]), (unsigned char)(255*color[2]), 255 };
  num_quads = stb_easy_font_print(0.0f, 0.0f, text, rgba, buffer, sizeof(buffer));

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(2, GL_FLOAT, 16, buffer);
  glEnableClientState(GL_COLOR_ARRAY);
  glColorPointer(3, GL_UNSIGNED_BYTE, 16, buffer+12);
  glTranslatef(x,y,0.0f);
  glScalef(2.0f,-2.0f,1.0f);
  glDrawArrays(GL_QUADS, 0, num_quads*4);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
} // drawText()

void Render(void) {
  glClear(GL_COLOR_BUFFER_BIT);
	
  glLoadIdentity();
  glOrtho(0, VIEW_WIDTH, 0, VIEW_HEIGHT, 0, 1);

  czm.render();

  char buffer[16];
  sprintf (buffer, "%d", czm.grid.brushColor->quantity);
  drawText(6, WINDOW_HEIGHT-10, buffer, czm.grid.brushColor->color);

  glutSwapBuffers();
} // Render()

void Keyboard(unsigned char c, __attribute__((unused)) int x, __attribute__((unused)) int y) {   
  switch(c) {
  case 'r': 
  case 'R':  
    alSourcePlay(pop);
    czm.reset();
    alSourcef(shaking_music,  AL_GAIN, 0.0f);
    break;
  case 'e': 
  case 'E':  
    alSourcePlay(beep);
    czm.edit();
    alSourcef(shaking_music,  AL_GAIN, 0.0f);
    break;
  case 's':
  case 'S':
    alSourcePlay(boop);
    czm.simulation();
    alSourcef(shaking_music,  AL_GAIN, 1.0f);
    break;
  }
} // Keyboard()

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

void GridHighlight(int x, int y) {
  czm.grid.update_cursor(x, y);
} // GridHighlight()

int main(int argc, char** argv) {
  glutInitWindowSize(WINDOW_WIDTH,WINDOW_HEIGHT);
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutCreateWindow("CZM Library Demo");
  glutDisplayFunc(Render);
  glutIdleFunc(Update);
  glutPassiveMotionFunc(GridHighlight);
  glutMouseFunc(Mouse);
  glutKeyboardFunc(Keyboard);
  glutSpecialFunc(Arrows);

  InitGL();
  InitAL();
  InitCZM();

  // create sounds
  beep = loadAudio(220.0f);
  boop = loadAudio(440.0f);
  pop = loadAudioFile("sounds/pop.wav",0);

  // create music
  building_music = loadAudioFile("sounds/czm_building.wav",1);
  shaking_music = loadAudioFile("sounds/czm_shaking.wav",1);
  alSourcePlay(building_music);
  alSourcePlay(shaking_music);
  alSourcef(shaking_music, AL_GAIN, 0.0f);

  // populate material inventory
  Material::textures = LoadTexture("textures/textures.png");
  czm.inventory.insertMaterial(new Material("Rock",      3500.0,      16, 0.38, 0.38, 0.38, 0.0, 0.0, 0.2, 0.2));
  czm.inventory.insertMaterial(new Material("Soil",      1500.0,      64, 0.25, 0.50, 0.25, 0.2, 0.0, 0.4, 0.2));
  czm.inventory.insertMaterial(new Material("Concrete",  2400.0,      32, 0.75, 0.75, 0.75, 0.4, 0.0, 0.6, 0.2));
  czm.inventory.insertMaterial(new Material("Wood",       600.0,      32, 0.80, 0.45, 0.10, 0.6, 0.0, 0.8, 0.2));
  czm.inventory.insertMaterial(new Material("Steel",     8050.0,      16, 0.50, 0.63, 0.70, 0.8, 0.0, 1.0, 0.2));
  czm.inventory.insertMaterial(new Material("Brick",     2000.0,      32, 0.75, 0.10, 0.00, 0.0, 0.2, 0.2, 0.4));
  //czm.inventory.insertMaterial(new Material("Water",     1000.0,      32, 0.29, 0.22, 1.00, 0.4, 0.2, 0.6, 0.4));
  //czm.inventory.insertMaterial(new Material("Player",    1000.0,       1, 1.00, 0.00, 0.50, 0.6, 0.2, 0.8, 0.4));

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
