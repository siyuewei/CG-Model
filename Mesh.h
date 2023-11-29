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
    vector<unsigned int> collision_tra_indices; //存碰撞三角形三个顶点的indices
    vector<unsigned int> delete_indices;        //无重复顶点
    vector<unsigned int> delete_tri_indices;        //有重复顶点，三个一组存放三角形

    vector<Vertex> fragment_vertices; // 存储所有要飞出去的顶点
    vector<unsigned int> fragment_indices; 
    vector<glm::vec3> fragment_speed; //每个顶点飞出去的速度方向和大小

    vector<Vertex> not_draw; //debug用

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