#include <iostream>
#include <fstream>
#include <string>
#include <format>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_SWIZZLE 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
// user-friendly text message on failures
#define STBI_FAILURE_USERMSG 
#include "stb_image.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "shader.h"
#include "Sphere.h"

using namespace std;

struct Camera {
  glm::vec3 velocity;
  glm::vec3 position;
  glm::vec3 direction;
  glm::vec3 up;
  float speed;
};

struct Mouse {
  bool active;
  double sensitivity;
  glm::dvec2 pos;
  glm::dvec2 delta_pos;
  // roll, pitch, yaw
  glm::dvec2 angle;
};

struct Game {
  float last_frame;
  Camera camera;
  Mouse mouse;
  float r_a;
  float r_b;
  float r_c;
  float r_d;
};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  Game* game = (Game*)glfwGetWindowUserPointer(window);
  if (key == GLFW_KEY_F && action == GLFW_PRESS) {
    if (!game->mouse.active) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      cout << "captured mouse cursor" << endl;
      // set cursor position to middle of window to remove jank
      game->mouse.pos = glm::vec2(400., 300.);
    }
    else {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      cout << "freed mouse cursor" << endl;
    }
    game->mouse.active = !game->mouse.active;
  }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
  Game* game = (Game*)glfwGetWindowUserPointer(window);
  if (game->mouse.active) {
    glm::dvec2 current_pos = glm::dvec2(xpos, ypos);

    glm::dvec2 delta_pos = current_pos - game->mouse.pos;
    game->mouse.delta_pos += delta_pos;

    game->mouse.pos = current_pos;
  }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

string vec3_format(glm::vec3 v) {
  return format("({},{},{})", v.x, v.y, v.z);
}

