#include "headers.h"
#include "trianglemesh.h"
#include "camera.h"
#include "shaderprog.h"
#include "light.h"
#include "imagetexture.h"
#include "skybox.h"
#include "ui_manager.h"

// Deferred shading
#include "g_buffer.h"

// Maybe use a base class for all objects, so that they could transform in a unified way.

// Global variables.
int screenWidth = 600;
int screenHeight = 600;
// Triangle mesh.
std::vector<TriangleMesh*> model_list(1000, nullptr); // Index as model id, value as model pointer.
// Lights.
DirectionalLight* dirLight = nullptr;
glm::vec3 dirLightDirection = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 dirLightRadiance = glm::vec3(0.6f, 0.6f, 0.6f);
glm::vec3 pointLightPosition = glm::vec3(0.8f, 0.0f, 0.8f);
glm::vec3 pointLightIntensity = glm::vec3(0.5f, 0.1f, 0.1f);
glm::vec3 spotLightPosition = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 spotLightDirection = glm::vec3(0.0f, -1.0f, 0.0f);
glm::vec3 spotLightIntensity = glm::vec3(0.25f, 0.25f, 0.1f);
float spotLightCutoffStartInDegree = 30.0f;
float spotLightTotalWidthInDegree = 45.0f;
glm::vec3 ambientLight = glm::vec3(0.2f, 0.2f, 0.2f);
const int POINT_LIGHT_NUM = 1;
const int SPOT_LIGHT_NUM = 3;
std::vector<PointLight*> point_light_list(POINT_LIGHT_NUM);
std::vector<SpotLight*> spot_light_list(SPOT_LIGHT_NUM);
// Camera.
Camera* camera = nullptr;
glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 5.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float fovy = 30.0f;
float zNear = 0.1f;
float zFar = 1000.0f;
// Shader.
FillColorShaderProg* fillColorShader = nullptr;
PhongShadingDemoShaderProg* phongShadingShader = nullptr;
SkyboxShaderProg* skyboxShader = nullptr;
// UI.
const float lightMoveSpeed = 0.2f;
UIManager ui_manager;
// Skybox.
Skybox* skybox = nullptr;
// Deferred shading.
G_Buffer* g_buffer = nullptr;
GBufferShaderProg* g_buffer_shader = nullptr;
DeferredShadingShaderProg* deferred_shading_shader = nullptr;
LightPassShaderProg* light_pass_shader = nullptr;

// ScenePointLight (for visualization of a point light).
struct ScenePointLight
{
    ScenePointLight() {
        light = nullptr;
        worldMatrix = glm::mat4x4(1.0f);
        visColor = glm::vec3(1.0f, 1.0f, 1.0f);
    }
    PointLight* light;
    glm::mat4x4 worldMatrix;
    glm::vec3 visColor;
};
ScenePointLight pointLightObj;
ScenePointLight spotLightObj;

// Function prototypes.
int FindFreeModelID();
void ReleaseResources();
void RenderQuad();
// Callback functions.
void RenderSceneCB();
void ReshapeCB(int, int);
void ProcessSpecialKeysCB(int, int, int);
void ProcessKeysCB(unsigned char, int, int);
void SetupRenderState();
void LoadObjects(const std::string&);
void CreateCamera();
void CreateSkybox(const std::string, int, int, float);
void CreateShaderLib();

int FindFreeModelID() {
    for (int i = 0; i < model_list.size(); i++) {
        if (!model_list[i]) {
            return i;
        }
    }
    return -1; // No free ID found
}

