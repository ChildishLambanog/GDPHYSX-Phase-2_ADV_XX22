#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model.h"
#include "Shader.h"
#include "Light.h"
#include "DirectionLight.h"
#include "PointLight.h"
#include "Camera.h"
#include "OrthoCamera.h"
#include "PerspectiveCamera.h"
#include "P6/MyVector.h"
#include "P6/P6Particle.h"
#include "P6/PhysicsWorld.h"
#include "P6/ForceGenerator.h"
#include "P6/DragForceGenerator.h"
#include "P6/ParticleContact.h"
#include "P6/ContactResolver.h"
#include "P6/AnchoredSpring.h"
#include "P6/ParticleSpring.h"
#include "P6/Rod.h"
#include "P6/Bungee.h"
#include "P6/Chain.h"

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <chrono>
#include <ctime>
#include <cstdlib>

using namespace std::chrono_literals;
constexpr std::chrono::nanoseconds timestep(16ms);


float x = 0.f, y = 0.f, z = -20.0f;

bool stateControl = false;
bool stateCam = true;


float x_scale = 0.5f, y_scale = 0.5f, z_scale = 0.5f;
float thetaX = 1.f, thetaY = 1.f;
float x_axisX = 0.f, y_axisX = 1.f, z_axisX = 0.f;
float x_axisY = 1.f, y_axisY = 0.f, z_axisY = 0.f;

glm::vec3 cameraPos = glm::vec3(0, 0, 2.f);
glm::vec3 WorldUp = glm::vec3(0, 1.0f, 0);
glm::vec3 Front = glm::vec3(0, 0.0f, -1);

bool firstMouse = true;
float pitch = 0.0f;
float yaw = -90.0f;


float lastX = 400, lastY = 400;

float height = 600.0f; //800
float width = 600.0f; //1200

Model main_object({ 0, 0, 0 });
Model main_object2({ 0, 0, 0 });

OrthoCamera orca({ 0,2,0 }, width, height);
PerspectiveCamera perca({ 0,0,0 }, height, width);

P6::PhysicsWorld pWorld = P6::PhysicsWorld();

P6::P6Particle particle = P6::P6Particle(-150, 0, 0); //Position of Partile 1
P6::P6Particle particle2 = P6::P6Particle(150, 0, 0); //Position of Partile 2

P6::MyVector accumulatedAcceleration(0.f, 0.f, 0.f);


