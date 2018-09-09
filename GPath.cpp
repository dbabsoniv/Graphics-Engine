//#ifndef GPath_DEFINED
//#define GPath_DEFINED

#include <vector>
#include "GPoint.h"
#include "GRect.h"
#include "GPath.h"
#include "GMatrix.h"
//class GMatrix;

//class EmptyPath : public GPath {
//public:

//    enum Direction {
//        kCW_Direction,
//        kCCW_Direction,
//    };
using namespace std;

#include <stack>
#include <vector>
#include <math.h>
#include <iostream>
//std:: cout << "";
#include <string>

GPath& GPath::addCircle(GPoint center, float radius, GPath::Direction direction) {

//	if(direction == kCW_Direction)
//		std::cout<<"drawing clockwise\n";
//	else
//		std::cout<<"drawing counter-clockwise\n";
	if(direction == kCW_Direction) {
		GPoint p[16];

		//QUADRANT 1 --> (1,0) (1, tan(pi/8)) (sqrt(2)/2, sqrt(2)/2)
		p[0].fX = 1;
		p[0].fY = 0;

		p[1].fX = 1;
		p[1].fY = std::tan(M_PI/8);
		p[2].fX = std::sqrt(2) / 2;
		p[2].fY = std::sqrt(2) / 2;

		p[3].fX = std::tan(M_PI/8);
		p[3].fY = 1;
		p[4].fX = 0;
		p[4].fY = 1;

		//QUADRANT 2 --> ()
		p[5].fX = -std::tan(M_PI/8);
		p[5].fY = 1;
		p[6].fX = -std::sqrt(2) / 2;
		p[6].fY = std::sqrt(2) / 2;

		p[7].fX = -1;
		p[7].fY = std::tan(M_PI/8);
		p[8].fX = -1;
		p[8].fY = 0;

		//QUADRANT 3
		p[9].fX = -1;
		p[9].fY = -std::tan(M_PI/8);
		p[10].fX = -std::sqrt(2) / 2;
		p[10].fY = -std::sqrt(2) / 2;

		p[11].fX = -std::tan(M_PI/8);
		p[11].fY = -1;
		p[12].fX = 0;
		p[12].fY = -1;

		//QUADRANT 4
		p[13].fX = std::tan(M_PI/8);
		p[13].fY = -1;
		p[14].fX = std::sqrt(2) / 2;
		p[14].fY = -std::sqrt(2) / 2;

		p[15].fX = 1;
		p[15].fY = -std::tan(M_PI/8);
		p[0].fX = 1;
		p[0].fY = 0;


		GPoint newPoints[16];
	        GMatrix circleMatrix;
	        circleMatrix.set6(1,0,0,0,1,0);
//	        circleMatrix.setTranslate(center.fX, center.fY);
//	        circleMatrix.setScale(radius, radius);
//		circleMatrix.mapPoints(newPoints, p, 16);

                circleMatrix.setScale(radius, radius);
                circleMatrix.mapPoints(newPoints, p, 16);
                circleMatrix.setTranslate(center.fX, center.fY);
                circleMatrix.mapPoints(newPoints, newPoints, 16);

		moveTo(newPoints[0]);
		quadTo(newPoints[1], newPoints[2]);
		quadTo(newPoints[3], newPoints[4]);
		quadTo(newPoints[5], newPoints[6]);
		quadTo(newPoints[7], newPoints[8]);
		quadTo(newPoints[9], newPoints[10]);
		quadTo(newPoints[11], newPoints[12]);
		quadTo(newPoints[13], newPoints[14]);
		quadTo(newPoints[15], newPoints[0]);
	}
	if(direction == kCCW_Direction) {


		GPoint p[16];

                //QUADRANT 1 --> (1,0) (1, tan(pi/8)) (sqrt(2)/2, sqrt(2)/2)
                p[0].fX = 1;
                p[0].fY = 0;

                p[15].fX = 1;
                p[15].fY = std::tan(M_PI/8);
                p[14].fX = std::sqrt(2) / 2;
                p[14].fY = std::sqrt(2) / 2;

                p[13].fX = std::tan(M_PI/8);
                p[13].fY = 1;
                p[12].fX = 0;
                p[12].fY = 1;

                //QUADRANT 2 --> ()
                p[11].fX = -std::tan(M_PI/8);
                p[11].fY = 1;
                p[10].fX = -std::sqrt(2) / 2;
                p[10].fY = std::sqrt(2) / 2;

                p[9].fX = -1;
                p[9].fY = std::tan(M_PI/8);
                p[8].fX = -1;
                p[8].fY = 0;


		//QUADRANT 3
                p[7].fX = -1;
                p[7].fY = -std::tan(M_PI/8);
                p[6].fX = -std::sqrt(2) / 2;
                p[6].fY = -std::sqrt(2) / 2;

                p[5].fX = -std::tan(M_PI/8);
                p[5].fY = -1;
                p[4].fX = 0;
                p[4].fY = -1;

                //QUADRANT 4
                p[3].fX = std::tan(M_PI/8);
                p[3].fY = -1;
                p[2].fX = std::sqrt(2) / 2;
                p[2].fY = -std::sqrt(2) / 2;

                p[1].fX = 1;
                p[1].fY = -std::tan(M_PI/8);
                p[0].fX = 1;
                p[0].fY = 0;


                GPoint newPoints[16];
                GMatrix circleMatrix;
                circleMatrix.set6(1,0,0,0,1,0);
//                circleMatrix.setTranslate(center.fX, center.fY);
//                circleMatrix.setScale(radius, radius);
//                circleMatrix.mapPoints(newPoints, p, 16);
                circleMatrix.setScale(radius, radius);
                circleMatrix.mapPoints(newPoints, p, 16);
                circleMatrix.setTranslate(center.fX, center.fY);
                circleMatrix.mapPoints(newPoints, newPoints, 16);

                moveTo(newPoints[0]);
                quadTo(newPoints[1], newPoints[2]);
                quadTo(newPoints[3], newPoints[4]);
                quadTo(newPoints[5], newPoints[6]);
                quadTo(newPoints[7], newPoints[8]);
                quadTo(newPoints[9], newPoints[10]);
                quadTo(newPoints[11], newPoints[12]);
                quadTo(newPoints[13], newPoints[14]);
                quadTo(newPoints[15], newPoints[0]);

	}



	return *this;

}





