#include "GCanvas.h"
#include "GBitmap.h"
#include "GPaint.h"
#include "GColor.h"
#include "GPixel.h"
#include "GPoint.h"
#include "GRect.h"
#include "GTime.h"
#include "GTypes.h"
//#include "GWindow.h"
#include "GShader.h"
#include "GFilter.h"
#include "GPath.h"

#include <stack>
#include <vector>
#include <math.h>
#include <iostream>
//std:: cout << "";
#include <string>
using namespace std;

//#include "GMatrix.h"

struct Layer {
	GBitmap bitmap;
	GMatrix ctmMatrix;
	int x, y;
	GPaint paint;
	bool isLayer; //true if complicated: need to draw onto previous layer
} ;

struct Edge{
        int top;
        int bottom;
        float currentX;
        float slope;

        friend bool operator< (const Edge one, const Edge two) {
                if(one.top == two.top)	return one.currentX < two.currentX;
                else			return one.top < two.top;
        }

	int windVal;
} ;

class EmptyCanvas : public GCanvas {
const static int SCALE = 255;
public:
    EmptyCanvas(const GBitmap& device) /*: baseDevice(device)*/ {
	Layer deviceLayer;
	deviceLayer.bitmap = device;
	deviceLayer.ctmMatrix = identityMatrix;
	deviceLayer.x = 0;
	deviceLayer.y = 0;
//	deviceLayer.paint = nullptr;
	deviceLayer.isLayer = false;

	layerStack.push(deviceLayer);
    }


    void final_addStrokedLine(GPath* path, GPoint p0, GPoint p1, float width, bool roundCap) {
	float radius = width / 2;
	
	float lineSlope = 696969;
	if(p1.fX - p0.fX != 0)
		lineSlope = (p1.fY - p0.fY) / (p1.fX - p0.fX);
	float perpendicular = 420420;
	if(lineSlope != 0)
		perpendicular = -1.0 / lineSlope;

//	std::cout<<"slope: "<<lineSlope<<"   --   perpendicular: "<<perpendicular<<"\n";
//	radius^2 = xInterval^2 + (xInterval*perpendicular)^2
//	radius^2 = xInt^2 + (xInt^2 * perp^2)
//	radius^2 = xInt^2 * (1 + perp^2)
//	xInt^2 = radius^2 / (1 + perp^2)

	float xInterval = sqrt( radius*radius / (1+(perpendicular*perpendicular)) );//check if perp = 420420
	float yInterval = xInterval * perpendicular;
//	std::cout<<"x: "<<xInterval<<"           y: "<<yInterval<<"        perp: "<<perpendicular<<"           radius: "<<radius<<"\n";

	GPoint p0plus, p0minus, p1plus, p1minus;
	p0plus.fX = p0.fX + xInterval;
	p0plus.fY = p0.fY + yInterval;
	p0minus.fX = p0.fX - xInterval;
	p0minus.fY = p0.fY - yInterval;

	p1plus.fX = p1.fX + xInterval;
	p1plus.fY = p1.fY + yInterval;
	p1minus.fX = p1.fX - xInterval;
	p1minus.fY = p1.fY - yInterval;

//	GPath strokePath;
	path->moveTo(p0plus);
	path->lineTo(p1plus);
	path->lineTo(p1minus);
	path->lineTo(p0minus);

	if(roundCap) {
		if(p0.fY >= p1.fY) {
			path->addCircle(p0, radius, GPath::kCCW_Direction);
			path->addCircle(p1, radius, GPath::kCCW_Direction);
		}
		else {
			path->addCircle(p0, radius, GPath::kCW_Direction);
                        path->addCircle(p1, radius, GPath::kCW_Direction);
		}
	}

    }

/*
    void addSemiCircle(GPath* path, GPoint center, float radius) {
	GPoint p[8];

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
//		p[8].fX = -1;
//		p[8].fY = 0;

		GPoint newPoints[8];
	        GMatrix circleMatrix;
	        circleMatrix.set6(1,0,0,0,1,0);
//	        circleMatrix.setTranslate(center.fX, center.fY);
//	        circleMatrix.setScale(radius, radius);
//		circleMatrix.mapPoints(newPoints, p, 16);

                circleMatrix.setScale(radius, radius);
                circleMatrix.mapPoints(newPoints, p, 16);
                circleMatrix.setTranslate(center.fX, center.fY);
                circleMatrix.mapPoints(newPoints, newPoints, 16);
		

		path->moveTo(newPoints[0]);
//		path->quadTo(newPoints[0], newPoints[1]);
		path->quadTo(newPoints[1], newPoints[2]);
//		path->quadTo(newPoints[2], newPoints[3]);
		path->quadTo(newPoints[3], newPoints[4]);
//		path->quadTo(newPoints[4], newPoints[5]);
		path->quadTo(newPoints[5], newPoints[6]);
//		path->quadTo(newPoints[6], newPoints[7]);
		path->lineTo(newPoints[7]);
		path->lineTo(newPoints[0]);

    }

*/





