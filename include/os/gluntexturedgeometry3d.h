/*! \file gluntexturedgeometry3d.h

    A geometry as set of points, required for rendering and texturing
 */
#pragma once
#include "glheaders.h"
#include "geometry2d.h"
#include "geometry3d.h"

namespace sad
{
class Renderer;

namespace os
{

/*! A geometry for rendering
 */
class GLUntexturedGeometry3D
{
public:
    /*! Makes new geometry
     *  \param[in] renderer a renderer
     *  \param[in] points a point count
     */
    GLUntexturedGeometry3D(sad::Renderer* renderer, unsigned int points);
    /*! Destroys geometry
     */
    ~GLUntexturedGeometry3D();
    /*! Sets vertices for vertex buffer
     *  \param[in] vertices rectangle
     */
    void setVertices(const sad::Rect< sad::Point3D >& vertices) const;
    /*! Sets vertices for vertex buffer
     *  \param[in] vertices rectangle
     */
    void setVertices(const float* vertices) const;
    /*! Tries to upload geometry to GPU
     */
    void loadToGPU();
    /*! Unloads resources from GPU
     */
    void unload();
    /*! Draws array list
     * \param[in] mode a mode
     * \param[in] vertexes vertexes
     */
    void drawArrays(GLenum mode, const float* vertexes);
    /*! Draws array list
     *  \param[in] mode a mode
      * \param[in] vertexes vertexes
      * \param[in] tc texture coordinates
      */
    void drawArrays(GLenum mode, const sad::Rect<sad::Point3D>& vertexes);
    /*! Tries to log OpenGL error if occurred
     *  \param[in] op log info
     */
    void tryLogGlError(const char* op) const;
private:
    /*! Draws array list
     * \param[in] mode a mode
     */
    void drawArrays(GLenum mode) const;
    /*! A renderer for geometry
     */
    sad::Renderer* m_renderer;
    /*! A vertex array for rendering
     */
    GLuint m_vertex_array;
    /*! A vertex buffer
     */
    GLuint m_vertex_buffer;
    /*! A point count for rendering points
     */
    unsigned int m_point_count;
    /*! Whether geometry is already uploaded to GPU
     */
    bool m_is_on_gpu;
};

}

}