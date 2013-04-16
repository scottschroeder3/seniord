/*garmin_gesture_system.cpp*/

#include "cv.h"
#include "highgui.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "gesture_util.h"
#include "util.h"
#include "opencv2/core/core.hpp"
#include "opencv2/video/tracking.hpp"
#include "time.h"

using namespace cv;
using namespace std;

// colors
const Scalar COLOR_BLUE        = Scalar(240,40,0);
const Scalar COLOR_DARK_GREEN  = Scalar(0, 255, 0);
const Scalar COLOR_LIGHT_GREEN = Scalar(0,255,0);
const Scalar COLOR_YELLOW      = Scalar(0,128,200);
const Scalar COLOR_RED         = Scalar(0,0,255);

int seconds_count;


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
	free(defectArray);

    }
}



bool findHand(Mat input, Mat colorImage, Point &center_ref){

			bool handfound=false;

   			Mat drawing = Mat::zeros( input.size(), CV_8UC3 );
			vector<Vec4i> hierarchy;
			std::vector<std::vector<Point> > contours;

			findContours(input,contours,CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

			vector<int> hullI( contours.size() );
			vector<Point> hullP( contours.size() );
			vector<Vec4i> defects( contours.size() );


			//Tom's test section for finding centroids
			//vector<Point2f> centers = getBlobCenters(contours)
					


			//End of Tom's test section

			

			if (contours.size()) {
                for (int i = 0; i < contours.size(); i++) {

                    vector<Point> contour = contours[i];
                    Mat contourMat = Mat(contour);
                    double cArea = contourArea(contourMat);
					//printf("area: %lf.\n", cArea);

                    if(cArea > 7000 && cArea < 30000) // likely the hand
                    {
                        Scalar center = mean(contourMat);
                        Point centerPoint = Point(center.val[0], center.val[1]);
						circle(drawing, centerPoint, 8, COLOR_RED, 2);
						center_ref = centerPoint;

                        // approximate the contour by a simple curve
                        vector<Point> approxCurve;
                        approxPolyDP(contourMat, approxCurve, 10, true);

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
                        //printf("Number of defects: %d.\n", (int) convexDefects.size());

                        for(int j = 0; j < convexDefects.size(); j++)
                        {
                            circle(drawing, convexDefects[j].depth_point, 3, COLOR_BLUE, 2);
							//circle(drawing, convexDefects[j].start, 3, COLOR_DARK_GREEN, 2);
							//circle(drawing, convexDefects[j].end, 3, COLOR_RED, 2);

                        }

						if(convexDefects.size()>=4 && convexDefects.size()<=8)
						{
							float totalDepth = 0;
							for(int j = 0; j < convexDefects.size(); j++)
							{
								totalDepth+=convexDefects[j].depth;
							}
							//printf("Total depth: %lf.\n", totalDepth);
							if(totalDepth>160)
							{
								drawContours(drawing, debugContourV, 0, COLOR_YELLOW, 5);
								seconds_count+=1;
							}
							else
							{
								seconds_count=0;
							}
						}
						else
						{
							seconds_count=0;
						}
						if(seconds_count>8)
						{
							//printf("Hand!     ");
							handfound=true;
							seconds_count=0;
							drawContours(drawing, debugContourV, 0, COLOR_RED, 5);
							drawContours(colorImage, debugContourV, 0, COLOR_RED, 5);
						}
						//printf("time: %d.\n", seconds_count);                        

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

	//imshow("BGR",colorImage);
	imshow("wireframe",drawing);
	  
	return handfound;
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

	namedWindow( "COLOR", CV_WINDOW_AUTOSIZE );
	namedWindow( "wireframe", CV_WINDOW_AUTOSIZE );
	namedWindow( "FILT", CV_WINDOW_AUTOSIZE );
	namedWindow( "BlobCenters", CV_WINDOW_AUTOSIZE );

	moveWindow("COLOR", 10, 10);
	moveWindow("wireframe", 710, 10);
	moveWindow("FILT", 10, 540);
	moveWindow("BlobCenters", 710, 540);
	
    if(writer.isOpened())
    {

    	Mat depthMap;
		Mat bgrImage ;
		Mat filtered;
		Mat filtered2;
		Point centerOfHand;

		//Motion History Mats
		Mat blobCenters = Mat::zeros(size,CV_8U);
		imshow("BlobCenters",blobCenters);
		int prevX, prevY = -1;


		bool foundHand;
		clock_t gestureTimer;
		seconds_count=0;

		int X_Displacement=0;
		int Y_Displacement=0;

        while ( 1 )
		{
			capture.grab();
			capture.retrieve( depthMap, CV_CAP_OPENNI_DEPTH_MAP );
			capture.retrieve( bgrImage, CV_CAP_OPENNI_BGR_IMAGE );
		
			//imshow("depthmap",depthMap);
			//Find the minimum value greater than 0 in the matrix
			//TEST SECTION

			flip(depthMap,depthMap,1);
			flip(bgrImage,bgrImage,1);

			MatConstIterator_<unsigned short> it = depthMap.begin<unsigned short>(), it_end = depthMap.end<unsigned short>();
			unsigned short minVal=60000;
			
			for(;it != it_end; ++it){
				if(*it<minVal && *it>0){
					minVal=*it;
				}
			}			

			//cout << "minVal: " <<minVal<<endl;
			unsigned short minRange = minVal-30;
			unsigned short maxRange = minVal+60;


			//cout << "min,max: "<<minRange<<", "<<maxRange<<endl;

			//Perhaps just create another mat with size 8u. This seems to be what happens when

			Mat thtwBitDepth;// = cvCreateImage(size,IPL_DEPTH_32F,0);

			depthMap.convertTo(thtwBitDepth,CV_32F);//,1.0/256,0);

			
			//imshow("32 Bit",thtwBitDepth);

			filtered2 = thresholdDistance(thtwBitDepth,minRange,maxRange);
			filtered2 = thresholdDistance(filtered2,25,900);
			

			//imshow("ThresholdDistance",filtered2);

			//END TEST SECTION

			//inRange(depthMap,25,800,filtered);
			//filtered2 = filtered.clone();
			filtered2 = smoothImage(filtered2);
			imshow("FILT",filtered2);


			Mat thtwsfiltered;// = cvCreateImage(size,IPL_DEPTH_8U,0);
			filtered2.convertTo(thtwsfiltered,CV_8U);
			filtered2 = thtwsfiltered.clone();
			
			if(!foundHand){
				foundHand = findHand(thtwsfiltered, bgrImage, centerOfHand);
				//foundHand = findHand(filtered2, bgrImage);
				if(foundHand) gestureTimer=clock();
				//cout << "found hand = "<< foundHand << endl;
			} else {
				
				//THIS IS THE DEPTH AT THE POINT WHERE THE CENTER OF THE HAND WAS CALCULATED
				cout  << "depth: " << depthMap.at<unsigned short>(centerOfHand) << endl;

				//A hand was detected and now a gesture is being analyzed.

				//Find center of mass of all blobs in window and draw a circle on them.
				//This image will be fed to the motion history functions.

				std::vector<std::vector<Point> > contours;

				findContours(filtered2,contours,CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

            	for (int i = 0; i < contours.size(); i++) {

                	vector<Point> contour = contours[i];
                	Mat contourMat = Mat(contour);
                	double cArea = contourArea(contourMat);

                	if(cArea > 4000 && cArea < 30000) // likely the hand
                	{
                    	Scalar center = mean(contourMat);
                    	Point centerPoint = Point(center.val[0], center.val[1]);
						//Point centerPoint = Point(50, 50);
						
						if(prevX>=0 && prevY>=0 && center.val[0]>=0 && center.val[1]>=0){
							line(blobCenters,centerPoint,Point(prevX,prevY),Scalar(255,255,255),30);
							line(bgrImage,centerPoint,Point(prevX,prevY),Scalar(0,255,0),15);
							X_Displacement += (center.val[0]-prevX);
							Y_Displacement += (center.val[1]-prevY);
						}

						prevX = center.val[0];
						prevY = center.val[1];
						//cout <<"Displacement(x,y): "<<X_Displacement<<","<<Y_Displacement<<endl;
						//circle(blobCenters, centerPoint, 8, Scalar(255,255,255), -1);
					}
				}


				if(X_Displacement>160 || X_Displacement<-160 || Y_Displacement>120 || Y_Displacement<-120){
					






					gestureTimer=0;
					foundHand=0;
					prevX=-1;
					prevY=-1;
					X_Displacement=0;
					Y_Displacement=0;
					blobCenters = Mat::zeros(size,CV_8U);
				}

				//cout << "Clock: "<<(clock()-gestureTimer)/CLOCKS_PER_SEC<<endl;
				if(((clock()-gestureTimer)/CLOCKS_PER_SEC)>=1){
					//Gesture time has exceeded 10 seconds. Give up on finding gesture.
					gestureTimer=0;
					foundHand=0;
					prevX=-1;
					prevY=-1;
					X_Displacement=0;
					Y_Displacement=0;
					blobCenters = Mat::zeros(size,CV_8U);
				}

				imshow("BlobCenters",blobCenters);

				//NOTE: Need to add a check to determine if a gesture was determined correctly.
				//      If it was gestureTimer and foundHand both need to be set to 0.

			}

			/*WRITE TO FILE*/
			//writer.write(filtered);

			/*DISPLAY IMAGES*/
			imshow("COLOR",bgrImage);
			
			//imshow("FILT",thtwsfiltered);

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


