/**
 * Rendering Manager for Volume Rendering
 *
 * https://www.cg.tuwien.ac.at/research/vis/datasets/
**/
#ifndef RENDERING_MANAGER_H
#define RENDERING_MANAGER_H
#include "defines.h"

#include <gl_utils/arrayobject.h>
#include <gl_utils/pipelineshader.h>

#include <vis_utils/camera.h>

#include <glm/glm.hpp>
#include <vector>

#include <volvis_utils/datamanager.h>
#include <volvis_utils/renderingparameters.h>

class RayCasting1Pass;

class RenderingManager
{
public:
  typedef void(*SwapBufferFunc) (void* data);
  SwapBufferFunc f_swapbuffer;
  void* d_swapbuffer;

  /*! Returns the current instance of Viewer (lazy instantiation).
  */
  static RenderingManager *Instance ();

  /*! Verify if already exists an instance of the Viewer.
  \return exist or not exist (true or false)
  */
  static bool Exists ();

  /*! Just Destroy the instance of the singleton.
  */
  static void DestroyInstance ();

  void InitGL ();
  void InitData ();

  void SetVolumeRenderer (RayCasting1Pass* volrend);

  void Display ();
  void Reshape (int w, int h);
  void Keyboard (unsigned char key, int x, int y);
  void KeyboardUp (unsigned char key, int x, int y);
  void MouseButton (int bt, int st, int x, int y);
  void MouseMotion (int x, int y);
  void CloseFunc ();
  void IdleFunc ();
  void PostRedisplay ();

  // Update the volume renderer with the current volume and transfer function
  void UpdateDataAndResetCurrentVRMode ();

  unsigned int GetScreenWidth ()
  {
    return curr_rdr_parameters.GetScreenWidth();
  }

  unsigned int GetScreenHeight ()
  {
    return curr_rdr_parameters.GetScreenHeight();
  }

  bool IdleRendering ()
  {
    return m_idle_rendering;
  }

protected:


private:
  void ResetGLStateConfig ();

  GLubyte* GetFrontBufferPixelData (bool alpha = true);
  void SetCurrentVolumeRenderer ();

  RayCasting1Pass* curr_vol_renderer;
  vis::RenderingParameters curr_rdr_parameters;

  vis::DataManager m_data_mgr;

  void UpdateFrameRate ();

  bool m_idle_rendering;
  double m_ts_current_time;
  double m_ts_last_time;
  int m_ts_n_frames;
  double m_ts_window_fps;
  double m_ts_window_ms;

  /*! Constructor.*/
  RenderingManager ();
  /*! Destructor.*/
  virtual ~RenderingManager ();
  /*! The pointer to the singleton instance.*/
  static RenderingManager* crr_instance;
};

#endif