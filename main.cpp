/*
    Created by LuckyIntel from github.

    Version 0.1

    For best performance atleast compile with -O1 parameter in GCC.

    To load your OBJ, change "OBJpath" to your OBJ file's
    path, no other info is needed to load the OBJ file.

    I made this app in Windows however this must work
    just like Windows in Linux.I mean, i think it will.
    I don't own a Linux so there can be some problems.
    If anything unexpected happens just create a new 
    issue in the repo and i will try my best to help you!

    About Mac OS, i don't own one.If you occur any issue
    and use Mac OS, you can use issues too.I will try my 
    best.Alternatively problaly pray to god or something.

    WARNING : If you want to change eye speed in the
    runtime please remove "constexpr" keyword from it
    so you can change it.

    WARNING : Please atleast use OpenGL Core version 3.3
    to prevent any unexpected behavior.This application
    also runs with never versions such as OpenGL Core
    version 4.6
    
    How can you change the version?
    in the code change your OpenGL version with
    changing the 3's in the following code:

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    So if you want to set your OpenGL version to 4.6
    you can just do this :

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    Major is the number before dot, minor is the vice versa of it.

    TODO LIST (For V0.2):
     1. Fix texture reusing and color reusing
     2. Add lighting (default.frag)
     3. Make code more read-able, add more commentaries (half way done)
     4. Make OBJ loading easier with dear imgui
*/

#include "OBJLoader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stbi/stb_image.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

const std::string OBJpath = "your/obj/file/path"; // Your OBJ file's path.

int WIDTH = 800;
int HEIGHT = 600;
float aspectRatio = (float)WIDTH / HEIGHT;

glm::vec3 eyePos = glm::vec3(0.0f, 0.0f, -3.0f);
glm::vec3 eyeFront = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 eyeUp = glm::vec3(0.0f, 1.0f, 0.0f);
constexpr float eyeSpeed = 0.05;

char* filetobuf(const char *file)
{
    /*
        "filetobuf" function comes from www.khronos.org tutorials.
    */
    FILE *fptr;
    long length;
    char *buf;

    fptr = fopen(file, "rb"); /* Open file for reading */
    if (!fptr) /* Return NULL on failure */
        return NULL;
    fseek(fptr, 0, SEEK_END); /* Seek to the end of the file */
    length = ftell(fptr); /* Find out how many bytes into the file we are */
    buf = (char*)malloc(length+1); /* Allocate a buffer for the entire length of the file and a null terminator */
    fseek(fptr, 0, SEEK_SET); /* Go back to the beginning of the file */
    fread(buf, length, 1, fptr); /* Read the contents of the file in to the buffer */
    fclose(fptr); /* Close the file */
    buf[length] = 0; /* Null terminator */

    return buf; /* Return the buffer */
};

GLuint loadTexture(std::string filePath)
{
    GLuint id;
    int iW, iH, iCc, iF; // width, height, color channel, format
    unsigned char* img = stbi_load(filePath.c_str(), &iW, &iH, &iCc, 0);
    switch (iCc) // Check color channel to find the format of the image.
    {
        case 1:
            iF = GL_RED; // R
            break;
        case 3:
            iF = GL_RGB; // RGB
            break;
        case 4:
            iF = GL_RGBA; // RGBA
            break;
    };
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glGenerateMipmap(id);
    glTexImage2D(GL_TEXTURE_2D, 0, iF, iW, iH, 0, iF, GL_UNSIGNED_BYTE, img);
    stbi_image_free(img);
    return id;
};

class Shader
{
    public:
        GLuint id;
        Shader(const char* vertexPath, const char* fragmentPath)
        {
            GLchar* vSrc = filetobuf(vertexPath);
            GLchar* fSrc = filetobuf(fragmentPath);
            GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
            GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(vShader, 1, const_cast<const GLchar**>(&vSrc), nullptr);
            glShaderSource(fShader, 1, const_cast<const GLchar**>(&fSrc), nullptr);
            glCompileShader(vShader);
            glCompileShader(fShader);
            
            char info[1024];
            GLint vSuc, fSuc, pSuc;
            glGetShaderiv(vShader, GL_COMPILE_STATUS, &vSuc);
            glGetShaderiv(fShader, GL_COMPILE_STATUS, &fSuc);
            if (vSuc == GL_FALSE)
            {
                glGetShaderInfoLog(vShader, 1024, nullptr, info);
                std::cout << "Vertex Shader error : " << info << "\n";
            };
            if (fSuc == GL_FALSE)
            {
                glGetShaderInfoLog(fShader, 1024, nullptr, info);
                std::cout << "Fragment Shader error : " << info << "\n";
            };

            id = glCreateProgram();
            glAttachShader(id, vShader);
            glAttachShader(id, fShader);
            glLinkProgram(id);

            glGetProgramiv(id, GL_LINK_STATUS, &pSuc);
            if (pSuc == GL_FALSE)
            {
                glGetProgramInfoLog(id, 1024, nullptr, info);
                std::cout << "Shader Program error : " << info << "\n";
            };

            glDeleteShader(vShader);
            glDeleteShader(fShader);
        };
        GLint findUniform(const char* uniStr) { return glGetUniformLocation(id, uniStr); };
        void dump() { glDeleteProgram(id); };
        void use() { glUseProgram(id); };
};

