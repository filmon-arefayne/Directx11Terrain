#include "Camera.h"



Camera::Camera():

positionX(0.0f),
positionY(10.0f),
positionZ(-2.0f),

rotationX(0.0f),
rotationY(0.0f),
rotationZ(0.0f),

forwardSpeed(0.0f),
backwardSpeed(0.0f),
salita(0.0f),
discesa(0.0f),
leftSpeed(0.0f),
rightSpeed(0.0f),
lookDownSpeed(0.0f),
lookUpSpeed(0.0f)
{
	
}


Camera::~Camera()
{

}

void Camera::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Load it into a XMVECTOR structure.
	upVector = XMLoadFloat3(&up);

	// Setup the position of the camera in the world.
	position.x = positionX;
	position.y = positionY;
	position.z = positionZ;

	// Load it into a XMVECTOR structure.
	positionVector = XMLoadFloat3(&position);

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Load it into a XMVECTOR structure.
	lookAtVector = XMLoadFloat3(&lookAt);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = rotationX * 0.0174532925f;
	yaw = rotationY * 0.0174532925f;
	roll = rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// Finally create the view matrix from the three updated vectors.
	view= XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	
	
}
XMMATRIX Camera::GetViewMatrix()
{
	return view;
}

void Camera::SetPosition(float x, float y, float z)
{
	positionX = x;
	positionY = y;
	positionZ = z;

}


void Camera::SetRotation(float x, float y, float z)
{
	rotationX = x;
	rotationY = y;
	rotationZ = z;

}

void Camera::setTime()
{
	frameTime = clock.update();
}

void Camera::avanti(bool key)
{


	float radians;


	if (key)
	{
		forwardSpeed += frameTime * 1.0f;

		if (forwardSpeed > (frameTime * 50.0f))
		{
			forwardSpeed = frameTime * 50.0f;
		}
	}
	else
	{
			forwardSpeed -= frameTime * 0.5f;

			if (forwardSpeed < 0.0f)
			{
				forwardSpeed = 0.0f;
			}
	}
		radians = rotationY * 0.0174532925f;

		// Update the position.
		positionX += sinf(radians) * forwardSpeed;
		positionZ += cosf(radians) * forwardSpeed;
}

void Camera::indietro(bool keydown)
{
	float radians;


	// Update the backward speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown)
	{
		backwardSpeed += frameTime * 1.0f;

		if (backwardSpeed > (frameTime * 50.0f))
		{
			backwardSpeed = frameTime * 50.0f;
		}
	}
	else
	{
		backwardSpeed -= frameTime * 0.5f;

		if (backwardSpeed < 0.0f)
		{
			backwardSpeed = 0.0f;
		}
	}

	// Convert degrees to radians.
	radians = rotationY * 0.0174532925f;

	// Update the position.
	positionX -= sinf(radians) * backwardSpeed;
	positionZ -= cosf(radians) * backwardSpeed;

	
}
void Camera::su(bool keydown)
{
	// Update the upward speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown)
	{
		salita += frameTime * 1.5f;

		if (salita > (frameTime * 15.0f))
		{
			salita = frameTime * 15.0f;
		}
	}
	else
	{
		salita -= frameTime * 0.5f;

		if (salita < 0.0f)
		{
			salita = 0.0f;
		}
	}

	// Update the height position.
	positionY += salita;

	return;
}


void Camera::giu(bool keydown)
{
	// Update the downward speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown)
	{
		discesa += frameTime * 1.5f;

		if (discesa > (frameTime * 15.0f))
		{
			discesa = frameTime * 15.0f;
		}
	}
	else
	{
		discesa -= frameTime * 0.5f;

		if (discesa < 0.0f)
		{
			discesa = 0.0f;
		}
	}

	// Update the height position.
	positionY -= discesa;

	return;
}


void Camera::sinistra(bool keydown)
{
	// Update the left turn speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown)
	{
		leftSpeed += frameTime * 5.0f;

		if (leftSpeed > (frameTime * 150.0f))
		{
			leftSpeed = frameTime * 150.0f;
		}
	}
	else
	{
		leftSpeed -= frameTime* 3.5f;

		if (leftSpeed < 0.0f)
		{
			leftSpeed = 0.0f;
		}
	}

	// Update the rotation.
	rotationY -= leftSpeed;

	// Keep the rotation in the 0 to 360 range.
	if (rotationY < 0.0f)
	{
		rotationY += 360.0f;
	}

	return;
}


void Camera::destra(bool keydown)
{
	// Update the right turn speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown)
	{
		rightSpeed += frameTime * 5.0f;

		if (rightSpeed > (frameTime * 150.0f))
		{
			rightSpeed = frameTime * 150.0f;
		}
	}
	else
	{
		rightSpeed -= frameTime* 3.5f;

		if (rightSpeed < 0.0f)
		{
			rightSpeed = 0.0f;
		}
	}

	// Update the rotation.
	rotationY += rightSpeed;

	// Keep the rotation in the 0 to 360 range.
	if (rotationY > 360.0f)
	{
		rotationY -= 360.0f;
	}

	return;
}


void Camera::ruotaSu(bool keydown)
{
	// Update the upward rotation speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown)
	{
		lookUpSpeed += frameTime * 7.5f;

		if (lookUpSpeed > (frameTime * 75.0f))
		{
			lookUpSpeed = frameTime * 75.0f;
		}
	}
	else
	{
		lookUpSpeed -= frameTime* 2.0f;

		if (lookUpSpeed < 0.0f)
		{
			lookUpSpeed = 0.0f;
		}
	}

	// Update the rotation.
	rotationX -= lookUpSpeed;

	// Keep the rotation maximum 90 degrees.
	if (rotationX > 90.0f)
	{
		rotationX = 90.0f;
	}

	return;
}


void Camera::ruotaGiu(bool keydown)
{
	// Update the downward rotation speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown)
	{
		lookDownSpeed += frameTime * 7.5f;

		if (lookDownSpeed > (frameTime * 75.0f))
		{
			lookDownSpeed = frameTime * 75.0f;
		}
	}
	else
	{
		lookDownSpeed -= frameTime* 2.0f;

		if (lookDownSpeed < 0.0f)
		{
			lookDownSpeed = 0.0f;
		}
	}

	// Update the rotation.
	rotationX += lookDownSpeed;

	// Keep the rotation maximum 90 degrees.
	if (rotationX < -90.0f)
	{
		rotationX = -90.0f;
	}

	return;
}

XMFLOAT3 Camera::GetPosition()
{
	return XMFLOAT3(positionX, positionY, positionZ);
}


XMFLOAT3 Camera::GetRotation()
{
	return XMFLOAT3(rotationX, rotationY, rotationZ);
}