#include "camera.h"

#include <cstdio>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef DEGREE_TO_RADIANS
  #define DEGREE_TO_RADIANS(s) (s * (glm::pi<double>() / 180.0))
#endif

namespace vis
{
  CameraData::CameraData ()
  {
    cam_setup_name = "";
    eye = glm::vec3(0.0);
    center = glm::vec3(0.0);
    up = glm::vec3(0.0);

    field_of_view_y = 45.0f;
    aspect_ratio = 1.0f;
    z_near = 1.0f;
    z_far = 5000.0f;

    z_axis = glm::vec3(0.0);
    y_axis = glm::vec3(0.0);
    x_axis = glm::vec3(0.0);
    c_eye = glm::vec3(0.0);
  }

  CameraData::CameraData (std::string setup_name, glm::vec3 ieye, glm::vec3 icenter, glm::vec3 iup)
  {
    cam_setup_name = setup_name;
    eye = ieye;
    center = icenter;
    up = iup;
    
    field_of_view_y = 45.0f;
    aspect_ratio = 1.0f;
    z_near = 1.0f;
    z_far = 5000.0f;

    z_axis = glm::normalize(eye - center);
    x_axis = glm::normalize(glm::cross(up, z_axis));
    y_axis = glm::normalize(glm::cross(z_axis, x_axis));

    c_eye = eye;
  }

  CameraData::~CameraData ()
  {
  }

  Camera::Camera ()
  {
    f_curr_time_func = nullptr;
    f_curr_time_data = nullptr;

    radius = 50;
    speed = 0.001f;
    speed_radius = 1.0f;

    min_radius = 0;
    max_radius = 1000;

    perspective = true;

    speed_keyboard_movement = 1.0f;
    speed_keyboard_rotation = 1.0f;
    speed_mouse_rotation = 1.0f;

    m_changing_camera = false;
  }

  Camera::Camera (float _radius, float _min_rad, float _max_rad)
  {
    f_curr_time_func = nullptr;
    f_curr_time_data = nullptr;

    radius = _radius;
    speed = 0.001f;
    speed_radius = 1.0f;
  
    min_radius = _min_rad;
    max_radius = _max_rad;
    
    perspective = true;

    speed_keyboard_movement = 1.0f;
    speed_keyboard_rotation = 1.0f;
    speed_mouse_rotation    = 1.0f;

    m_changing_camera = false;
  }
  
  Camera::~Camera ()
  {
  }

  bool Camera::UpdatePositionAndRotations ()
  {
    return false;
  }

  bool Camera::Changing ()
  {
    return m_changing_camera;
  }

  bool Camera::KeyboardDown (unsigned char key, int x, int y)
  {
    return false;
  }

  bool Camera::KeyboardUp (unsigned char key, int x, int y)
  {
    return false;
  }

  int Camera::MouseButton (int bt, int st, int x, int y)
  {
    if (st == 0 && bt == 0) {
      arcball_on = true;
      changing_radius = false;

      last_my = cur_my = y;
      last_mx = cur_mx = x;
      m_changing_camera = false;
    }
    else if (st == 0 && (bt == 1 || bt == 2)) {
      speed_radius = bt == 1 ? 1.0f : 0.01f;
      changing_radius = true;
      arcball_on = false;

      last_my = cur_my = y;
      last_mx = cur_mx = x;
      m_changing_camera = false;
    }
    else if (st == 1 && bt == 0) {
      arcball_on = false;
    }
    else if (st == 1 && (bt == 1 || bt == 2)) {
      changing_radius = false;
      m_changing_camera = false;
    }

    return 0;
  }

