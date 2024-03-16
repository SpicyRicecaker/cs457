#pragma once

// code inspired by https://learnopengl.com/Getting-started/Shaders

#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <string>
#include <format>

using namespace std;

class Shader {
public:
  // the program ID
  unsigned ID; 

  Shader(const char* vertexPath, const char* fragmentPath) {
    const string debugPathPrefix = "../../../../CMakeProject1";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    string vertexShaderSource = readFile(format("{}/shaders/{}", debugPathPrefix, vertexPath).c_str());
    const GLchar* vertexShaderSourceRef = vertexShaderSource.c_str();
    glShaderSource(vertexShader, 1, &vertexShaderSourceRef, NULL);
    glCompileShader(vertexShader);
    checkCompileErrors(vertexShader, "VERTEX");

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    string fragmentShaderSource = readFile(format("{}/shaders/{}", debugPathPrefix, fragmentPath).c_str());
    const GLchar* fragmentShaderSourceRef = fragmentShaderSource.c_str();
    glShaderSource(fragmentShader, 1, &fragmentShaderSourceRef, NULL);
    glCompileShader(fragmentShader);
    checkCompileErrors(fragmentShader, "FRAGMENT");

    ID = glCreateProgram();

    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
  }

  void use() {
    glUseProgram(ID);
  }

  void setBool(const std::string& name, bool value) const
  {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
  }
  void setInt(const std::string& name, int value) const
  {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
  }
  void setFloat(const std::string& name, float value) const
  {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
  }
  // WIP 
  // the most scuffed readfile function I have ever seen in my life
  static string readFile(const char* path) {
    ifstream file(path);

    if (!file.is_open()) {
      cerr << "Could not open the file - '" << path << "'" << endl;
    }
    
    string output;
    string s;
    while (getline(file, s)) {
      // Process or print the line
      for (auto c : s) {
        output.push_back(c);
      }
      output.push_back('\n');
    }
    // cout << "running read file" << endl;
    // cout << output << endl;
    // cout << "done running read file" << endl;

    file.close();
    return output;
  }
private:
  // utility function for checking shader compilation/linking errors.
  // ------------------------------------------------------------------------
  void checkCompileErrors(unsigned int shader, std::string type)
  {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
      if (!success)
      {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
      }
    }
    else
    {
      glGetProgramiv(shader, GL_LINK_STATUS, &success);
      if (!success)
      {
        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
      }
    }
  }
};