    GBitmap setupBitmap(int w, int h) {
	GBitmap fBitmap;
	if (fBitmap.pixels()) {
		free(fBitmap.pixels());
	}
	size_t rb = w * sizeof(GPixel);
	fBitmap.reset(w, h, rb, (GPixel*)calloc(h, rb), GBitmap::kNo_IsOpaque);
	return fBitmap;
    }

    void save() override {

	Layer newSave;
	newSave.ctmMatrix = layerStack.top().ctmMatrix;
	newSave.isLayer = false;

        newSave.bitmap = layerStack.top().bitmap;
        newSave.x = layerStack.top().x;
	newSave.y = layerStack.top().y;
        newSave.paint = layerStack.top().paint;

	layerStack.push(newSave);
    }

    void restore() override {

	if(layerStack.size() == 1) {
		layerStack.top().ctmMatrix = identityMatrix;
		return;
	}

	if( !layerStack.top().isLayer) {
		Layer tempLayer = layerStack.top();
		layerStack.pop();

		layerStack.top().bitmap = tempLayer.bitmap;
		return;
	}


        GFilter* filterObject = layerStack.top().paint.getFilter();
        bool isFilter = false;
        if( filterObject != nullptr )
                isFilter = true;

	Layer tempLayer = layerStack.top();
	int xCoord = tempLayer.x;
	int yCoord = tempLayer.y;

        layerStack.pop();

	//draws the layer onto the prvious layer in correct position
	ModeProc blendMode = proc[static_cast<int>(tempLayer.paint.getBlendMode())];
	for(int yIterator = 0; yIterator < tempLayer.bitmap.height(); yIterator++)
		for(int xIterator = 0; xIterator < tempLayer.bitmap.width(); xIterator++)
			if(isLegalPoint(xCoord + xIterator, yCoord + yIterator)) {
				GPixel* oldPixel = layerStack.top().bitmap.getAddr(xCoord + xIterator, yCoord + yIterator);
				GPixel* newPixel = tempLayer.bitmap.getAddr(xIterator, yIterator);
				GPixel newPixelArray[1] = {*newPixel};
				if(isFilter)
					filterObject -> filter(newPixelArray, newPixelArray, 1);
				*oldPixel = blendMode(newPixelArray[0], *oldPixel);
			}


    }

    void onSaveLayer(const GRect* bounds, const GPaint& paint) override {
	//turn bounds into points and run through ctm

	int x = 0;
	int y = 0;
	int width = layerStack.top().bitmap.width();
	int height = layerStack.top().bitmap.height();

	if(bounds) {

		//create GRect out of current bounds to interect with new bounds
		GRect oldBounds;
		oldBounds.fTop = 0;
		oldBounds.fLeft = 0;
		oldBounds.fRight = width;
		oldBounds.fBottom = height;
		GRect newBounds = *bounds;

		//intersect with real bounds
		if(! newBounds.intersect(oldBounds) )
			return; //does the intersecting, and quits if no overlap

		//create GPoints for the rectangle
		GPoint topLef, topRig, botLef, botRig;
		topLef.fY = bounds->top();	topLef.fX = bounds->left();
		topRig.fY = bounds->top();	topRig.fX = bounds->right();
		botLef.fY = bounds->bottom();	botLef.fX = bounds->left();
		botRig.fY = bounds->bottom();	botRig.fX = bounds->right();

		GPoint vertices[4] = {topLef, topRig, botRig, botLef};
		layerStack.top().ctmMatrix.mapPoints(vertices, vertices, 4);

		float topEdge = vertices[0].fY;
		float botEdge = vertices[0].fY;
		float rigEdge = vertices[0].fX;
		float lefEdge = vertices[0].fX;
		for(int i = 1; i<4; i++) {
			if(topEdge > vertices[i].fY)	topEdge = vertices[i].fY;
			if(botEdge < vertices[i].fY)	botEdge = vertices[i].fY;
			if(lefEdge > vertices[i].fX)	lefEdge = vertices[i].fX;
			if(rigEdge < vertices[i].fX)	rigEdge = vertices[i].fX;
		}

		y = (int) (topEdge + 0.5);
		x = (int) (lefEdge + 0.5);

		//add bitmap to top of stack
		width  = (int)(rigEdge + 0.5) - (int)(lefEdge + 0.5);
		height = (int)(botEdge + 0.5) - (int)(topEdge + 0.5);
	}

	Layer newLayer;
	newLayer.bitmap = setupBitmap(width, height);
	newLayer.ctmMatrix = layerStack.top().ctmMatrix;;
	newLayer.x = x;
	newLayer.y = y;
	newLayer.paint = paint;
	newLayer.isLayer = true;

	layerStack.push(newLayer);
	layerStack.top().ctmMatrix.postTranslate(-x, -y);//		its gotta be negative!

	return;
    }


