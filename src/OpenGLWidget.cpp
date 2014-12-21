#include <QGuiApplication>

#include "OpenGLWidget.h"
#include <glm/gtc/matrix_inverse.hpp>
#include <iostream>

const static float INCREMENT=0.01;
//------------------------------------------------------------------------------------------------------------------------------------
/// @brief the increment for the wheel zoom
//------------------------------------------------------------------------------------------------------------------------------------
const static float ZOOM=0.1;
OpenGLWidget::OpenGLWidget(const QGLFormat _format, QWidget *_parent) : QGLWidget(_format,_parent){
    // set this widget to have the initial keyboard focus
    setFocus();
    // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
    m_rotate=false;
    // mouse rotation values set to 0
    m_spinXFace=0;
    m_spinYFace=0;
    // Roughness of the material
    m_roughness = 0;
    // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
    this->resize(_parent->size());
}
//----------------------------------------------------------------------------------------------------------------------
OpenGLWidget::~OpenGLWidget(){
    delete m_cam;
    delete m_shaderProgram;
    delete m_model;
}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::initializeGL(){

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    // enable depth testing for drawing
    glEnable(GL_DEPTH_TEST);
    // enable multisampling for smoother drawing
    //glEnable(GL_MULTISAMPLE);

    // as re-size is not explicitly called we need to do this.
    glViewport(0,0,width(),height());

    // Initialise the model matrix
    m_modelMatrix = glm::mat4(1.0);

    // Create a shader program
    m_shaderProgram = new ShaderProgram();
    m_vertexShader = new Shader("shaders/CookTorrance2Vert.glsl", GL_VERTEX_SHADER);
    m_fragmentShader = new Shader("shaders/CookTorrance2Frag.glsl", GL_FRAGMENT_SHADER);

    m_shaderProgram->attachShader(m_vertexShader);
    m_shaderProgram->attachShader(m_fragmentShader);
    m_shaderProgram->bindFragDataLocation(0, "fragColour");
    m_shaderProgram->link();
    m_shaderProgram->use();

    delete m_vertexShader;
    delete m_fragmentShader;

    m_model = new Model("models/bunny.obj");
    //m_model = new Model();
   // m_model->loadCube();

    m_projLoc = m_shaderProgram->getUniformLoc("projectionMatrix");
    m_normalLoc = m_shaderProgram->getUniformLoc("normalMatrix");
    m_modelViewProjectionLoc = m_shaderProgram->getUniformLoc("modelViewProjectionMatrix");
    m_modelViewLoc = m_shaderProgram->getUniformLoc("modelViewMatrix");

    GLuint lightDirLoc = m_shaderProgram->getUniformLoc("lightDirection");
    GLuint light1DirLoc = m_shaderProgram->getUniformLoc("light[0].direction");
    GLuint light1ColLoc = m_shaderProgram->getUniformLoc("light[0].colour");
    GLuint light2DirLoc = m_shaderProgram->getUniformLoc("light[1].direction");
    GLuint light2ColLoc = m_shaderProgram->getUniformLoc("light[1].colour");

    GLuint numLightsLoc = m_shaderProgram->getUniformLoc("numLights");


    glUniform3f(lightDirLoc, 0.0, 0.0, 1.0);
    glUniform3f(light1DirLoc, -0.5, 0.2, 1.0);
    glUniform3f(light1ColLoc, 1.0, 1.0, 1.0);
    glUniform3f(light2DirLoc, 0.5, 0.0, 1.0);
    glUniform3f(light2ColLoc, 1.0, 1.0, 1.0);

    glUniform1i(numLightsLoc, 2);


    // Initialize the camera
    m_cam = new Camera(glm::vec3(0.0, 0.0, 5.0));

    startTimer(0);

}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::resizeGL(const int _w, const int _h){
    // set the viewport for openGL
    glViewport(0,0,_w,_h);
    m_cam->setShape(_w, _h);

}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::timerEvent(QTimerEvent *){
    updateGL();
    update();
}

