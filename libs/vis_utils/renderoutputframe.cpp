#include "renderoutputframe.h"

#include <gl_utils/texture2d.h>
#include <gl_utils/pipelineshader.h>
#include <vis_utils/defines.h>

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "defines.h"


namespace vis
{

RenderFrameToScreen::RenderFrameToScreen(std::string shader_folder)
  : m_screen_output(nullptr)
  , m_shader_folder(shader_folder)
  , m_ps_shader(nullptr)
  , m_cb_vao(nullptr)
  , m_cb_vbo(nullptr)
  , m_cb_ibo(nullptr)
{
}

RenderFrameToScreen::~RenderFrameToScreen ()
{
  Clean();
}

void RenderFrameToScreen::Clean ()
{
  ClearTextures();

  ClearShaders();

  if (m_cb_vao) delete m_cb_vao;
  m_cb_vao = nullptr;

  if (m_cb_vbo) delete m_cb_vbo;
  m_cb_vbo = nullptr;

  if (m_cb_ibo) delete m_cb_ibo;
  m_cb_ibo = nullptr;
}

void RenderFrameToScreen::UpdateScreenResolution (int s_w, int s_h)
{
  if (m_screen_output == nullptr)
  {
    m_screen_output = new gl::Texture2D(s_w, s_h);
    m_screen_output->GenerateTexture(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    m_screen_output->SetData(NULL, GL_RGBA16F, GL_RGBA, GL_FLOAT);
  }
  else
  {
    if (m_screen_output->GetWidth() != s_w || m_screen_output->GetHeight() != s_h)
    {
      delete m_screen_output;

      m_screen_output = new gl::Texture2D(s_w, s_h);
      m_screen_output->GenerateTexture(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
      m_screen_output->SetData(NULL, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    }
  }
  gl::ExitOnGLError("RenderFrameToScreen: Error on UpdateScreenResolution.");
}

int RenderFrameToScreen::GetWidth ()
{
  if (m_screen_output) return m_screen_output->GetWidth();
  return 0;
}

int RenderFrameToScreen::GetHeight ()
{
  if (m_screen_output) return m_screen_output->GetHeight();
  return 0;
}

void RenderFrameToScreen::ClearShaders ()
{
  if (m_ps_shader) delete m_ps_shader;
  m_ps_shader = nullptr;
}

void RenderFrameToScreen::ClearTextures ()
{
  if (m_screen_output) delete m_screen_output;
  m_screen_output = nullptr;
}

void RenderFrameToScreen::ClearTexture ()
{
  glClearTexImage(m_screen_output->GetTextureID(), 0, GL_RGBA, GL_FLOAT, 0);
}

void RenderFrameToScreen::ClearTextureImage ()
{
  glClearTexImage(m_screen_output->GetTextureID(), 0, GL_RGBA, GL_FLOAT, 0);
}

void RenderFrameToScreen::BindImageTexture (bool multisample)
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_screen_output->GetTextureID());
  glBindImageTexture(0, m_screen_output->GetTextureID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
}

void RenderFrameToScreen::Draw ()
{
  Draw(m_screen_output->GetTextureID());
}

void RenderFrameToScreen::Draw (gl::Texture2D* screen_output)
{
  Draw(screen_output->GetTextureID());
}

void RenderFrameToScreen::Draw (GLuint screen_output_id)
{
  if (m_ps_shader == nullptr)
  {
    // Shader to blend the rendered frame to the output screen (used by compute shaders)
    m_ps_shader = new gl::PipelineShader();

    m_ps_shader->AddShaderFile(gl::PipelineShader::TYPE::VERTEX, vis::Utils::GetShaderPath() + "blendframe_render.vert");
    m_ps_shader->AddShaderFile(gl::PipelineShader::TYPE::FRAGMENT, vis::Utils::GetShaderPath() + "blendframe_render.frag");
    m_ps_shader->LoadAndLink();
    m_ps_shader->Bind();
    gl::ExitOnGLError("vis::RenderFrameToScreen: Could not create pipeline blend shader...");

    glm::mat4 projMat = glm::ortho<float>(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
    m_ps_shader->SetUniform("ProjectionMatrix", projMat);
    m_ps_shader->BindUniform("ProjectionMatrix");
    gl::ExitOnGLError("vis::RenderFrameToScreen: Could not bind uniforms...");

    m_ps_shader->Unbind();
    gl::ExitOnGLError("vis::RenderFrameToScreen: Could not unbind pipeline shader...");

    CreateVertexBuffers();
  }

  m_ps_shader->Bind();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, screen_output_id);

  m_ps_shader->SetUniformTexture2D("TexGeneratedFrame", screen_output_id, 0);
  m_ps_shader->BindUniform("TexGeneratedFrame");

  m_cb_vao->Bind();
  m_cb_vbo->Bind();
  m_cb_ibo->Bind();
  m_cb_vao->DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT);
  m_cb_ibo->Unbind();
  m_cb_vbo->Unbind();
  gl::ArrayObject::Unbind();

  gl::ExitOnGLError("vis::RenderFrameToScreen: Could not get shader uniform locations");
  gl::PipelineShader::Unbind();

  glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderFrameToScreen::CreateVertexBuffers ()
{
  const GLfloat VERTICES[12] = { -1.0f, -1.0f, 0.0f,
                                 +1.0f, -1.0f, 0.0f,
                                 +1.0f, +1.0f, 0.0f,
                                 -1.0f, +1.0f, 0.0f };
  const GLuint INDICES[6] = { 0, 1, 2, 0, 2, 3 };

  if (m_cb_vao == nullptr)
  {
    m_cb_vao = new gl::ArrayObject(1);
    m_cb_vao->Bind();

    m_cb_vbo = new gl::BufferObject(gl::BufferObject::TYPES::VERTEXBUFFEROBJECT);
    m_cb_ibo = new gl::BufferObject(gl::BufferObject::TYPES::INDEXBUFFEROBJECT);

    //bind the VBO to the VAO
    m_cb_vbo->SetBufferData(sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

    m_cb_vao->SetVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3.0f, (GLvoid*)0);

    //bind the IBO to the VAO
    m_cb_ibo->SetBufferData(sizeof(INDICES), INDICES, GL_STATIC_DRAW);

    m_cb_ibo->Unbind();
    m_cb_vbo->Unbind();
    gl::ArrayObject::Unbind();
  }
}

}