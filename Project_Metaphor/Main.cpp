//GDPHYSX Phase 2 created by Rameses P. Amar, Arvin Lawrence B. Dacanay, Rafael Ira R. Villanueva
//GDPHYSX XX22

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
#include "P6/FallingNCradle.h"
#include "P6/LineRender.h"

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
bool forceApplied = false;

float x_scale = 0.5f, y_scale = 0.5f, z_scale = 0.5f;
float thetaX = 1.f, thetaY = 1.f;
float x_axisX = 0.f, y_axisX = 1.f, z_axisX = 0.f;
float x_axisY = 1.f, y_axisY = 0.f, z_axisY = 0.f;

//initialize camera vars
//glm::vec3 cameraPos = glm::vec3(0, 0, 2.f);
glm::vec3 WorldUp = glm::vec3(0, 1.0f, 0);
glm::vec3 Front = glm::vec3(0, 0.0f, -1);
const float cameraRotateSpeed = 1.5f;       //degrees per key press/frame
const float orbitRadius = 800.0f;        // distance from center
const float heightOffset = 300.0f;       // camera height above center

float pitch = 0.0f;
float yaw = 90.0f; // instead of -90.0f

float lastX = 400, lastY = 400;

float height = 800.0f; //800
float width = 800.0f; //1200

Model main_object({ 0, 0, 0 });
Model main_object2({ 0, 0, 0 });
Model main_object3({ 0, 0, 0 });
Model main_object4({ 0, 0, 0 });
Model main_object5({ 0, 0, 0 });

OrthoCamera orca({ 0,2,0 }, width, height);
PerspectiveCamera perca({ 0,0,0 }, height, width);

P6::PhysicsWorld pWorld = P6::PhysicsWorld();

P6::MyVector accumulatedAcceleration(0.f, 0.f, 0.f);


