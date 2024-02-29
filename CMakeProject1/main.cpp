#include <iostream>
#include <fstream>
#include <string>
#include <format>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

// WIP 
// the most scuffed readfile function I have ever seen in my life
string readFile(const char* path) {
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
  cout << "running read file" << endl;
  cout << output << endl;
  cout << "done running read file" << endl;

  file.close();
  return output;
}

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "Andy Li Graphics", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  glViewport(0, 0, 800, 600);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  const string debugPathPrefix = "../../../../CMakeProject1";

  float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
  };

  // vertex buffer
  unsigned int VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  // vertex attributes (vertex buffer description)
  // specify vertex attributes?
  // copy actual data over
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  string vertexShaderSource = readFile(format("{}/shaders/pattern.vert", debugPathPrefix).c_str());
  const GLchar* vertexShaderSourceRef = vertexShaderSource.c_str();
  glShaderSource(vertexShader, 1, &vertexShaderSourceRef, NULL);
  glCompileShader(vertexShader);

  {
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
      glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
  }

  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  string fragmentShaderSource = readFile(format("{}/shaders/pattern.frag", debugPathPrefix).c_str());
  const GLchar* fragmentShaderSourceRef = fragmentShaderSource.c_str();
  glShaderSource(fragmentShader, 1, &fragmentShaderSourceRef, NULL);
  glCompileShader(fragmentShader);

  {
    int  success;
    char infoLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
      glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
  }

  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();

  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  while (!glfwWindowShouldClose(window))
  {
    processInput(window);

    {
      glClearColor(0.6f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      // render pass
      glUseProgram(shaderProgram);
      glBindVertexArray(VAO);
      glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwTerminate();

  return 0;
}