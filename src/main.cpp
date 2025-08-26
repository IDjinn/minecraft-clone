#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "game/players/Player.h"
#include "game/world/blocks/Block.h"
#include "game/world/blocks/BlockType.h"
#include "game/world/chunks/Chunk.h"
#include "game/world/generation/WorldGenerator.h"
#include "utils/DebugGui.h"

#define PITCH 89.0f
#define CAMERA_SPEED 10.0f

#define WIDTH 1920
#define HEIGHT 1024

void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, const int width, const int height) {
    glViewport(0, 0, width, height);
}

auto vertexShaderSource = R"glsl(
    #version 330 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec2 aTexCoord;

    uniform mat4 view;
    uniform mat4 projection;

    out vec2 TexCoord;

    void main() {
        gl_Position = projection * view * vec4(aPos, 1.0);
        TexCoord = aTexCoord;
    }
)glsl";


auto fragmentShaderSource = R"glsl(
    #version 330 core
    out vec4 FragColor;

    in vec2 TexCoord;
    uniform sampler2D texture1;

    void main() {
        FragColor = texture(texture1, TexCoord);
    }
)glsl";


std::vector<std::string> debug_output;

void load_texture(const std::string &texture_name) {
    stbi_set_flip_vertically_on_load(1);
    int width, height, nrChannels;
    unsigned char *data = stbi_load(("assets/" + texture_name).c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture " << texture_name << " because " << stbi_failure_reason() << std::endl;
        __debugbreak();
    }
    stbi_image_free(data);
}


static float deltaTime = 0.0f;
static float lastFrame = 0.0f;

auto player = Player(World::generate_entity_id(), "Steve", WORLD_SPAWN_COORDS);


static float yaw;
static float pitch;
static float lastX;
static float lastY;
static bool firstMouse;
static bool mouseEnabled;
static bool spacePressed;
static bool draw_line;

void mouse_callback([[maybe_unused]] GLFWwindow *window, double xpos, double ypos) {
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
    player.cameraFront = glm::normalize(direction);
}

void scroll_callback(
    [[maybe_unused]] GLFWwindow *window,
    [[maybe_unused]] double xoffset,
    [[maybe_unused]] double yoffset
) {
}

