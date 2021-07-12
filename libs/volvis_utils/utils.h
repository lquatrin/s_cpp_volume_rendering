/**
 * http://idav.ucdavis.edu/~okreylos/PhDStudies/Winter2000/TextureMapping.html
**/
#ifndef VIS_UTILS_H
#define VIS_UTILS_H

#include <gl_utils/texture3d.h>
#include <gl_utils/texture2d.h>
#include <volvis_utils/transferfunction.h>
#include <volvis_utils/structuredgridvolume.h>

#include <glm/glm.hpp>

#define USE_16F_INTERNAL_FORMAT

namespace vis
{
  typedef struct Vertex {
    float Position[3];
    float Color[3];
  } Vertex;

  typedef struct Vertex1p {
    float Position[3];
  } Vertex1p;

  typedef struct Vertex2p {
    float Position[3];
    float Color[3];
  } Vertex2p;

  gl::Texture3D* GenerateRTexture (StructuredGridVolume* vol,
    int init_x = 0,
    int init_y = 0,
    int init_z = 0,
    int last_x = 0,
    int last_y = 0,
    int last_z = 0);

  enum VIS_UTILS_DATA_TYPE : unsigned int {
    UNSIGNED_BYTE  = 0,
    UNSIGNED_SHORT = 1,
    HALF_FLOAT     = 2,
    FLOAT          = 3,
  };

  gl::Texture3D* GenerateRTexture (StructuredGridVolume* vol, VIS_UTILS_DATA_TYPE vdatatype);

  gl::Texture3D* GenerateGradientTexture (StructuredGridVolume* vol,
    int gradient_sample_size = 1,
    int filter_nxnxn = 0,
    bool normalized_gradient = true,
    int init_x = -1,
    int init_y = -1,
    int init_z = -1,
    int last_x = -1,
    int last_y = -1,
    int last_z = -1);

  // https://en.wikipedia.org/wiki/Sobel_operator  
  gl::Texture3D* GenerateSobelFeldmanGradientTexture (StructuredGridVolume* vol);
}

#endif