class Mesh
{
    private:
        GLuint VAO, VBO, EBO;
        std::vector<unsigned int> indices;
        glm::vec3 color;
        std::vector<GLuint> texturesID;
        void createTextures(std::vector<std::string> textures)
        {
            for (int i = 0; i < textures.size(); i++)
            {
                texturesID.push_back(loadTexture(textures[i]));
            };
        };
    public:
        Mesh(std::vector<float> vertices, std::vector<unsigned int> indices, std::vector<std::string> textures, glm::vec3 color)
        {
            Mesh::indices = indices;
            Mesh::color = color;
            createTextures(textures);
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        };
        void render(Shader& shader, glm::mat4 pvm, glm::vec3 size)
        {
            glUniform3f(shader.findUniform("color"), color.r, color.g, color.b);
            glUniform3f(shader.findUniform("size"), size.x, size.y, size.z);
            for (int i = 0; i < texturesID.size(); i++)
                {
                glActiveTexture(GL_TEXTURE0 + i);
                glUniform1i(shader.findUniform("tex_d" + i), i);
                glBindTexture(GL_TEXTURE_2D, texturesID[i]);
            };
            glActiveTexture(GL_TEXTURE0);
            glUniformMatrix4fv(shader.findUniform("pvm"), 1, GL_FALSE, glm::value_ptr(pvm));
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        };
};

class Model
{
    private:
        std::vector<Mesh> meshList;
    public:
        glm::vec3 position = glm::vec3(0.0f);
        float rotation = 180.0f;
        glm::vec3 size = glm::vec3(1.0f);
        Model(std::string filePath)
        {
            ImportOBJ contents(filePath);
            std::vector<ModelARGS> margs = contents.getContents();
            //std::cout << margs.size() << "\n";
            for (auto arg : margs)
            {   
                Mesh mesh(arg.vertices, arg.indices, arg.textures, arg.color);
                meshList.insert(meshList.begin(), mesh);
            };
            contents.dump();
            //std::cout << meshList.size() << "\n";
        };
        void render(Shader& shader, glm::mat4 pv)
        {
            for (auto mesh : meshList)
            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
                model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
                mesh.render(shader, pv * model, size);
            };
        };
};

void onResize(GLFWwindow* window, int width, int height);

void keyInput(GLFWwindow* window);

int main()
{
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_SAMPLES, 4); // Uncomment to enable MSAA

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Mac OS support
    #endif
    
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL OBJ Loader(LuckyIntel)", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, onResize);
    glfwSwapInterval(1); //FPS limit, comment if you want no limit.
    
    gladLoadGL();
    
    stbi_set_flip_vertically_on_load(true);

    Shader shader("shaders/default.vert", "shaders/default.frag"); // Default shaders are being used.

    Model model(OBJpath);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    //glEnable(GL_MULTISAMPLE); // Uncomment to enable MSAA
    glCullFace(GL_BACK);

    glViewport(0, 0, WIDTH, HEIGHT);
    double totalFrames = 0, framesPerSecond = 0;
    double prevTime = glfwGetTime(), newTime;
    while (!glfwWindowShouldClose(window))
    {
        newTime = glfwGetTime();
        framesPerSecond = static_cast<double>(++totalFrames / (newTime - prevTime));
        prevTime = newTime;
        totalFrames = 0;
        std::cout << framesPerSecond << "\n";
        if (model.rotation >= 360.0f) model.rotation = 0.0f; // Believe me this single rule really helps.
        model.rotation += 0.1;
        
        glClearColor(0.5, 0.5, 0.5, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        keyInput(window);
        
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(eyePos, eyePos + eyeFront, eyeUp);
        
        shader.use();
        model.render(shader, proj * view);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    };

    shader.dump();
    glfwDestroyWindow(window);
    glfwTerminate();
};

void onResize(GLFWwindow* window, int width, int height)
{
    WIDTH = width;
    HEIGHT = height;
    if (WIDTH == 0 && HEIGHT == 0) aspectRatio = (float)4/3; // Prevent w/h(aspect) ratio faults.
    else aspectRatio = (float)WIDTH / HEIGHT;
    glViewport(0, 0, WIDTH, HEIGHT);
};

void keyInput(GLFWwindow* window)
{
    /* 
        No need for glm::normalize and glm::cross since
        we just move in the Z axis.
    */
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && eyePos.z < 0.0f) eyePos += eyeFront * eyeSpeed;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && eyePos.z > -10.0f) eyePos += eyeFront * -eyeSpeed;
};