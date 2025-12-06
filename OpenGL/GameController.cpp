#include "GameController.h"
#include "WindowController.h"
#include "ToolWindow.h"
#include "EngineTime.h"
#include "Skybox.h"
#include "PostProcessor.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    GameController* gc = static_cast<GameController*>(glfwGetWindowUserPointer(window));
    
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        if (gc->GetIt() == gc->GetEffectShaders().begin())
        {
            gc->GetIt() = --gc->GetEffectShaders().end();
        }
        else
        {
            gc->GetIt()--;
        }
        gc->GetProcessor()->SetShader(gc->GetIt()->first, gc->GetIt()->second);
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        if (++gc->GetIt() == gc->GetEffectShaders().end())
        {
            gc->GetIt() = gc->GetEffectShaders().begin();
        }
        gc->GetProcessor()->SetShader(gc->GetIt()->first, gc->GetIt()->second);
    }
}

void GameController::Initialize()
{
    GLFWwindow* window = WindowController::GetInstance().GetWindow();
    
    M_ASSERT(glewInit() == GLEW_OK, "Unable");
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    /*glCullFace(GL_BACK);
    glFrontFace(GL_CW);*/
    srand(time(0));

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    Load();
}

void GameController::RunGame()
{
    GLFWwindow* window = WindowController::GetInstance().GetWindow();
    
    Time::Instance().Initialize();

    it = effectShaders.begin();
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowUserPointer(window, this);

    do {
        Time::Instance().Update();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (postProcessor != nullptr) postProcessor->Start();

        if (skybox != nullptr)
        {
            camera->Rotate();
            glm::mat4 view = glm::mat4(glm::mat3(camera->GetView()));
            skybox->Render(camera->GetProjection() * view);
        }
        
        for (auto& light: lights)
        {
            light->Render(camera->GetProjection() * camera->GetView(), lights);
        }
        
        for (auto& mesh : meshes)
        {
            mesh->SetRotation(mesh->GetRotation() + Time::Instance().DeltaTime() * glm::vec3(0.0f, mesh->GetRotationRate(), 0.0f));
            mesh->Render(camera->GetProjection() * camera->GetView(), lights, meshCount);
        }

        if (postProcessor != nullptr) postProcessor->End();

        textController->RenderText(postProcessor->effect, 20, 60, 0.5f, {1.0f, 0.5f, 1.0f});

        glfwSwapBuffers(window);
        glfwPollEvents();

    } while (
        glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0
    );

    for (auto& mesh : meshes)
    {
        delete mesh;
    }

    for (auto& light: lights)
    {
        delete light;
    }

    for (auto& shader : shaders)
    {
        delete shader.second;
    }

    for (auto& font : fonts)
    {
        delete font.second;
    }

    if (textController != nullptr)
    {
        delete textController;
    }

    if (skybox != nullptr)
    {
        delete skybox;
    }

    if (postProcessor != nullptr)
    {
        delete postProcessor;
    }

    delete camera;
}

void GameController::Load()
{
#pragma region Settings
    const std::string str = "../Assets/settings.json";
    json::JSON document = LoadJson(str);
#pragma endregion

#pragma region Clear Color
    glm::vec3 ClearColor{ 0, 0, 0 };
    json::JSON& jsonClearColor = Get(document, "ClearColor");
    ClearColor.x = Get(jsonClearColor, "r").ToFloat();
    ClearColor.y = Get(jsonClearColor, "g").ToFloat();
    ClearColor.z = Get(jsonClearColor, "b").ToFloat();
    glClearColor(ClearColor.r, ClearColor.g, ClearColor.b, 0.0f);
#pragma endregion

#pragma region Camera    
    camera = new Camera(WindowController::GetInstance().GetResolution());
    if (document.hasKey("Camera"))
    {
        camera->Create(WindowController::GetInstance().GetResolution(), document["Camera"]);
    }    
#pragma endregion

#pragma region Shader
    json::JSON& shadersJSON = Get(document, "Shaders");
    for (auto& shaderJSON : shadersJSON.ArrayRange())
    {
        assert(shaderJSON.hasKey("name"));
        assert(shaderJSON.hasKey("vertex"));
        assert(shaderJSON.hasKey("fragment"));
        Shader* shaderColor = new Shader();
        shaderColor->LoadShaders(shaderJSON["vertex"].ToString().c_str(), shaderJSON["fragment"].ToString().c_str());
        shaders.emplace(shaderJSON["name"].ToString().c_str(), shaderColor);
    }
#pragma endregion

#pragma region Effect Shader
    json::JSON& effectShadersJSON = Get(document, "EffectShaders");
    for (auto& shaderJSON : effectShadersJSON.ArrayRange())
    {
        assert(shaderJSON.hasKey("name"));
        assert(shaderJSON.hasKey("vertex"));
        assert(shaderJSON.hasKey("fragment"));
        Shader* effectShaderColor = new Shader();
        effectShaderColor->LoadShaders(shaderJSON["vertex"].ToString().c_str(), shaderJSON["fragment"].ToString().c_str());
        effectShaders.emplace(shaderJSON["name"].ToString().c_str(), effectShaderColor);
    }
#pragma endregion

#pragma region Scene
    M_ASSERT(document.hasKey("DefaultFile"), "Settings requires a default file");
    std::string defaultFile = document["DefaultFile"].ToString();

    document = LoadJson(defaultFile);
    
    json::JSON& lightsJSON = Get(document, "Lights");
    for (auto& lightJSON : lightsJSON.ArrayRange())
    {
        Mesh* light = new Mesh();
        light->Create(lightJSON);
        light->SetCameraPosition(camera->GetPosition());
        lights.push_back(light);
    }

    json::JSON& meshesJSON = Get(document, "Meshes");
    for (auto& meshJSON : meshesJSON.ArrayRange())
    {
        Mesh* mesh = new Mesh();
        mesh->Create(meshJSON);
        mesh->SetCameraPosition(camera->GetPosition());
        meshes.push_back(mesh);
    }
#pragma endregion

#pragma region Skyboxes
    if (document.hasKey("Skybox"))
    {
        skybox = new Skybox();
        skybox->Create(document["Skybox"]);
    }
#pragma endregion 


#pragma region Fonts
    if (document.hasKey("Fonts"))
    {
        json::JSON& fontsJSON = document["Fonts"];
        for (auto& fontJSON : fontsJSON.ArrayRange())
        {
            M_ASSERT(fontJSON.hasKey("Name"), "Font requires a name");
            std::string fontName = fontJSON["Name"].ToString();

            M_ASSERT(fontJSON.hasKey("Font"), "Font requires a Font node");

            Font* font = new Font();
            font->Create(fontJSON["Font"]);
            fonts.emplace(fontName, font);
        }
    }
#pragma endregion

#pragma region TextController
    if (document.hasKey("TextController"))
    {
        textController = new TextController();
        textController->Create(document["TextController"]);
    }
#pragma endregion

#pragma region Post Processing
    if (document.hasKey("PostProcessor"))
    {
        postProcessor = new PostProcessor();
        postProcessor->Create(document["PostProcessor"]);
    }
#pragma endregion
}
