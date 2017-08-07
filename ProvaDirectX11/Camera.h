#pragma once

#include <DirectXMath.h>
#include "Clock.h"

using namespace DirectX;

class Camera
{
private:
	// matrici
	XMMATRIX view;

	float rotationX, rotationY, rotationZ;
	float positionX, positionY, positionZ;

	float forwardSpeed, backwardSpeed, salita, discesa, rightSpeed, leftSpeed, lookUpSpeed, lookDownSpeed;

	float frameTime;
public:

	Clock clock;

	Camera();
	~Camera();

	void Render();
	void setTime();
	XMMATRIX GetViewMatrix();
	
	void SetPosition(float, float, float);
	void SetRotation(float, float, float);
	
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	

	void avanti(bool);
	void indietro(bool);
	void su(bool);
	void giu(bool);
	void sinistra(bool);
	void destra(bool);
	void ruotaSu(bool);
	void ruotaGiu(bool);

};