void ReleaseResources()
{
    // Delete scene objects and lights.
	/*if (mesh != nullptr) { All meshes are handled by model_list
        delete mesh;
        mesh = nullptr;
    }*/
	ui_manager.ResetScene(); // This will delete all models in model_list.
    for (int i = 0; i < point_light_list.size(); i++) {
        if (point_light_list[i] != nullptr) {
            delete point_light_list[i];
            point_light_list[i] = nullptr;
        }
	}
    if (dirLight != nullptr) {
        delete dirLight;
        dirLight = nullptr;
    }
    for (int i = 0; i < spot_light_list.size(); i++) {
        if (spot_light_list[i] != nullptr) {
            delete spot_light_list[i];
            spot_light_list[i] = nullptr;
        }
    }
    // Delete camera.
    if (camera != nullptr) {
        delete camera;
        camera = nullptr;
    }
    // Delete shaders.
    if (fillColorShader != nullptr) {
        delete fillColorShader;
        fillColorShader = nullptr;
    }
    if (phongShadingShader != nullptr) {
        delete phongShadingShader;
        phongShadingShader = nullptr;
    }
    if (skyboxShader != nullptr) {
        delete skyboxShader;
        skyboxShader = nullptr;
    }
    if (g_buffer != nullptr) {
        delete g_buffer;
        g_buffer = nullptr;
	}
}

