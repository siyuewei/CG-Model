#include "Mesh.h"
#include <algorithm>

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setupMesh();
}


void Mesh::setupMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
    glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader)
{
    // bind appropriate textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        string number;
        string name = textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++); // transfer unsigned int to string
        else if (name == "texture_normal")
            number = std::to_string(normalNr++); // transfer unsigned int to string
        else if (name == "texture_height")
            number = std::to_string(heightNr++); // transfer unsigned int to string

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // draw mesh
    glBindVertexArray(VAO);
    for (unsigned int i = 0; i < indices.size(); i += 3) {
        unsigned int ind1 = indices[i];
        unsigned int ind2 = indices[i + 1];
        unsigned int ind3 = indices[i + 2];

        // 检查当前三角形的顶点是否在 delete_indices 中
        if (std::find(delete_indices.begin(), delete_indices.end(), ind1) == delete_indices.end() &&
            std::find(delete_indices.begin(), delete_indices.end(), ind2) == delete_indices.end() &&
            std::find(delete_indices.begin(), delete_indices.end(), ind3) == delete_indices.end()) {
            // 如果不在 delete_indices 中，绘制这个三角形
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, reinterpret_cast<void*>(i * sizeof(unsigned int)));
        }
        
    }
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::sortVerticesCCW(glm::vec3& normal, unsigned int& ind1, unsigned int& ind2, unsigned int& ind3)
{
    // 获取三个顶点
    glm::vec3 v1 = vertices[ind1].Position;
    glm::vec3 v2 = vertices[ind2].Position;
    glm::vec3 v3 = vertices[ind3].Position;

    // 计算三个顶点相对于法线的法线方向（右手法则）
    glm::vec3 crossProduct = glm::cross(v2 - v1, v3 - v1);

    // 如果法线方向与顶点法线一致，则不需要调整顺序
    if (glm::dot(crossProduct, normal) >= 0.0f) {
        return;
    }

    // 否则，调整顺序
    std::swap(ind1, ind3);
}

void Mesh::findNeighbors(unsigned int indices1, unsigned int indices2, unsigned int indices3)
{
    //for (unsigned int i = 0; i < indices.size(); i += 3) {
    //    unsigned int ind1 = this->indices[i];
    //    unsigned int ind2 = this->indices[i + 1];
    //    unsigned int ind3 = this->indices[i + 2];

    //    // 检查当前三角形是否包含给定的顶点
    //    if ((ind1 == indices1 || ind1 == indices2 || ind1 == indices3) &&
    //        (ind2 == indices1 || ind2 == indices2 || ind2 == indices3) &&
    //        (ind3 == indices1 || ind3 == indices2 || ind3 == indices3)) {

    //        // 调整顶点顺序以保证与法线方向一致
    //        sortVerticesCCW(vertices[ind1].Normal, ind1, ind2, ind3);

    //        // 将当前三角形的顶点按顺序添加到 neighbors 中
    //        neighbors.push_back(ind1);
    //        neighbors.push_back(ind2);
    //        neighbors.push_back(ind3);
    //    }
    //}
     // 计算给定三个顶点组成的三角形的中心点
    glm::vec3 center = (vertices[indices1].Position + vertices[indices2].Position + vertices[indices3].Position) / 3.0f;
    float radius = 0.05f;

    for (unsigned int i = 0; i < vertices.size(); ++i) {
        float distance = glm::length(vertices[i].Position - center);
        if (distance <= radius) {
            delete_indices.push_back(i);
        }
    }
    for (unsigned int i = 0; i < indices.size(); i += 3) {
        unsigned int ind1 = this->indices[i];
        unsigned int ind2 = this->indices[i + 1];
        unsigned int ind3 = this->indices[i + 2];

        if (!(std::find(delete_indices.begin(), delete_indices.end(), ind1) == delete_indices.end() &&
            std::find(delete_indices.begin(), delete_indices.end(), ind2) == delete_indices.end() &&
            std::find(delete_indices.begin(), delete_indices.end(), ind3) == delete_indices.end())) {
            // 如果在 delete_indices 中，加入delete_tri_indices
            delete_tri_indices.push_back(ind1);
            delete_tri_indices.push_back(ind2);
            delete_tri_indices.push_back(ind3);
        }
    }
}

void Mesh::explosion()
{
    if (collision_tra_indices.size() == 0) {
        std::cout << "ERROR: COLLISION INDICE IS NOT SET" << std::endl;
        return;
    }
    findNeighbors(collision_tra_indices[0], collision_tra_indices[1], collision_tra_indices[2]);

}
