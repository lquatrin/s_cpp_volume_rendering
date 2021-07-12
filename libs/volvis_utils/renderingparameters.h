/**
 * renderingparameters.h
 *
 * Leonardo Quatrin Campagnolo
 * . campagnolo.lq@gmail.com
**/
#ifndef VIS_UTILS_RENDERING_PARAMETERS_VOLUME_RENDERING_H
#define VIS_UTILS_RENDERING_PARAMETERS_VOLUME_RENDERING_H

#include <glm/glm.hpp>

#include <cstring>
#include <sstream>

#include <gl_utils/texture1d.h>
#include <gl_utils/texture2d.h>
#include <gl_utils/texture3d.h>

#include <vis_utils/camera.h>

#define USING_IMGUI

namespace vis
{
  class RenderingParameters
  {
  public:
    RenderingParameters();
    ~RenderingParameters();

    ///////////////////////////////////////
    // Blinn Phong Data
    void SetPhongParameters (float amb, 
                             float diff, 
                             float spec, 
                             float shini);
    float GetBlinnPhongKambient ();
    float GetBlinnPhongKdiffuse ();
    float GetBlinnPhongKspecular ();
    float GetBlinnPhongNshininess ();

    glm::vec3 GetLightSourceColor ();

    glm::vec3 GetLightSourceSpecular ();

    void SetBlinnPhongLightingPosition (glm::vec3 lightpos);
    glm::vec3 GetBlinnPhongLightingPosition ();

    void SetBlinnPhongLightSourceCameraVectors (glm::vec3 lcamforward,
                                                glm::vec3 lcamup, 
                                                glm::vec3 lcamright);
    glm::vec3 GetBlinnPhongLightSourceCameraForward ();
    glm::vec3 GetBlinnPhongLightSourceCameraUp ();
    glm::vec3 GetBlinnPhongLightSourceCameraRight ();

    ///////////////////////////////////////
    // Screen properties
    void SetScreenSize (int width, int height);
    int GetScreenWidth ();
    int GetScreenHeight ();
    
    ///////////////////////////////////////
    // Screen properties
    vis::Camera* GetCamera ();

  protected:
  
  private:
    vis::Camera s_camera;

    unsigned int screen_width;
    unsigned int screen_height;

    float m_blinnphong_ka;
    float m_blinnphong_kd;
    float m_blinnphong_ks;
    float m_blinnphong_shininess;
    
    glm::vec3 m_cam_color;
    glm::vec3 m_cam_specular;
    glm::vec3 m_cam_position;
    glm::vec3 m_cam_z_axis;
    glm::vec3 m_cam_y_axis;
    glm::vec3 m_cam_x_axis;
  };

}

#endif