void key_callback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) {
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

void processInput(GLFWwindow *window, float deltaTime) {
    float cameraSpeed = CAMERA_SPEED * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (!spacePressed) {
            draw_line = !draw_line;
            spacePressed = true;
        }
    } else {
        spacePressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        player.position += cameraSpeed * player.cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        player.position -= cameraSpeed * player.cameraFront;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        player.position -= glm::normalize(glm::cross(player.cameraFront, player.cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        player.position += glm::normalize(glm::cross(player.cameraFront, player.cameraUp)) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        player.position += cameraSpeed * player.cameraUp;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        player.position -= cameraSpeed * player.cameraUp;
}

int main() {
    if (!glfwInit()) {
        std::cerr << "failed to init GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const auto window = glfwCreateWindow(WIDTH, HEIGHT, "Minecraft 2.0", nullptr, nullptr);
    if (!window) {
        std::cerr << "failed to init window GLFW\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "failed to load GLAD\n";
        return -1;
    }

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    DebugGui::setup(window);

    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    auto world = WorldGenerator::generateWorld();
    world->add_player(player);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    load_texture("grass.png");

    const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    const unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    const unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    long long totalCubes = 0;
    std::vector<float> visibleVertices;
    visibleVertices.reserve(WORLD_RENDER_VERTICES_RESERVE);
    for (auto &[chunk_index, chunk]: world->chunks) {
        const auto [chunk_x, chunk_y, chunk_z] = World::chunk_id_to_world_coordinates(chunk_index);
        const auto chunk_id = World::chunk_id_from_world_coords({chunk_x, chunk_y, chunk_z});
        ASSERT_DEBUG(chunk_index == chunk_id, "Mismatch chunking id => coordinates");

        for (auto y = 0; y < CHUNK_SIZE_Y; y++) {
            for (auto x = 0; x < CHUNK_SIZE_X; x++) {
                for (auto z = 0; z < CHUNK_SIZE_Z; z++) {
                    auto index = Chunk::block_index(x, y, z);
                    if (auto &block = chunk->blocks[index]; block.block_type() == BlockType::AIR) continue;

                    totalCubes++;
                    for (auto face = 0; face < 6; ++face) {
                        auto nx = x + directions[face][0];
                        auto ny = y + directions[face][1];
                        auto nz = z + directions[face][2];

                        auto neighborIsSolid = false;
                        const auto world_position_current_block = glm::vec3(chunk_x + x, chunk_y + y, chunk_z + z);
                        if (nx >= 0 && nx < CHUNK_SIZE_X &&
                            ny >= 0 && ny < CHUNK_SIZE_Y &&
                            nz >= 0 && nz < CHUNK_SIZE_Z) {
                            auto neighbor_block_index = Chunk::block_index(nx, ny, nz);
                            neighborIsSolid = chunk->blocks[neighbor_block_index].block_type() != BlockType::AIR;
                        } else {
                            auto neighbor_x = x + directions[face][0];
                            auto neighbor_y = y + directions[face][1];
                            auto neighbor_z = z + directions[face][2];
                            auto neighbor_chunk_id = World::chunk_id_from_world_coords(
                                {chunk_x + neighbor_x, chunk_y + neighbor_y, chunk_z + neighbor_z}
                            );

                            if (auto it = world->chunks.find(neighbor_chunk_id); it != world->chunks.end()) {
                                auto &neighbor_chunk = it->second;
                                auto neighbor_block_x = (neighbor_x + CHUNK_SIZE_X) % CHUNK_SIZE_X;
                                auto neighbor_block_y = (neighbor_y + CHUNK_SIZE_Y) % CHUNK_SIZE_Y;
                                auto neighbor_block_z = (neighbor_z + CHUNK_SIZE_Z) % CHUNK_SIZE_Z;
                                auto neighbor_index = Chunk::block_index(
                                    neighbor_block_x, neighbor_block_y, neighbor_block_z);
                                neighborIsSolid = neighbor_chunk->blocks[neighbor_index].block_type() != BlockType::AIR;
                            }
                        }

                        if (neighborIsSolid) continue;
                        for (int i = 0; i < 30; i += 5) {
                            auto vx = faceVertices[face][i] + world_position_current_block.x;
                            auto vy = faceVertices[face][i + 1] + world_position_current_block.y;
                            auto vz = faceVertices[face][i + 2] + world_position_current_block.z;
                            auto u = faceVertices[face][i + 3];
                            auto v = faceVertices[face][i + 4];
                            visibleVertices.insert(visibleVertices.end(), {vx, vy, vz, u, v});
                        }
                    }
                }
            }
        }
    }


    ASSERT_DEBUG(visibleVertices.size() < WORLD_MAX_VERTICES,
                 "created more vertices than possible in this implementation (something wrong with culling?)");
    PRINT_DEBUG("Total vertices: " << visibleVertices.size()
        << " max: " << WORLD_MAX_VERTICES
        << " culling: " << (static_cast<double>(visibleVertices.size()) / WORLD_MAX_VERTICES * 100.0) << "%"
        << std::endl);
    PRINT_DEBUG("Total cubes: " << totalCubes);

    unsigned int visibleVAO, visibleVBO;
    glGenVertexArrays(1, &visibleVAO);
    glGenBuffers(1, &visibleVBO);

    glBindVertexArray(visibleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, visibleVBO);
    glBufferData(GL_ARRAY_BUFFER, visibleVertices.size() * sizeof(float), visibleVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void *>(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        DebugGui::prepare();

        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window, deltaTime);

        glPolygonMode(GL_FRONT_AND_BACK, draw_line ? GL_LINE : GL_FILL);
        glUseProgram(shaderProgram);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

        glm::mat4 view = glm::lookAt(player.position, player.position + player.cameraFront, player.cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                                static_cast<float>(WIDTH) / static_cast<float>(HEIGHT), 0.1f, 1000.0f);

        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(visibleVAO);
        glDrawArrays(GL_TRIANGLES, 0, visibleVertices.size());
        glBindVertexArray(0);

        DebugGui::render(deltaTime);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    DebugGui::destroy();

    glDeleteVertexArrays(1, &visibleVAO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
