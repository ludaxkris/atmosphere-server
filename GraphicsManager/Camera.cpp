#include "Camera.h"

Camera::Camera(float width, float height, float FOV, float _zoom)
{	
	up=D3DXVECTOR3(0,1,0);
	look=D3DXVECTOR3(0,0,1);
	right=D3DXVECTOR3(1,0,0);
	zoom = _zoom;
	D3DXMatrixPerspectiveFovLH(&projMatrix, FOV, width/height, 1, 2000);

	D3DXMATRIX baseMatrix;
	D3DXMatrixTranslation(&baseMatrix, 0, 0, zoom);
	D3DXVECTOR3 newPos = D3DXVECTOR3(baseMatrix._41, baseMatrix._42, baseMatrix._43);

	D3DXMatrixLookAtLH(&viewMatrix, &newPos, &look, &up);
}

void Camera::CalculateViewMatrix(D3DXMATRIX transMatrix)
{
	D3DXVECTOR3 xaxis = D3DXVECTOR3(1,0,0);
	D3DXVECTOR3 yaxis = D3DXVECTOR3(0,1,0);

	D3DXVec3TransformCoord(&right, &xaxis, &transMatrix);
	D3DXVec3TransformCoord(&up, &yaxis, &transMatrix); 

	D3DXVECTOR3 newPos;
       
	D3DXMATRIX baseMatrix;										// position matrix
	D3DXMatrixTranslation(&baseMatrix, 0, 0, zoom);				// inital position
	baseMatrix = baseMatrix * transMatrix;						// translated

	look = D3DXVECTOR3(transMatrix._41, transMatrix._42, transMatrix._43);
	newPos = D3DXVECTOR3(baseMatrix._41, baseMatrix._42, baseMatrix._43);
	
	D3DXMatrixLookAtLH(&viewMatrix, &newPos, &look, &up);
}

D3DXMATRIX Camera::GetProjMatrix()
{
	return projMatrix;
}

D3DXMATRIX Camera::GetViewMatrix()
{
	return viewMatrix;
}