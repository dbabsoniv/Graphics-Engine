
#include <memory>
#include "GPixel.h"
#include "GShader.h"
#include "GMatrix.h"
#include "GBitmap.h"
#include "GPoint.h"
#include <stdio.h>
//class GBitmap;
//class GMatrix;

using namespace std;

#include <stack>
#include <vector>
#include <math.h>
#include <iostream>
//std:: cout << "";
#include <string>

class theShader : public GShader {
public:

//virtual ~GShader() {}
    theShader(const GMatrix fLMInverse, const GBitmap& device, TileMode tile) : fDevice(device), fInverse(fLMInverse), tilemode(tile) {
	fInverse.invert(&fLocalMatrix);
	//on other shader, set localMatrix to [ dx -dy P0.x ]	[ dy  dx P0.y ]	[  0   0  1   ]
	//set new array of colors up

    }

    bool isOpaque() {// override = 0;
	//Return true iff all of the GPixels that may be returned by this shader will be opaque.
	return fDevice.isOpaque();
    }

    bool setContext(const GMatrix& ctm) {//override = 0;
	//The draw calls in GCanvas must call this with the CTM before any calls to shadeSpan().

	GMatrix fCTM = ctm;
//	fCTM.invert(&(fCTM));
//	fInverse.setConcat(fLocalMatrix, fCTM);
//	return true;

	GMatrix extra;
	extra.setConcat(ctm, fLocalMatrix);
	return extra.invert(&fInverse);
    }

    void shadeRow(int x, int y, int count, GPixel row[]) {//override = 0;
	// Given a row of pixels in device space [x, y] ... [x + count - 1, y], return the
	// corresponding src pixels in row[0...count - 1]. The caller must ensure that row[]
	// can hold at least [count] entries.

	GPoint point;
	GPixel pixel;
	int newX;
	int newY = (int) (y + 0.5);
	for(int i = 0; i < count; i++) {
		newX = (int) (x + i + 0.5);
		point = fInverse.mapXY(x + i + 0.5, y + 0.5);

		//clamp
		if(tilemode==kClamp) {
			if(point.fX < 0) point.fX = 0;
			if(point.fY < 0) point.fY = 0;
			if(point.fX >= fDevice.width()) point.fX = fDevice.width()-1;
			if(point.fY >= fDevice.height()) point.fY = fDevice.height()-1;
		}
		if(tilemode==kRepeat) {
			if(point.fY < 0)
				point.fY = point.fY;
			float term = (int)(point.fY) % fDevice.height();
			float extra = abs(point.fY-(int)point.fY);
			if(point.fY < 0) {
				term = fDevice.height() + ((int)(point.fY) % fDevice.height());
				extra = -extra;
			}

			point.fX = (int)(point.fX) % fDevice.width() + abs(point.fX-(int)point.fX);
			point.fY = term + extra;
		}
		if(tilemode==kMirror) {
			if((int) (point.fX / fDevice.width()) % 2 == 0)
				point.fX = (int)(point.fX) % fDevice.width() + (point.fX-(int)point.fX);
			else
				point.fX = fDevice.width() - ((int)(point.fX) % fDevice.width() + (point.fX-(int)point.fX));

			if((int) (point.fY / fDevice.height()) % 2 == 0)
                                point.fY = (int)(point.fY) % fDevice.height() + (point.fY-(int)point.fY);
                        else
                                point.fY = fDevice.height() - ((int)(point.fY) % fDevice.height() + (point.fY-(int)point.fY));

		}

		//find on fDevice
		newX = (int) (point.fX);
		newY = (int) (point.fY);
	//	pixel = fDevice.pixels()[newX + (newY*fDevice.rowBytes()/4)];

		//put into row[]
	//	row[i] = pixel;

		if(newX>=fDevice.width() || newY>=fDevice.height() || newX<0 || newY<0)
			return;
		GPixel* pixel = fDevice.getAddr(newX, newY);
		row[i] = *pixel;


	}
    }



private:
	TileMode tilemode;
	GBitmap fDevice;
	GMatrix fInverse;
	GMatrix fLocalMatrix;
};

std::unique_ptr<GShader> GCreateBitmapShader(const GBitmap& device, const GMatrix& localInv, GShader::TileMode tile) {
	// Return a subclass of GShader that draws the specified bitmap and the inverse of a local matrix.
	// Returns null if the either parameter is invalid.

	if (!device.pixels()) {
		return nullptr;
	}
	GMatrix temp = localInv;
	if (/*!HelperFunctions::isInvertable(localInv))*/!localInv.invert(&temp))
		return nullptr;
	return std::unique_ptr<GShader>(new theShader(localInv, device, tile));

}

