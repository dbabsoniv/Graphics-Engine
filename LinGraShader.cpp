
#include <memory>
#include "GPixel.h"
#include "GShader.h"
#include "GMatrix.h"
#include "GBitmap.h"
#include "GPoint.h"
#include <stdio.h>
#include <iostream>
#include "GColor.h"
#include "GMath.h"
//class GBitmap;
using namespace std;
//class GMatrix;

class LinGraShader : public GShader {
public:

//virtual ~GShader() {}
    LinGraShader(GPoint p0, GPoint p1, const GColor colorArray[], int count, TileMode tile) {
			//	fInverse.invert(&fLocalMatrix);
	//on other shader, set localMatrix to [ dx -dy P0.x ]	[ dy  dx P0.y ]	[  0   0  1   ]
	tilemode = tile;
	one = p0;
	two = p1;
	colorCount = count;
	colors = (GColor*) malloc(count*sizeof(GColor));
	for(int i=0; i<colorCount; i++)
		colors[i] = colorArray[i];


	float dx = two.fX - one.fX;
	float dy = two.fY - one.fY;
	fLocalMatrix.set6(dx, -dy, one.fX, dy, dx, one.fY);
	//set new array of colors up

    }

    bool isOpaque() {// override = 0;
	//Return true iff all of the GPixels that may be returned by this shader will be opaque.
	return false;
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

	for(int i=0; i<count; i++) {
		//apply matrix to points, find t
		GPoint local = fInverse.mapXY(x+i+0.5, y+0.5);
		float t;
		if(tilemode == kClamp)
			t = GPinToUnit(local.fX);
		if(tilemode == kRepeat) {
			t = local.fX - ((int) local.fX);
			if(t<0)
				t = 1 + t;
	/*		if(local.fY < 0)
                                local.fY = local.fY;
                        float term = (int)(local.fY) % 1;
                        float extra = abs(point.fY-(int)point.fY);
                        if(point.fY < 0) {
                                term = fDevice.height() + ((int)(point.fY) % fDevice.height());
                                extra = -extra;
                        }

                        point.fX = (int)(point.fX) % fDevice.width() + abs(point.fX-(int)point.fX);
                        point.fY = term + extra;
*/		}
		if(tilemode == kMirror) {
			t = local.fX;
			if((int) t % 2 == 0) // int t is even
				t = t - ((int) t);
			else
				t = 1 - (t - ((int) t));
		}


		//find color immediately before (c) and after (c+1) t
		int colorIndex = floor( t*(colorCount-1) );
//		std::cout<<"  t = "<<t<<"\n";//  ---   colorIndex="<<colorIndex<<" when there are "<<colorCount<<" total colors\n";

		if(colorIndex < 0 || colorIndex > colorCount-1) {
			std::cout<<"shadeRow linear gradient bug!\n";
			if(tilemode==kClamp) std::cout<<"       in kClamp\n";
			if(tilemode==kRepeat) std::cout<<"       in kRepeat\n";
			if(tilemode==kMirror) std::cout<<"       in kMirror\n";
		}
		GColor leftColor = colors[colorIndex];
		GColor rightColor = colors[colorIndex+1];

		//recalculate t for distance between color left and right
		float factor = 1.0/(colorCount-1);
		t = (t - factor*colorIndex) / factor;

		//use t as % to calculate exact val
		int newA = 255*( leftColor.fA * (1-t) + rightColor.fA * t);
		int newR = 255*( leftColor.fR * (1-t) + rightColor.fR * t);
		int newG = 255*( leftColor.fG * (1-t) + rightColor.fG * t);
		int newB = 255*( leftColor.fB * (1-t) + rightColor.fB * t);
		if(newA>=0 && newR>=0 && newG>=0 && newB>=0)
			row[i] = GPixel_PackARGB(newA, newR, newG, newB);

		
//		std::cout<<"left Color  (a, r, b, g) -- ("<<leftColor.fA<<", "<<leftColor.fR<<", "<<leftColor.fG<<", "<<leftColor.fB<<")\n";
//		std::cout<<"right color (a, r, b, g) -- ("<<rightColor.fA<<", "<<rightColor.fR<<", "<<rightColor.fG<<", "<<rightColor.fB<<")\n";
//		std::cout<<"t="<<t<<":  (a, r, b, g) -- ("<<newA<<", "<<newR<<", "<<newG<<", "<<newB<<")\n\n";
	}

    }



private:
	TileMode tilemode;
	GPoint one, two;
	GColor *colors;
	int colorCount;

	GMatrix fInverse;
	GMatrix fLocalMatrix;
};


std::unique_ptr<GShader> GCreateLinearGradient(GPoint p0, GPoint p1, const GColor colorArray[], int count, GShader::TileMode tile) {
//clamp points if out
//	GColor colorArray[count];
	return std::unique_ptr<GShader>(new LinGraShader(p0, p1, colorArray, count, tile));

	//shadeRow:
		//for loop 0 to count
		//make local Point
}

