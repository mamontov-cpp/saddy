#include <camera.h>
#include <scene.h>
#include <renderer.h>

#ifdef WIN32
// ReSharper disable once CppUnusedIncludeDirective
#include <windows.h>
#endif

#include <GL/gl.h>                                                      
// ReSharper disable once CppUnusedIncludeDirective
#include <GL/glu.h>

DECLARE_SOBJ(sad::Camera)

sad::Camera::Camera() 
: m_translation_offset(0,0,0), m_angle(0), m_temporary_rotation_offset(0, 0, 0), m_rotation_vector_direction(0, 0, 0), m_scene(NULL), m_transform_is_cached(false), m_projection_matrix{0}, m_model_view_matrix{0}
{

}

void sad::Camera::setScene(sad::Scene* s)
{
    m_scene = s;
    m_transform_is_cached = false;
}

sad::Scene* sad::Camera::scene() const
{
    return m_scene;
}

void sad::Camera::setTranslationOffset(const sad::Vector3D& v)
{
    m_translation_offset = v;
    m_transform_is_cached = false;
}

const sad::Vector3D& sad::Camera::translationOffset() const
{
    return m_translation_offset;
}

void sad::Camera::setRotationVectorDirection(const sad::Vector3D& v)
{
    m_rotation_vector_direction = v;
    m_transform_is_cached = false;
}


const sad::Vector3D& sad::Camera::rotationVectorDirection() const
{
    return m_rotation_vector_direction;
}

void sad::Camera::setAngle(double angle)
{
    m_angle = angle;
    m_transform_is_cached = false;
}

double sad::Camera::angle() const
{
    return m_angle;
}

void sad::Camera::setTemporaryRotationOffset(const sad::Vector3D& o)
{
    m_temporary_rotation_offset = o;
    m_transform_is_cached = false;
}

const sad::Vector3D& sad::Camera::temporaryRotationOffset() const
{
    return m_temporary_rotation_offset;
}


void sad::Camera::apply()
{
    sad::Vector3D vector3 = m_translation_offset;
    sad::Scene* scene = m_scene;
    if (scene)
    {
        sad::Renderer* renderer  = m_scene->renderer();
        if (renderer)
        {
            vector3 += renderer->globalTranslationOffset();
        }
    }
    glPushMatrix();
    glTranslatef(
        static_cast<GLfloat>(vector3.x()),
        static_cast<GLfloat>(vector3.y()),
        static_cast<GLfloat>(vector3.z())
    );
    glTranslatef(
        static_cast<GLfloat>(m_temporary_rotation_offset.x()),
        static_cast<GLfloat>(m_temporary_rotation_offset.y()),
        static_cast<GLfloat>(m_temporary_rotation_offset.z())
    );
    glRotatef(
        static_cast<GLfloat>(m_angle),
        static_cast<GLfloat>(m_rotation_vector_direction.x()),
        static_cast<GLfloat>(m_rotation_vector_direction.y()),
        static_cast<GLfloat>(m_rotation_vector_direction.z())
    );
    glTranslatef(
        static_cast<GLfloat>(-(m_temporary_rotation_offset.x())),
        static_cast<GLfloat>(-(m_temporary_rotation_offset.y())),
        static_cast<GLfloat>(-(m_temporary_rotation_offset.z()))
    );
}

void sad::Camera::clearTransformCache()
{
    m_transform_is_cached = false;
}

void sad::Camera::restore()
{
    glPopMatrix();
}

float* sad::Camera::projectionMatrix()
{
    if (!this->m_transform_is_cached)
    {
         glGetFloatv(GL_PROJECTION_MATRIX, &(this->m_projection_matrix[0]));
         glGetFloatv(GL_MODELVIEW_MATRIX, &(this->m_model_view_matrix[0]));
         this->m_transform_is_cached = true;
    }
    return &(this->m_projection_matrix[0]);
}

float* sad::Camera::modelViewMatrix()
{
    if (!this->m_transform_is_cached)
    {
         glGetFloatv(GL_PROJECTION_MATRIX, &(this->m_projection_matrix[0]));
         glGetFloatv(GL_MODELVIEW_MATRIX, &(this->m_model_view_matrix[0]));
         this->m_transform_is_cached = true;
    }
    return &(this->m_projection_matrix[0]);
}

sad::Camera::~Camera()
{

}