    void concat(const GMatrix& matrix) override {
	layerStack.top().ctmMatrix.setConcat(layerStack.top().ctmMatrix, matrix);
    }


    typedef GPixel (*ModeProc)(const GPixel&, const GPixel&);
    const ModeProc proc[12] = {kClear, kSrc, kDst, kSrcOver, kDstOver, kSrcIn, kDstIn, kSrcOut, kDstOut, kSrcATop, kDstATop, kXor};

    void drawPaint(const GPaint& paint) override {
	GBitmap map = layerStack.top().bitmap;
	GPoint topRig;	topRig.fY = 0;			topRig.fX = map.width();
	GPoint topLef;	topLef.fY = 0;			topLef.fX = 0;
	GPoint botRig;	botRig.fY = map.height();	botRig.fX = map.width();
	GPoint botLef;	botLef.fY = map.height();	botLef.fX = 0;

	const GPoint pointsArray[4] = {topLef, topRig, botRig, botLef};
	drawConvexPolygon(pointsArray, 4, paint);
    }

    void drawRect(const GRect& rect, const GPaint& paint) override {
	GPoint topLef;	topLef.fY = rect.top();      topLef.fX = rect.left();
        GPoint topRig;	topRig.fY = rect.top();      topRig.fX = rect.right();
        GPoint botLef;	botLef.fY = rect.bottom();   botLef.fX = rect.left();
        GPoint botRig;	botRig.fY = rect.bottom();   botRig.fX = rect.right();

	const GPoint rectPoints[4] = {topLef, topRig, botRig, botLef};
	drawConvexPolygon(rectPoints, 4, paint);
    }

    void drawConvexPolygon(const GPoint points[], int count, const GPaint& paint) override {
        ModeProc blendMode = proc[static_cast<int>(paint.getBlendMode())];
        int rowBytes = layerStack.top().bitmap.rowBytes();
        if(count < 3) return;

	//set up Shader and Filter Objects
	GShader* shaderObject = paint.getShader();
	GFilter* filterObject = paint.getFilter();
	bool isShader = false;
	bool isFilter = false;
	if( shaderObject != nullptr ) {
		if(!shaderObject -> setContext(layerStack.top().ctmMatrix))
			return;
		isShader = true;
	}
	if( filterObject != nullptr )
		isFilter = true;

	GPoint newPoints[count];
	layerStack.top().ctmMatrix.mapPoints(newPoints, points, count);

        //Turn points into edges, add to EdgeVector
        vector<Edge> edgeVector(0);
        vector<Edge> tempVector(0);
        for(int i = 0; i < count-1; i++) {
                tempVector = clip(newPoints[i], newPoints[i+1]);
                for(int j = 0; j < tempVector.size(); j++) {
                        edgeVector.push_back(tempVector.at(j));
//			Edge currentEdge = tempVector.at(j);
//			float bottomX = currentEdge.currentX + (currentEdge.slope * (currentEdge.bottom-currentEdge.top-1));
//			if(bottomX <= 0 || bottomX >= layerStack.top().bitmap.width()) {
//				std::cout<<"bottomX goes out of bounds -- clipping failed\n";
//			}
		}
        }
        tempVector = clip(newPoints[count-1], newPoints[0]);
        for(int i = 0; i < tempVector.size(); i++)
                edgeVector.push_back( tempVector.at(i) );

        std::sort(edgeVector.begin(), edgeVector.end());
        count = edgeVector.size();

        //convert GPaint to GPixel:
        GPixel newPixel = convert(paint.getColor());
        //CHECK IF WRONG ADDRESSING HERE)

        //loop thru each pair of edges
        int edge1Index = 0;
        int edge2Index = 1;
        int overlapTop, overlapBottom;
        int yIterator, xIterator;
        int leftX, rightX;
        float x1, x2;
	GPixel *previousPixel, incomingPixel;
        for(int edgeCount = 2; edgeCount <= count; ) {

                //find overlap in y value
                overlapTop = std::max(edgeVector.at(edge1Index).top, edgeVector.at(edge2Index).top);
                overlapBottom = std::min(edgeVector.at(edge1Index).bottom, edgeVector.at(edge2Index).bottom);

                //loop thru the overlap, increaing currentX as it goes
                for(yIterator = overlapTop; yIterator < overlapBottom; yIterator++) {

                        x1 = edgeVector.at(edge1Index).currentX;
                        x2 = edgeVector.at(edge2Index).currentX;
 /*remove min/max here*/leftX = (int) floor(0.5 + std::min(x1, x2) );
 /**/                   rightX = (int) floor(0.5 + std::max(x1, x2) );
			xIterator = 0; //to iterate through shadeRow's colors, or stay @ zero if no shader

			int srcPixelsLength;
			if(isShader)	srcPixelsLength = rightX - leftX;
			else		srcPixelsLength = 1;
			GPixel srcPixels[srcPixelsLength];

			if(isShader)	shaderObject -> shadeRow(leftX, yIterator, rightX-leftX, srcPixels);
			else		srcPixels[0] = newPixel;

			if(isFilter)	filterObject -> filter(srcPixels, srcPixels, srcPixelsLength);

                        for(; leftX < rightX; leftX++) { //Walk and Blit
				if(isLegalPoint(leftX, yIterator)) {
					previousPixel = layerStack.top().bitmap.getAddr(leftX, yIterator);
					incomingPixel = srcPixels[xIterator];

					if(isLegalPixel(*previousPixel) && isLegalPixel(incomingPixel) )
						*previousPixel = blendMode(incomingPixel, *previousPixel);
					if(isShader)	xIterator++;
				}
			}

			//increase curX value for both edges (using slope)
			/*if(!isinf(edgeVector.at(edge1Index).slope))*/	edgeVector.at(edge1Index).currentX += edgeVector.at(edge1Index).slope;
			/*if(!isinf(edgeVector.at(edge2Index).slope))*/	edgeVector.at(edge2Index).currentX += edgeVector.at(edge2Index).slope;

                }

                //increment the edge that has ended (or both if both)
                if(overlapBottom == edgeVector.at(edge1Index).bottom)	edge1Index = edgeCount++;
		if(overlapBottom == edgeVector.at(edge2Index).bottom)	edge2Index = edgeCount++;
        }


    }