static float curObjRotationY = 30.0f;
const float rotStep = 0.002f;
void RenderSceneCB()
{
    if (glutGetWindow() != ui_manager.GetRenderWindow()) // avoid GLUI window from flashing
        return;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Render skybox. ----------------------------------------------------------------------------
    if (skybox != nullptr && ui_manager.GetUseSkybox()) {
        // -------------------------------------------------------
	    // Add your code to rotate the skybox.
        // -------------------------------------------------------
        skybox->Render(camera, skyboxShader);
    }
    // -------------------------------------------------------------------------------------------
	// Lights setup. -------------------------------------------------------------------------------
    glm::vec3 pointLightPositions[POINT_LIGHT_NUM];
	glm::vec3 pointLightIntensities[POINT_LIGHT_NUM];
    for (int i = 0; i < POINT_LIGHT_NUM; i++) {
        if (point_light_list[i] != nullptr) {
			pointLightPositions[i] = point_light_list[i]->GetPosition();
			pointLightIntensities[i] = point_light_list[i]->GetIntensity();
        }
	}
	glm::vec3 spotLightPositions[SPOT_LIGHT_NUM];
	glm::vec3 spotLightDirections[SPOT_LIGHT_NUM];
	glm::vec3 spotLightIntensities[SPOT_LIGHT_NUM];
	float spotLightCutoffStarts[SPOT_LIGHT_NUM];
	float spotLightTotalWidths[SPOT_LIGHT_NUM];
    for (int i = 0; i < SPOT_LIGHT_NUM; i++) {
        if (spot_light_list[i] != nullptr) {
            spotLightPositions[i] = spot_light_list[i]->GetPosition();
            spotLightDirections[i] = spot_light_list[i]->GetDirection();
            spotLightIntensities[i] = spot_light_list[i]->GetIntensity();
            spotLightCutoffStarts[i] = spot_light_list[i]->GetCutoffStart();
            spotLightTotalWidths[i] = spot_light_list[i]->GetTotalWidth();
        }
    }
	// -------------------------------------------------------------------------------------------
    TriangleMesh* pMesh;
    for (int i = 0; i < model_list.size(); i++) {
        if (model_list[i] != nullptr) {
			pMesh = model_list[i];
            // Update transform.
            // curObjRotationY += rotStep;
            // -------------------------------------------------------
            // Note: if you want to compute lighting in the View Space, 
            //       you might need to change the code below.
            // -------------------------------------------------------
            // Compute lighting in view space
			glm::mat4x4 worldMatrix = pMesh->GetWorldMatrix();
			glm::mat4x4 viewMatrix = camera->GetViewMatrix();
            glm::mat4x4 normalMatrix = glm::transpose(glm::inverse(viewMatrix * worldMatrix)); 
            glm::mat4x4 MVP = camera->GetProjMatrix() * viewMatrix * worldMatrix;
            // -------------------------------------------------------
            // Add your rendering code here.

            std::vector<SubMesh> sub_meshes = pMesh->GetSubMeshes(); // Return by value.
            for (SubMesh sub_mesh : sub_meshes) {
				glm::vec3 Ka = sub_mesh.material->GetKa();
				glm::vec3 Kd = sub_mesh.material->GetKd();
				glm::vec3 Ks = sub_mesh.material->GetKs();
				float Ns = sub_mesh.material->GetNs();
                
                phongShadingShader->Bind();
				// Transformation matrices.
                glUniformMatrix4fv(phongShadingShader->GetLocMVP(), 1, GL_FALSE, glm::value_ptr(MVP));
				glUniformMatrix4fv(phongShadingShader->GetLocM(), 1, GL_FALSE, glm::value_ptr(worldMatrix));
                glUniformMatrix4fv(phongShadingShader->GetLocV(), 1, GL_FALSE, glm::value_ptr(viewMatrix));
                glUniformMatrix4fv(phongShadingShader->GetLocNM(), 1, GL_FALSE, glm::value_ptr(normalMatrix));
                // Material properties.
				glUniform3fv(phongShadingShader->GetLocKa(), 1, glm::value_ptr(Ka));
				glUniform3fv(phongShadingShader->GetLocKd(), 1, glm::value_ptr(Kd));
				glUniform3fv(phongShadingShader->GetLocKs(), 1, glm::value_ptr(Ks));
				glUniform1f(phongShadingShader->GetLocNs(), Ns);
 
                sub_mesh.material->GetMapKd()->Bind(GL_TEXTURE0);
                glUniform1i(phongShadingShader->GetLocMapKd(), 0);

				// Light data.
				glUniform3fv(phongShadingShader->GetLocAmbientLight(), 1, glm::value_ptr(ambientLight));
                if (dirLight != nullptr) {
                    glUniform3fv(phongShadingShader->GetLocDirLightDir(), 1, glm::value_ptr(dirLight->GetDirection()));
                    glUniform3fv(phongShadingShader->GetLocDirLightRadiance(), 1, glm::value_ptr(dirLight->GetRadiance()));
                }
                glUniform3fv(phongShadingShader->GetLocPointLightPos(), POINT_LIGHT_NUM, glm::value_ptr(pointLightPositions[0]));
                glUniform3fv(phongShadingShader->GetLocPointLightIntensity(), POINT_LIGHT_NUM, glm::value_ptr(pointLightIntensities[0]));

                glUniform3fv(phongShadingShader->GetLocSpotLightPos(), SPOT_LIGHT_NUM, glm::value_ptr(spotLightPositions[0]));
				glUniform3fv(phongShadingShader->GetLocSpotLightIntensity(), SPOT_LIGHT_NUM, glm::value_ptr(spotLightIntensities[0]));
				glUniform3fv(phongShadingShader->GetLocSpotLightDir(), SPOT_LIGHT_NUM, glm::value_ptr(spotLightDirections[0]));
                glUniform1fv(phongShadingShader->GetLocSpotLightCutoffStart(), SPOT_LIGHT_NUM, spotLightCutoffStarts);
                glUniform1fv(phongShadingShader->GetLocSpotLightTotalWidth(), SPOT_LIGHT_NUM, spotLightTotalWidths);
                pMesh->Render(sub_mesh);

                phongShadingShader->UnBind();
            }
            // -------------------------------------------------------
        }
	}

    // -------------------------------------------------------------------------------------------
    // Visualize the light with fill color. ------------------------------------------------------
    if (ui_manager.GetShowLight()) {
        for (PointLight* pointLight : point_light_list) {
            if (pointLight != nullptr) {
                glm::mat4x4 T = glm::translate(glm::mat4x4(1.0f), pointLight->GetPosition());
                pointLightObj.worldMatrix = T;
                glm::mat4x4 MVP = camera->GetProjMatrix() * camera->GetViewMatrix() * pointLightObj.worldMatrix;
                fillColorShader->Bind();
                glUniformMatrix4fv(fillColorShader->GetLocMVP(), 1, GL_FALSE, glm::value_ptr(MVP));
                glUniform3fv(fillColorShader->GetLocFillColor(), 1, glm::value_ptr(glm::normalize(pointLight->GetIntensity())));
                // Render the point light.
                pointLight->Draw();
                fillColorShader->UnBind();
            }
        }
        for (SpotLight* spotLight : spot_light_list) {
            if (spotLight != nullptr) {
                glm::mat4x4 T = glm::translate(glm::mat4x4(1.0f), spotLight->GetPosition());
                spotLightObj.worldMatrix = T;
                glm::mat4x4 MVP = camera->GetProjMatrix() * camera->GetViewMatrix() * spotLightObj.worldMatrix;
                fillColorShader->Bind();
                glUniformMatrix4fv(fillColorShader->GetLocMVP(), 1, GL_FALSE, glm::value_ptr(MVP));
                glUniform3fv(fillColorShader->GetLocFillColor(), 1, glm::value_ptr(glm::normalize(spotLight->GetIntensity())));
                // Render the spot light.
                spotLight->Draw();
                fillColorShader->UnBind();
            }
        }
    }
    // -------------------------------------------------------------------------------------------

    glutSwapBuffers();
}

