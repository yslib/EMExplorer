#ifndef CAMERA_H
#define CAMERA_H

#include "qmath.h"
#include <QMatrix4x4>



/**
 * \enum	CameraMovement
 *
 * \brief	Defines several possible options for camera movement. 
 * 			Used as abstraction to stay away from window-system specific input methods
 */

enum class CameraMovement {
	Forward,
	Backward,
	Left,
	Right
};

// Default camera values
constexpr float YAW = -90.0f;
constexpr float PITCH = 0.0f;
constexpr float SPEED = 2.5f;
constexpr float SENSITIVITY = 0.1f;
constexpr float ZOOM = 45.0f;

/**
 * \class	Camera
 *
 * \brief	A classic fps camera.
 *
 * \author	Ysl
 * \date	2018.07.19
 */

class Camera
{
public:
	// Camera Attributes
	QVector3D m_position;
	QVector3D m_front;
	QVector3D m_up;
	QVector3D m_right;
	QVector3D m_worldUp;
	// Euler Angles for a fps camera
	float m_yaw;
	float m_pitch;
	// Camera options
	float m_movementSpeed;
	float m_mouseSensitivity;
	float m_zoom;
	// Constructor with vectors
	Camera(QVector3D position = QVector3D(0.0f, 0.0f, 0.0f), QVector3D up = QVector3D(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : m_front(m_position), m_movementSpeed(SPEED), m_mouseSensitivity(SENSITIVITY), m_zoom(ZOOM)
	{
		m_position = position;
		m_worldUp = up;
		m_yaw = yaw;
		m_pitch = pitch;
		updateForFPSCameraVectors();
	}
	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : m_front(QVector3D(0.0f, 0.0f, -1.0f)), m_movementSpeed(SPEED), m_mouseSensitivity(SENSITIVITY), m_zoom(ZOOM)
	{
		m_position = QVector3D(posX, posY, posZ);
		m_worldUp = QVector3D(upX, upY, upZ);
		m_yaw = yaw;
		m_pitch = pitch;
		updateForFPSCameraVectors();
	}

	QVector3D front()const {return m_front;}
	QVector3D right()const {return m_right;}
	QVector3D up()const {return m_up;}
	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	QMatrix4x4 view()
	{
		QMatrix4x4 view;
		view.setToIdentity();
		view.lookAt(m_position, m_position + m_front, m_up);
		return view;
	}
	QVector3D position()const {return m_position;}

	void processKeyboard(CameraMovement direction, float deltaTime)
	{
		float velocity = m_movementSpeed * deltaTime;
		if (direction == CameraMovement::Forward)
			m_position += m_front * velocity;
		if (direction == CameraMovement::Backward)
			m_position -= m_front * velocity;
		if (direction == CameraMovement::Left)
			m_position -= m_right * velocity;
		if (direction == CameraMovement::Right)
			m_position += m_right * velocity;
	}
	void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
	{
		xoffset *= m_mouseSensitivity;
		yoffset *= m_mouseSensitivity;

		m_yaw += xoffset;
		m_pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (m_pitch > 89.0f)
				m_pitch = 89.0f;
			if (m_pitch < -89.0f)
				m_pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Euler angles
		updateForFPSCameraVectors();
	}
	void processMouseScroll(float yoffset)
	{
		if (m_zoom >= 1.0f && m_zoom <= 45.0f)
			m_zoom -= yoffset;
		if (m_zoom <= 1.0f)
			m_zoom = 1.0f;
		if (m_zoom >= 45.0f)
			m_zoom = 45.0f;
	}
private:
	void updateForFPSCameraVectors()
	{
		// Calculate the new Front vector
		QVector3D front;
		front.setX(cos(qDegreesToRadians(m_yaw)) * cos(qDegreesToRadians(m_pitch)));
		front.setY(sin(qDegreesToRadians(m_pitch)));
		front.setZ(sin(qDegreesToRadians(m_yaw)) * cos(qDegreesToRadians(m_pitch)));
		m_front = front.normalized();
		// Also re-calculate the Right and Up vector
		m_right = QVector3D::crossProduct(m_front, m_worldUp).normalized();
		// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		m_up = QVector3D::crossProduct(m_right, m_front).normalized();
	}
};

inline
QDebug operator<<(QDebug db, const Camera & cam) {db << "[ Pos:" << cam.position() << ",Up:" << cam.up() << ",Yaw:" << cam.m_yaw << ",Pitch:" << cam.m_pitch<<" ]";return db;}

/**
 * \class	FocusCamera
 *
 * \brief	The focus camera.
 *
 * \author	Ysl
 * \date	2018.07.19
 */

class FocusCamera
{
public:
	// Camera Attributes
	QVector3D m_position;
	QVector3D m_front;
	QVector3D m_up;
	QVector3D m_right;
	QVector3D m_worldUp;
	QVector3D m_center;

	// Camera options
	float m_movementSpeed;
	float m_mouseSensitivity;
	float m_zoom;
	// Constructor with vectors
	FocusCamera(QVector3D position = QVector3D(0.0f, 0.0f, 0.0f), QVector3D up = QVector3D(0.0f, 1.0f, 0.0f),const QVector3D & center = QVector3D(0,0,0)) 
	:m_position(position),
	m_front(center-position),
	m_worldUp(up),
	m_movementSpeed(SPEED),
	m_mouseSensitivity(SENSITIVITY), 
	m_zoom(ZOOM)
	{

		m_right = QVector3D::crossProduct(m_front, m_worldUp);
		m_up = QVector3D::crossProduct(m_right, m_front);
		//updateCameraVectors(QVector3D(0,1,0),QVector3D(0,0,0),0);
	}

	QVector3D front()const { return m_front; }
	QVector3D right()const { return m_right; }
	QVector3D up()const { return m_up; }
	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	QMatrix4x4 view()const
	{
		QMatrix4x4 view;
		view.setToIdentity();
		view.lookAt(m_position, m_position + m_front, m_up);
		return view;
	}
	QVector3D position()const { return m_position; }

	void setCenter(const QVector3D & center) {
		m_front = (center - m_position);
		m_right = QVector3D::crossProduct(m_front, m_worldUp);
		m_up = QVector3D::crossProduct(m_right, m_front);
	}

	void movement(const QVector3D & direction, float deltaTime)
	{
		auto velocity = m_movementSpeed * direction*deltaTime;
		m_position += velocity;
	}

	void rotation(float xoffset, float yoffset,const QVector3D & center)
	{
		xoffset *= m_mouseSensitivity;
		yoffset *= m_mouseSensitivity;
		double theta = 4.0 * (std::fabs(xoffset) + std::fabs(yoffset));
		QVector3D v = (m_right*xoffset) + (m_up*yoffset);
		QVector3D axis = QVector3D::crossProduct(v, -m_front).normalized();
		updateCameraVectors(axis, center, theta);

	}
	void processMouseScroll(float yoffset)
	{
		if (m_zoom >= 1.0f && m_zoom <= 45.0f)
			m_zoom -= yoffset;
		if (m_zoom <= 1.0f)
			m_zoom = 1.0f;
		if (m_zoom >= 45.0f)
			m_zoom = 45.0f;
	}
private:

	void updateCameraVectors(const QVector3D & axis, const QVector3D& center, double theta)
	{
		QMatrix4x4 rotation;
		rotation.setToIdentity();
		rotation.rotate(theta, axis);

		m_position = (rotation * m_position);
		m_front = (rotation * m_front);
		m_up = (rotation * m_up);
		m_right = QVector3D::crossProduct(m_front, m_up);
		m_up = QVector3D::crossProduct(m_right, m_front);
		m_front.normalize();
		m_right.normalize();
		m_up.normalize();
	}
};

#endif // CAMERA_H