    void drawPath(const GPath& path, const GPaint& paint) {
	ModeProc blendMode = proc[static_cast<int>(paint.getBlendMode())];

	//set up Shader and Filter Objects
        GShader* shaderObject = paint.getShader();
        GFilter* filterObject = paint.getFilter();
        bool isShader = false;
        bool isFilter = false;
        if( shaderObject != nullptr ) {
                if(!shaderObject -> setContext(layerStack.top().ctmMatrix))
                        return;
                isShader = true;
        }
        if( filterObject != nullptr )
                isFilter = true;

	GPath::Edger edger(path);
	GPath::Verb verb; //kLine, kDone ... kMove
	vector<Edge> /*edgeVector, */clippedEdgeVector;
	edgeVec.clear();
	GPoint endpoints[4];

	//get edges from GPath
	verb = edger.next(endpoints);
	float xVal, yVal, magnitudeL;
	int lineCount;
	while(verb != GPath::kDone) {
		if(verb == GPath::kLine) {
			//map points
           	     	layerStack.top().ctmMatrix.mapPoints(endpoints, endpoints, 2);
           	     	//clip
       	     	        clippedEdgeVector = clip(endpoints[0], endpoints[1]);
      	     	        for(int i=0; i<clippedEdgeVector.size(); i++)
   	     	                edgeVec.push_back( clippedEdgeVector.at(i) );
		}

		if(verb == GPath::kQuad) {
			layerStack.top().ctmMatrix.mapPoints(endpoints, endpoints, 3);
			//L vector => (a-2b+c)/4
			//D = |L| (magnitude)
/*			//count = ceiling(sqt(D / toleracne))
			xVal = (endpoints[0].fX - 2*endpoints[1].fX + endpoints[2].fX)/4;
			yVal = (endpoints[0].fY - 2*endpoints[1].fY + endpoints[2].fY)/4;
			magnitudeL = std::sqrt(xVal*xVal + yVal*yVal);
			lineCount = GCeilToInt( std::sqrt(magnitudeL / 0.25) );

	//		std::cout<<"("<<xVal<<", "<<yVal<<") -> magL:"<<magnitudeL<<"      gives a count of "<<lineCount<<"\n";
			//std::cout<<"edgeVector begins chopper at "<<edgeVec.size()<<" elements,";
			GPoint srcPoints[3] = {endpoints[0], endpoints[1], endpoints[2]};
			GPoint dstPoints[5];

//			lineCount = 25;
			GPoint tempPoints[lineCount+1];
			tempPoints[0] = endpoints[0];
			tempPoints[lineCount] = endpoints[2];

			for(int pointNum=1; pointNum<lineCount; pointNum++) {
				GPath::ChopQuadAt(srcPoints, dstPoints, pointNum/1.0/lineCount);
				tempPoints[pointNum] = dstPoints[2];
			}
			for(int i=0; i<lineCount; i++)
				edgeVec.push_back(createEdge(tempPoints[i], tempPoints[i+1]));


*/


			quadChopper(endpoints, 5);  //8 is optimal for quad_clock

			//std::cout<<" and ends with "<<edgeVec.size()<<" elements\n";
		}



		if(verb == GPath::kCubic) {
			layerStack.top().ctmMatrix.mapPoints(endpoints, endpoints, 4);

			cubicChopper(endpoints, 8);
		}

/*		//map points
		layerStack.top().ctmMatrix.mapPoints(endpoints, endpoints, 4);
		//clip
		for(int index = 0; index < verbNum; index++) {
			clippedEdgeVector = clip(endpoints[index], endpoints[index + 1]);
			for(int i=0; i<clippedEdgeVector.size(); i++)
				edgeVector.push_back( clippedEdgeVector.at(i) );
		}
*/

		verb = edger.next(endpoints);
	}
	//sort edges
	std::sort(edgeVec.begin(), edgeVec.end());

	int boundsTop = -layerStack.top().bitmap.height();//path.bounds().top();
	int boundsBot = layerStack.top().bitmap.height();//path.bounds().bottom();
	int boundsLef = -layerStack.top().bitmap.width();//path.bounds().left();
	int boundsRig = layerStack.top().bitmap.width();//path.bounds().right();

	GPixel *previousPixel, incomingPixel;
	int yIterator,xIterator;
	int windingSum, tempX;
	Edge tempEdge;
	std::vector<Edge> intersectingEdges;
	std::vector<int> edgeIntersections;
	std::vector<int> windingInts;
	//yIterator not increaing past 1 for cout statements? why?
	for(yIterator = boundsTop; yIterator<boundsBot; yIterator++) {

//		int srcPixelsLength = boundsRig - boundsLef;
  //              GPixel srcPixels[srcPixelsLength];
//		if(isShader)	shaderObject -> shadeRow(xIterator, yIterator, boundsRig-boundsLef, srcPixels);
//		else		for(int j=0; j<srcPixelsLength; j++) srcPixels[j] = convert(paint.getColor());
//		if(isFilter)	filterObject -> filter(srcPixels, srcPixels, srcPixelsLength);

		intersectingEdges.clear();
		edgeIntersections.clear();
		windingInts.clear();
		for(int edgeIndex = 0; edgeIndex<edgeVec.size(); edgeIndex++) {
			tempEdge = edgeVec.at(edgeIndex);
			if(tempEdge.top<yIterator && tempEdge.bottom>=yIterator && tempEdge.bottom != tempEdge.top) {
				intersectingEdges.push_back(tempEdge);
				//if edge contain this y value, find the x value at this specific y
	//			edgeIntersections.push_back(tempEdge.currentX + tempEdge.slope*( yIterator-tempEdge.top ));
			}
		}
		std::sort(intersectingEdges.begin(), intersectingEdges.end(), currentXSort);
   //		std::cout<<" ---------------NEW Y VALUE --------------\n";
   //		for(int i=0; i<intersectingEdges.size(); i++)
   //	                std::cout<<" - - Edges after currentXSort:     y range ("<<intersectingEdges.at(i).top<<", "<<intersectingEdges.at(i).bottom<< ") x at "<<intersectingEdges.at(i).currentX<<" ("<<intersectingEdges.at(i).windVal<<")\n";



//		std::cout<<"new y row: intersections at: \n";
//		for(int k=0; k<intersectingEdges.size(); k++)
//			std::cout<<""<<intersectingEdges.at(k).currentX<<" ";
//		std::cout<<"\n";

		windingSum = 0;

		//new x loop
		if(intersectingEdges.size() < 2)
			continue;
		for(int edg=0; edg<intersectingEdges.size() - 1; edg++) { 	//loop through each space between edges
			windingSum += intersectingEdges.at(edg).windVal;

			if(windingSum != 0) {
				//for loop from tempEdge.currentX to 
				xIterator = intersectingEdges.at(edg).currentX;
				int srcPixelsLength = intersectingEdges.at(edg+1).currentX - intersectingEdges.at(edg).currentX + 1;
	 	               	GPixel srcPixels[srcPixelsLength];
	 	               	if(isShader)    shaderObject -> shadeRow(xIterator, yIterator, srcPixelsLength, srcPixels);
		               	else            for(int j=0; j<srcPixelsLength; j++) srcPixels[j] = convert(paint.getColor());
	 	               	if(isFilter)    filterObject -> filter(srcPixels, srcPixels, srcPixelsLength);
				int srcPixelIterator=0;

				//for(; xIterator<intersectingEdges.at(edg+1).currentX + 1; xIterator++) {
				for(int srcPixelIterator=0; srcPixelIterator<srcPixelsLength; srcPixelIterator++) {
					if(isLegalPoint(xIterator, yIterator)) {
                                 	       previousPixel = layerStack.top().bitmap.getAddr(xIterator, yIterator);
                                 	       incomingPixel = srcPixels[srcPixelIterator];
//
                                        	if(isLegalPixel(*previousPixel) && isLegalPixel(incomingPixel) )
                                                	*previousPixel = blendMode(incomingPixel, *previousPixel);

                                	}
					xIterator++;
				}

			}
		}

		for(int edg=0; edg<edgeVec.size(); edg++) {
                        if(edgeVec.at(edg).top < yIterator && edgeVec.at(edg).bottom > yIterator ) {
//                        	std::cout<<"increasing edge "<<edg<<"'s currentX from "<<edgeVector.at(edg).currentX<<" by "<<edgeVector.at(edg).slope<<"\n";
      				edgeVec.at(edg).currentX += edgeVec.at(edg).slope;
			}
                }


	}

    }