void DeferredRenderSceneCB() {
    if (glutGetWindow() != ui_manager.GetRenderWindow()) // avoid GLUI window from flashing
        return;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Lights setup. -------------------------------------------------------------------------------
    glm::vec3 pointLightPositions[POINT_LIGHT_NUM];
    glm::vec3 pointLightIntensities[POINT_LIGHT_NUM];
    for (int i = 0; i < POINT_LIGHT_NUM; i++) {
        if (point_light_list[i] != nullptr) {
            pointLightPositions[i] = point_light_list[i]->GetPosition();
            pointLightIntensities[i] = point_light_list[i]->GetIntensity();
        }
    }
    glm::vec3 spotLightPositions[SPOT_LIGHT_NUM];
    glm::vec3 spotLightDirections[SPOT_LIGHT_NUM];
    glm::vec3 spotLightIntensities[SPOT_LIGHT_NUM];
    float spotLightCutoffStarts[SPOT_LIGHT_NUM];
    float spotLightTotalWidths[SPOT_LIGHT_NUM];
    for (int i = 0; i < SPOT_LIGHT_NUM; i++) {
        if (spot_light_list[i] != nullptr) {
            spotLightPositions[i] = spot_light_list[i]->GetPosition();
            spotLightDirections[i] = spot_light_list[i]->GetDirection();
            spotLightIntensities[i] = spot_light_list[i]->GetIntensity();
            spotLightCutoffStarts[i] = spot_light_list[i]->GetCutoffStart();
            spotLightTotalWidths[i] = spot_light_list[i]->GetTotalWidth();
        }
    }
    // 1. geometry pass: render scene's geometry/color data into gbuffer
    // -----------------------------------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, g_buffer->GetGBuffer());
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    g_buffer_shader->Bind();

    TriangleMesh* pMesh;
    for (int i = 0; i < model_list.size(); i++) {
        if (model_list[i] != nullptr) {
            pMesh = model_list[i];
            glm::mat4x4 worldMatrix = pMesh->GetWorldMatrix();
            glm::mat4x4 viewMatrix = camera->GetViewMatrix();
			glm::mat4x4 projectionMatrix = camera->GetProjMatrix();
            glm::mat4x4 normalMatrix = glm::transpose(glm::inverse(viewMatrix * worldMatrix));
            glm::mat4x4 MVP = camera->GetProjMatrix() * viewMatrix * worldMatrix;

            glUniformMatrix4fv(g_buffer_shader->GetLocM(), 1, GL_FALSE, glm::value_ptr(worldMatrix));
            glUniformMatrix4fv(g_buffer_shader->GetLocV(), 1, GL_FALSE, glm::value_ptr(viewMatrix));
            glUniformMatrix4fv(g_buffer_shader->GetLocP(), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
            glUniformMatrix4fv(g_buffer_shader->GetLocNM(), 1, GL_FALSE, glm::value_ptr(normalMatrix));
			glUniformMatrix4fv(g_buffer_shader->GetLocMVP(), 1, GL_FALSE, glm::value_ptr(MVP));
            // -------------------------------------------------------
            std::vector<SubMesh> sub_meshes = pMesh->GetSubMeshes(); // Return by value.
            for (SubMesh sub_mesh : sub_meshes) {
                sub_mesh.material->GetMapKd()->Bind(GL_TEXTURE0);
                glUniform1i(g_buffer_shader->GetLocTexDiffuse(), 0);
                glUniform1f(g_buffer_shader->GetLocKs(), sub_mesh.material->GetKs().r);
                pMesh->Render(sub_mesh);
            }
        }
    }
	g_buffer_shader->UnBind();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
    // -----------------------------------------------------------------------------------------------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    deferred_shading_shader->Bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_buffer->GetGPosition());
	glUniform1i(deferred_shading_shader->GetLocGPosition(), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, g_buffer->GetGNormal());
	glUniform1i(deferred_shading_shader->GetLocGNormal(), 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, g_buffer->GetGAlbedoSpec());
	glUniform1i(deferred_shading_shader->GetLocGAlbedoSpec(), 2);
    // send light relevant uniforms
    if (dirLight != nullptr) {
        glUniform3fv(deferred_shading_shader->GetLocDirLightDir(), 1, glm::value_ptr(dirLight->GetDirection()));
        glUniform3fv(deferred_shading_shader->GetLocDirLightRadiance(), 1, glm::value_ptr(dirLight->GetRadiance()));
    }
    glUniform3fv(deferred_shading_shader->GetLocPointLightPos(), POINT_LIGHT_NUM, glm::value_ptr(pointLightPositions[0]));
    glUniform3fv(deferred_shading_shader->GetLocPointLightIntensity(), POINT_LIGHT_NUM, glm::value_ptr(pointLightIntensities[0]));

    glUniform3fv(deferred_shading_shader->GetLocSpotLightPos(), SPOT_LIGHT_NUM, glm::value_ptr(spotLightPositions[0]));
    glUniform3fv(deferred_shading_shader->GetLocSpotLightIntensity(), SPOT_LIGHT_NUM, glm::value_ptr(spotLightIntensities[0]));
    glUniform3fv(deferred_shading_shader->GetLocSpotLightDir(), SPOT_LIGHT_NUM, glm::value_ptr(spotLightDirections[0]));
    glUniform1fv(deferred_shading_shader->GetLocSpotLightCutoffStart(), SPOT_LIGHT_NUM, spotLightCutoffStarts);
    glUniform1fv(deferred_shading_shader->GetLocSpotLightTotalWidth(), SPOT_LIGHT_NUM, spotLightTotalWidths);
    
    glm::mat4x4 viewMatrix = camera->GetViewMatrix();
	glUniformMatrix4fv(deferred_shading_shader->GetLocV(), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    // finally render quad
    RenderQuad();
    deferred_shading_shader->UnBind();
    // -------------------------------------------------------
    // 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
    // ----------------------------------------------------------------------------------
    glBindFramebuffer(GL_READ_FRAMEBUFFER, g_buffer->GetGBuffer());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
    // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
    // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
    // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
    glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// -------------------------------------------------------------------------------------------
    // 3. render lights on top of scene
	// -----------------------------
	//light_pass_shader->Bind();
  
    // Render skybox. ----------------------------------------------------------------------------
    if (skybox != nullptr && ui_manager.GetUseSkybox()) {
        // -------------------------------------------------------
        // Add your code to rotate the skybox.
        // -------------------------------------------------------
        skybox->Render(camera, skyboxShader);
    }
    // -------------------------------------------------------------------------------------------
    // Visualize the light with fill color. ------------------------------------------------------
    if (ui_manager.GetShowLight()) {
        for (PointLight* pointLight : point_light_list) {
            if (pointLight != nullptr) {
                glm::mat4x4 T = glm::translate(glm::mat4x4(1.0f), pointLight->GetPosition());
                pointLightObj.worldMatrix = T;
                glm::mat4x4 MVP = camera->GetProjMatrix() * camera->GetViewMatrix() * pointLightObj.worldMatrix;
                fillColorShader->Bind();
                glUniformMatrix4fv(fillColorShader->GetLocMVP(), 1, GL_FALSE, glm::value_ptr(MVP));
                glUniform3fv(fillColorShader->GetLocFillColor(), 1, glm::value_ptr(glm::normalize(pointLight->GetIntensity())));
                // Render the point light.
                pointLight->Draw();
                fillColorShader->UnBind();
            }
        }
        for (SpotLight* spotLight : spot_light_list) {
            if (spotLight != nullptr) {
                glm::mat4x4 T = glm::translate(glm::mat4x4(1.0f), spotLight->GetPosition());
                spotLightObj.worldMatrix = T;
                glm::mat4x4 MVP = camera->GetProjMatrix() * camera->GetViewMatrix() * spotLightObj.worldMatrix;
                fillColorShader->Bind();
                glUniformMatrix4fv(fillColorShader->GetLocMVP(), 1, GL_FALSE, glm::value_ptr(MVP));
                glUniform3fv(fillColorShader->GetLocFillColor(), 1, glm::value_ptr(glm::normalize(spotLight->GetIntensity())));
                // Render the spot light.
                spotLight->Draw();
                fillColorShader->UnBind();
            }
        }
    }
	// -------------------------------------------------------------------------------------------
    glutSwapBuffers();
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void RenderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
// ------------------------------------------------------------------------------------------------

void ReshapeCB(int w, int h)
{
    // Update viewport.
    screenWidth = w;
    screenHeight = h;
    glViewport(0, 0, screenWidth, screenHeight);
    // Adjust camera and projection.
    float aspectRatio = (float)screenWidth / (float)screenHeight;
    camera->UpdateProjection(fovy, aspectRatio, zNear, zFar);
}

void ProcessSpecialKeysCB(int key, int x, int y)
{
    // Handle special (functional) keyboard inputs such as F1, spacebar, page up, etc. 
    switch (key) {
        // Rendering mode.
    case GLUT_KEY_F1:
        // Render with point mode.
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        break;
    case GLUT_KEY_F2:
        // Render with line mode.
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case GLUT_KEY_F3:
        // Render with fill mode.
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
	default:
		break;
    }
}

void ProcessKeysCB(unsigned char key, int x, int y)
{
    // Handle other keyboard inputs those are not defined as special keys.
    if (key == 27) {
        // Release memory allocation if needed.
        ReleaseResources();
        exit(0);
    }
}

void SetupRenderState()
{
    glEnable(GL_DEPTH_TEST);

    glm::vec4 clearColor = glm::vec4(0.44f, 0.57f, 0.75f, 1.00f);
    glClearColor(
        (GLclampf)(clearColor.r), 
        (GLclampf)(clearColor.g), 
        (GLclampf)(clearColor.b), 
        (GLclampf)(clearColor.a)
    );
}

void LoadObjects(const std::string& modelPath)
{
    // -------------------------------------------------------
	// Note: you can change the code below if you want to load
    //       the model dynamically.
	// -------------------------------------------------------
    
    int model_id = FindFreeModelID();
    TriangleMesh* mesh = new TriangleMesh();
    mesh->LoadFromFile(modelPath, true);
    mesh->ShowInfo();
	mesh->SetModelID(model_id);
    mesh->CreateVertexBuffer();
    mesh->CreateIndexBuffer();
	model_list[mesh->GetModelID()] = mesh;
    ui_manager.AddOrDeleteModel(false, mesh->GetModelID(), modelPath);

	mesh = nullptr; // Let model_list manage the model memory.
}

void CreateLights()
{
    // Create a directional light.
    dirLight = new DirectionalLight(dirLightDirection, dirLightRadiance);
    // Create a point light.
    PointLight* pointLight = new PointLight(pointLightPosition, pointLightIntensity);
    point_light_list[0] = pointLight;
    pointLightObj.light = pointLight;
    pointLightObj.visColor = glm::normalize((pointLightObj.light)->GetIntensity());
    // Create a spot light.
    SpotLight* spotLight = new SpotLight(spotLightPosition, spotLightIntensity, spotLightDirection, 
            spotLightCutoffStartInDegree, spotLightTotalWidthInDegree);
    spot_light_list[0] = spotLight;
    spotLightObj.light = spotLight;
    spotLightObj.visColor = glm::normalize((spotLightObj.light)->GetIntensity());

    spotLight = new SpotLight(spotLightPosition, glm::vec3(0.0, 1.0, 0.0), spotLightDirection,
            spotLightCutoffStartInDegree, spotLightTotalWidthInDegree);
    spot_light_list[1] = spotLight;
    spotLight = new SpotLight(spotLightPosition, glm::vec3(0.0, 0.0, 1.0), spotLightDirection,
        spotLightCutoffStartInDegree, spotLightTotalWidthInDegree);
    spot_light_list[2] = spotLight;
}

void CreateCamera()
{
    // Create a camera and update view and proj matrices.
    camera = new Camera((float)screenWidth / (float)screenHeight);
    camera->UpdateView(cameraPos, cameraTarget, cameraUp);
    float aspectRatio = (float)screenWidth / (float)screenHeight;
    camera->UpdateProjection(fovy, aspectRatio, zNear, zFar);
}

void CreateSkybox(const std::string texFilePath, int numSlices = 36, int numStacks = 18, float radius = 50.0f)
{
    // -------------------------------------------------------
	// Note: you can change the code below if you want to change
    //       the skybox texture dynamically.
	// -------------------------------------------------------
    if (skybox != nullptr) {
        delete skybox;
        skybox = nullptr;
	}
    skybox = new Skybox(texFilePath, numSlices, numStacks, radius);
	ui_manager.SetSkybox(skybox);
}

void CreateShaderLib()
{
    fillColorShader = new FillColorShaderProg();
    if (!fillColorShader->LoadFromFiles("shaders/fixed_color.vs", "shaders/fixed_color.fs"))
        exit(1);

    phongShadingShader = new PhongShadingDemoShaderProg();
    if (!phongShadingShader->LoadFromFiles("shaders/phong_shading_demo.vs", "shaders/phong_shading_demo.fs"))
        exit(1);

    skyboxShader = new SkyboxShaderProg();
    if (!skyboxShader->LoadFromFiles("shaders/skybox.vs", "shaders/skybox.fs"))
        exit(1);
	g_buffer_shader = new GBufferShaderProg();
    if (!g_buffer_shader->LoadFromFiles("shaders/g_buffer.vs", "shaders/g_buffer.fs"))
		exit(1);
    deferred_shading_shader = new DeferredShadingShaderProg();
    if (!deferred_shading_shader->LoadFromFiles("shaders/deferred_shading.vs", "shaders/deferred_shading.fs"))
		exit(1);
	light_pass_shader = new LightPassShaderProg();
    if (!light_pass_shader->LoadFromFiles("shaders/deferred_light_box.vs", "shaders/deferred_light_box.fs"))
		exit(1);
}

int main(int argc, char** argv)
{
    // Setting window properties.
    glutInit(&argc, argv);
    glutSetOption(GLUT_MULTISAMPLE, 4);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(100, 100);
    int render_window = glutCreateWindow("Lighting and Texturing");

    // Initialize GLEW.
    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        std::cerr << "GLEW initialization error: " 
                  << glewGetErrorString(res) << std::endl;
        return 1;
    }

    // Initialization.
    SetupRenderState();
	g_buffer = new G_Buffer(screenWidth, screenHeight);
    CreateLights();
    CreateCamera();
    CreateSkybox("textures/moonlit_golf.jpg");
    CreateShaderLib();
	ui_manager.SetModelList(&model_list);
	ui_manager.SetPointLightList(&point_light_list); 
	ui_manager.SetSpotLightList(&spot_light_list);
	ui_manager.SetSkybox(skybox);
    ui_manager.InitGLUI(render_window, screenWidth);
    for(int i = 0; i < POINT_LIGHT_NUM; i++) {
		ui_manager.AddOrDeleteLight(false, i, 0); // Point light
	}
    for (int i = 0; i < SPOT_LIGHT_NUM; i++) {
        ui_manager.AddOrDeleteLight(false, i, 1); // Spot light
    }
    // Register callback functions.
    glutDisplayFunc(DeferredRenderSceneCB);
    glutIdleFunc(DeferredRenderSceneCB);
    glutReshapeFunc(ReshapeCB);
    glutSpecialFunc(ProcessSpecialKeysCB);
    glutKeyboardFunc(ProcessKeysCB);

    // Start rendering loop.
    glutMainLoop();

    return 0;
}
