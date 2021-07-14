/**
 * renderingparameters.cpp
 *
 * https://docs.unrealengine.com/en-US/Engine/Audio/DistanceModelAttenuation
 * https://en.wikipedia.org/wiki/Attenuation
 * http://learnwebgl.brown37.net/09_lights/lights_attenuation.html
 * https://www.miniphysics.com/light-attenuation.html
 * https://www.sciencedirect.com/topics/earth-and-planetary-sciences/light-attenuation
 *
 * Leonardo Quatrin Campagnolo
 * . campagnolo.lq@gmail.com
**/
#include "renderingparameters.h"

namespace vis
{
  RenderingParameters::RenderingParameters ()
    : s_camera(200.0f)
  {
    screen_width = 768;
    screen_height = 768;

    m_blinnphong_ka = 0.5f;
    m_blinnphong_kd = 0.5f;
    m_blinnphong_ks = 0.8f;
    m_blinnphong_shininess = 30.0f;

    m_cam_color    = glm::vec3(1.0f);
    m_cam_specular = glm::vec3(1.0f);
    m_cam_position = glm::vec3(-45.5841, 1367.85, -181.179);
    m_cam_z_axis = -glm::vec3(-0.0330187, 0.990801, -0.131237);
    m_cam_y_axis = glm::vec3(0.241748, 0.135327, 0.960856);
    m_cam_x_axis = glm::vec3(-0.969777, 0, 0.243993);
  }

  RenderingParameters::~RenderingParameters ()
  {}

  void RenderingParameters::SetPhongParameters (float amb, 
                                                float diff, 
                                                float spec, 
                                                float shini)
  {
    m_blinnphong_ka = amb;
    m_blinnphong_kd = diff;
    m_blinnphong_ks = spec;
    m_blinnphong_shininess = shini;
  }

  float RenderingParameters::GetBlinnPhongKambient()
  {
    return m_blinnphong_ka;
  }

  float RenderingParameters::GetBlinnPhongKdiffuse ()
  {
    return m_blinnphong_kd;
  }

  float RenderingParameters::GetBlinnPhongKspecular ()
  {
    return m_blinnphong_ks;
  }

  float RenderingParameters::GetBlinnPhongNshininess ()
  {
    return m_blinnphong_shininess;
  }

  glm::vec3 RenderingParameters::GetLightSourceColor ()
  {
    return m_cam_color;
  }

  glm::vec3 RenderingParameters::GetLightSourceSpecular ()
  {
    return m_cam_specular;
  }

  void RenderingParameters::SetBlinnPhongLightingPosition (glm::vec3 lightpos)
  {
    m_cam_position = lightpos;
  }

  glm::vec3 RenderingParameters::GetBlinnPhongLightingPosition ()
  {
    return m_cam_position;
  }

  void RenderingParameters::SetBlinnPhongLightSourceCameraVectors (glm::vec3 lcamforward, glm::vec3 lcamup, glm::vec3 lcamright)
  {
    m_cam_z_axis = -lcamforward;
    m_cam_y_axis = lcamup;
    m_cam_x_axis = lcamright;
  }

  glm::vec3 RenderingParameters::GetBlinnPhongLightSourceCameraForward ()
  {
    return -m_cam_z_axis;
  }

  glm::vec3 RenderingParameters::GetBlinnPhongLightSourceCameraUp ()
  {
    return m_cam_y_axis;
  }

  glm::vec3 RenderingParameters::GetBlinnPhongLightSourceCameraRight ()
  {
    return m_cam_x_axis;
  }

  void RenderingParameters::SetScreenSize (int width, int height)
  {
    screen_width = width;
    screen_height = height;
  }

  int RenderingParameters::GetScreenWidth ()
  {
    return screen_width;
  }

  int RenderingParameters::GetScreenHeight ()
  {
    return screen_height;
  }

  vis::Camera* RenderingParameters::GetCamera ()
  {
    return &s_camera;
  }
}