    static bool currentXSort(const Edge one, const Edge two) {
	return one.currentX < two.currentX;
    }

    void quadChopper(GPoint enter[],int countdown) {
	//enter is length of 3
	if(countdown == 0) {
		edgeVec.push_back(createEdge(enter[0], enter[1]));
		edgeVec.push_back(createEdge(enter[1], enter[2]));
		return;
	}

	const GPoint src[3] = {enter[0], enter[1], enter[2]};
	GPoint dst[5];
	GPath::ChopQuadAt(src, dst, 0.5);

	GPoint newSrc1[3] = {dst[0],dst[1],dst[2]};
	GPoint newSrc2[3] = {dst[2],dst[3],dst[4]};
	quadChopper( newSrc1 , countdown-1);
	quadChopper( newSrc2 , countdown-1);
    }

    void cubicChopper(GPoint enter[], int countdown) {
	if(countdown == 0) {
		edgeVec.push_back(createEdge(enter[0], enter[1]));
		edgeVec.push_back(createEdge(enter[1], enter[2]));
		edgeVec.push_back(createEdge(enter[2], enter[3]));
		return;
	}

	const GPoint src[4] = {enter[0], enter[1], enter[2], enter[3]};
	GPoint dst[7];
	GPath::ChopCubicAt(src, dst, 0.5);

	GPoint newSrc1[4] = {dst[0], dst[1], dst[2], dst[3]};
	GPoint newSrc2[4] = {dst[3], dst[4], dst[5], dst[6]};
	cubicChopper(newSrc1, countdown-1);
	cubicChopper(newSrc2, countdown-1);
    }










