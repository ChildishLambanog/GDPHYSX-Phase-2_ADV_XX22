#include "Model.h"

Model::Model(glm::vec3 position)
{
	this->position = position;
	this->rotation = { 0,0,0 };
	this->scale = { 0.1f,0.1f,0.1f }; 
    this->rotX = 0;
    this->rotY = 0;
    this->rotZ = 0;
 
    this->identity_matrix4 = glm::mat4(1.0f);

    //Initial Camera Variables
    projection = glm::perspective(
        glm::radians(60.f),    //FOV
        800.f / 800.f,         //aspect ratio
        0.1f,                  //znear > 0
        1000.f                 //zfar
    );

    this->cameraPos = glm::vec3(0.0f, 0.0f, 500.0f); //0.0f, 0.0f, 2.f
    this->WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    this->Front = glm::vec3(0.0f, 0.0f, -1.0f); //0.0f, 0.0f, -1.0f

    //this->viewMatrix = glm::lookAt(this->cameraPos, this->cameraPos + this->Front, this->WorldUp);
    this->viewMatrix = glm::lookAt(this->cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), this->WorldUp);
}

void Model::draw(GLuint* shaderProg, GLuint* VAO, std::vector<GLfloat>* fullVertexData)
{

    glm::mat4 transformation_matrix = glm::translate(identity_matrix4, glm::vec3(position.x, position.y, position.z));

    transformation_matrix = glm::scale(transformation_matrix, glm::vec3(scale.x, scale.y, scale.z));

    transformation_matrix = glm::rotate(transformation_matrix, glm::radians(rotX), glm::normalize(glm::vec3(1, 0, 0)));

    transformation_matrix = glm::rotate(transformation_matrix, glm::radians(rotY), glm::normalize(glm::vec3(0, 1, 0)));

    transformation_matrix = glm::rotate(transformation_matrix, glm::radians(rotZ), glm::normalize(glm::vec3(0, 0, 1)));

    unsigned int transformLoc = glGetUniformLocation(*shaderProg, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));

    unsigned int projLoc = glGetUniformLocation(*shaderProg, "projection");

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    unsigned int viewLoc = glGetUniformLocation(*shaderProg, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    GLuint cameraPosLoc = glGetUniformLocation(*shaderProg, "cameraPos");
    glUniform3fv(cameraPosLoc, 1, glm::value_ptr(cameraPos));

    glBindVertexArray(*VAO);

    glDrawArrays(GL_TRIANGLES, 0, fullVertexData->size() / 8);
}

void Model::mainDraw(Shader* shaderProg, GLuint* VAO, std::vector<GLfloat>* fullVertexData)
{
    
    glm::mat4 transformation_matrix = this->mainTrans();
    
    shaderProg->setMat4("transform", transformation_matrix);

    shaderProg->setMat4("projection", projection);

    shaderProg->setMat4("view", viewMatrix);
    shaderProg->setVec3("cameraPos", cameraPos);

    shaderProg->use();

    glBindVertexArray(*VAO);

    glDrawArrays(GL_TRIANGLES, 0, fullVertexData->size() / 8);
}


void Model::rotate(char axis, int direction)
{
    switch (direction) {
    case 0:
        switch (axis)
        {
        case 'x':
            this->rotX += 1;
            break;
        case 'y':
            this->rotY += 1;
            break;
        case 'z':
            this->rotZ += 1;
            break;
        default:
            break;
        }
        break;
    case 1:
        switch (axis)
        {
        case 'x':
            this->rotX -= 1;
            break;
        case 'y':
            this->rotY -= 1;
            break;
        case 'z':
            this->rotZ -= 1;
            break;
        default:
            break;
        }
    default:
        break;
    } 
}


glm::mat4 Model::mainTrans() 
{
    glm::mat4 transformation_matrix = glm::translate(this->identity_matrix4, glm::vec3(position.x, position.y, position.z));
    
    transformation_matrix = glm::scale(transformation_matrix, glm::vec3(scale.x, scale.y, scale.z));

    transformation_matrix = glm::rotate(transformation_matrix, glm::radians(rotX), glm::normalize(glm::vec3(1, 0, 0)));

    transformation_matrix = glm::rotate(transformation_matrix, glm::radians(rotY), glm::normalize(glm::vec3(0, 1, 0)));

    transformation_matrix = glm::rotate(
        transformation_matrix,
        glm::radians(rotZ),
        glm::normalize(glm::vec3(0, 0, 1))
    );
    return transformation_matrix;
}

void Model::setCamera(glm::mat4 projection, glm::vec3 cameraPos, glm::vec3 Front,glm::mat4 viewMatrix)
{
    this->projection = projection;
    this->cameraPos = cameraPos;
    this->Front = Front;
    
    this->viewMatrix = glm::lookAt(this->cameraPos, this->cameraPos + this->Front, this->WorldUp);
}

void Model::setTexture(Shader* shaderProg, GLuint* texture, const std::string& name)
{
    GLuint texAddress = glGetUniformLocation(shaderProg->getID(), name.c_str());
    glBindTexture(GL_TEXTURE_2D, *texture);
    glUniform1i(texAddress, 0);
}

void Model::setPosition(glm::vec3 newPos)
{
    this->position = newPos;
}

void Model::setScale(glm::vec3 newScale)
{
    this->scale = newScale;
}

glm::vec3 Model::getPosition()
{
    return this->position;
}

glm::vec3 Model::getPosition(bool fromMatrix)
{
    if (!fromMatrix)
    {
        return glm::vec3(0.f, 0.f, 0.f);
    }

    glm::mat4 transMatrix = this->mainTrans();

    glm::vec4 buffer = glm::vec4(this->position,1.0f) * transMatrix;
    glm::vec3 currentPos = { buffer.x * -10, buffer.y * 10, buffer.z * -10 };

    return currentPos;
}