void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

};

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Assignment3 Rafael Ira R. Villanueva", NULL, NULL);
    
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();

    //Texture
    int img_width, img_height, colorChannels;

    stbi_set_flip_vertically_on_load(true);

    GLuint texture;

    glGenTextures(1, &texture);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, texture);


    glEnable(GL_DEPTH_TEST);


    glfwSetKeyCallback(window, Key_Callback);

    Shader mainObjShader("Shaders/mainObj.vert", "Shaders/mainObj.frag");

    std::fstream vertSrc("Shaders/directionLight.vert");
    std::stringstream vertBuff;

    vertBuff << vertSrc.rdbuf();

    std::string vertS = vertBuff.str();
    const char* v = vertS.c_str();

    std::fstream fragSrc("Shaders/directionLight.frag");
    std::stringstream fragBuff;

    fragBuff << fragSrc.rdbuf();

    std::string fragS = fragBuff.str();
    const char* f = fragS.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &v, NULL);

    glCompileShader(vertexShader);

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragShader, 1, &f, NULL);

    glCompileShader(fragShader);

    GLuint shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vertexShader);
    glAttachShader(shaderProg, fragShader);

    glLinkProgram(shaderProg);

    std::string path = "3D/sphere.obj";
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> material;
    std::string warning, error;

    tinyobj::attrib_t attributes;

    bool success = tinyobj::LoadObj(
        &attributes,
        &shapes,
        &material,
        &warning,
        &error,
        path.c_str()
    );

    GLfloat UV[]{
        0.f, 1.f,
        0.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        0.f, 1.f,
        0.f, 0.f
    };

    std::vector<GLfloat> fullVertexData;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        tinyobj::index_t vData = shapes[0].mesh.indices[i];

        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3)]
        );

        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3) + 1]
        );

        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3) + 2]
        );

        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3)]
        );

        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3) + 1]
        );

        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3) + 2]
        );

        fullVertexData.push_back(
            attributes.texcoords[(vData.texcoord_index * 2)]
        );

        fullVertexData.push_back(
            attributes.texcoords[(vData.texcoord_index * 2) + 1]
        );

    }
    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GLfloat) * fullVertexData.size(),
        fullVertexData.data(),
        GL_DYNAMIC_DRAW
    );


    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,

        8 * sizeof(float),
        (void*)0

    );

    glEnableVertexAttribArray(0);

    GLintptr litPtr = 3 * sizeof(float);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,

        8 * sizeof(float),
        (void*)litPtr

    );

    glEnableVertexAttribArray(1);

    GLintptr uvPtr = 6 * sizeof(float);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,

        8 * sizeof(float),
        (void*)uvPtr

    );

    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glm::mat4 identity_matrix = glm::mat4(1.0f);

    glm::mat4 projection = glm::perspective(
        glm::radians(60.f),     
        height / width,          
        0.1f,                    
        1000.f                  
    );

    P6::MyVector sample(0.0f, 0.0f, 0.0f);

    //particle.Velocity = P6::MyVector(0, -100, 0); 
    //particle.Acceleration = P6::MyVector(0, 0, 0); 
    particle.mass = 10.0f;
    particle.radius = 90.0f;
    particle.restitution = 1.0f;
    particle.AddForce(P6::MyVector(50000, 0, 0)); //150k

    //particle2.Velocity = P6::MyVector(0, -100, 0);
    //particle2.Acceleration = P6::MyVector(0, 0, 0);
    particle2.mass = 10.0f; //100
    particle2.radius = 100.0f;
    particle2.restitution = 1.0f;
    //particle2.AddForce(P6::MyVector(0, 0, 0));

    pWorld.AddParticle(&particle);
    pWorld.AddParticle(&particle2);

    main_object.setScale(glm::vec3(particle.radius, particle.radius, particle.radius));
    main_object2.setScale(glm::vec3(particle2.radius, particle2.radius, particle2.radius));

    using clock = std::chrono::high_resolution_clock;
    auto curr_time = clock::now();
    auto prev_time = curr_time;
    std::chrono::nanoseconds curr_ns(0);

    //P6::ParticleContact contact = P6::ParticleContact();
    //contact.particles[0] = &particle;
	//contact.particles[1] = &particle2;

	//contact.contactNormal = particle.Position - particle2.Position;
	//contact.contactNormal = contact.contactNormal.normalize();

    //contact.restitution = 1.0f;

    //pWorld.AddContact(&particle, &particle2,1.0f,(particle.Position - particle2.Position).normalize());

    //particle.Velocity = P6::MyVector(-60, 0, 0);
    //particle2.Velocity = P6::MyVector(15, 0, 0);

    //AnchoredSpring aSpring = AnchoredSpring(MyVector(20, 0, 0), 5, 0.5);
    //pWorld.forceRegistry.Add(&particle, &aSpring);

    //ParticleSpring pS = ParticleSpring(&particle, 5, 1);
    //pWorld.forceRegistry.Add(&particle2, &pS);

    //ParticleSpring pS2 = ParticleSpring(&particle2, 5, 1);
    //pWorld.forceRegistry.Add(&particle, &pS2);

    //particle.AddForce(P6::MyVector(0.0f, 1.0f, 0.0f) * 500000);
    //Rod* r = new Rod();
    //r->particles[0] = &particle;
    //r->particles[1] = &particle2;
    //r->length = 200;

    //pWorld.Links.push_back(r);

    //Bungee bungee = Bungee(P6::MyVector(-150, 0, 0), 5, 200);
    //pWorld.forceRegistry.Add(&particle, &bungee);

	//Chain chain = Chain(P6::MyVector(150, 0, 0), 200);
	//pWorld.forceRegistry.Add(&particle2, &chain);

    while (!glfwWindowShouldClose(window))
    {
        curr_time = clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::nanoseconds> (curr_time - prev_time);
        prev_time = curr_time;

        curr_ns += dur;

        if (curr_ns >= timestep)
        {
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(curr_ns);

            curr_ns -= timestep;
            particle.Acceleration += accumulatedAcceleration;

            pWorld.Update((float)ms.count() / 1000);

            //contact.Resolve((float)ms.count()/1000);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 viewMatrix;

        main_object.setPosition(glm::vec3(particle.Position.x, particle.Position.y, particle.Position.z));
        main_object2.setPosition(glm::vec3(particle2.Position.x, particle2.Position.y, particle2.Position.z));

        if (stateCam)
        {
            viewMatrix = glm::lookAt(orca.getCameraPos(), orca.getCameraPos() + orca.getFront(), orca.getWorldUp());
        }

        else
        {
            viewMatrix = glm::lookAt(perca.getCameraPos(), perca.getCameraPos() + perca.getFront(), perca.getWorldUp());
        }

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        if (stateCam)
        {
            main_object.setCamera(orca.getProjection(), orca.getCameraPos(), orca.getFront(), orca.getViewMat());
            main_object2.setCamera(orca.getProjection(), orca.getCameraPos(), orca.getFront(), orca.getViewMat());
        }

        else  
        {
            main_object.setCamera(perca.getProjection(), perca.getCameraPos(), perca.getFront(), perca.getViewMat());
        }

       
        mainObjShader.use();
        mainObjShader.setVec4("objectColor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)); 
        main_object.mainDraw(&mainObjShader, &VAO, &fullVertexData);

        mainObjShader.setVec4("objectColor", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        main_object2.mainDraw(&mainObjShader, &VAO, &fullVertexData);

        glfwSwapBuffers(window);


        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}