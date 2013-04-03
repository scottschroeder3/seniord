/*garmin_gesture_system.cpp*/

#include "cv.h"
#include "highgui.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "gesture_util.h"
#include "opencv2/core/core.hpp"
#include "opencv2/video/tracking.hpp"

using namespace cv;
using namespace std;

// colors
const Scalar COLOR_BLUE        = Scalar(240,40,0);
const Scalar COLOR_DARK_GREEN  = Scalar(0, 255, 0);
const Scalar COLOR_LIGHT_GREEN = Scalar(0,255,0);
const Scalar COLOR_YELLOW      = Scalar(0,128,200);
const Scalar COLOR_RED         = Scalar(0,0,255);

struct ConvexityDefect
{
    Point start;
    Point end;
    Point depth_point;
    float depth;
};



void findConvexityDefects(vector<Point>& contour, vector<int>& hull, vector<ConvexityDefect>& convexDefects)
{
    if(hull.size() > 0 && contour.size() > 0)
    {
        CvSeq* contourPoints;
        CvSeq* defects;
        CvMemStorage* storage;
        CvMemStorage* strDefects;
        CvMemStorage* contourStr;
        CvConvexityDefect *defectArray = 0;

        strDefects = cvCreateMemStorage();
        defects = cvCreateSeq( CV_SEQ_KIND_GENERIC|CV_32SC2, sizeof(CvSeq),sizeof(CvPoint), strDefects );

        //We transform our vector<Point> into a CvSeq* object of CvPoint.
        contourStr = cvCreateMemStorage();
        contourPoints = cvCreateSeq(CV_SEQ_KIND_GENERIC|CV_32SC2, sizeof(CvSeq), sizeof(CvPoint), contourStr);
        for(int i = 0; i < (int)contour.size(); i++) {
            CvPoint cp = {contour[i].x,  contour[i].y};
            cvSeqPush(contourPoints, &cp);
        }

        //Now, we do the same thing with the hull index
        int count = (int) hull.size();
        //int hullK[count];
        int* hullK = (int*) malloc(count*sizeof(int));
        for(int i = 0; i < count; i++) { hullK[i] = hull.at(i); }
        CvMat hullMat = cvMat(1, count, CV_32SC1, hullK);

        // calculate convexity defects
        storage = cvCreateMemStorage(0);
        defects = cvConvexityDefects(contourPoints, &hullMat, storage);
        defectArray = (CvConvexityDefect*)malloc(sizeof(CvConvexityDefect)*defects->total);
        cvCvtSeqToArray(defects, defectArray, CV_WHOLE_SEQ);
        //printf("DefectArray %i %i\n",defectArray->end->x, defectArray->end->y);

        //We store defects points in the convexDefects parameter.
        for(int i = 0; i<defects->total; i++){
            ConvexityDefect def;
            def.start       = Point(defectArray[i].start->x, defectArray[i].start->y);
            def.end         = Point(defectArray[i].end->x, defectArray[i].end->y);
            def.depth_point = Point(defectArray[i].depth_point->x, defectArray[i].depth_point->y);
            def.depth       = defectArray[i].depth;
            convexDefects.push_back(def);
        }

    // release memory
    cvReleaseMemStorage(&contourStr);
    cvReleaseMemStorage(&strDefects);
    cvReleaseMemStorage(&storage);

    }
}



