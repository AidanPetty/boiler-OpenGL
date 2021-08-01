#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include<iostream>  
#include<fstream>
#include<string>    
#include<sstream>

#define ASSERT(x) if (!(x)) __debugbreak()
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))
// Clears the error buffer
static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}
// Gets and displays errors
static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGl ERROR] " << "(" << error << ")" << std::endl << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}
// Holds the shader when ShaderProgramSource() is called
struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;

};
// Reads the input Shader(filePath).
static ShaderProgramSource ParseShader(const std::string& filePath)
{
    std::ifstream stream(filePath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
            
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    return{ ss[0].str(), ss[1].str() };
}

// 
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
   unsigned int id = glCreateShader(type);
   const char* src = source.c_str();
   glShaderSource(id, 1, &src, nullptr);
   glCompileShader(id);         // This Compiles the shader

   // Error checking
   int result;
   glGetShaderiv(id, GL_COMPILE_STATUS, &result);
   if (result == GL_FALSE)
   {
       int length;
       glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
       char* message = (char*)alloca(length * sizeof(char));
       glGetShaderInfoLog(id, length, &length, message);
       std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
       std::cout << message << std::endl;
       glDeleteShader(id);
       return 0;
   }
   return id;
}

// Provide opengl with source code
static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error!" << std::endl;
    }
    std::cout << glGetString(GL_VERSION) << std::endl;  

    float positions[] = {
        -0.5f, -0.5f, // 0
         0.5f, -0.5f, // 1
         0.5f,  0.5f, // 2
        -0.5f,  0.5f, // 3
    };
    
    unsigned int indices[] =
    {
        0, 1, 2,
        2, 3 ,0
    };


    unsigned int buffer;                // This is a vertex buffer
    GLCall(glGenBuffers(1, &buffer));               // This is where OpenGL Creates the buffer and stores it in the variable
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));              // This binds the information.
    GLCall(glBufferData(GL_ARRAY_BUFFER, 6*2*sizeof(float), positions, GL_STATIC_DRAW));                // creates a new data store for the buffer object currently bound to target(The first parameter)
    
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0));

    GLCall(unsigned int ibo);
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));


    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

    
    /*
    std::cout << "VERTEX" << std::endl;
    std::cout << source.VertexSource << std::endl;
    std::cout << "FRAGMENT" << std::endl;
    std::cout << source.FragmentSource << std::endl;
    */
   ShaderProgramSource source = ParseShader("res\\shaders\\basic.shader");
   unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
   GLCall(glUseProgram(shader));
   
  
   GLCall(int location = glGetUniformLocation(shader, "u_Color"));
   ASSERT(location != -1);
   GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));
   
   float r = 0.0f;
   float increment = 0.05f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {


        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        if (r > 1.0f)
            increment = -0.05f;
        else if (r < 0.0f)
            increment = 0.05f;
        r += increment;
        
        /* Swap front and back buffers */
        GLCall(glfwSwapBuffers(window));

        /* Poll for and process events */
        GLCall(glfwPollEvents());


    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}