#include "camera.h"

Camera::Camera(QVector3D pos, QVector3D worldUp): pos(pos), worldUp(worldUp),yaw(YAW),pitch(PITCH),front({0.0f,0.0f,-1.0f}), movementSpeed(0.1f)
{
    sens=0.1f;
    updateCamVectors();
}

QMatrix4x4 Camera::getMatrix()
{
    QMatrix4x4 a;
    a.lookAt(this->pos,this->pos+this->front,this->up);
    return a;
}

void Camera::changeYawAndPitch(float yaw, float pitch)
{
    this->yaw   += yaw*sens;
    this->pitch += pitch*sens;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (true)
    {
        if (this->pitch > 89.0f)
            this->pitch = 89.0f;
        if (this->pitch < -89.0f)
            this->pitch = -89.0f;
    }

    // Update Front, Right and Up Vectors using the updated Eular angles
    this->updateCamVectors();
}

void Camera::moveCam(QSet<int> *keys)
{
    if(keys->contains(Qt::Key_W))
        this->pos+=this->movementSpeed*this->front;
    if(keys->contains(Qt::Key_S))
       this->pos-=this->movementSpeed*this->front;
    if(keys->contains(Qt::Key_A))
        this->pos-=this->right*this->movementSpeed;
    if(keys->contains(Qt::Key_D))
        this->pos+=this->right*this->movementSpeed;
    if(keys->contains(Qt::Key_Space))
        this->pos+=this->up*this->movementSpeed;
    if(keys->contains(Qt::Key_Control))
        this->pos-=this->up*this->movementSpeed;
}

void Camera::updateCamVectors()
{
    QVector3D front;
    front.setX( cosf(qDegreesToRadians(this->yaw)) * cosf(qDegreesToRadians(this->pitch)) );
    front.setY( sinf(qDegreesToRadians(this->pitch)) );
    front.setZ( sinf(qDegreesToRadians(this->yaw)) * cosf(qDegreesToRadians(this->pitch)) );
    this->front=front;
    this->front.normalize();

    this->right = QVector3D::normal(this->front, this->worldUp);  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    this->up    = QVector3D::normal(this->right, this->front);
}
