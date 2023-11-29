#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh {
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;
    unsigned int fragmentVAO;
    vector<unsigned int> collision_tra_indices; //����ײ���������������indices
    vector<unsigned int> delete_indices;        //���ظ�����
    vector<unsigned int> delete_tri_indices;        //���ظ����㣬����һ����������

    vector<Vertex> fragment_vertices; // �洢����Ҫ�ɳ�ȥ�Ķ���
    vector<unsigned int> fragment_indices; 
    vector<glm::vec3> fragment_speed; //ÿ������ɳ�ȥ���ٶȷ���ʹ�С

    vector<Vertex> not_draw; //debug��

    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);

    // render the mesh
    void Draw(Shader& shader);
    void DrawFragment(Shader& shader);

    void generateFragment();
    void updateFragment(float deltatime,float minY);
    void setSpeed(float speed);
    void findDeleteTraingle();

private:
    // render data 
    unsigned int VBO, EBO;
    unsigned int fragmentVBO, fragmentEBO;

    // initializes all the buffer objects/arrays
    void setupMesh();
    void sortVerticesCCW(glm::vec3& normal, unsigned int& ind1, unsigned int& ind2, unsigned int& ind3);
    void findNeighbors(unsigned int indices1, unsigned int indices2, unsigned int indices3);
};
#endif