void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
  Game* game = (Game*)glfwGetWindowUserPointer(window);
  Camera* cam = &game->camera;
  glm::vec3 cam_right = glm::cross(cam->direction, cam->up);

  cam->velocity = glm::vec3(0., 0., 0.);

  if (glfwGetKey(window, GLFW_KEY_W)) {
    cam->velocity += cam->speed * cam->direction;
  }
  if (glfwGetKey(window, GLFW_KEY_A)) {
    cam->velocity -= cam->speed * cam_right;
  }
  if (glfwGetKey(window, GLFW_KEY_S)) {
    cam->velocity -= cam->speed * cam->direction;
  }
  if (glfwGetKey(window, GLFW_KEY_D)) {
    cam->velocity += cam->speed * cam_right;
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
    cam->velocity += cam->speed * glm::vec3(0., -1., 0.);
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE)) {
    cam->velocity += cam->speed * glm::vec3(0., 1., 0.);
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

  Game game = {
    .last_frame = 0.,
    .camera = {
      .velocity = glm::vec3(0., 0., 0.),
      .position = glm::vec3(0.5, 0.5, 1.5),
      .direction = glm::vec3(0., 0., -1.),
      .up = glm::vec3(0., 1., 0.),
      .speed = 2.5,
    },
    .mouse = {
      .active = false,
      .sensitivity = 0.1,
      .pos = glm::vec2(0., 0.),
    },
    .r_a = 0.,
    .r_b = 0.,
    .r_c = 0.,
    .r_d = 0.,
  };
  // let game be accessible in the input callback function
  glfwSetWindowUserPointer(window, &game);
  glfwSetCursorPosCallback(window, mouse_callback);  
  glfwSetKeyCallback(window, key_callback);

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
  Sphere sphere(1.0f, 100, 100, true, 2);

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

    // float vertices[] = {
    //    1., 2., 3.,
    // };
    
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
  glm::mat4 projection = glm::perspective(glm::radians(90.), aspect_ratio, 0.1, 1000.);
  glm::mat4 view = glm::lookAt(game.camera.position, game.camera.direction, game.camera.up);
  // scuffed, move object into [0,1]x[0,1]x[0,1] space lol
  // need buffer of bboxes if we want to have multiple or moving objects
  glm::mat4 model = glm::translate(glm::mat4(1.), glm::vec3(0.5, 0.5, 0.5));
  model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));

  glEnable(GL_DEPTH_TEST);  

  // configure opengl texture behavior
  // 2d textures
  unsigned int texture_1;
  glGenTextures(1, &texture_1);
  glBindTexture(GL_TEXTURE_2D, texture_1);

  // specify wrapping & magnification behavior
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  // use auto-generated mipmaps when scaling down
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // the nrChannels refers to the number of values per pixel, like R/G/B for a jpg
  {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(format("{}/textures/container.jpg", DEBUG_PATH_PREFIX).c_str(), &width, &height, &nrChannels, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
      // cout << width << "," << height << "," << nrChannels << endl; // debug
      cout << stbi_failure_reason() << endl;
    }
    stbi_image_free(data);
  }

  unsigned int texture_2;
  glGenTextures(1, &texture_2);
  glBindTexture(GL_TEXTURE_2D, texture_2);

  // specify wrapping & magnification behavior
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  // use auto-generated mipmaps when scaling down
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(format("{}/textures/awesomeface.png", DEBUG_PATH_PREFIX).c_str(), &width, &height, &nrChannels, 0);
    if (data) {
      // cout << width << "," << height << "," << nrChannels << endl; // debug
      // keep in mind if there's a mismatch between gl_rgba and the nrChannels, you will get random-looking textures
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
      // cout << width << "," << height << "," << nrChannels << endl; // debug
      cout << stbi_failure_reason() << endl;
    }
    stbi_image_free(data);
  }

  // 3d textures
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // storage buffer for random numbers
  // get 100 random vec3s ranging from 0 to 1 for each component

  // srand(0);
  // vector<glm::vec4> random_points;
  // for (int i = 0; i < 1000; i++) {
  //   float x = (float) rand() / (float) RAND_MAX;
  //   float y = (float) rand() / (float) RAND_MAX;
  //   float z = (float) rand() / (float) RAND_MAX;
  //   // padding 
  //   random_points.push_back(glm::vec4(x, y, z, 0.));
  //   
  //   // if (i == 0) {
  //   //   cout << x << "," << y << "," << z << endl;
  //   // }
  // }

  // unsigned int ssbo;
  // glGenBuffers(1, &ssbo);
  // glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
  // glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * random_points.size(), random_points.data(), GL_DYNAMIC_DRAW);
  // unsigned int ssbo_binding_point = 0; // You can choose any binding point that's not already in use

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    processInput(window);

    float current_frame = glfwGetTime();
    float delta_t = current_frame - game.last_frame;
    game.last_frame = current_frame;

    // update player camera based off rotations
    // modification of my work at 
    // https://github.com/SpicyRicecaker/sylveon/blob/d565648ac6f160bea889851b1a07f19453c215f5/src/main.rs#L138-L153
    {
      auto t = -game.mouse.sensitivity * game.mouse.delta_pos;
      game.mouse.angle += glm::vec2(t.x, t.y);

      glm::mat4 model = glm::mat4(1.);
      // yaw 
      model = glm::rotate(model, (float) glm::radians(game.mouse.angle.x), glm::vec3(0., 1., 0.));
      // pitch
      model = glm::rotate(model, (float) glm::radians(game.mouse.angle.y), glm::vec3(1., 0., 0.));
      game.camera.direction = (model * glm::vec4(0., 0., -1., 0.)).xyz;
      game.camera.up = (model * glm::vec4(0., 1., 0., 0.)).xyz;

      game.mouse.delta_pos.x = 0;
      game.mouse.delta_pos.y = 0;
    }

    // update player position based on velocity
    game.camera.position += delta_t * game.camera.velocity;


    // update transforms
    view = glm::lookAt(game.camera.position, game.camera.position + game.camera.direction, game.camera.up);

    {
      glClearColor(0.6f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // render pass
      // activate the shader program
      shaderProgram.use();
      float timeValue = glfwGetTime();
      float amplitude = 0.5 * (sin(timeValue) + 1.);

      shaderProgram.setFloat("amplitude", amplitude);

      shaderProgram.setMat4("model", model);
      shaderProgram.setMat4("view", view);
      shaderProgram.setMat4("projection", projection);

      glm::mat3 normal = glm::mat3(glm::transpose(glm::inverse(model)));
      shaderProgram.setMat3("normal", normal);

      shaderProgram.setVec3("eye", game.camera.position);

      shaderProgram.setInt("texture_1", 0);
      shaderProgram.setInt("texture_2", 1);

      shaderProgram.setFloat("r_a", game.r_a);
      shaderProgram.setFloat("r_b", game.r_b);
      shaderProgram.setFloat("r_c", game.r_c);
      shaderProgram.setFloat("r_d", game.r_d);

      // draw the sphere
      glBindVertexArray(VAO1);
      // vbo gets added automatically when vertex array is bound, but not the
      // element array buffer, but that kinda makes sense
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1); // debug
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture_1);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, texture_2);

      // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point, ssbo);
      // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe debug mode
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
    // ImGui::ShowDemoWindow(); // Show demo window! :)
    ImGui::Begin("Hello, world!");
    ImGui::Text(format("pos: {}", vec3_format(game.camera.position)).c_str());
    ImGui::Text(format("direction: {}", vec3_format(game.camera.direction)).c_str());
    ImGui::Text(format("up: {}", vec3_format(game.camera.up)).c_str());
    ImGui::SliderFloat("r_a", &game.r_a, 0.0f, 1.0f, "Value = %.3f");
    ImGui::SliderFloat("r_b", &game.r_b, 0.0f, 1.0f, "Value = %.3f");
    ImGui::SliderFloat("r_c", &game.r_c, 0.0f, 1.0f, "Value = %.3f");
    ImGui::SliderFloat("r_d", &game.r_d, 0.0f, 1.0f, "Value = %.3f");
    ImGui::End();
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