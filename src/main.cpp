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


float deltaTime = 0.0f;
float lastFrame = 0.0f;
float cameraDistance = 15.0f;
float cameraAngleX = 0.0f;
float cameraAngleY = 0.0f;

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


    constexpr float cubeVertices[] = {
        // positions
        // front face
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        // back face
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        // left face
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,

        // right face
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f,

        // top face
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f,

        // bottom face
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
    };

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

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Dados das faces com posição e coordenadas de textura
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

    auto isSolid = [&](const int x, const int y, const int z) {
        if (World::isOutOfBounds(x, y, z)) return false;


        return x >= 0 && x < 10 && y >= 0 && y < 10 && z >= 0 && z < 10;
    };

    std::vector<float> visibleVertices;
    for (auto chunk: world->chunks) {
        for (auto block: chunk.blocks) {
            if (block.block_type() == AIR) continue;

            for (int face = 0; face < 6; ++face) {
                auto nx = block.x() + directions[face][0];
                auto ny = block.y() + directions[face][1];
                auto nz = block.z() + directions[face][2];

                if (isSolid(nx, ny, nz)) continue;

                for (int i = 0; i < 30; i += 5) {
                    float vx = faceVertices[face][i] + static_cast<float>(block.x());
                    float vy = faceVertices[face][i + 1] + static_cast<float>(block.y());
                    float vz = faceVertices[face][i + 2] + static_cast<float>(block.z());
                    float u = faceVertices[face][i + 3];
                    float v = faceVertices[face][i + 4];

                    visibleVertices.insert(visibleVertices.end(), {vx, vy, vz, u, v});
                }
            }
        }
    }


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
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

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


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
