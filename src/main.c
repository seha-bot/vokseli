#include "../inc/glad/glad.h"
#include "../inc/GLFW/glfw3.h"
#include "../inc/nec.h"
#include <stdio.h>

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
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vert);
    glAttachShader(shaderProgram, frag);
    glLinkProgram(shaderProgram);
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

    float* kocke = 0;
    nec_push(kocke, -1.0f);
    nec_push(kocke, 1.0f);
    nec_push(kocke, 0.0f);
    nec_push(kocke, 0.5f);

    unsigned int uKocke;
    glGenTextures(1, &uKocke);
    glBindTexture(GL_TEXTURE_1D, uKocke);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, nec_size(kocke) >> 2, 0, GL_RGBA, GL_FLOAT, kocke);

    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "uKocke"), 0);

    //GLenum err;
    //while((err = glGetError()) != GL_NO_ERROR) printf("ERROR\n%u\n", err);

    while(!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_1D, uKocke);

        int uResolution = glGetUniformLocation(shaderProgram, "uResolution");
        glUniform2f(uResolution, W, H);

        int uTime = glGetUniformLocation(shaderProgram, "uTime");
        glUniform1f(uTime, glfwGetTime());

        int uSize = glGetUniformLocation(shaderProgram, "uSize");
        glUniform1i(uSize, nec_size(kocke) >> 2);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    nec_free(kocke);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

