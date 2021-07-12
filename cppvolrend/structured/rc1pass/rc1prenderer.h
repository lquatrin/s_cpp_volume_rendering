/**
 * 1-Pass - Ray Casting
 * . Structured Datasets
 * . Faster when implemented with Compute Shader:
 *   . A Comparison between GPU-based Volume Ray Casting Implementations:
 *     Fragment Shader, Compute Shader, OpenCL, and CUDA
 *   . Francisco Sans, Rhadamés Carmona
 *   . CLEI Electronic Journal, Volume 20, Number 2, Paper 7, 2017
 *   . DOI: 10.19153/cleiej.20.2.7
 *
 * Leonardo Quatrin Campagnolo
 * . campagnolo.lq@gmail.com
**/
#ifndef SINGLE_PASS_VOLUME_RENDERING_RAY_CASTING_H
#define SINGLE_PASS_VOLUME_RENDERING_RAY_CASTING_H

#include <volvis_utils/datamanager.h>
#include <volvis_utils/renderingparameters.h>
#include <volvis_utils/structuredgridvolume.h>
#include <volvis_utils/transferfunction.h>

#include <vis_utils/camera.h>
#include <vis_utils/renderoutputframe.h>

#include <gl_utils/texture1d.h>
#include <gl_utils/texture2d.h>
#include <gl_utils/texture3d.h>

#include <gl_utils/arrayobject.h>
#include <gl_utils/bufferobject.h>

#include <gl_utils/pipelineshader.h>
#include <gl_utils/computeshader.h>

class RayCasting1Pass
{
public:
  RayCasting1Pass ();
  virtual ~RayCasting1Pass ();

  //////////////////////////////////////////
  // Virtual base functions
  virtual const char* GetName () { return "1-Pass - Ray Casting"; }
  virtual const char* GetAbbreviationName () { return "s_1rc"; }

  void SetExternalResources (vis::DataManager* data_mgr, vis::RenderingParameters* rdr_prm);

  virtual void Clean ();
  virtual void ReloadShaders ();

  virtual bool Init (int shader_width, int shader_height);
  virtual bool Update (vis::Camera* camera);
  virtual void Redraw ();
  virtual void Reshape (int w, int h);

  virtual vis::GRID_VOLUME_DATA_TYPE GetDataTypeSupport ()
  {
    return vis::GRID_VOLUME_DATA_TYPE::STRUCTURED;
  }

  void PrepareRender (vis::Camera* camera);
  virtual void SetOutdated ();
  bool IsOutdated ();

  bool IsBuilt ();

  virtual int GetScreenTextureID() {
    return m_rdr_frame_to_screen.GetScreenOutputTexture()->GetTextureID();
  }

protected:
  void SetBuilt(bool b_built);

  //////////////////////////////////////////
  // State Variables
  bool vr_built;
  bool vr_outdated;

  //////////////////////////////////////////
  // External Resources
  vis::DataManager* m_ext_data_manager;
  vis::RenderingParameters* m_ext_rendering_parameters;

  //////////////////////////////////////////
  // Render Screen Texture
  vis::RenderFrameToScreen m_rdr_frame_to_screen;

private:
  void CreateRenderingPass ();
  void DestroyRenderingPass ();
  void RecreateRenderingPass ();
  
  gl::Texture1D* m_glsl_transfer_function;

  gl::ComputeShader*  cp_shader_rendering;

  float m_u_step_size;

  bool m_apply_gradient_shading;
  
};

#endif