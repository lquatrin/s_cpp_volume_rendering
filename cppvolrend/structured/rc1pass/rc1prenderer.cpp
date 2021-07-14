#include "../../defines.h"
#include "rc1prenderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include <volvis_utils/camera.h>

#include <volvis_utils/utils.h>

#ifndef DEGREE_TO_RADIANS
  #define DEGREE_TO_RADIANS(s) (s * (glm::pi<double>() / 180.0))
#endif

RayCasting1Pass::RayCasting1Pass ()
  : m_glsl_transfer_function(nullptr)
  , cp_shader_rendering(nullptr)
  , m_u_step_size(0.5f)
  , m_apply_gradient_shading(true)
  , m_rdr_frame_to_screen(CPPVOLREND_DIR)
{
  SetBuilt(false);
  SetOutdated();
}

RayCasting1Pass::~RayCasting1Pass ()
{
  Clean();
}

void RayCasting1Pass::SetExternalResources (vis::DataManager* data_mgr, vis::RenderingParameters* rdr_prm)
{
  m_ext_data_manager = data_mgr;
  m_ext_rendering_parameters = rdr_prm;
}

void RayCasting1Pass::Clean ()
{
  if (m_glsl_transfer_function) delete m_glsl_transfer_function;
  m_glsl_transfer_function = nullptr;

  DestroyRenderingPass();

  m_rdr_frame_to_screen.Clean();
  SetBuilt(false);
}

void RayCasting1Pass::ReloadShaders ()
{
  cp_shader_rendering->Reload();
  m_rdr_frame_to_screen.ClearShaders();
}

bool RayCasting1Pass::Init (int swidth, int sheight)
{
  if (IsBuilt()) Clean();

  if (m_ext_data_manager->GetCurrentVolumeTexture() == nullptr) return false;
  m_glsl_transfer_function = m_ext_data_manager->GetCurrentTransferFunction()->GenerateTexture_1D_RGBt();
  // Create Rendering Buffers and Shaders
  CreateRenderingPass();
  gl::ExitOnGLError("RayCasting1Pass: Error on Preparing Models and Shaders");
  
  // estimate initial integration step
  glm::dvec3 sv = m_ext_data_manager->GetCurrentStructuredVolume()->GetScale();
  m_u_step_size = float((0.5f / glm::sqrt(3.0f)) * glm::sqrt(sv.x * sv.x + sv.y * sv.y + sv.z * sv.z));

  Reshape(swidth, sheight);

  SetBuilt(true);
  SetOutdated();
  return true;
}

bool RayCasting1Pass::Update (vis::Camera* camera)
{
  cp_shader_rendering->Bind();

  cp_shader_rendering->RecomputeNumberOfGroups(m_ext_rendering_parameters->GetScreenWidth(),
                                               m_ext_rendering_parameters->GetScreenHeight(), 0);

  cp_shader_rendering->SetUniform("CameraEye", camera->GetEye());
  cp_shader_rendering->BindUniform("CameraEye");

  cp_shader_rendering->SetUniform("u_CameraLookAt", camera->LookAt());
  cp_shader_rendering->BindUniform("u_CameraLookAt");

  cp_shader_rendering->SetUniform("ProjectionMatrix", camera->Projection());
  cp_shader_rendering->BindUniform("ProjectionMatrix");

  cp_shader_rendering->SetUniform("u_TanCameraFovY", (float)tan(DEGREE_TO_RADIANS(camera->GetFovY()) / 2.0));
  cp_shader_rendering->BindUniform("u_TanCameraFovY");

  cp_shader_rendering->SetUniform("u_CameraAspectRatio", camera->GetAspectRatio());
  cp_shader_rendering->BindUniform("u_CameraAspectRatio");

  cp_shader_rendering->SetUniform("StepSize", m_u_step_size);
  cp_shader_rendering->BindUniform("StepSize");

  cp_shader_rendering->SetUniform("ApplyOcclusion", 1);
  cp_shader_rendering->BindUniform("ApplyOcclusion");

  cp_shader_rendering->SetUniform("ApplyShadow", 1);
  cp_shader_rendering->BindUniform("ApplyShadow");

  cp_shader_rendering->SetUniform("ApplyGradientPhongShading", (m_apply_gradient_shading && m_ext_data_manager->GetCurrentGradientTexture()) ? 1 : 0);
  cp_shader_rendering->BindUniform("ApplyGradientPhongShading");

  cp_shader_rendering->SetUniform("BlinnPhongKa", m_ext_rendering_parameters->GetBlinnPhongKambient());
  cp_shader_rendering->BindUniform("BlinnPhongKa");
  cp_shader_rendering->SetUniform("BlinnPhongKd", m_ext_rendering_parameters->GetBlinnPhongKdiffuse());
  cp_shader_rendering->BindUniform("BlinnPhongKd");
  cp_shader_rendering->SetUniform("BlinnPhongKs", m_ext_rendering_parameters->GetBlinnPhongKspecular());
  cp_shader_rendering->BindUniform("BlinnPhongKs");
  cp_shader_rendering->SetUniform("BlinnPhongShininess", m_ext_rendering_parameters->GetBlinnPhongNshininess());
  cp_shader_rendering->BindUniform("BlinnPhongShininess");

  cp_shader_rendering->SetUniform("BlinnPhongIspecular", m_ext_rendering_parameters->GetLightSourceSpecular());
  cp_shader_rendering->BindUniform("BlinnPhongIspecular");

  cp_shader_rendering->SetUniform("WorldEyePos", camera->GetEye());
  cp_shader_rendering->BindUniform("WorldEyePos");

  cp_shader_rendering->SetUniform("LightSourcePosition", m_ext_rendering_parameters->GetBlinnPhongLightingPosition());
  cp_shader_rendering->BindUniform("LightSourcePosition");

  cp_shader_rendering->BindUniforms();

  gl::Shader::Unbind();
  gl::ExitOnGLError("RayCasting1Pass: After Update.");
  return true;
}

