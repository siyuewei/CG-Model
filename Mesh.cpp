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
    not_draw.clear();
    for (unsigned int i = 0; i < indices.size(); i += 3) {
        unsigned int ind1 = indices[i];
        unsigned int ind2 = indices[i + 1];
        unsigned int ind3 = indices[i + 2];


        if (delete_indices.empty()) {
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, reinterpret_cast<void*>(i * sizeof(unsigned int)));
        }
        else {
            // 检查当前三角形的顶点是否在 delete_indices 中
            if (std::find(delete_indices.begin(), delete_indices.end(), ind1) == delete_indices.end() &&
                std::find(delete_indices.begin(), delete_indices.end(), ind2) == delete_indices.end() &&
                std::find(delete_indices.begin(), delete_indices.end(), ind3) == delete_indices.end()) {
                // 如果不在 delete_indices 中，绘制这个三角形
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, reinterpret_cast<void*>(i * sizeof(unsigned int)));
            }
            else {
                /* std::cout << "Dont not draw : ("
                     << vertices[ind1].Position.x << ", "
                     << vertices[ind2].Position.y << ", "
                     << vertices[ind3].Position.z << "), ("
                     << vertices[ind1].Position.x << ", "
                     << vertices[ind2].Position.y<< ", "
                     << vertices[ind3].Position.z << "), ("
                     << vertices[ind1].Position.x << ", "
                     << vertices[ind2].Position.y << ", "
                     << vertices[ind3].Position.z << ")" << std::endl;*/
                not_draw.push_back(vertices[ind1]);
                not_draw.push_back(vertices[ind2]);
                not_draw.push_back(vertices[ind3]);
            }
        }
    }
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}



void Mesh::updateFragment(float deltatime,float minY)
{
    if (fragment_indices.empty()) {
        return;
    }
    //每帧每个三角形沿着法线方向飞跃，同时加重力加速度
    // 如果三角形碎片碰到了平面，那么三角形反弹，当
    for (unsigned int i = 0; i < fragment_indices.size(); i += 3) {
        Vertex& ver1 = fragment_vertices[fragment_indices[i]];
        Vertex& ver2 = fragment_vertices[fragment_indices[i + 1]];
        Vertex& ver3 = fragment_vertices[fragment_indices[i + 2]];

        glm::vec3& speed = fragment_speed[i / 3];

        //如果速度小于0.1，三角形掉在地上不动了
        if (glm::length(speed) < 0.01) {
            speed = glm::vec3(0, 0, 0);
            ver1.Position = glm::vec3(0, 0, 0);
            ver2.Position = glm::vec3(0, 0, 0);
            ver3.Position = glm::vec3(0, 0, 0);
        }

        //重力加速度
        if (glm::length(speed) != 0) {
            speed += glm::vec3(0.0, -0.2, 0.0) * deltatime;  //v+=at
        }

        ver1.Position += speed * deltatime;
        ver2.Position += speed * deltatime;
        ver3.Position += speed * deltatime;

        //如果有一个点掉在了平面上，整个三角形反弹,同时速度大小减少
        if ((ver1.Position.y <= minY || ver2.Position.y <= minY || ver3.Position.y <= minY) && glm::length(speed)!=0) {
            float friction = 0.6;
            glm::vec3 tmp = glm::vec3(-speed.x * friction, -speed.y * friction, -speed.z * friction);
            speed = tmp;
        }


    }
}

void Mesh::setSpeed(float speed)
{
    if (fragment_indices.empty()) {
        std::cout << "Fragment is not set, can not set speed" << std::endl;
        return;
    }
    for (unsigned int i = 0; i < fragment_indices.size(); i += 3) {
        Vertex& ver1 = fragment_vertices[fragment_indices[i]];
        Vertex& ver2 = fragment_vertices[fragment_indices[i + 1]];
        Vertex& ver3 = fragment_vertices[fragment_indices[i + 2]];

        //沿法线方向飞出
        glm::vec3 direction = glm::normalize(ver1.Normal + ver2.Normal + ver3.Normal);
        glm::vec3 nor_speed = glm::vec3(direction.x * speed * 70.0, direction.y * speed * 70.0, direction.z * speed * 70.0); //沿法线方向弹出去的速度
        glm::vec3 g_speed = glm::vec3(0, 0, 0);  //重力方向的速度
        glm::vec3 final_speed = nor_speed + g_speed;

        fragment_speed.push_back(final_speed);
    }
}

void Mesh::DrawFragment(Shader& shader)
{
    // 生成并绑定VAO
    glGenVertexArrays(1, &fragmentVAO);
    glGenBuffers(1, &fragmentVBO);
    glGenBuffers(1, &fragmentEBO);

    glBindVertexArray(fragmentVAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, fragmentVBO);
    glBufferData(GL_ARRAY_BUFFER, fragment_vertices.size() * sizeof(Vertex), &fragment_vertices[0], GL_STATIC_DRAW);

  /*  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, fragment_indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);*/

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

    glBindVertexArray(fragmentVAO);
    //glDrawElements(GL_TRIANGLES, fragment_indices.size(), GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, fragment_vertices.size());
    glBindVertexArray(0);

}

void Mesh::generateFragment()
{
    for (unsigned int i = 0; i < delete_tri_indices.size(); i++) {
        Vertex vertex = this->vertices[delete_tri_indices[i]];
        fragment_vertices.push_back(vertex);
        fragment_indices.push_back(i);
    }
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

void Mesh::findDeleteTraingle()
{
    if (collision_tra_indices.size() == 0) {
        std::cout << "ERROR: COLLISION INDICE IS NOT SET" << std::endl;
        return;
    }
    findNeighbors(collision_tra_indices[0], collision_tra_indices[1], collision_tra_indices[2]);

}
