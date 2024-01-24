#include "glslprogram.h"

struct GLshadertype
{
  char* extension;
  GLenum name;
} ShaderTypes[] = {
  { (char*)".vert", GL_VERTEX_SHADER },
  { (char*)".vs", GL_VERTEX_SHADER },
  { (char*)".frag", GL_FRAGMENT_SHADER },
  { (char*)".fs", GL_FRAGMENT_SHADER },
};

static char*
GetExtension(char* file)
{
  int n = (int)strlen(file) - 1; // index of last non-null character

  // look for a '.':

  do {
    if (file[n] == '.')
      return &file[n]; // the extension includes the '.'
    n--;
  } while (n >= 0);

  // never found a '.':

  return NULL;
}

GLSLProgram::GLSLProgram() {}

// this is what is exposed to the user
// file1 - file5 are defaulted as NULL if not given
// CreateHelper is a varargs procedure, so must end in a NULL argument,
//	which I know to supply but I'm worried users won't

bool
GLSLProgram::Create(char* file0,
                    char* file1,
                    char* file2,
                    char* file3,
                    char* file4,
                    char* file5)
{
  printf("creating file \n");
  return CreateHelper(file0, file1, file2, file3, file4, file5, NULL);
}

// this is the varargs version of the Create method

bool
GLSLProgram::CreateHelper(char* file0, ...)
{
  uniform_buffer_object_exists = false;

  GLsizei n = 0;
  GLchar* buf;
  Valid = true;

  Vshader = Fshader = 0;
  Program = 0;
  AttributeLocs.clear();
  UniformLocs.clear();

  // DBG
  // printf("starting program\n");

  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    // Handle OpenGL error
    // std::cout << "BIG ERROR" << std::endl;
    std::cout << error << std::endl;
  }
  // endDBG

  if (Program == 0) {
    Program = glCreateProgram();
    CheckGlErrors("glCreateProgram");
  }

  // printf("done starting program\n");

  va_list args;
  va_start(args, file0);

  // This is a little dicey
  // There is no way, using var args, to know how many arguments were passed
  // I am depending on the caller passing in a NULL as the final argument.
  // If they don't, bad things will happen.

  char* file = file0;
  int type;
  while (file != NULL) {
    type = -1;
    char* extension = GetExtension(file);
    // fprintf( stderr, "File = '%s', extension = '%s'\n", file, extension );

    int maxShaderTypes = sizeof(ShaderTypes) / sizeof(struct GLshadertype);
    for (int i = 0; i < maxShaderTypes; i++) {
      if (strcmp(extension, ShaderTypes[i].extension) == 0) {
        // fprintf( stderr, "Legal extension = '%s'\n", extension );
        type = i;
        break;
      }
    }

    GLuint shader;
    bool SkipToNextVararg = false;
    if (type < 0) {
      fprintf(stderr, "Unknown filename extension: '%s'\n", extension);
      fprintf(stderr, "Legal Extensions are: ");
      for (int i = 0; i < maxShaderTypes; i++) {
        if (i != 0)
          fprintf(stderr, " , ");
        fprintf(stderr, "%s", ShaderTypes[i].extension);
      }
      fprintf(stderr, "\n");
      Valid = false;
      SkipToNextVararg = true;
    }

    if (!SkipToNextVararg) {
      switch (ShaderTypes[type].name) {
        case GL_VERTEX_SHADER:
          if (!CanDoVertexShaders) {
            fprintf(stderr,
                    "Warning: this system cannot handle vertex shaders\n");
            Valid = false;
            SkipToNextVararg = true;
          } else {
            shader = glCreateShader(GL_VERTEX_SHADER);
          }
          break;

        case GL_FRAGMENT_SHADER:
          if (!CanDoFragmentShaders) {
            fprintf(stderr,
                    "Warning: this system cannot handle fragment shaders\n");
            Valid = false;
            SkipToNextVararg = true;
          } else {
            shader = glCreateShader(GL_FRAGMENT_SHADER);
          }
          break;
      }
    }

    // read the shader source into a buffer:

    if (!SkipToNextVararg) {
      FILE* in;
      int length;
      FILE* logfile;

      in = fopen(file, "rb");
      if (in == NULL) {
        fprintf(stderr, "Cannot open shader file '%s'\n", file);
        Valid = false;
        SkipToNextVararg = true;
      }

      if (!SkipToNextVararg) {
        fseek(in, 0, SEEK_END);
        length = ftell(in);
        fseek(in, 0, SEEK_SET); // rewind

        buf = new GLchar[length + 1];
        fread(buf, sizeof(GLchar), length, in);
        buf[length] = '\0';
        fclose(in);

        GLchar* strings[2];
        int n = 0;
        strings[n] = buf;
        n++;

        // Tell GL about the source:

        glShaderSource(shader, n, (const GLchar**)strings, NULL);
        delete[] buf;
        CheckGlErrors("Shader Source");

        // compile:

        glCompileShader(shader);
        GLint infoLogLen;
        GLint compileStatus;
        CheckGlErrors("CompileShader:");
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

        if (compileStatus == 0) {
          fprintf(stderr, "Shader '%s' did not compile.\n", file);
          glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
          if (infoLogLen > 0) {
            GLchar* infoLog = new GLchar[infoLogLen + 1];
            glGetShaderInfoLog(shader, infoLogLen, NULL, infoLog);
            infoLog[infoLogLen] = '\0';
            logfile = fopen("glsllog.txt", "w");
            if (logfile != NULL) {
              fprintf(logfile, "\n%s\n", infoLog);
              fclose(logfile);
            }
            fprintf(stderr, "\n%s\n", infoLog);
            delete[] infoLog;
          }
          glDeleteShader(shader);
          Valid = false;
        } else {
          if (Verbose)
            fprintf(stderr, "Shader '%s' compiled.\n", file);

          glAttachShader(this->Program, shader);
        }
      }
    }

    // go to the next vararg file:

    file = va_arg(args, char*);
  }

  va_end(args);

  // link the entire shader program:

  glLinkProgram(Program);
  CheckGlErrors("Link Shader 1");

  GLchar* infoLog;
  GLint infoLogLen;
  GLint linkStatus;
  glGetProgramiv(this->Program, GL_LINK_STATUS, &linkStatus);
  CheckGlErrors("Link Shader 2");

  if (linkStatus == 0) {
    glGetProgramiv(this->Program, GL_INFO_LOG_LENGTH, &infoLogLen);
    fprintf(
      stderr, "Failed to link program -- Info Log Length = %d\n", infoLogLen);
    if (infoLogLen > 0) {
      infoLog = new GLchar[infoLogLen + 1];
      glGetProgramInfoLog(this->Program, infoLogLen, NULL, infoLog);
      infoLog[infoLogLen] = '\0';
      fprintf(stderr, "Info Log:\n%s\n", infoLog);
      delete[] infoLog;
    }
    glDeleteProgram(Program);
    Valid = false;
  } else {
    if (Verbose)
      fprintf(stderr, "Shader Program linked.\n");
    // validate the program:

    GLint status;
    glValidateProgram(Program);
    glGetProgramiv(Program, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE) {
      fprintf(stderr, "Program is invalid.\n");
      Valid = false;
    } else {
      if (Verbose)
        fprintf(stderr, "Shader Program validated.\n");
    }
  }

  return Valid;
}

