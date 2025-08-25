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

#include "game/world/blocks/Block.h"
#include "game/world/blocks/BlockType.h"
#include "game/world/chunks/Chunk.h"
#include "game/world/generation/WorldGenerator.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#define WIDTH 1920
#define HEIGHT 1024

#define CAMERA_SPEED 5.0f
#define ZOOM_SPEED 2.0f

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


float cameraDistance = 15.0f;
float cameraAngleX = 0.0f;
float cameraAngleY = 0.0f;
long long frameCounter = 0;

std::vector<std::string> debug_output;

void scroll_callback([[maybe_unused]] GLFWwindow *window, [[maybe_unused]] double xoffset, const double yoffset) {
    cameraDistance -= static_cast<float>(yoffset) * ZOOM_SPEED;
    if (cameraDistance < 2.0f) cameraDistance = 2.0f;
    if (cameraDistance > 50.0f) cameraDistance = 50.0f;
}

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

    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    auto world = WorldGenerator::generateWorld();

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

    const float faceVertices[6][30] = {
        // front face (z+) - posição + tex coords
        {
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f
        },
        // back face (z-)
        {
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f
        },
        // left face (x-)
        {
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
            -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f
        },
        // right face (x+)
        {
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f
        },
        // top face (y+)
        {
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f
        },
        // bottom face (y-)
        {
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f
        }
    };

    const int directions[6][3] = {
        {0, 0, 1}, // front
        {0, 0, -1}, // back
        {-1, 0, 0}, // left
        {1, 0, 0}, // right
        {0, 1, 0}, // top
        {0, -1, 0} // bottom
    };


    std::vector<float> visibleVertices;
    long long totalCubes = 0;
    for (auto &[chunk_index, chunk]: world->chunks) {
        for (auto x = 0; x < CHUNK_SIZE_X; x++) {
            for (auto y = 0; y < CHUNK_SIZE_Y; y++) {
                for (auto z = 0; z < CHUNK_SIZE_Z; z++) {
                    auto index = Chunk::block_index(x, y, z);
                    auto &block = chunk->blocks[index];
                    if (block.block_type() == AIR) continue;
                    totalCubes++;

                    for (auto face = 0; face < 6; ++face) {
                        auto nx = x + directions[face][0];
                        auto ny = y + directions[face][1];
                        auto nz = z + directions[face][2];

                        auto neighborIsSolid = false;

                        if (nx >= 0 && nx < CHUNK_SIZE_X &&
                            ny >= 0 && ny < CHUNK_SIZE_Y &&
                            nz >= 0 && nz < CHUNK_SIZE_Z) {
                            int neighbor_index = Chunk::block_index(nx, ny, nz);
                            neighborIsSolid = chunk->blocks[neighbor_index].block_type() != AIR;
                        }

                        if (neighborIsSolid) continue;

                        for (int i = 0; i < 30; i += 5) {
                            auto vx = faceVertices[face][i] + static_cast<float>(x);
                            auto vy = faceVertices[face][i + 1] + static_cast<float>(y);
                            auto vz = faceVertices[face][i + 2] + static_cast<float>(z);
                            auto u = faceVertices[face][i + 3];
                            auto v = faceVertices[face][i + 4];
                            visibleVertices.insert(visibleVertices.end(), {vx, vy, vz, u, v});
                        }
                    }
                }
            }
        }
    }
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    DEBUG_ASSERT(visibleVertices.size() < WORLD_MAX_VERTICES,
                 "created more vertices than possible in this implementation (something wrong with culling?)");
    DEBUG_PRINT("Total vertices: " << visibleVertices.size()
                << " max: " << WORLD_MAX_VERTICES
                << " culling: " << (static_cast<double>(visibleVertices.size()) / WORLD_MAX_VERTICES * 100.0) << "%"
                << std::endl);
    DEBUG_PRINT("Total cubes: " << totalCubes);

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

    auto worldCenter = glm::vec3(5.0f, 5.0f, 5.0f);
    bool draw_line = false;
    static float deltaTime = 0.0f;
    static float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        frameCounter++;
        float cameraSpeed = CAMERA_SPEED * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cameraAngleY += cameraSpeed * 0.5f;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraAngleY -= cameraSpeed * 0.5f;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraAngleX -= cameraSpeed * 0.5f;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraAngleX += cameraSpeed * 0.5f;
        }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            worldCenter.y += cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            worldCenter.y -= cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            worldCenter.x -= cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            worldCenter.x += cameraSpeed;
        }

        static bool spacePressed = false;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            if (!spacePressed) {
                draw_line = !draw_line;
                spacePressed = true;
            }
        } else {
            spacePressed = false;
        }

        glPolygonMode(GL_FRONT_AND_BACK, draw_line ? GL_LINE : GL_FILL);
        glUseProgram(shaderProgram);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

        float camX = worldCenter.x + cameraDistance * cos(cameraAngleY) * cos(cameraAngleX);
        float camY = worldCenter.y + cameraDistance * sin(cameraAngleY);
        float camZ = worldCenter.z + cameraDistance * cos(cameraAngleY) * sin(cameraAngleX);

        auto cameraPos = glm::vec3(camX, camY, camZ);
        auto target = glm::vec3(0.0f, 0.0f, 0.0f);
        auto cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

        auto view = glm::lookAt(cameraPos, target, cameraUp);
        auto projection = glm::perspective(glm::radians(45.0f), static_cast<float>(WIDTH) / HEIGHT, 0.1f, 100.0f);

        const auto viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        const auto projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(visibleVAO);
        glDrawArrays(GL_TRIANGLES, 0, visibleVertices.size());
        glBindVertexArray(0);


        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ImGui::Begin("Debug");
        ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
        ImGui::SliderFloat("Camera Distance", &cameraDistance, 2.0f, 50.0f); {
            ImGui::BeginChild("Console", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false,
                              ImGuiWindowFlags_HorizontalScrollbar);
            for (auto &line: debug_output)
                ImGui::TextUnformatted(line.c_str());
            ImGui::EndChild();
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &visibleVAO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}