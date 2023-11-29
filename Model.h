#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "shader.h"
#include "Box.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

struct edge {
    glm::vec3 start;
    glm::vec3 end;
    unsigned int start_indice;
    unsigned int end_indice;

    unsigned int count;

    edge(glm::vec3 start, glm::vec3 end, unsigned int start_indice, unsigned int end_indice)
        :start(start), end(end) ,start_indice(start_indice),end_indice(end_indice)
    {
        count = 1;
    }
};

class Model
{
public:
    // model data 
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh> meshes_out;
    vector<Mesh> meshes_in;
    Box box;

    string directory;
    bool gammaCorrection;
    float distance = 0.01;
    unsigned int collision_mesh_indice;//记录碰撞发生在哪个mesh上
    vector<unsigned int> delete_indices;
    vector<unsigned int> strip_conven_indices;
    unsigned int stripVAO;

    bool is_coll;

    // constructor, expects a filepath to a 3D model.
    Model(string const& path, bool gamma = false);

    // draws the model, and thus all its meshes
    void DrawOut(Shader& shader);
    void DrawIn(Shader& shader);
    void DrawBox(Shader& shader);
    void DrawStrip(Shader& shader);
    void DrawFragment(Shader& shader);

    void explosion(float ball_speed);
    void updateFragment(float deltatime);
private:
    vector<glm::vec3> strip_vertex;
    vector<unsigned int> strip_indices;
    vector<unsigned int> convexHull(vector<unsigned int> points);
    void addEdge(vector<edge>& edges, glm::vec3 start, glm::vec3 end, unsigned int start_indice, unsigned int end_indice);
    unsigned int findNextEdge(const vector<edge>& prei_edges, glm::vec3 end_indice);
    void setUpStrip();

    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const& path);
    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene);
    std::pair<Mesh, Mesh> processMesh(aiMesh* mesh, const aiScene* scene);

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};

#endif