GPoint findTPoint(GPoint a, GPoint b, float t) {
        GPoint ab;
        ab.fY = a.fY + t*(b.fY - a.fY);
        ab.fX = a.fX + t*(b.fX - a.fX);
        return ab;
}

void GPath::ChopQuadAt(const GPoint src[3], GPoint dst[5], float t) {
	dst[0] = src[0];
	dst[1] = findTPoint(src[0], src[1], t);
	dst[3] = findTPoint(src[1], src[2], t);
	dst[4] = src[2];

	dst[2] = findTPoint(dst[1], dst[3], t);
}

void GPath::ChopCubicAt(const GPoint src[4], GPoint dst[7], float t) {
	dst[0] = src[0];
	dst[1] = findTPoint(src[0], src[1], t);
	dst[5] = findTPoint(src[2], src[3], t);
	dst[6] = src[3];

	GPoint bc = findTPoint(src[1], src[2], t);
	dst[2] = findTPoint(dst[1], bc, t);
	dst[4] = findTPoint(bc, dst[5], t);

	dst[3] = findTPoint(dst[2], dst[4], t);
}












    /**
     *  Adds 4 points to the path, traversing the rect in the specified direction, beginning
     *  with the top-left corner.
     */
    GPath& GPath::addRect(const GRect& rect, Direction direction) {
	GPoint topLef;  topLef.fY = rect.top();      topLef.fX = rect.left();
        GPoint topRig;  topRig.fY = rect.top();      topRig.fX = rect.right();
        GPoint botLef;  botLef.fY = rect.bottom();   botLef.fX = rect.left();
        GPoint botRig;  botRig.fY = rect.bottom();   botRig.fX = rect.right();

	moveTo(topLef);
	if(direction == kCW_Direction) {
		lineTo(topRig);
		lineTo(botRig);
		lineTo(botLef);
	}
	else {
		lineTo(botLef);
		lineTo(botRig);
		lineTo(topRig);
	}

	return *this;

    }

    // moveTo(pts[0]), lineTo(pts[1..count-1])
    GPath& GPath::addPolygon(const GPoint pts[], int count) {
	moveTo(pts[0]);
	for(int i=1; i<count; i++)
		lineTo(pts[i]);
	return *this;
    }

    /**
     *  Return the bounds of all of the control-points in the path.
     *
     *  If there are no points, return {0, 0, 0, 0}
     */
    GRect GPath::bounds() const {
	GRect bounds;
	bounds.fTop = 0;
	bounds.fBottom = 0;
	bounds.fLeft = 0;
	bounds.fRight = 0;
	if(fPts.size() == 0)
		return bounds;
	bounds.fTop = 10000;
	bounds.fLeft = 10000;

	for(int i=0; i<fPts.size(); i++) {
		GPoint currPoint = fPts.at(i);
		if(currPoint.fY < bounds.fTop)
			bounds.fTop = currPoint.fY;
		if(currPoint.fY > bounds.fBottom)
                        bounds.fBottom = currPoint.fY;
		if(currPoint.fX < bounds.fLeft)
                        bounds.fLeft = currPoint.fX;
		if(currPoint.fX > bounds.fRight)
                        bounds.fRight = currPoint.fX;
	}
//	bounds.top() = 0;
//	bounds.bottom() = 0;
//	bounds
	return bounds;
    }

    /**
     *  Transform the path in-place by the specified matrix.
     */
    void GPath::transform(const GMatrix& matrix) {
//	GMatrix nonConstMatrix = matrix;

	for(int i=0; i<fPts.size(); i++)
		fPts.at(i) = matrix.mapPt(fPts.at(i));
    }
/*
    enum Verb {
        kMove,  // returns pts[0] from Iter
        kLine,  // returns pts[0]..pts[1] from Iter
        kDone   // returns nothing in pts, Iter is done
    };

    /**
     *  Walks the path, returning each verb that was entered.
     *  e.g.    moveTo() returns kMove
     *          lineTo() returns kLine
     */
/*    class Iter {
    public:
//        Iter(const GPath&);
//        Verb next(GPoint pts[]);

    private:
        const GPoint* fPrevMove;
        const GPoint* fCurrPt;
        const Verb*   fCurrVb;
        const Verb*   fStopVb;
    };

    /**
     *  Walks the path, returning "edges" only. Thus it does not return kMove, but will return
     *  the final closing "edge" for each contour.
     *  e.g.
     *      path.moveTo(A).lineTo(B).lineTo(C).moveTo(D).lineTo(E)
     *  will return
     *      kLine   A..B
     *      kLine   B..C
     *      kLine   C..A
     *      kLine   D..E
     *      kLine   E..D
     *      kDone
     */
/*    class GPath::Edger {
    public:
        Edger(const GPath&);
        Verb next(GPoint pts[]);

    private:
        const GPoint* fPrevMove;
        const GPoint* fCurrPt;
        const Verb*   fCurrVb;
        const Verb*   fStopVb;
        Verb fPrevVerb;
    };
*/
//private:
//    std::vector<GPoint> fPts;
//   std::vector<Verb>   fVbs;
//};

//#endif