  int Camera::MouseMotion (int x, int y)
  {
    if (arcball_on) {
      float xrot = -(y - last_my) * speed;
      float yrot = -(x - last_mx) * speed;

      glm::quat p = glm::quat(0, c_data.eye.x, c_data.eye.y, c_data.eye.z);

      glm::quat qy = glm::quat(cos(yrot), sin(yrot) * c_data.up);

      glm::vec3 loc_up = c_data.up;

      float max = 0.99f;
      float dt = glm::dot(glm::normalize(glm::vec3(c_data.center - c_data.eye)), loc_up);
      if ((dt > max&& xrot > 0.0f) || (dt < -max && xrot < 0.0f))
        xrot = 0.0f;

      glm::vec3 vr = glm::normalize(glm::cross(glm::normalize(glm::vec3(c_data.center - c_data.eye)), loc_up));
      glm::quat qx = glm::quat(cos(xrot), sin(xrot) * vr);

      glm::quat rq =
        glm::cross(glm::cross(glm::cross(glm::cross(qx, qy), p),
          glm::inverse(qy)), glm::inverse(qx));

      c_data.eye = glm::vec3(rq.x, rq.y, rq.z);


      last_mx = cur_mx;
      last_my = cur_my;
      cur_mx = x;
      cur_my = y;

      m_changing_camera = true;
      return 1;
    }
    //////////////////////////////////////////
    //////////////////////////////////////////
    if (changing_radius) {
      float ydiff = (y - last_my) * speed_radius;

      radius += ydiff;
      if (radius < min_radius)
        radius = min_radius;
      if (radius > max_radius)
        radius = max_radius;


      glm::vec3 c_e = glm::normalize(glm::vec3(c_data.eye - c_data.center));

      c_data.eye = c_e * radius;

      last_my = cur_my;
      cur_my = y;

      m_changing_camera = true;
      return 1;
    }

    return 0;
  }

  float Camera::GetSpeedKeyboardMovement ()
  {
    return speed_keyboard_movement;
  }

  void Camera::SetSpeedKeyboardMovement (float sskm)
  {
    speed_keyboard_movement = sskm;
  }

  float Camera::GetSpeedKeyboardRotation ()
  {
    return speed_keyboard_rotation;
  }

  void Camera::SetSpeedKeyboardRotation (float sskr)
  {
    speed_keyboard_rotation = sskr;
  }

  float Camera::GetSpeedMouseRotation ()
  {
    return speed_mouse_rotation;
  }

  void Camera::SetSpeedMouseRotation (float ssmr)
  {
    speed_mouse_rotation = ssmr;
  }
  
  void Camera::SetInitialConfig (glm::vec3 _center, glm::vec3 _up)
  {
    c_data.center = _center;
    c_data.eye = glm::vec3(_center.x, _center.y, _center.z + radius);
    c_data.up = _up;
  }
    
  void Camera::SetSpeedRadius (float spd)
  {
    speed_radius = spd;
  }
  
  glm::mat4 Camera::LookAt ()
  {
    return glm::lookAt(c_data.eye, c_data.center, c_data.up);
  }
 
  glm::mat4 Camera::Projection ()
  {
    if (perspective)
    {
      glm::mat4 perspection = glm::perspective(c_data.field_of_view_y,
                                               c_data.aspect_ratio,
                                               c_data.z_near, c_data.z_far);
      return perspection;
    }
    else
    {
      printf("TODO: Ortographic\n");
      return glm::mat4();
    }
  }
  
  glm::vec3 Camera::GetDir ()
  {
    return glm::normalize(c_data.center - c_data.eye);
  }
  
  void Camera::UpdateAspectRatio (float w, float h)
  {
    c_data.aspect_ratio = w / h;
  }
  
  float Camera::GetAspectRatio ()
  {
    return c_data.aspect_ratio;
  }
  
  float Camera::GetFovY ()
  {
    return c_data.field_of_view_y;
  }
  
  float Camera::GetTanFovY ()
  {
    return (float)tan(DEGREE_TO_RADIANS(GetFovY()) / 2.0);
  }
  
  void Camera::SetData (CameraData* data)
  {
    c_data.eye    = data->eye;
    c_data.center = data->center;
    c_data.up     = data->up;
  
    radius = glm::distance(c_data.eye, c_data.center);
  }
  
  void Camera::GetCameraVectors (glm::vec3* cforward, glm::vec3* cup, glm::vec3* cright)
  {
    (*cforward) = -GetDir();
    (*cright) = glm::normalize(glm::cross(c_data.up, (*cforward)));
    (*cup) = glm::normalize(glm::cross((*cforward), (*cright)));
  }

  glm::vec3 Camera::GetEye ()
  {
    return c_data.eye;
  }

  glm::vec3 Camera::GetZAxis ()
  {
    return c_data.z_axis;
  }
  
  glm::vec3 Camera::GetYAxis ()
  {
    return c_data.y_axis;
  }
  
  glm::vec3 Camera::GetXAxis ()
  {
    return c_data.x_axis;
  }
}