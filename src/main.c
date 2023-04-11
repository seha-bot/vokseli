#include "../inc/glad/glad.h"
#include "../inc/GLFW/glfw3.h"
#include "../inc/nec.h"
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#define W 800
#define H 600

char* load_shader(const char* path)
{
    char* shader = 0;
    FILE* source = fopen(path, "r");
    if(!source)
    {
        printf("ERROR LOADING SHADER FILE\n");
        return 0;
    }

    char c = 0;
    while((c = fgetc(source)) != EOF)
    {
        nec_push(shader, c);
    }
    nec_push(shader, 0);

    fclose(source);
    return shader;
}

int success;
char infoLog[512];
unsigned int create_shader(GLenum type, const char* source)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, 0, infoLog);
        printf("SHADER ERROR:\n%s\n", infoLog);
    }
    return shader;
}

typedef struct
{
    float x, y, z, a;
} kocka;

int main()
{
    if(!glfwInit())
    {
        printf("FAILED TO INIT GLFW!\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(W, H, "RAY MARCHING", 0, 0);
    if(!window)
    {
        printf("FAILED TO CREATE A WINDOW!\n");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glViewport(0, 0, W, H);

    const char* vert_source = load_shader("shaders/main.vs");
    const char* frag_source = load_shader("shaders/main.fs");
    unsigned int vert = create_shader(GL_VERTEX_SHADER, vert_source);
    unsigned int frag = create_shader(GL_FRAGMENT_SHADER, frag_source);
    unsigned int program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
    glDeleteShader(vert);
    glDeleteShader(frag);
    nec_free(vert_source);
    nec_free(frag_source);

    float vertices[] = {
         1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
    };
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int u_kocke;
    glGenTextures(1, &u_kocke);
    glBindTexture(GL_TEXTURE_1D, u_kocke);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "u_kocke"), 0);

    kocka* kocke = 0;
    kocka a = {-1.5f, 1.0f, 1.0f, 1.5f};
    kocka b = {2.5f, 0.0f, 3.0f, 0.5f};
    nec_push(kocke, a);
    nec_push(kocke, b);

    while(!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE))
    {
        kocke[1].a = sin(glfwGetTime());

        glBindTexture(GL_TEXTURE_1D, u_kocke);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, nec_size(kocke), 0, GL_RGBA, GL_FLOAT, kocke);

        glUseProgram(program);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_1D, u_kocke);

        glUniform2f(glGetUniformLocation(program, "u_resolution"), W, H);
        glUniform1f(glGetUniformLocation(program, "u_time"), glfwGetTime());
        glUniform1i(glGetUniformLocation(program, "u_size"), nec_size(kocke));

        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteTextures(1, &u_kocke);
    nec_free(kocke);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

