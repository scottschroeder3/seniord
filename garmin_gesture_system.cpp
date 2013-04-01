/*garmin_gesture_system.cpp*/

#include "cv.h"
#include "highgui.h"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/core/core.hpp"
//#include "opencv2/video/tracking.hpp"

using namespace cv;
using namespace std;


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
        while ( 1 )
		{
			capture.grab();
			capture.retrieve( depthMap, CV_CAP_OPENNI_DEPTH_MAP );
			capture.retrieve( bgrImage, CV_CAP_OPENNI_BGR_IMAGE );
		
			inRange(depthMap,25,900,filtered);
			


			vector<Vec4i> hierarchy;
			vector<vector<Point> > contours;
			//vector<Point> contours;
			findContours(filtered,contours,hierarchy,CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE,Point(0,0));
			
			vector<vector<Point> >hull( contours.size() );
   			for( int i = 0; i < contours.size(); i++ )
      		{  
				convexHull( Mat(contours[i]), hull[i], false ); 
			}

   			Mat drawing = Mat::zeros( filtered.size(), CV_8UC3 );
   			for( int i = 0; i< contours.size(); i++ )
      		{

				if(contourArea(contours[i])>300)
				{

        			Scalar redcolor = Scalar( 0,0,255);
					Scalar greencolor = Scalar( 0,255,0);
					vector<Vec4i> convexityDefectsSet;     
					convexityDefects(contours[i], hull[i], convexityDefectsSet);
					/*for (int cDefIt = 0; cDefIt < convexityDefectsSet.size(); cDefIt++)
					{
						int startIdx = convexityDefectsSet[cDefIt].val[0];
						int endIdx = convexityDefectsSet[cDefIt].val[1];
						int defectPtIdx = convexityDefectsSet[cDefIt].val[2];
						double depth = (double)convexityDefectsSet[cDefIt].val[3]/256.0f;  
						//line(drawing, Point(startIdx), Point(endIdx), Scalar(0,255,255), 1,8,0);
					}
					*/
        			drawContours( drawing, contours, i, greencolor, 1, 8, vector<Vec4i>(), 0, Point() );
        			drawContours( drawing, hull, i, redcolor, 1, 8, vector<Vec4i>(), 0, Point() );
				}
      		}

			
			imshow("Drawing",drawing);









			//circle(bgrImage,cvPoint(200,200),2,cvScalar(0,255,0),5);
			//circle(filtered,cvPoint(200,200),2,cvScalar(0,255,0),5);

			//writer.write(filtered);
			//imshow("FILT",filtered);

			//imshow("COLOR",bgrImage);

			//cout  << "depth: " << depthMap.at<unsigned short>(200,200);
			//cout  << "  filt: " << filtered.at<unsigned short>(200,200) << endl;

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


