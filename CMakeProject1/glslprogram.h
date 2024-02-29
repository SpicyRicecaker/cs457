#pragma once

// if on a Mac, don't need glew.h and the <GL/ should be <OpenGL/
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <iostream>
#include <ctype.h>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#endif

#include <map>
#include <stdarg.h>

#include "pika.h"

inline int
GetOSU(int flag)
{
  int i;
  glGetIntegerv(flag, &i);
  return i;
}

void
CheckGlErrors(const char*);

class GLSLProgram
{
private:
  std::map<char*, int> AttributeLocs;
  char* Ffile;
  unsigned int Fshader;
  bool IncludeGstap;
  GLuint Program;
  std::map<char*, int> UniformLocs;
  bool Valid;
  char* Vfile;
  GLuint Vshader;
  bool Verbose;

  static int CurrentProgram;

  bool uniform_buffer_object_exists;
  GLuint uniform_buffer_object;
  GLuint uniform_buffer_object_2;

  void AttachShader(GLuint);
  bool CanDoFragmentShaders;
  bool CanDoVertexShaders;
  int CompileShader(GLuint);
  bool CreateHelper(char*, ...);
  int GetAttributeLocation(char*);
  int GetUniformLocation(char*);

public:
  GLSLProgram();

  bool Create(char*,
              char* = NULL,
              char* = NULL,
              char* = NULL,
              char* = NULL,
              char* = NULL);
  void DisableVertexAttribArray(const char*);
  void EnableVertexAttribArray(const char*);
  void EnsureUniformBufferObjectInitialized();
  void SetUniformBufferObject(Pika);
  void SetUniformBufferObject(PikaColors);
  void Init();
  bool IsExtensionSupported(const char*);
  bool IsNotValid();
  bool IsValid();
  void SetAttributePointer3fv(char*, float*);
  void SetAttributeVariable(char*, int);
  void SetAttributeVariable(char*, float);
  void SetAttributeVariable(char*, float, float, float);
  void SetAttributeVariable(char*, float[3]);
  void VertexAttrib3f(const char*, float, float, float);
  void SetUniformVariable(const char*, int);
  void SetUniformVariable(const char*, float);
  void SetUniformVariable(const char*, float, float, float);
  void SetUniformVariable(const char*, float[3]);
  void SetVerbose(bool);
  void UnUse();
  void Use();
  void Use(GLuint);
  void UseFixedFunction();
  void CreateUniformBufferObject();
};