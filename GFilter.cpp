

#include <memory>
#include "GPixel.h"
#include "GShader.h"
#include "GMatrix.h"
#include "GBitmap.h"
#include "GPoint.h"
#include <stdio.h>
#include "GBlendMode.h"
#include "GFilter.h"
//class GBitmap;
//class GMatrix;

#include <math.h>
#include <iostream>
//std:: cout << "";
#include <string>
using namespace std;

/**
 *  GShaders create colors to fill whatever geometry is being drawn to a GCanvas.
 */

class EmptyFilter : public GFilter {
public:

    EmptyFilter(GBlendMode mode, GColor src_color) : /*blendmode(mode),*/ color(src_color) {
		blendIndex = static_cast<int>(mode);
		srcPixel = convert(color);
    }
//given a mode and src_color
//check if mode returns the same alpha as src_color
    bool preservesAlpha() override {
	return false;
	//return 0/1;
    }

    GPixel colorToPixel(GColor oldColor) {
	int newA = (int) (oldColor.fA + 0.5);
	int newR = (int) (oldColor.fR + 0.5);
	int newG = (int) (oldColor.fG + 0.5);
	int newB = (int) (oldColor.fB + 0.5);
	return GPixel_PackARGB(newA, newR, newG, newB);
    }

    GPixel convert(GColor color) {
	int SCALE = 255;
        //1st step: pin to legal units (0..1)
        //2nd step: make premul
        //3rd step: scale to 255 and round
        color = color.pinToUnit();
        int alpha = (int) (color.fA * SCALE + 0.5);
        int red = (int) (color.fR * color.fA * SCALE + 0.5);
        int green = (int) (color.fG * color.fA * SCALE + 0.5);
        int blue = (int) (color.fB * color.fA * SCALE + 0.5);
        //4th step: pack to GPixel
        return GPixel_PackARGB(alpha, red, green, blue);
    }