Mat findHand(Mat input){

   			Mat drawing = Mat::zeros( input.size(), CV_8UC3 );
			vector<Vec4i> hierarchy;
			std::vector<std::vector<Point> > contours;

			findContours(input,contours,CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

			vector<int> hullI( contours.size() );
			vector<Point> hullP( contours.size() );
			vector<Vec4i> defects( contours.size() );


			if (contours.size()) {
                for (int i = 0; i < contours.size(); i++) {
                    vector<Point> contour = contours[i];
                    Mat contourMat = Mat(contour);
                    double cArea = contourArea(contourMat);

                    if(cArea > 300) // likely the hand
                    {
                        Scalar center = mean(contourMat);
                        Point centerPoint = Point(center.val[0], center.val[1]);

                        // approximate the contour by a simple curve
                        vector<Point> approxCurve;
                        approxPolyDP(contourMat, approxCurve, 4, true);

                        vector< vector<Point> > debugContourV;
                        debugContourV.push_back(approxCurve);
                        drawContours(drawing, debugContourV, 0, COLOR_DARK_GREEN, 1);
						//drawContours(drawing, contour, 0, COLOR_DARK_GREEN, 1);

                        vector<int> hull;
                        convexHull(Mat(approxCurve), hull, false, false);

                        // draw the hull points
                        for(int j = 0; j < hull.size(); j++)
                        {
                            int index = hull[j];
                            circle(drawing, approxCurve[index], 3, COLOR_YELLOW, 2);
                        }

                        // find convexity defects
                        vector<ConvexityDefect> convexDefects;
                        findConvexityDefects(approxCurve, hull, convexDefects);
                        printf("Number of defects: %d.\n", (int) convexDefects.size());

                        for(int j = 0; j < convexDefects.size(); j++)
                        {
                            circle(drawing, convexDefects[j].depth_point, 3, COLOR_BLUE, 2);
							circle(drawing, convexDefects[j].start, 3, COLOR_DARK_GREEN, 2);
							circle(drawing, convexDefects[j].end, 3, COLOR_RED, 2);

                        }
                        
                        // assemble point set of convex hull
                        vector<Point> hullPoints;
                        for(int k = 0; k < hull.size(); k++)
                        {
                            int curveIndex = hull[k];
                            Point p = approxCurve[curveIndex];
                            hullPoints.push_back(p);
                        }

                        // area of hull and curve
                        double hullArea  = contourArea(Mat(hullPoints));
                        double curveArea = contourArea(Mat(approxCurve));
                        double handRatio = curveArea/hullArea;

                        
                    }
                } // contour conditional
				}
	return drawing;
}

Mat smoothImage(Mat image){

			GaussianBlur(image, image, Size(5,5), 0,0);
			medianBlur(image, image, 5);
			//threshold(filtered2, filtered2, 100, 255, THRESH_BINARY);
			//int erosion_size = 2;
			//Mat element = getStructuringElement(MORPH_RECT, Size(2*erosion_size+1, 2*erosion_size+1), Point(erosion_size,erosion_size));
			//erode(filtered2,filtered2,element);
			//dilate(filtered2,filtered2,element);
			//dilate(filtered2,filtered2,element);
			//erode(filtered2,filtered2,element);	
	return image;
}


int main() 
{
	
	VideoCapture capture = VideoCapture(CV_CAP_OPENNI);
	Size size = Size(capture.get(CV_CAP_PROP_FRAME_WIDTH),capture.get(CV_CAP_PROP_FRAME_HEIGHT));
	int codec = CV_FOURCC('D', 'I', 'V', 'X');    
	VideoWriter writer("video.avi",codec,capture.get(CV_CAP_PROP_FPS),size,0);
	
    if(writer.isOpened())
    {

    	Mat depthMap;
		Mat bgrImage ;
		Mat filtered;
		Mat filtered2;
        while ( 1 )
		{
			capture.grab();
			capture.retrieve( depthMap, CV_CAP_OPENNI_DEPTH_MAP );
			capture.retrieve( bgrImage, CV_CAP_OPENNI_BGR_IMAGE );
		
			inRange(depthMap,25,770,filtered);
			filtered2 = filtered.clone();
			filtered2 = smoothImage(filtered2);
			imshow("FILT",filtered2);
			Mat drawing = findHand(filtered2);


			/*WRITE TO FILE*/
			//writer.write(filtered);

			/*DISPLAY IMAGES*/
			//imshow("COLOR",bgrImage);
			//imshow("FILT",filtered);
			imshow("Drawing",drawing);


            if(waitKey(100)>=0)
			{
				break;
			}
        }
	}
    else
    {
       	cout<<"ERROR while opening"<<endl;
    }
	writer.release();
	capture.release();

	
	
    return 0;
}


