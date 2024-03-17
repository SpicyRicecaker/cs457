#include <iostream>
#include <fstream>
#include <string>
#include <format>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "shader.h"
#include "Sphere.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}


int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "Andy Li Graphics", NULL, NULL);
  double aspect_ratio = 800. / 600.;

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
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
  ImGui_ImplOpenGL3_Init();

  // initialize glfw
  glViewport(0, 0, 800, 600);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  Shader shaderProgram("pattern.vert", "pattern.frag");

  // float vertices[] = {
  //   -0.5f, -0.5f, 0.0f, /* position */ 1., 0., 0. /* color */,
  //    0.5f, -0.5f, 0.0f, 0., 1., 0., 
  //    0.0f,  0.5f, 0.0f, 0., 0., 1.
  // };

  // create sphere of radius 1, sector count of 36, stack count of 18, smooth shaded, with up axis of y (2)
  Sphere sphere(1.0f, 36, 18, true, 2);

  // create a new description for vertex buffer, along with the vertex buffer itself, along with an index buffer
  unsigned int VAO1, VBO1, EBO1;
  glGenVertexArrays(1, &VAO1);
  glGenBuffers(1, &VBO1);
  glGenBuffers(1, &EBO1);

  // specify the attributes for our current VBO1
  // ...then copy actual data over
  glBindVertexArray(VAO1);
  glBindBuffer(GL_ARRAY_BUFFER, VBO1);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
  {
    // for the vertex buffer...
    glBufferData(GL_ARRAY_BUFFER, sphere.getInterleavedVertexSize(), sphere.getInterleavedVertices(), GL_STATIC_DRAW);

    // actual vertices themselves
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal coordinates
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coordinates
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // for the index buffer...
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.getIndexSize(), sphere.getIndices(), GL_STATIC_DRAW);
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // make camera
  glm::vec3 cam = glm::vec3(0., 0., 2.);
  glm::mat4 projection = glm::perspective(glm::radians(90.), aspect_ratio, 0.1, 1000.);
  glm::mat4 view = glm::mat4(1.);
  view = glm::translate(view, -cam);
  glm::mat4 model = glm::mat4(1.);

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    processInput(window);
    {
      glClearColor(0.6f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      // render pass
      // activate the shader program
      shaderProgram.use();
      float timeValue = glfwGetTime();
      float amplitude = 0.5 * (sin(timeValue) + 1.);

      shaderProgram.setFloat("amplitude", amplitude);

      glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
      glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
      glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

      // draw the sphere
      glBindVertexArray(VAO1);
      // vbo gets added automatically when vertex array is bound, but not the
      // element array buffer, but that kinda makes sense
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1); // debug
      // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      {
        glDrawElements(GL_TRIANGLES, sphere.getIndexSize(), GL_UNSIGNED_INT, 0);
      }
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
    }
    // make imgui generate the vertex buffer information
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow(); // Show demo window! :)
    // actually render the generated vertex buffer
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    // display frame from frame buffer
    glfwSwapBuffers(window);
  }
  // cleanup imgui
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  // cleanup glfw 
  glfwTerminate();

  return 0;
}