    bool isLegalPixel(GPixel pixel) {
	if(GPixel_GetA(pixel)<GPixel_GetR(pixel) || GPixel_GetA(pixel)<GPixel_GetG(pixel) || GPixel_GetA(pixel)<GPixel_GetB(pixel)  )
		return false;
	return true;
    }

    bool isLegalPoint(float x, float y) {
	if(x>=0 && x<layerStack.top().bitmap.width() && y>=0 && y<layerStack.top().bitmap.height())
		return true;
	return false;
    }

    vector<Edge> clip(const GPoint one, const GPoint two) {
        vector<Edge> edgeVector(0);
	if(one.fY == two.fY)
		return edgeVector;
	edgeVector.push_back(createEdge(one,two));
	return edgeVector;

        float slope = (two.fX - one.fX) / (two.fY - one.fY);
        float displace, tempX, tempY;
        GPoint a, b, u, w;

	//Y CLIPPING
        //sort by Y vals
        if(min(one.fY, two.fY) == one.fY) {
                a = one;
                b = two;
                slope = (one.fX - two.fX) / (one.fY - two.fY);
        }
        else {
                b = one;
                a = two;
                slope = (two.fX - one.fX) / (two.fY - one.fY);
        }

        //remove if entirely out
        if(b.fY < 0 || a.fY > layerStack.top().bitmap.height() || a.fY == b.fY) {
                return edgeVector;
	}
        //move a if out on top
        if(a.fY < 0) {
                a.fY = 0;
                displace = b.fY;
                a.fX = b.fX - displace*slope;
        }
        //move b if out below
        if(b.fY > layerStack.top().bitmap.height()) {
                b.fY = layerStack.top().bitmap.height();
                displace = layerStack.top().bitmap.height() - a.fY;
                b.fX = a.fX + displace*slope;
        }



        //X CLIPPING
        //switch a and b if diferently sorted by X (use u as a temp)
        if(max(a.fX, b.fX) == a.fX) {
                u = b;
                b = a;
                a = u;
        }
	//check if entire thing is out
        if(a.fX > layerStack.top().bitmap.width()) {
                a.fX = layerStack.top().bitmap.width();
                b.fX = layerStack.top().bitmap.width();
                edgeVector.push_back(createEdge(a,b));
                return edgeVector;
        }
        if(b.fX < 0) {
                a.fX = 0;
                b.fX = 0;
                edgeVector.push_back(createEdge(a,b));
                return edgeVector;
        }
        //project if left side is sticking out
        if(a.fX < 0) {
                u.fX = 0;
                u.fY = a.fY;

                a.fX = 0;
                displace = b.fX;
                a.fY = b.fY - displace/slope;

                edgeVector.push_back(createEdge(a,u));
        }
        //project if right side is sticking out
        if(b.fX > layerStack.top().bitmap.width()) {
                w.fX = layerStack.top().bitmap.width();
                w.fY = b.fY;

                b.fX = layerStack.top().bitmap.width();
                displace = layerStack.top().bitmap.width() - a.fX;
                b.fY = a.fY + displace/slope;

                edgeVector.push_back(createEdge(b,w));
        }

        edgeVector.push_back(createEdge(a,b));
        return edgeVector;
    }