void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (glfwGetKey(window, GLFW_KEY_1)) //Change the camera into Orthographic Mode
    {
        stateCam = true;
        std::cout << "Camera changed to Ortographic" << std::endl;
    }

    if (glfwGetKey(window, GLFW_KEY_2)) //Change the camera into Perspective Mode
    {
        stateCam = false;
        std::cout << "Camera changed to Perspective" << std::endl;
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        //will implement later but this will handlle the logic of starting the cradle by adding addForce to the 1st particle
        forceApplied = true;
        std::cout << "Force Applied" << std::endl;
    }

    if (!stateCam)   //Only rotate camera in perspective mode
    {
        bool updated = false;

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) //Rotate Right
        {
            yaw += cameraRotateSpeed;
            updated = true;
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) //Rotate Left
        {
            yaw -= cameraRotateSpeed;
            updated = true;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) //Rotate Up
        {
            pitch += cameraRotateSpeed;
            updated = true;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) //Rotate Down
        {
            pitch -= cameraRotateSpeed;
            updated = true;
        }

        //Clamp the pitch to avoid flipping
        if (pitch > 89.0f)
        {
            pitch = 89.0f;
        }

        if (pitch < -89.0f)
        {
            pitch = -89.0f;
        }
    }
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

    using clock = std::chrono::high_resolution_clock;
    auto curr_time = clock::now();
    auto prev_time = curr_time;
    std::chrono::nanoseconds curr_ns(0);

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
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) 
    {
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

    float cableLength;
    float pGap, pRadius;
    //float particleRadius = 50.0f;
    float gravityStr;
    float pushX, pushY, pushZ;
    float forceMultiplier;

    std::cout << "Cable Length: ";
    std::cin >> cableLength;
    std::cout << "Particle Gap: ";
    std::cin >> pGap;
    std::cout << "Particle Radius: ";
    std::cin >> pRadius;
    std::cout << "Gravity Strength (This a multiplier for the base gravity -9.8): ";
    std::cin >> gravityStr;

    std::cout << "Apply Force" << std::endl;
    std::cout << "X: ";
    std::cin >> pushX;
    std::cout << "Y: ";
    std::cin >> pushY;
    std::cout << "Z: ";
    std::cin >> pushZ;
	std::cout << "Force Multiplier (Use a negative value to make the particle go left if X Force is positive): ";
	std::cin >> forceMultiplier;

    //Calculate starting x so particles are centered
    float startX = -2 * pGap;
    float startY = cableLength;

	float Pos1X = startX;               //Position of the first particle
	float Pos2X = startX + pGap;        //Position of the second particle
	float Pos3X = startX + 2 * pGap;    //Position of the third particle
	float Pos4X = startX + 3 * pGap;    //Position of the fourth particle
	float Pos5X = startX + 4 * pGap;    //Position of the fifth particle

    P6::P6Particle particle = P6::P6Particle(Pos1X, 200 /*+ cableLength*/, 0);      //Particle 1
	P6::P6Particle particle2 = P6::P6Particle(Pos2X, 200, 0);     //Particle 2
    P6::P6Particle particle3 = P6::P6Particle(Pos3X, 200, 0);     //Particle 3
    P6::P6Particle particle4 = P6::P6Particle(Pos4X, 200, 0);     //Particle 4
    P6::P6Particle particle5 = P6::P6Particle(Pos5X, 200, 0);     //Particle 5

    //pWorld.setGravity(gravityStr);

    // After creating particles and before the simulation loop
    //P6::GravityForceGenerator gravity(P6::MyVector(0, 0, gravityStr));

   // P6::GravityForceGenerator gravity(P6::MyVector(0, gravityStr * -9.8, 0));

    //pWorld.forceRegistry.Add(&particle, &gravity);
    //pWorld.forceRegistry.Add(&particle2, &gravity);
    //pWorld.forceRegistry.Add(&particle3, &gravity);
    //pWorld.forceRegistry.Add(&particle4, &gravity);
    //pWorld.forceRegistry.Add(&particle5, &gravity);

    //Particle 1 (this is where 
    particle.mass = 50;
    particle.radius = pRadius;
    particle.restitution = .9f;

    particle2.mass = 50; //100
    particle2.radius = pRadius;
    particle2.restitution = .9f;

    particle3.mass = 50;
    particle3.radius = pRadius;
    particle.restitution = .9f;

	particle4.mass = 50;
	particle4.radius = pRadius;
    particle.restitution = .9f;

	particle5.mass = 50;
	particle5.radius = pRadius;
	particle5.restitution = .9f;

    pWorld.AddParticle(&particle);
    pWorld.AddParticle(&particle2);
	pWorld.AddParticle(&particle3);
	pWorld.AddParticle(&particle4);
	pWorld.AddParticle(&particle5);
	pWorld.gravMULT(gravityStr); // Set the gravity strength in the physics world

    main_object.setScale(glm::vec3(particle.radius, particle.radius, particle.radius));
    main_object2.setScale(glm::vec3(particle2.radius, particle2.radius, particle2.radius));
	main_object3.setScale(glm::vec3(particle3.radius, particle3.radius, particle3.radius));
	main_object4.setScale(glm::vec3(particle4.radius, particle4.radius, particle4.radius));
	main_object5.setScale(glm::vec3(particle5.radius, particle5.radius, particle5.radius));

    LineRender line1(P6::MyVector(Pos1X, 200, 0), particle.Position, glm::mat4(1.f));
	LineRender line2(P6::MyVector(Pos2X, 200, 0), particle2.Position, glm::mat4(1.f));
	LineRender line3(P6::MyVector(Pos3X, 200, 0), particle3.Position, glm::mat4(1.f));
	LineRender line4(P6::MyVector(Pos4X, 200, 0), particle4.Position, glm::mat4(1.f));
	LineRender line5(P6::MyVector(Pos5X, 200, 0), particle5.Position, glm::mat4(1.f));


    FallingNCradle* fNC1 = new FallingNCradle(P6::MyVector(Pos1X, 200, 0), cableLength);
    fNC1->SetParticle(&particle);
    FallingNCradle* fNC2 = new FallingNCradle(P6::MyVector(Pos2X, 200, 0), cableLength);
    fNC2->SetParticle(&particle2);
    FallingNCradle* fNC3 = new FallingNCradle(P6::MyVector(Pos3X, 200, 0), cableLength);
    fNC3->SetParticle(&particle3);
    FallingNCradle* fNC4 = new FallingNCradle(P6::MyVector(Pos4X, 200, 0), cableLength);
    fNC4->SetParticle(&particle4);
    FallingNCradle* fNC5 = new FallingNCradle(P6::MyVector(Pos5X, 200, 0), cableLength);
    fNC5->SetParticle(&particle5);

	//Chain chain1 = Chain(P6::MyVector(Pos1X, 200, 0), cableLength);
	//Chain chain2 = Chain(P6::MyVector(Pos2X, 200, 0), cableLength);
	//Chain chain3 = Chain(P6::MyVector(Pos3X, 200, 0), cableLength);
	//Chain chain4 = Chain(P6::MyVector(Pos4X, 200, 0), cableLength);
	//Chain chain5 = Chain(P6::MyVector(Pos5X, 200, 0), cableLength);

	pWorld.Links.push_back(fNC1);
	pWorld.Links.push_back(fNC2);
	pWorld.Links.push_back(fNC3);
	pWorld.Links.push_back(fNC4);
	pWorld.Links.push_back(fNC5);

    //pWorld.forceRegistry.Add(&particle, &fNC1);
	//pWorld.forceRegistry.Add(&particle2, &fNC2);
	//pWorld.forceRegistry.Add(&particle3, &fNC3);
	//pWorld.forceRegistry.Add(&particle4, &fNC4);
	//pWorld.forceRegistry.Add(&particle5, &fNC5);

	/*pWorld.forceRegistry.Add(&particle, &chain1);
	pWorld.forceRegistry.Add(&particle2, &chain2);
	pWorld.forceRegistry.Add(&particle3, &chain3);
	pWorld.forceRegistry.Add(&particle4, &chain4);
	pWorld.forceRegistry.Add(&particle5, &chain5);*/

	/*std::cout << "particle 1 position: " << particle.Position.x << ", " << particle.Position.y << ", " << particle.Position.z << std::endl;
	std::cout << "particle 2 position: " << particle2.Position.x << ", " << particle2.Position.y << ", " << particle2.Position.z << std::endl;
	std::cout << "particle 3 position: " << particle3.Position.x << ", " << particle3.Position.y << ", " << particle3.Position.z << std::endl;
	std::cout << "particle 4 position: " << particle4.Position.x << ", " << particle4.Position.y << ", " << particle4.Position.z << std::endl;
	std::cout << "particle 5 position: " << particle5.Position.x << ", " << particle5.Position.y << ", " << particle5.Position.z << std::endl;*/

    while (!glfwWindowShouldClose(window))
    {
        curr_time = clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::nanoseconds> (curr_time - prev_time);
        prev_time = curr_time;

        curr_ns += dur;

        if (curr_ns >= timestep)
        {
            //std::cout << "particle 1 position: " << particle.Position.x << ", " << particle.Position.y << ", " << particle.Position.z << std::endl;
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(curr_ns);

            curr_ns -= timestep;

            if (forceApplied)
            {
                particle.AddForce(P6::MyVector(pushX*forceMultiplier, pushY * forceMultiplier, pushZ * forceMultiplier)); //Apply force to the first particle
              //  particle.AddForce(P6::MyVector(-10000, 0, 0)); //-100, 0, 0
                forceApplied = false;
            }

            //pWorld.setGravity(gravityStr); 

            particle.Acceleration += accumulatedAcceleration;

            pWorld.Update((float)ms.count() / 1000);

            //contact.Resolve((float)ms.count()/1000);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 viewMatrix;

        main_object.setPosition(glm::vec3(particle.Position.x, particle.Position.y, particle.Position.z));
        main_object2.setPosition(glm::vec3(particle2.Position.x, particle2.Position.y, particle2.Position.z));
		main_object3.setPosition(glm::vec3(particle3.Position.x, particle3.Position.y, particle3.Position.z));
		main_object4.setPosition(glm::vec3(particle4.Position.x, particle4.Position.y, particle4.Position.z));
		main_object5.setPosition(glm::vec3(particle5.Position.x, particle5.Position.y, particle5.Position.z));

        glm::vec3 centerPoint = (main_object.getPosition() + main_object2.getPosition() + main_object3.getPosition() + main_object4.getPosition() + main_object5.getPosition()) / 5.0f;
        glm::vec3 offset;
        offset.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)) * orbitRadius;
        offset.y = sin(glm::radians(pitch)) * orbitRadius + heightOffset;
        offset.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * orbitRadius;

        glm::vec3 camPos = centerPoint + offset;
        perca.setCameraPos(camPos);
        perca.setFront(glm::normalize(centerPoint - camPos));

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
			main_object3.setCamera(orca.getProjection(), orca.getCameraPos(), orca.getFront(), orca.getViewMat());
			main_object4.setCamera(orca.getProjection(), orca.getCameraPos(), orca.getFront(), orca.getViewMat());
			main_object5.setCamera(orca.getProjection(), orca.getCameraPos(), orca.getFront(), orca.getViewMat());
        }

        else  
        {
            main_object.setCamera(perca.getProjection(), perca.getCameraPos(), perca.getFront(), perca.getViewMat());
            main_object2.setCamera(perca.getProjection(), perca.getCameraPos(), perca.getFront(), perca.getViewMat());
            main_object3.setCamera(perca.getProjection(), perca.getCameraPos(), perca.getFront(), perca.getViewMat());
            main_object4.setCamera(perca.getProjection(), perca.getCameraPos(), perca.getFront(), perca.getViewMat());
            main_object5.setCamera(perca.getProjection(), perca.getCameraPos(), perca.getFront(), perca.getViewMat());
        }

        mainObjShader.use();
        
        //Particle 1
        mainObjShader.setVec4("objectColor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)); //red
        main_object.mainDraw(&mainObjShader, &VAO, &fullVertexData);

        //Particle 2
        mainObjShader.setVec4("objectColor", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)); //green
        main_object2.mainDraw(&mainObjShader, &VAO, &fullVertexData);

        //Particle 3
		mainObjShader.setVec4("objectColor", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)); //blue
		main_object3.mainDraw(&mainObjShader, &VAO, &fullVertexData);

        //Particle 4
		mainObjShader.setVec4("objectColor", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)); //yellow
        main_object4.mainDraw(&mainObjShader, &VAO, &fullVertexData);

        //Particle 5
		mainObjShader.setVec4("objectColor", glm::vec4(1.0f, 0.65f, 0.0f, 1.0f)); //orange
        main_object5.mainDraw(&mainObjShader, &VAO, &fullVertexData);

        glfwSwapBuffers(window);


        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}