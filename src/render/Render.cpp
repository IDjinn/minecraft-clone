//
// Created by Luke on 26/08/2025.
//

#include "Render.h"

Render::Render(const std::weak_ptr<World> &current_world) : current_world(current_world) {
    yaw = -90.0f;
    pitch = 0.0f;
    lastX = WIDTH / 2.0f;
    lastY = HEIGHT / 2.0f;
    firstMouse = true;
    mouseEnabled = true;

    auto world = current_world.lock();
    if (!world)
        throw new std::runtime_error("current_world is invalid");

    if (!glfwInit()) {
        std::cerr << "failed to init GLFW\n";
        throw std::runtime_error("failed to init GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Minecraft 2.0", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("failed to init glfw");
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "failed to load GLAD\n";
        throw std::runtime_error("failed to open glfw");
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos) {
        const auto render = static_cast<Render *>(glfwGetWindowUserPointer(window));
        render->mouse_callback(window, xpos, ypos);
    });
    glfwSetScrollCallback(window, [](GLFWwindow *window, double xoffset, double yoffset) {
        const auto render = static_cast<Render *>(glfwGetWindowUserPointer(window));
        render->scroll_callback(window, xoffset, yoffset);
    });
    glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        const auto render = static_cast<Render *>(glfwGetWindowUserPointer(window));
        render->key_callback(window, key, scancode, action, mods);
    });

    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    textureManager.load_texture("grass.png");
    const auto vertexShaderSource = shaderManager.load_shader("shader.vert");
    const auto fragmentShaderSource = shaderManager.load_shader("shader.frag");

    const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    const unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    DebugGui::setup(window);

    // ASSERT_DEBUG(visibleVertices.size() < WORLD_MAX_VERTICES,
    //              "created more vertices than possible in this implementation (something wrong with culling?)");
    // PRINT_DEBUG("Total vertices: " << visibleVertices.size()
    //     << " max: " << WORLD_MAX_VERTICES
    //     << " culling: " << (static_cast<double>(visibleVertices.size()) / WORLD_MAX_VERTICES * 100.0) << "%"
    //     << std::endl);

    glGenVertexArrays(1, &visibleVAO);
    glGenBuffers(1, &visibleVBO);

    glBindVertexArray(visibleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, visibleVBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void *>(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}

void Render::render() {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto world = current_world.lock();
    if (!world) return;

    DebugGui::prepare();
    const auto player = world->players.at(0);

    glPolygonMode(GL_FRONT_AND_BACK, player->draw_line ? GL_LINE : GL_FILL);
    glUseProgram(shaderProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

    const auto currentFrame = static_cast<float>(glfwGetTime());
    delta_time = currentFrame - lastFrame;
    lastFrame = currentFrame;

    player->processInput(window, delta_time);

    glm::mat4 view = glm::lookAt(player->position, player->position + player->cameraFront, player->cameraUp);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                            static_cast<float>(WIDTH) / static_cast<float>(HEIGHT), 0.1f, 1000.0f);

    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    const auto vertices = world->generate_visible_vertices();
    glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(float), vertices->data(), GL_STATIC_DRAW);

    glBindVertexArray(visibleVAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices->size());
    glBindVertexArray(0);

    DebugGui::render(this, player);
    glfwSwapBuffers(window);
    glfwPollEvents();
}

Render::~Render() {
    DebugGui::destroy();
    glDeleteVertexArrays(1, &visibleVAO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Render::mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (!mouseEnabled) return;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > PITCH)
        pitch = PITCH;
    if (pitch < -PITCH)
        pitch = -PITCH;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    if (const auto world = current_world.lock()) {
        const auto player = world->players.at(0);
        player->cameraFront = glm::normalize(direction);
    }
}

void Render::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
}

void Render::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        mouseEnabled = !mouseEnabled;
        if (mouseEnabled) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            firstMouse = true;
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void Render::framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, const int width, const int height) {
    glViewport(0, 0, width, height);
}