    Edge createEdge(GPoint one, GPoint two) {
	Edge newEdge;
        float xIntercept;
        if(one.fY < two.fY) {
                newEdge.top = (int) floor(one.fY + 0.5);
                newEdge.bottom = (int) floor(two.fY + 0.5);
                newEdge.slope = (one.fX - two.fX) / (one.fY - two.fY);

                xIntercept = newEdge.slope * (newEdge.top - one.fY + 0.5);
                newEdge.currentX = one.fX + xIntercept;

		newEdge.windVal = 1;
        }
        else {
                newEdge.top = (int) floor(two.fY + 0.5);
                newEdge.bottom = (int) floor(one.fY + 0.5);
                newEdge.slope = (two.fX - one.fX) / (two.fY - one.fY);

                xIntercept = newEdge.slope * (newEdge.top - two.fY + 0.5);
                newEdge.currentX = two.fX + xIntercept;

		newEdge.windVal = -1;
        }

        return newEdge;

    }

    GPixel convert(GColor color) {
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

    static GPixel kClear(const GPixel& src, const GPixel& des) {
        return GPixel_PackARGB(0,0,0,0);
    }//!< [0, 0]
    static GPixel kSrc(const GPixel& src, const GPixel& des) {
        return src;
    }//!< [Sa, Sc]
    static GPixel kDst(const GPixel& src, const GPixel& des) {
        return des;
    }//!< [Da, Dc]
    static GPixel kSrcOver(const GPixel& src, const GPixel& des) {
        int newA = GPixel_GetA(src) + (SCALE - GPixel_GetA(src) ) * GPixel_GetA(des) / SCALE;
        int newR = GPixel_GetR(src) + (SCALE - GPixel_GetA(src) ) * GPixel_GetR(des) / SCALE;
        int newG = GPixel_GetG(src) + (SCALE - GPixel_GetA(src) ) * GPixel_GetG(des) / SCALE;
        int newB = GPixel_GetB(src) + (SCALE - GPixel_GetA(src) ) * GPixel_GetB(des) / SCALE;
        return GPixel_PackARGB(newA, newR, newG, newB);
    }//!< [Sa + Da * (1 - Sa), Sc + Dc * (1 - Sa)]
    static GPixel kDstOver(const GPixel& src, const GPixel& des) {
        int newA = GPixel_GetA(des) + (SCALE - GPixel_GetA(des) ) * GPixel_GetA(src) / SCALE;
        int newR = GPixel_GetR(des) + (SCALE - GPixel_GetA(des) ) * GPixel_GetR(src) / SCALE;
        int newG = GPixel_GetG(des) + (SCALE - GPixel_GetA(des) ) * GPixel_GetG(src) / SCALE;
        int newB = GPixel_GetB(des) + (SCALE - GPixel_GetA(des) ) * GPixel_GetB(src) / SCALE;
        return GPixel_PackARGB(newA, newR, newG, newB);
    }//!< [Da + Sa * (1 - Da), Dc + Sc * (1 - Da)]
    static GPixel kSrcIn(const GPixel& src, const GPixel& des) {
        int newA = GPixel_GetA(src) * GPixel_GetA(des) / SCALE;
        int newR = GPixel_GetR(src) * GPixel_GetA(des) / SCALE;
        int newG = GPixel_GetG(src) * GPixel_GetA(des) / SCALE;
        int newB = GPixel_GetB(src) * GPixel_GetA(des) / SCALE;
        return GPixel_PackARGB(newA, newR, newG, newB);
    }//!< [Sa * Da, Sc * Da]
    static GPixel kDstIn(const GPixel& src, const GPixel& des) {
        int newA = GPixel_GetA(des) * GPixel_GetA(src) / SCALE;
        int newR = GPixel_GetR(des) * GPixel_GetA(src) / SCALE;
        int newG = GPixel_GetG(des) * GPixel_GetA(src) / SCALE;
        int newB = GPixel_GetB(des) * GPixel_GetA(src) / SCALE;
        return GPixel_PackARGB(newA, newR, newG, newB);
    }//!< [Da * Sa, Dc * Sa]
    static GPixel kSrcOut(const GPixel& src, const GPixel& des) {
        int newA = (SCALE - GPixel_GetA(des) ) * GPixel_GetA(src) / SCALE;
        int newR = (SCALE - GPixel_GetA(des) ) * GPixel_GetR(src) / SCALE;
        int newG = (SCALE - GPixel_GetA(des) ) * GPixel_GetG(src) / SCALE;
        int newB = (SCALE - GPixel_GetA(des) ) * GPixel_GetB(src) / SCALE;
        return GPixel_PackARGB(newA, newR, newG, newB);
    }//!< [Sa * (1 - Da), Sc * (1 - Da)]
    static GPixel kDstOut(const GPixel& src, const GPixel& des) {
        int newA = (SCALE - GPixel_GetA(src) ) * GPixel_GetA(des) / SCALE;
        int newR = (SCALE - GPixel_GetA(src) ) * GPixel_GetR(des) / SCALE;
        int newG = (SCALE - GPixel_GetA(src) ) * GPixel_GetG(des) / SCALE;
        int newB = (SCALE - GPixel_GetA(src) ) * GPixel_GetB(des) / SCALE;
        return GPixel_PackARGB(newA, newR, newG, newB);
    }//!< [Da * (1 - Sa), Dc * (1 - Sa)]
    static GPixel kSrcATop(const GPixel& src, const GPixel& des) {
        int newA = GPixel_GetA(des);
        int newR = (   GPixel_GetR(src) * GPixel_GetA(des)  +   (SCALE - GPixel_GetA(src) ) * GPixel_GetR(des)  ) / SCALE;
        int newG = (   GPixel_GetG(src) * GPixel_GetA(des)  +   (SCALE - GPixel_GetA(src) ) * GPixel_GetG(des)  ) / SCALE;
        int newB = (   GPixel_GetB(src) * GPixel_GetA(des)  +   (SCALE - GPixel_GetA(src) ) * GPixel_GetB(des)  ) / SCALE;
        return GPixel_PackARGB(newA, newR, newG, newB);
    }//!< [Da, Sc * Da + Dc * (1 - Sa)]
    static GPixel kDstATop(const GPixel& src, const GPixel& des) {
        int newA = GPixel_GetA(src);
        int newR = (   GPixel_GetR(des) * GPixel_GetA(src)   +   (SCALE - GPixel_GetA(des) ) * GPixel_GetR(src)  ) / SCALE;
        int newG = (   GPixel_GetG(des) * GPixel_GetA(src)   +   (SCALE - GPixel_GetA(des) ) * GPixel_GetG(src)  ) / SCALE;
        int newB = (   GPixel_GetB(des) * GPixel_GetA(src)   +   (SCALE - GPixel_GetA(des) ) * GPixel_GetB(src)  ) / SCALE;
        return GPixel_PackARGB(newA, newR, newG, newB);
    }//!< [Sa, Dc * Sa + Sc * (1 - Da)]
    static GPixel kXor(const GPixel& src, const GPixel& des) {
        int newA = (   (SCALE - GPixel_GetA(des) ) * GPixel_GetA(src)   +   (SCALE - GPixel_GetA(src) ) * GPixel_GetA(des)  ) / SCALE;
        int newR = (   (SCALE - GPixel_GetA(des) ) * GPixel_GetR(src)   +   (SCALE - GPixel_GetA(src) ) * GPixel_GetR(des)  ) / SCALE;
        int newG = (   (SCALE - GPixel_GetA(des) ) * GPixel_GetG(src)   +   (SCALE - GPixel_GetA(src) ) * GPixel_GetG(des)  ) / SCALE;
        int newB = (   (SCALE - GPixel_GetA(des) ) * GPixel_GetB(src)   +   (SCALE - GPixel_GetA(src) ) * GPixel_GetB(des)  ) / SCALE;
	return GPixel_PackARGB(newA, newR, newG, newB);
    }//!< [Sa + Da - 2 * Sa * Da, Sc * (1 - Da) + Dc * (1 - Sa)]



private:
    std::stack<Layer> layerStack;
    GMatrix identityMatrix;
    vector<Edge> edgeVec;
};

std::unique_ptr<GCanvas> GCreateCanvas(const GBitmap& device) {

    if (!device.pixels())	return nullptr;

    return std::unique_ptr<GCanvas>(new EmptyCanvas(device));

}