void
GLSLProgram::DisableVertexAttribArray(const char* name)
{
  int loc;
  if ((loc = GetAttributeLocation((char*)name)) >= 0) {
    this->Use();
    glDisableVertexAttribArray(loc);
  }
}

void
GLSLProgram::EnableVertexAttribArray(const char* name)
{
  int loc;
  if ((loc = GetAttributeLocation((char*)name)) >= 0) {
    this->Use();
    glEnableVertexAttribArray(loc);
  }
}

// always has bind group 0
void GLSLProgram::EnsureUniformBufferObjectInitialized()
{
  if (!uniform_buffer_object_exists) {
    // create empty object buffer
    glGenBuffers(1, &uniform_buffer_object);
    glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer_object);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Pika), NULL, GL_STREAM_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // bind empty object buffer to binding point 0
    GLuint binding_point = 0;
    glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, uniform_buffer_object);

    // create empty object buffer
    glGenBuffers(1, &uniform_buffer_object_2);
    glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer_object_2);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PikaColors), NULL, GL_STREAM_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // bind empty object buffer to binding point 0
    binding_point = 1;
    glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, uniform_buffer_object_2);

    uniform_buffer_object_exists = true;
  }
}

void
GLSLProgram::SetUniformBufferObject(Pika pika)
{
  // stream eyes data
  EnsureUniformBufferObjectInitialized();

  glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer_object);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Pika), &pika);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  // GLuint eyes_block_index = glGetUniformBlockIndex(Program, "EyesBlock");
  // std::cout << "eyes block index is " << eyes_block_index << std::endl;

  // create loc
  //   GLuint ubo;
  //   glGenBuffers(1, &ubo);
  //   // how do we bind multiple uniform buffers to things?
  //   glBindBuffer(GL_UNIFORM_BUFFER, ubo);
  //   glBufferData(GL_UNIFORM_BUFFER, sizeof(Eyes), &eyes, GL_STREAM_DRAW);

  //   GLuint block_index = glGetUniformBlockIndex(Program, "EyesBlock");
  //   glUniformBlockBinding(Program, block_index, block_binding);
}

