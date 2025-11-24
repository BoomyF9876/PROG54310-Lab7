#include "Skybox.h"
#include "Shader.h"
#include "GameController.h"
#include "OBJ_Loader.h"

Skybox::~Skybox()
{
	if (vertexBuffer != 0)
	{
		glDeleteBuffers(1, &vertexBuffer);
	}
	delete texture;
}

void Skybox::Create(json::JSON& jsonData)
{
    M_ASSERT(jsonData.hasKey("Shader"), "Shader is required");
    shader = GameController::GetInstance().GetShader(jsonData["Shader"].ToString().c_str());

    std::string model;
    M_ASSERT(jsonData.hasKey("Model"), "A model is required");
    model = jsonData["Model"].ToString();

    std::vector<std::string> _faces;
    M_ASSERT(jsonData.hasKey("Faces"), "Faces are required");
    for (auto& faceJSON : jsonData["Faces"].ArrayRange())
    {
        _faces.push_back(faceJSON.ToString());
    }

    objl::Loader Loader; // Initialize Loader
    M_ASSERT(Loader.LoadFile(model) == true, "Failed to load mesh."); // Load .obj File

    for (unsigned int i = 0; i < Loader.LoadedMeshes.size(); i++)
    {
        objl::Mesh curMesh = Loader.LoadedMeshes[i];
        for (unsigned int j = 0; j < curMesh.Vertices.size(); j++)
        {
            vertexData.push_back(curMesh.Vertices[j].Position.X);
            vertexData.push_back(curMesh.Vertices[j].Position.Y);
            vertexData.push_back(curMesh.Vertices[j].Position.Z);
        }
    }

    texture = new Texture();
    texture->LoadCubemap(_faces);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);
}

void Skybox::SetShaderVariables(glm::mat4 _pv)
{
    shader->SetMat4("PV", _pv);
    shader->SetTextureSampler("skyboxTexture", GL_TEXTURE_CUBE_MAP, 0, texture->GetTexture());
}

void Skybox::BindAttributes()
{
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); // Bind the vertex buffer

    // attribute buffer : vertices
    glEnableVertexAttribArray(shader->GetAttrVertices());
    glVertexAttribPointer(
        shader->GetAttrVertices(),   // The attribute we want to configure
        3,                           // size (3 components)
        GL_FLOAT,                    // type
        GL_FALSE,                    // normalized?
        3 * sizeof(float),           // stride (3 floats per vertex definition)
        (void*)0                     // array buffer offset
    );
}

void Skybox::Render(glm::mat4 _pv)
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glUseProgram(shader->GetProgramID());
    SetShaderVariables(_pv);
    BindAttributes();
    glDrawArrays(GL_TRIANGLES, 0, vertexData.size());
    glDisableVertexAttribArray(shader->GetAttrNormals());

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}
