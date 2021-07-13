/**
 * datamanager.cpp
 *
 * Leonardo Quatrin Campagnolo
 * . campagnolo.lq@gmail.com
**/
#include <volvis_utils/datamanager.h>

#include <fstream>
#include <gl_utils/computeshader.h>
#include <vis_utils/defines.h>
#include <volvis_utils/utils.h>

#include <volvis_utils/reader.h>

#define _DATA_VOLUME_PATH "S:/github/s_cpp_volume_rendering/data/raw/Bonsai.1.256x256x256.raw"
#define _DATA_TRANSFER_FUNCTION "S:/github/s_cpp_volume_rendering/data/tf1dcp/bonsai_01.tf1d"

namespace vis
{
  DataManager::DataManager ()
    : curr_vr_volume(nullptr)
    , curr_vr_transferfunction(nullptr)
    , curr_gradient_comp_model(DataManager::STRUCTURED_GRADIENT_TYPE::COMPUTE_SHADER_SOBEL)
    , curr_gl_tex_structured_volume(nullptr)
    , curr_gl_tex_structured_gradient(nullptr)
  {
  }

  DataManager::~DataManager ()
  {
    DeleteVolumeData();
    DeleteTransferFunctionData();
  }

  void DataManager::ReadData ()
  {
    GenerateStructuredVolumeTexture();
    
    vis::TransferFunctionReader tfr;
    curr_vr_transferfunction = tfr.ReadTransferFunction(_DATA_TRANSFER_FUNCTION);
    curr_vr_transferfunction->SetName("transfer_function");
  }

  vis::GridVolume* DataManager::GetCurrentGridVolume ()
  {
    return curr_vr_volume;
  }

  vis::StructuredGridVolume* DataManager::GetCurrentStructuredVolume ()
  {
    return curr_vr_volume;
  }

  vis::TransferFunction* DataManager::GetCurrentTransferFunction ()
  {
    return curr_vr_transferfunction;
  }

  gl::Texture3D* DataManager::GetCurrentVolumeTexture ()
  {
    return curr_gl_tex_structured_volume;
  }

  gl::Texture3D* DataManager::GetCurrentGradientTexture ()
  {
    return curr_gl_tex_structured_gradient;
  }
  
  ////////////////////////////////////////////////////////////////////////
  // Private Methods
  ////////////////////////////////////////////////////////////////////////
  void DataManager::DeleteVolumeData ()
  {
    if (curr_vr_volume) delete curr_vr_volume;
    curr_vr_volume = nullptr;

    if (curr_gl_tex_structured_volume) delete curr_gl_tex_structured_volume;
    curr_gl_tex_structured_volume = nullptr;

    DeleteGradientData();
  }

  void DataManager::DeleteGradientData ()
  {
    if (curr_gl_tex_structured_gradient) delete curr_gl_tex_structured_gradient;
    curr_gl_tex_structured_gradient = nullptr;
  }

  void DataManager::DeleteTransferFunctionData ()
  {
    if (curr_vr_transferfunction) delete curr_vr_transferfunction;
    curr_vr_transferfunction = nullptr;
  }

  bool DataManager::GenerateStructuredVolumeTexture ()
  {
    // Read Volume
    vis::VolumeReader vr;
    curr_vr_volume = vr.ReadStructuredVolume(_DATA_VOLUME_PATH);
    curr_vr_volume->SetName("volume");

    // Generate Volume Texture
    curr_gl_tex_structured_volume = vis::GenerateRTexture(curr_vr_volume, 0, 0, 0, curr_vr_volume->GetWidth(),
      curr_vr_volume->GetHeight(), curr_vr_volume->GetDepth());

    // Generate gradient, if enabled
    GenerateStructuredGradientTexture();

    return true;
  }

  bool DataManager::GenerateStructuredGradientTexture ()
  {
    if (curr_gradient_comp_model == STRUCTURED_GRADIENT_TYPE::SOBEL_FELDMAN_FILTER)
    {
      curr_gl_tex_structured_gradient = vis::GenerateSobelFeldmanGradientTexture(curr_vr_volume);
    }
    else if (curr_gradient_comp_model == STRUCTURED_GRADIENT_TYPE::FINITE_DIFERENCES)
    {
      curr_gl_tex_structured_gradient = vis::GenerateGradientTexture(curr_vr_volume);
    }
    else if (curr_gradient_comp_model == STRUCTURED_GRADIENT_TYPE::COMPUTE_SHADER_SOBEL)
    {
      curr_gl_tex_structured_gradient = GenerateGradientWithComputeShader();
    }
    else
    {
      curr_gl_tex_structured_gradient = nullptr;
      return false;
    }
    return true;
  }