void
GLSLProgram::SetUniformBufferObject(PikaColors colors)
{
  // stream eyes data
  EnsureUniformBufferObjectInitialized();

  glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer_object_2);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PikaColors), &colors);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  // GLuint eyes_block_index = glGetUniformBlockIndex(Program, "EyesBlock");
  // std::cout << "eyes block index is " << eyes_block_index << std::endl;

  // create loc
  //   GLuint ubo;
  //   glGenBuffers(1, &ubo);
  //   // how do we bind multiple uniform buffers to things?
  //   glBindBuffer(GL_UNIFORM_BUFFER, ubo);
  //   glBufferData(GL_UNIFORM_BUFFER, sizeof(Eyes), &eyes, GL_STREAM_DRAW);

  //   GLuint block_index = glGetUniformBlockIndex(Program, "EyesBlock");
  //   glUniformBlockBinding(Program, block_index, block_binding);
}


void
GLSLProgram::Init()
{
  Verbose = false;

  CanDoVertexShaders = IsExtensionSupported("GL_ARB_vertex_shader");
  CanDoFragmentShaders = IsExtensionSupported("GL_ARB_fragment_shader");

  fprintf(stderr, "Can do: ");
  if (CanDoVertexShaders)
    fprintf(stderr, "vertex shaders, ");
  if (CanDoFragmentShaders)
    fprintf(stderr, "fragment shaders, ");
  fprintf(stderr, "\n");
}

bool
GLSLProgram::IsValid()
{
  return Valid;
}

bool
GLSLProgram::IsNotValid()
{
  return !Valid;
}

void
GLSLProgram::SetVerbose(bool v)
{
  Verbose = v;
}

void
GLSLProgram::UnUse()
{
  Use(0);
}

void
GLSLProgram::Use()
{
  Use(this->Program);
};

void
GLSLProgram::Use(GLuint p)
{
  if (p != CurrentProgram) {
    glUseProgram(p);
    CurrentProgram = p;
  }
};

void
GLSLProgram::UseFixedFunction()
{
  this->Use(0);
};

int
GLSLProgram::GetAttributeLocation(char* name)
{
  std::map<char*, int>::iterator pos;

  pos = AttributeLocs.find(name);
  if (pos == AttributeLocs.end()) {
    AttributeLocs[name] = glGetAttribLocation(this->Program, name);
  }

  return AttributeLocs[name];
};

void
GLSLProgram::SetAttributePointer3fv(char* name, float* vals)
{
  int loc;
  if ((loc = GetAttributeLocation(name)) >= 0) {
    this->Use();
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, vals);
  }
};

#ifdef NOT_SUPPORTED_BY_OPENGL
void
GLSLProgram::SetAttributeVariable(char* name, int val)
{
  int loc;
  if ((loc = GetAttributeLocation(name)) >= 0) {
    this->Use();
    glVertexAttrib1i(loc, val);
  }
};
#endif

void
GLSLProgram::SetAttributeVariable(char* name, float val)
{
  int loc;
  if ((loc = GetAttributeLocation(name)) >= 0) {
    this->Use();
    glVertexAttrib1f(loc, val);
  }
};

void
GLSLProgram::SetAttributeVariable(char* name,
                                  float val0,
                                  float val1,
                                  float val2)
{
  int loc;
  if ((loc = GetAttributeLocation(name)) >= 0) {
    this->Use();
    glVertexAttrib3f(loc, val0, val1, val2);
  }
};