void RayCasting1Pass::Redraw ()
{
  m_rdr_frame_to_screen.ClearTexture();

  cp_shader_rendering->Bind();
  m_rdr_frame_to_screen.BindImageTexture();

  cp_shader_rendering->Dispatch();
  gl::ComputeShader::Unbind();
 
  m_rdr_frame_to_screen.Draw();
}

void RayCasting1Pass::Reshape (int w, int h)
{
  m_rdr_frame_to_screen.UpdateScreenResolution(w, h);
  gl::ExitOnGLError("Error on Reshape (screen_output texture).");
  SetOutdated();
}


void RayCasting1Pass::PrepareRender (vis::Camera* camera)
{
  if (IsOutdated())
  {
    Update(camera);
    vr_outdated = false;
  }
}

void RayCasting1Pass::SetOutdated ()
{
  vr_outdated = true;
}

bool RayCasting1Pass::IsOutdated ()
{
  return vr_outdated;
}

bool RayCasting1Pass::IsBuilt ()
{
  return vr_built;
}

void RayCasting1Pass::SetBuilt (bool b_built)
{
  vr_built = b_built;
}

void RayCasting1Pass::CreateRenderingPass ()
{
  glm::vec3 vol_resolution = glm::vec3(m_ext_data_manager->GetCurrentStructuredVolume()->GetWidth() ,
                                       m_ext_data_manager->GetCurrentStructuredVolume()->GetHeight(),
                                       m_ext_data_manager->GetCurrentStructuredVolume()->GetDepth() );

  glm::vec3 vol_voxelsize = glm::vec3(m_ext_data_manager->GetCurrentStructuredVolume()->GetScaleX(),
                                      m_ext_data_manager->GetCurrentStructuredVolume()->GetScaleY(),
                                      m_ext_data_manager->GetCurrentStructuredVolume()->GetScaleZ());

  glm::vec3 vol_aabb = vol_resolution * vol_voxelsize;
 
  cp_shader_rendering = new gl::ComputeShader();
  cp_shader_rendering->AddShaderFile(CPPVOLREND_DIR"structured/_common_shaders/ray_bbox_intersection.comp");
  cp_shader_rendering->AddShaderFile(CPPVOLREND_DIR"structured/rc1pass/ray_marching_1p.comp");
  cp_shader_rendering->LoadAndLink();
  cp_shader_rendering->Bind();

  if (m_ext_data_manager->GetCurrentVolumeTexture())
    cp_shader_rendering->SetUniformTexture3D("TexVolume", m_ext_data_manager->GetCurrentVolumeTexture()->GetTextureID(), 1);
  if (m_glsl_transfer_function)
    cp_shader_rendering->SetUniformTexture1D("TexTransferFunc", m_glsl_transfer_function->GetTextureID(), 2);
  if (m_apply_gradient_shading && m_ext_data_manager->GetCurrentGradientTexture())
    cp_shader_rendering->SetUniformTexture3D("TexVolumeGradient", m_ext_data_manager->GetCurrentGradientTexture()->GetTextureID(), 3);

  cp_shader_rendering->SetUniform("VolumeGridResolution", vol_resolution);
  cp_shader_rendering->SetUniform("VolumeVoxelSize", vol_voxelsize);
  cp_shader_rendering->SetUniform("VolumeGridSize", vol_aabb);

  cp_shader_rendering->BindUniforms();
  cp_shader_rendering->Unbind();
}

void RayCasting1Pass::DestroyRenderingPass ()
{
  if (cp_shader_rendering) delete cp_shader_rendering;
  cp_shader_rendering = nullptr;

  gl::ExitOnGLError("Could not destroy shaders");
}

void RayCasting1Pass::RecreateRenderingPass ()
{
  DestroyRenderingPass();
  CreateRenderingPass();

  gl::ExitOnGLError("Could not recreate rendering pass");
}