//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::update(){
    m_spinYFace += 0.01;
    glm::mat4 roty;
    roty = glm::rotate(roty, m_spinYFace, glm::vec3(0.0, 1.0, 0.0));
    m_mouseGlobalTX = roty;
}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::paintGL(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Initialise the model matrix

    glm::mat4 rotx;
    glm::mat4 roty;

    rotx = glm::rotate(rotx, m_spinXFace, glm::vec3(1.0, 0.0, 0.0));
    roty = glm::rotate(roty, m_spinYFace, glm::vec3(0.0, 1.0, 0.0));

    //m_mouseGlobalTX = rotx*roty;
//    // add the translations
    m_mouseGlobalTX[3][0] = m_modelPos.x;
    m_mouseGlobalTX[3][1] = m_modelPos.y;
    m_mouseGlobalTX[3][2] = m_modelPos.z;
    m_modelMatrix = m_mouseGlobalTX;

    m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(10.0, 10.0, 10.0));

    // Set the roughness of the material
    GLuint roughnessLoc = m_shaderProgram->getUniformLoc("roughness");
    glUniform1f(roughnessLoc, m_roughness);

    // Load the modelview and modelviewprojection matrix
    loadMatricesToShader();

    glBindVertexArray(m_model->getVAO());
    glDrawArrays(GL_TRIANGLES, 0, m_model->getNumVerts());
    glBindVertexArray(0);

}
//----------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::loadMatricesToShader(){

    // Calculate projection matrix

    //m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(10.0, 10.0, 10.0));
    glm::mat4 projectionMatrix = m_cam->getProjectionMatrix();

    glm::mat4 modelViewMatrix = m_cam->getViewMatrix() * m_modelMatrix;

    m_normalMatrix = glm::inverseTranspose(glm::mat3(modelViewMatrix));

    m_modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

    //glUniformMatrix4fv(m_modelLoc, 1, false, glm::value_ptr(m_modelMatrix));
    glUniformMatrix4fv(m_modelViewLoc, 1, false, glm::value_ptr(modelViewMatrix));
    glUniformMatrix4fv(m_projLoc, 1, false, glm::value_ptr(projectionMatrix));
    glUniformMatrix3fv(m_normalLoc, 1, false, glm::value_ptr(m_normalMatrix));
    glUniformMatrix4fv(m_modelViewProjectionLoc, 1, false, glm::value_ptr(m_modelViewProjectionMatrix));
}
//------------------------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::mouseMoveEvent (QMouseEvent *_event){
  // Sourced from Jon Macey's NGL library
  // note the method buttons() is the button state when event was called
  // this is different from button() which is used to check which button was
  // pressed when the mousePress/Release event is generated
  if(m_rotate && _event->buttons() == Qt::LeftButton){
    int diffx=_event->x()-m_origX;
    int diffy=_event->y()-m_origY;
    m_spinXFace += (float) 0.5f * diffy;
    m_spinYFace += (float) 0.5f * diffx;
    m_origX = _event->x();
    m_origY = _event->y();
  }
        // right mouse translate code
  else if(m_translate && _event->buttons() == Qt::RightButton){
    int diffX = (int)(_event->x() - m_origXPos);
    int diffY = (int)(_event->y() - m_origYPos);
    m_origXPos=_event->x();
    m_origYPos=_event->y();
    m_modelPos.x += INCREMENT * diffX;
    m_modelPos.y -= INCREMENT * diffY;
   }
}
//------------------------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::mousePressEvent ( QMouseEvent * _event){
    // Sourced from Jon Macey's NGL library
  // this method is called when the mouse button is pressed in this case we
  // store the value where the mouse was clicked (x,y) and set the Rotate flag to true
  if(_event->button() == Qt::LeftButton)
  {
    m_origX = _event->x();
    m_origY = _event->y();
    m_rotate = true;
  }
  // right mouse translate mode
  else if(_event->button() == Qt::RightButton)
  {
    m_origXPos = _event->x();
    m_origYPos = _event->y();
    m_translate = true;
  }

}
//------------------------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::mouseReleaseEvent ( QMouseEvent * _event ){
    // Sourced from Jon Macey's NGL library
  // this event is called when the mouse button is released
  // we then set Rotate to false
  if (_event->button() == Qt::LeftButton)
  {
    m_rotate=false;
  }
        // right mouse translate mode
  if (_event->button() == Qt::RightButton)
  {
    m_translate=false;
  }
}
//------------------------------------------------------------------------------------------------------------------------------------
void OpenGLWidget::wheelEvent(QWheelEvent *_event){
    // Sourced from Jon Macey's NGL library
    // check the diff of the wheel position (0 means no change)
    if(_event->delta() > 0)
    {
        m_modelPos.z+=ZOOM;
    }
    else if(_event->delta() <0 )
    {
        m_modelPos.z-=ZOOM;
    }
}

void OpenGLWidget::setRoughness(int _roughness){
    m_roughness = _roughness / 100.0;
}