  bool DataManager::UpdateStructuredGradientTexture ()
  {
    DeleteGradientData();
    return GenerateStructuredGradientTexture();
  }
  
  gl::Texture3D* DataManager::GenerateGradientWithComputeShader ()
  {
    // Get Current Volume
    vis::StructuredGridVolume* vol = GetCurrentStructuredVolume();

    // Initialize compute shader
    gl::ComputeShader* cpshader = new gl::ComputeShader();
    cpshader->SetShaderFile(MAKE_STR(CMAKE_VOLVIS_UTILS_PATH_TO_SHADER)"/_gradient_shading/sobelfeldman_generator.comp");
    cpshader->LoadAndLink();
    cpshader->Bind();
    
    // Initialize 1-channel textures
    gl::Texture3D* tex3d[3];
    for (int i = 0; i < 3; i++)
    {
      tex3d[i] = new gl::Texture3D(vol->GetWidth(), vol->GetHeight(), vol->GetDepth());
      tex3d[i]->GenerateTexture(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
      tex3d[i]->SetData(NULL, GL_R16F, GL_RED, GL_FLOAT);
    
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_3D, tex3d[i]->GetTextureID());
    }
    
    // Bind 1-channel textures
    for (int i = 0; i < 3; i++)
      glBindImageTexture(i, tex3d[i]->GetTextureID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R16F);
    
    // Bind volume and volume dimensions
    cpshader->SetUniformTexture3D("TexVolume", GetCurrentVolumeTexture()->GetTextureID(), 3);
    cpshader->BindUniform("TexVolume");
    
    cpshader->SetUniform("VolumeDimensions", glm::vec3(vol->GetWidth(), vol->GetHeight(), vol->GetDepth()));
    cpshader->BindUniform("VolumeDimensions");
    
    // Compute the number of groups and dispatch
    cpshader->RecomputeNumberOfGroups(vol->GetWidth(), vol->GetHeight(), vol->GetDepth());
    cpshader->Dispatch();
    
    // Delete compute shader
    cpshader->Unbind();
    delete cpshader;
    
    // Initialize Red Data
    GLfloat* red_data = new GLfloat[vol->GetWidth() * vol->GetHeight() * vol->GetDepth()];
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, tex3d[0]->GetTextureID());
    glGetTexImage(GL_TEXTURE_3D, 0, GL_RED, GL_FLOAT, red_data);
    glBindTexture(GL_TEXTURE_3D, 0);
    
    // Initialize Green Data
    GLfloat* green_data = new GLfloat[vol->GetWidth() * vol->GetHeight() * vol->GetDepth()];
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, tex3d[1]->GetTextureID());
    glGetTexImage(GL_TEXTURE_3D, 0, GL_RED, GL_FLOAT, green_data);
    glBindTexture(GL_TEXTURE_3D, 0);
    
    // Initialize Blue Data
    GLfloat* blue_data = new GLfloat[vol->GetWidth() * vol->GetHeight() * vol->GetDepth()];
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, tex3d[2]->GetTextureID());
    glGetTexImage(GL_TEXTURE_3D, 0, GL_RED, GL_FLOAT, blue_data);
    glBindTexture(GL_TEXTURE_3D, 0);
    
    // Delete 1-channel textures
    for (int i = 0; i < 3; i++)
      delete tex3d[i];
    
    // Initialize RGB Gradient Values Array
    GLfloat* gradient_values = new GLfloat[vol->GetWidth() * vol->GetHeight() * vol->GetDepth() * 3];
    for (int i = 0; i < vol->GetWidth() * vol->GetHeight() * vol->GetDepth(); i++)
    {
      gradient_values[i * 3 + 0] = red_data[i];
      gradient_values[i * 3 + 1] = green_data[i];
      gradient_values[i * 3 + 2] = blue_data[i];
    }
    
    // Delete Red Data
    delete[] red_data;
    
    // Delete Green Data
    delete[] green_data;
    
    // Delete Blue Data
    delete[] blue_data;
    
    // Generate a new terxture and set the gradient values [red, green, blue]
    gl::Texture3D* tex3d_gradient = new gl::Texture3D(vol->GetWidth(), vol->GetHeight(), vol->GetDepth());
    tex3d_gradient->GenerateTexture(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    tex3d_gradient->SetData((GLvoid*)gradient_values, GL_RGB16F, GL_RGB, GL_FLOAT);
    
    // Delete RGB Gradient Values Array
    delete[] gradient_values;
  
    return tex3d_gradient;
  }
}