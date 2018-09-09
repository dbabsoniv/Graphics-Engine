#include "GMatrix.h"
#include <cmath>

#include <iostream>
//std:: cout << "";
#include <string>
using namespace std;

    // These 7 methods must be implemented by the student.

    void GMatrix::setIdentity() {
    //Set this matrix to identity.
	set6(1, 0, 0, 0, 1, 0);
    }

    void GMatrix::setTranslate(float tx, float ty) {
    //Set this matrix to translate by the specified amounts.

	set6(1, 0, tx, 0, 1, ty);
    }

    void GMatrix::setScale(float sx, float sy) {
    //Set this matrix to scale by the specified amounts.

	set6(sx, 0, 0, 0, sy, 0);
    }

    void GMatrix::setRotate(float radians) {
    // Set this matrix to rotate by the specified radians.
    // Note: since positive-Y goes down, a small angle of rotation will increase Y.
	//float degrees = radians * 57.295779513;

	set6(cos(radians), -sin(radians), 0, sin(radians), cos(radians), 0);
    }

    void GMatrix::setConcat(const GMatrix& secundo, const GMatrix& primo) {
	// Set this matrix to the concatenation of the two specified matrices, such that the resulting
	// matrix, when applied to points will have the same effect as first applying the primo matrix
	// to the points, and then applying the secundo matrix to the resulting points.
	// Pts' = Secundo * Primo * Pts

//	std::cout << secundo[0]<<" "<<secundo[1]<<" "<<secundo[2]<<"\n";


	float a = (secundo[0] * primo[0])  +  (secundo[1] * primo[3]);
	float b = (secundo[0] * primo[1])  +  (secundo[1] * primo[4]);
	float c = (secundo[0] * primo[2])  +  (secundo[1] * primo[5]) + secundo[2];
	float d = (secundo[3] * primo[0])  +  (secundo[4] * primo[3]);
	float e = (secundo[3] * primo[1])  +  (secundo[4] * primo[4]);
	float f = (secundo[3] * primo[2])  +  (secundo[4] * primo[5]) + secundo[5];

//	std::cout<<"a="<<a<<"\n";

	set6(a,b,c,d,e,f);
    }

    bool GMatrix::invert(GMatrix* inverse) const{
	//determinant = 0*(4*8 - 5*7) - 1*(3*8 - 5*6) + 2*(3*7 - 4*6)
//	float det = fMat[0]*(fMat[4]*1/ - fMat[5]*0) - fMat[1]*(fMat[3]*1 - fMat[5]*0) + fMat[2]*(fMat[3]*0 - fMat[4]*0);
	//		0 (4)				- 1 (3)
	float det = (fMat[0] * fMat[4]) - (fMat[1] * fMat[3]);
	// If this matrix is not invertible, return false and ignore the inverse parameter.
	if(det == 0)
		return false;

	// If this matrix is invertible, return true and (if not null) set the inverse parameter.
	float a = fMat[4];
	float b = -fMat[1];
	float c = fMat[1]*fMat[5] - fMat[2]*fMat[4];
	float d = -fMat[3];
	float e = fMat[0];
	float f = fMat[2]*fMat[3] - fMat[0]*fMat[5];

	float fDet = 1.0/det;

	inverse -> set6(a*fDet, b*fDet, c*fDet, d*fDet, e*fDet, f*fDet);
	return true;
    }

    void GMatrix::mapPoints(GPoint dst[], const GPoint src[], int count) const{




	// Transform the set of points in src, storing the resulting points in dst, by applying this
	// matrix. It is the caller's responsibility to allocate dst to be at least as large as src.
	//
	// Note: It is legal for src and dst to point to the same memory (however, they may not
	// partially overlap). Thus the following is supported.
	//
	// GPoint pts[] = { ... };
	// matrix.mapPoints(pts, pts, count);


	//see iphone photo from feb 22
	float newX, newY;
	GPoint temp;
	for(int i=0; i<count; i++) {
		temp = src[i];
		newX = fMat[0] * temp.fX + fMat[1] * temp.fY + fMat[2];
		newY = fMat[3] * temp.fX + fMat[4] * temp.fY + fMat[5];
		temp.fX = newX;
		temp.fY = newY;
		dst[i] = temp;
	}
    }