    void filter(GPixel output[], const GPixel input[], int count) override {
	//GPixel srcPixel = colorToPixel(color);
//	std::cout << "src pixel has been made!	--  ";
	int blendNum = blendIndex;//static_cast<int>(blendmode);
	for(int i = 0; i < count; i++)
		output[i] = doTheBlending(srcPixel, input[i], blendNum);
//	std::cout << "blendNum = " << blendNum << "\n";
  //  	std::cout << "filtering has happened \n";
    }

/*    void filterPixel(GPixel output, const GPixel input) override {
	GPixel srcPixel = colorToPixel(color);
	std::cout << "# src pixel has been made!  --  ";
	int blendNum = blendIndex;
	output = doTheBlending(srcPixel, input, blendNum);
	std::cout << "# filtering has happened \n";
    }
*/
    GPixel doTheBlending(GPixel src, GPixel des, int blendNum) {
	int SCALE = 255;

        if(blendNum == 3) { //kSrcOver
	          int newA = GPixel_GetA(src) + (SCALE - GPixel_GetA(src) ) * GPixel_GetA(des) / SCALE;
                int newR = GPixel_GetR(src) + (SCALE - GPixel_GetA(src) ) * GPixel_GetR(des) / SCALE;
                int newG = GPixel_GetG(src) + (SCALE - GPixel_GetA(src) ) * GPixel_GetG(des) / SCALE;
                int newB = GPixel_GetB(src) + (SCALE - GPixel_GetA(src) ) * GPixel_GetB(des) / SCALE;
           	return GPixel_PackARGB(newA, newR, newG, newB);
        }//!< [Sa + Da * (1 - Sa), Sc + Dc * (1 - Sa)]

        else if(blendNum == 0) { //kClear
           return GPixel_PackARGB(0,0,0,0);
        }//!< [0, 0]
        else if(blendNum == 1) { //kSrc
           return src;
        }//!< [Sa, Sc]
        else if(blendNum == 2) { //kDst
           return des;
        }//!< [Da, Dc]

        else if(blendNum == 4) { //kDstOver
                int newA = GPixel_GetA(des) + (SCALE - GPixel_GetA(des) ) * GPixel_GetA(src) / SCALE;
                int newR = GPixel_GetR(des) + (SCALE - GPixel_GetA(des) ) * GPixel_GetR(src) / SCALE;
                int newG = GPixel_GetG(des) + (SCALE - GPixel_GetA(des) ) * GPixel_GetG(src) / SCALE;
                int newB = GPixel_GetB(des) + (SCALE - GPixel_GetA(des) ) * GPixel_GetB(src) / SCALE;
           	return GPixel_PackARGB(newA, newR, newG, newB);
        }//!< [Da + Sa * (1 - Da), Dc + Sc * (1 - Da)]
        else if(blendNum == 5) { //kSrcIn
                int newA = GPixel_GetA(src) * GPixel_GetA(des) / SCALE;
                int newR = GPixel_GetR(src) * GPixel_GetA(des) / SCALE;
                int newG = GPixel_GetG(src) * GPixel_GetA(des) / SCALE;
                int newB = GPixel_GetB(src) * GPixel_GetA(des) / SCALE;
           	return GPixel_PackARGB(newA, newR, newG, newB);
        }//!< [Sa * Da, Sc * Da]
        else if(blendNum == 6) { //kDstIn
                int newA = GPixel_GetA(des) * GPixel_GetA(src) / SCALE;
                int newR = GPixel_GetR(des) * GPixel_GetA(src) / SCALE;
                int newG = GPixel_GetG(des) * GPixel_GetA(src) / SCALE;
                int newB = GPixel_GetB(des) * GPixel_GetA(src) / SCALE;
           	return GPixel_PackARGB(newA, newR, newG, newB);
        }//!< [Da * Sa, Dc * Sa]
        else if(blendNum == 7) { //kSrcOut
                int newA = (SCALE - GPixel_GetA(des) ) * GPixel_GetA(src) / SCALE;
                int newR = (SCALE - GPixel_GetA(des) ) * GPixel_GetR(src) / SCALE;
                int newG = (SCALE - GPixel_GetA(des) ) * GPixel_GetG(src) / SCALE;
                int newB = (SCALE - GPixel_GetA(des) ) * GPixel_GetB(src) / SCALE;
           	return GPixel_PackARGB(newA, newR, newG, newB);
        }//!< [Sa * (1 - Da), Sc * (1 - Da)]
        else if(blendNum == 8) { //kDstOut
                int newA = (SCALE - GPixel_GetA(src) ) * GPixel_GetA(des) / SCALE;
                int newR = (SCALE - GPixel_GetA(src) ) * GPixel_GetR(des) / SCALE;
                int newG = (SCALE - GPixel_GetA(src) ) * GPixel_GetG(des) / SCALE;
                int newB = (SCALE - GPixel_GetA(src) ) * GPixel_GetB(des) / SCALE;
           	return GPixel_PackARGB(newA, newR, newG, newB);
        }//!< [Da * (1 - Sa), Dc * (1 - Sa)]
        else if(blendNum == 9) { //kSrcATop
                int newA = GPixel_GetA(des);
                int newR = (   GPixel_GetR(src) * GPixel_GetA(des)  +   (SCALE - GPixel_GetA(src) ) * GPixel_GetR(des)  ) / SCALE;
                int newG = (   GPixel_GetG(src) * GPixel_GetA(des)  +   (SCALE - GPixel_GetA(src) ) * GPixel_GetG(des)  ) / SCALE;
                int newB = (   GPixel_GetB(src) * GPixel_GetA(des)  +   (SCALE - GPixel_GetA(src) ) * GPixel_GetB(des)  ) / SCALE;
           	return GPixel_PackARGB(newA, newR, newG, newB);
        }//!< [Da, Sc * Da + Dc * (1 - Sa)]
        else if(blendNum == 10) { //kDstATop
                int newA = GPixel_GetA(src);
                int newR = (   GPixel_GetR(des) * GPixel_GetA(src)   +   (SCALE - GPixel_GetA(des) ) * GPixel_GetR(src)  ) / SCALE;
                int newG = (   GPixel_GetG(des) * GPixel_GetA(src)   +   (SCALE - GPixel_GetA(des) ) * GPixel_GetG(src)  ) / SCALE;
                int newB = (   GPixel_GetB(des) * GPixel_GetA(src)   +   (SCALE - GPixel_GetA(des) ) * GPixel_GetB(src)  ) / SCALE;
           	return GPixel_PackARGB(newA, newR, newG, newB);
        }//!< [Sa, Dc * Sa + Sc * (1 - Da)]
        else /*if(blendNum == 11)*/ { //kXor
                int newA = (   (SCALE - GPixel_GetA(des) ) * GPixel_GetA(src)   +   (SCALE - GPixel_GetA(src) ) * GPixel_GetA(des)  ) / SCALE;
                int newR = (   (SCALE - GPixel_GetA(des) ) * GPixel_GetR(src)   +   (SCALE - GPixel_GetA(src) ) * GPixel_GetR(des)  ) / SCALE;
                int newG = (   (SCALE - GPixel_GetA(des) ) * GPixel_GetG(src)   +   (SCALE - GPixel_GetA(src) ) * GPixel_GetG(des)  ) / SCALE;
                int newB = (   (SCALE - GPixel_GetA(des) ) * GPixel_GetB(src)   +   (SCALE - GPixel_GetA(src) ) * GPixel_GetB(des)  ) / SCALE;
           	return GPixel_PackARGB(newA, newR, newG, newB);
        }//!< [Sa + Da - 2 * Sa * Da, Sc * (1 - Da) + Dc * (1 - Sa)]


    }




private:
//        GBlendmode blendmode;
	GPixel srcPixel;
	GColor color;
	int blendIndex;
};

std::unique_ptr<GFilter> GCreateBlendFilter(GBlendMode mode, const GColor& src) {
//	std::cout << " filter is created \n";
	return std::unique_ptr<GFilter>(new EmptyFilter(mode, src) );
}


