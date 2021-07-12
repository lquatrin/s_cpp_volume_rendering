#ifndef VIS_UTILS_RENDER_OUTPUT_FRAME_H
#define VIS_UTILS_RENDER_OUTPUT_FRAME_H

#include <gl_utils/texture2d.h>
#include <gl_utils/pipelineshader.h>
#include <gl_utils/computeshader.h>

#include <gl_utils/arrayobject.h>
#include <gl_utils/bufferobject.h>

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

namespace vis
{

class RenderFrameToScreen
{
public:
  RenderFrameToScreen (std::string shader_folder);
  ~RenderFrameToScreen ();

  void Clean ();

  void UpdateScreenResolution (int s_w, int s_h);
  int GetWidth ();
  int GetHeight ();

  void ClearShaders ();
  void ClearTextures ();
  void ClearTexture ();
  void ClearTextureImage ();
  void BindImageTexture (bool multisample = false);
  void Draw ();

  void Draw (gl::Texture2D* screen_output);
  void Draw (GLuint screen_output_id);
  
  gl::Texture2D* GetScreenOutputTexture ()
  {
    return m_screen_output;
  }
protected:

private:
  void CreateVertexBuffers ();

  gl::Texture2D* m_screen_output;

  std::string m_shader_folder;
  gl::PipelineShader* m_ps_shader;

  gl::ArrayObject* m_cb_vao;
  gl::BufferObject* m_cb_vbo;
  gl::BufferObject* m_cb_ibo;
};

}

#endif