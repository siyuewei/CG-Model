#include "Model.h"

Model::Model(string const& path, bool gamme) : gammaCorrection(gamme)
{
    collision_mesh_indice = -1;
    is_coll = false;
	loadModel(path);   
}

void Model::DrawOut(Shader& shader)
{
	for (unsigned int i = 0; i < meshes_out.size(); i++) {
        meshes_out[i].Draw(shader);
	}
}

void Model::DrawIn(Shader& shader)
{
    for (unsigned int i = 0; i < meshes_in.size(); i++) {
        meshes_in[i].Draw(shader);
    }
}

void Model::DrawBox(Shader& shader)
{
    //box.Draw(shader);
    box.DrawPlane(shader);
}

void Model::DrawStrip(Shader& shader)
{
    if (strip_conven_indices.empty()) {
        return;
    }

    glBindVertexArray(stripVAO);
    glDrawElements(GL_TRIANGLES, strip_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Model::DrawFragment(Shader& shader)
{
    meshes_out[collision_mesh_indice].DrawFragment(shader);
}

void Model::updateFragment(float deltatime)
{
    if (collision_mesh_indice == -1) {
        return;
    }
    meshes_out[collision_mesh_indice].updateFragment(deltatime, box.min_y);
}

void Model::setUpStrip()
{
    //按顺序取出strip_indices中的每个顶点，编号依次是0，1，2...
//内外表面之间的三角形条带满足关系：
//  1. 编号相同的顶点之间有一条线
//  2. 外表面中编号为i的顶点和内表面中编号为i+1的顶点之间有一条线
//      如果i是最后一个编号，那就和内表面中编号为0的顶点之间有一条线
//这样就可以得到所有的三角形进行绘制
    for (unsigned int i = 0; i < strip_conven_indices.size(); ++i) {
        strip_vertex.push_back(meshes_out[collision_mesh_indice].vertices[strip_conven_indices[i]].Position);
        strip_vertex.push_back(meshes_in[collision_mesh_indice].vertices[strip_conven_indices[i]].Position);
        if (i != strip_conven_indices.size() - 1) {
            //三角形1
            strip_indices.push_back(2 * i);
            strip_indices.push_back(2 * i + 1);
            strip_indices.push_back(2 * i + 3);

            //三角形2
            strip_indices.push_back(2 * i);
            strip_indices.push_back(2 * i + 2);
            strip_indices.push_back(2 * i + 3);
        }
    }
    //特殊处理最后一组
    strip_indices.push_back(2 * (strip_conven_indices.size() - 1));
    strip_indices.push_back(2 * (strip_conven_indices.size() - 1) + 1);
    strip_indices.push_back(1);

    strip_indices.push_back(2 * (strip_conven_indices.size() - 1));
    strip_indices.push_back(0);
    strip_indices.push_back(1);

    // 生成并绑定VAO
    unsigned int stripVBO, stripEBO;
    glGenVertexArrays(1, &stripVAO);
    glGenBuffers(1, &stripVBO);
    glGenBuffers(1, &stripEBO);

    glBindVertexArray(stripVAO);

    // 绑定顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, stripVBO);
    glBufferData(GL_ARRAY_BUFFER, strip_vertex.size() * sizeof(glm::vec3), &strip_vertex[0], GL_STATIC_DRAW);

    // 绑定索引数据
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, stripEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, strip_indices.size() * sizeof(unsigned int), &strip_indices[0], GL_STATIC_DRAW);

    // 设置顶点属性指针
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // 绑定结束
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Model::explosion(float ball_speed)
{
    is_coll = true;

    //清除内外表面碰撞的部分
    this->meshes_out[collision_mesh_indice].findDeleteTraingle();
    this->meshes_in[collision_mesh_indice].delete_indices = this->meshes_out[collision_mesh_indice].delete_indices;

    this->delete_indices = meshes_out[collision_mesh_indice].delete_tri_indices;
    //对delete_indices进行选择排序
    strip_conven_indices = convexHull(delete_indices);
    setUpStrip();

    //设置mesh中每个三角形的速度
    meshes_out[collision_mesh_indice].generateFragment();
    meshes_out[collision_mesh_indice].setSpeed(ball_speed);
}

vector<unsigned int> Model::convexHull(vector<unsigned int> indices)
{
    //1. 遍历所有三角形的三条边，放入edges里面
    //      如果边在edges里面已经存在，就只增加数量，否则新加一条边
    //      起点和终点相反的边算同一条边
    //2. 找到所有数量为1的边放入prei_edges里面
    //3. 从prei_edges[0]边开始，把start_indice,end_indice依次放入convexHullIndices里面
    //   然后在prei_edges里面找start_indice或end_indice为prei_edges[0]的end_indice的边，放入convexHullIndices里面
    //   直到prei_edges中的所有边都加入convexHullIndices
    vector<edge> edges;
    vector<edge> prei_edges;

    // 1. 遍历所有三角形的三条边，放入edges里面
    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int ind1 = indices[i];
        unsigned int ind2 = indices[i + 1];
        unsigned int ind3 = indices[i + 2];
        glm::vec3 pos1 = meshes_out[collision_mesh_indice].vertices[ind1].Position;
        glm::vec3 pos2 = meshes_out[collision_mesh_indice].vertices[ind2].Position;
        glm::vec3 pos3 = meshes_out[collision_mesh_indice].vertices[ind3].Position;


        // 添加边到edges
        addEdge(edges, pos1,pos2,  ind1, ind2);
        addEdge(edges, pos2,pos3,ind2, ind3);
        addEdge(edges,pos3,pos1, ind3, ind1);
    }

    // 2. 找到所有数量为1的边放入prei_edges里面
    for (const edge& e : edges) {
        if (e.count == 1) {
            prei_edges.push_back(e);
        }
    }

    vector<unsigned int> convexHullIndices;

    // 3. 构建凸包
    if (!prei_edges.empty()) {
        // 从prei_edges[0]边开始
        edge currentEdge = prei_edges[0];
        prei_edges.erase(prei_edges.begin());

        // 把start_indice, end_indice依次放入convexHullIndices里面
        convexHullIndices.push_back(currentEdge.start_indice);
        convexHullIndices.push_back(currentEdge.end_indice);

        // 在prei_edges里面找start_indice或end_indice为currentEdge的end_indice的边，放入convexHullIndices里面
        while (!prei_edges.empty()) {
            unsigned int nextIndex = findNextEdge(prei_edges, currentEdge.end);
            if (nextIndex != UINT_MAX) {
                currentEdge = prei_edges[nextIndex];
                prei_edges.erase(prei_edges.begin() + nextIndex);

                if (currentEdge.start_indice == convexHullIndices.back()) {
                    convexHullIndices.push_back(currentEdge.end_indice);
                }
                else {
                    convexHullIndices.push_back(currentEdge.start_indice);
                }
            }
            else {
                break;
            }
        }
    }

    return convexHullIndices;
}

void Model::addEdge(vector<edge>& edges, glm::vec3 start, glm::vec3 end, unsigned int start_indice, unsigned int end_indice)
{
    // 起点和终点相反的边算同一条边
    for (edge& e : edges) {
        if ((e.start == start && e.end == end) || (e.start == end && e.end == start)) {
            e.count++;
            return;
        }
    }

    // 边在edges里面不存在，新加一条边
    edges.emplace_back(start, end,start_indice,end_indice);
}

unsigned int Model::findNextEdge(const vector<edge>& prei_edges, glm::vec3 end)
{
    for (size_t i = 0; i < prei_edges.size(); ++i) {
        if (prei_edges[i].start == end || prei_edges[i].end == end) {
            return static_cast<unsigned int>(i);
        }
    }
    return UINT_MAX;
}

void Model::loadModel(string const& path)
{
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}
	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));

	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);

    //保存内表面
    string outputPath = path.substr(0, path.find_last_of('.')) + "_inner.obj";
    saveInnerSurfaceToObj(outputPath);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        std::pair<Mesh, Mesh> mesh_out_in = processMesh(mesh, scene);
		meshes_out.push_back(mesh_out_in.first);
        meshes_in.push_back(mesh_out_in.second);
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}

}