void
GLSLProgram::SetAttributeVariable(char* name, float vals[3])
{
  int loc;
  if ((loc = GetAttributeLocation(name)) >= 0) {
    this->Use();
    glVertexAttrib3fv(loc, vals);
  }
};

// This function caches the location of the uniform
int
GLSLProgram::GetUniformLocation(char* name)
{
  std::map<char*, int>::iterator pos;

  pos = UniformLocs.find(name);
  // if( Verbose )
  // fprintf( stderr, "Uniform: pos = 0x%016x ; size = %d ; end = 0x%016x\n",
  // pos, UniformLocs.size(), UniformLocs.end() );
  if (pos == UniformLocs.end()) {
    GLuint loc = glGetUniformLocation(this->Program, name);
    UniformLocs[name] = loc;
    if (Verbose)
      fprintf(stderr,
              "Location of '%s' in Program %d = %d\n",
              name,
              this->Program,
              loc);
  } else {
    if (Verbose) {
      fprintf(stderr, "Location = %d\n", UniformLocs[name]);
      if (UniformLocs[name] == -1)
        fprintf(stderr, "Location of uniform variable '%s' is -1\n", name);
    }
  }

  return UniformLocs[name];
};

// This function uses the internally cached location of a uniform varible to
// modify it
void
GLSLProgram::SetUniformVariable(char* name, int val)
{
  int loc;
  if ((loc = GetUniformLocation(name)) >= 0) {
    this->Use();
    glUniform1i(loc, val);
  }
};

void
GLSLProgram::SetUniformVariable(char* name, float val)
{
  int loc;
  if ((loc = GetUniformLocation(name)) >= 0) {
    this->Use();
    glUniform1f(loc, val);
  }
};

void
GLSLProgram::SetUniformVariable(char* name, float val0, float val1, float val2)
{
  int loc;
  if ((loc = GetUniformLocation(name)) >= 0) {
    this->Use();
    glUniform3f(loc, val0, val1, val2);
  }
};

void
GLSLProgram::SetUniformVariable(char* name, float vals[3])
{
  int loc;
  fprintf(stderr, "Found a 3-element array\n");

  if ((loc = GetUniformLocation(name)) >= 0) {
    this->Use();
    glUniform3fv(loc, 1, vals);
  }
};

bool
GLSLProgram::IsExtensionSupported(const char* extension)
{
  // see if the extension is bogus:

  if (extension == NULL || extension[0] == '\0')
    return false;

  GLubyte* where = (GLubyte*)strchr(extension, ' ');
  if (where != 0)
    return false;

  // get the full list of extensions:

  const GLubyte* extensions = glGetString(GL_EXTENSIONS);

  for (const GLubyte* start = extensions;;) {
    where = (GLubyte*)strstr((const char*)start, extension);
    if (where == 0)
      return false;

    GLubyte* terminator = where + strlen(extension);

    if (where == start || *(where - 1) == '\n' || *(where - 1) == ' ')
      if (*terminator == ' ' || *terminator == '\n' || *terminator == '\0')
        return true;
    start = terminator;
  }
  return false;
}

int GLSLProgram::CurrentProgram = 0;

#ifndef CHECK_GL_ERRORS
#define CHECK_GL_ERRORS
void
CheckGlErrors(const char* caller)
{
  unsigned int gle = glGetError();

  if (gle != GL_NO_ERROR) {
    fprintf(stderr, "GL Error discovered from caller %s: ", caller);
    switch (gle) {
      case GL_INVALID_ENUM:
        fprintf(stderr, "Invalid enum.\n");
        break;
      case GL_INVALID_VALUE:
        fprintf(stderr, "Invalid value.\n");
        break;
      case GL_INVALID_OPERATION:
        fprintf(stderr, "Invalid Operation.\n");
        break;
      case GL_STACK_OVERFLOW:
        fprintf(stderr, "Stack overflow.\n");
        break;
      case GL_STACK_UNDERFLOW:
        fprintf(stderr, "Stack underflow.\n");
        break;
      case GL_OUT_OF_MEMORY:
        fprintf(stderr, "Out of memory.\n");
        break;
    }
    return;
  }
}
#endif