std::pair<Mesh,Mesh> Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    // data to fill
    vector<Vertex> vertices_out;
    vector<Vertex> vertices_in;
    vector<unsigned int> indices;
    vector<Texture> textures;

    float minX = 0;
    float maxX = 0;
    float minY = 0;
    float maxY = 0;
    float minZ = 0;
    float maxZ = 0;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex_out, vertex_in;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex_out.Normal = vector;
            vertex_in.Normal = vector;
        }
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex_out.Position = vector;
        vertex_in.Position = vertex_out.Position - distance * vertex_out.Normal;

        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex_out.TexCoords = vec;
            vertex_in.TexCoords = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex_out.Tangent = vector;
            vertex_in.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex_out.Tangent = vector;
            vertex_in.Tangent = vector;
        }
        else {
            vertex_out.TexCoords = glm::vec2(0.0f, 0.0f);
            vertex_in.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices_out.push_back(vertex_out);
        vertices_in.push_back(vertex_in);

        //计算包围盒
        if (vertex_out.Position.x > maxX) {
            maxX = vertex_out.Position.x;
        }
        else if (vertex_out.Position.x < minX){
            minX = vertex_out.Position.x;
        }
        if (vertex_out.Position.y > maxY) {
            maxY = vertex_out.Position.y;
        }
        else if (vertex_out.Position.y < minY) {
            minY = vertex_out.Position.y;
        }
        if (vertex_out.Position.z > maxZ) {
            maxZ = vertex_out.Position.z;
        }
        else if (vertex_out.Position.z < minZ) {
            minZ = vertex_out.Position.z;
        }
    }

    box = Box(minX, maxX, minY, maxY, minZ, maxZ);

    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // return a mesh object created from the extracted mesh data
    Mesh mesh_out = Mesh(vertices_out, indices, textures);
    Mesh mesh_in = Mesh(vertices_in, indices, textures);
    return std::pair<Mesh, Mesh>(mesh_out, mesh_in);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if (!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
        }
    }
    return textures;
}

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
{
    //std::cout << path << std::endl;
    string filename = string(path);
    filename = directory + "/"+ filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// 存储新生成的顶点和表面
void Model::saveInnerSurfaceToObj(const string& outputPath)
{
    ofstream outFile(outputPath);
    if (!outFile.is_open())
    {
        cout << "Failed to open output file: " << outputPath << endl;
        return;
    }

    // 写入顶点数据
    for (const auto& mesh : meshes_in)
    {
        for (const auto& vertex : mesh.vertices)
        {
            outFile << "v " << vertex.Position.x << " " << vertex.Position.y << " " << vertex.Position.z << endl;
        }
    }

    // 写入法线数据
    for (const auto& mesh : meshes_in)
    {
        for (const auto& vertex : mesh.vertices)
        {
            outFile << "vn " << vertex.Normal.x << " " << vertex.Normal.y << " " << vertex.Normal.z << endl;
        }
    }

    // 写入纹理坐标数据
    for (const auto& mesh : meshes_in)
    {
        for (const auto& vertex : mesh.vertices)
        {
            outFile << "vt " << vertex.TexCoords.x << " " << vertex.TexCoords.y << endl;
        }
    }

    // 写入面数据
    for (const auto& mesh : meshes_in)
    {
        for (size_t i = 0; i < mesh.indices.size(); i += 3)
        {
            outFile << "f " << mesh.indices[i] + 1 << "/" << mesh.indices[i] + 1 << "/" << mesh.indices[i] + 1 << " "
                << mesh.indices[i + 1] + 1 << "/" << mesh.indices[i + 1] + 1 << "/" << mesh.indices[i + 1] + 1 << " "
                << mesh.indices[i + 2] + 1 << "/" << mesh.indices[i + 2] + 1 << "/" << mesh.indices[i + 2] + 1 << endl;
        }
    }

    outFile.close();
    cout << "Inner surface saved to: " << outputPath << endl;
}