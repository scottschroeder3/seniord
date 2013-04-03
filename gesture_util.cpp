/*gesture_util.cpp*/
#include "cv.h"
#include "highgui.h"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;
using namespace std;

Mat findHand(Mat input){
	vector<Vec4i> hierarchy;
			vector<vector<Point> > contours;

			findContours(input,contours,hierarchy,CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE,Point(0,0));
			
			vector<vector<int> >hullI( contours.size() );
			vector<vector<Point> >hullP( contours.size() );
			vector< vector<Vec4i> > defects( contours.size() );

   			Mat drawing = Mat::zeros( filtered.size(), CV_8UC3 );

			
   			for( int i = 0; i< contours.size(); i++ )
      		{

				if(contourArea(contours[i])>300)
				{

        			Scalar redcolor = Scalar( 0,0,255);
					Scalar greencolor = Scalar( 0,255,0);

					convexHull( Mat(contours[i]), hullI[i], false ); 
					convexHull( Mat(contours[i]), hullP[i], false ); 
		     		convexityDefects(contours[i], hullI[i], defects[i]);
/*
					vector<Point> temp;
					//cout<<"********"<<endl;
					if(hullP[i].size()>1){
						int numgroups = 0;
						for(int j=1;j<hullP[i].size();j++){
							//circle(drawing, hullP[i][j], 5, Scalar(255,255,0), -1,8);
							Point cur = hullP[i][j];
							Point prev = hullP[i][j-1];
							double distance = sqrt(abs(cur.x - prev.x)^2 + abs(cur.y - prev.y)^2);
							if(distance<5)
							{
								 							
								temp.push_back(cur);
		
							}
							else
							{
								numgroups++;
									Scalar color;
								if(numgroups==1)
									color = 	Scalar(0,255,255);							
								else if(numgroups==2)								
									color = 	Scalar(0,128,255);
								else if(numgroups==3)								
									color = 	Scalar(255,255,0);
								else if(numgroups==4)								
									color = 	Scalar(255,0,0);
								else if(numgroups==5)								
									color = 	Scalar(255,0,255);
								else if(numgroups>5)								
									color = 	Scalar(128,128,128);

									if(temp.size()>0)
									circle(drawing, temp[0], 5, color, -1,8);
								temp.clear();
							}

						}
					}
*/

/*
					for (int cDefIt = 0; cDefIt < convexityDefectsSet.size(); cDefIt++)
					{
						int startIdx = convexityDefectsSet[cDefIt].val[0];
						int endIdx = convexityDefectsSet[cDefIt].val[1];
						int defectPtIdx = convexityDefectsSet[cDefIt].val[2];
						double depth = (double)convexityDefectsSet[cDefIt].val[3]/256.0f;  
						//line(drawing, Point(startIdx), Point(endIdx), Scalar(0,255,255), 1,8,0);
						
						vector<Point> hull1 = hullP[i]; // get the contour
						//Point point1 = hull1[startIdx];
						//circle(drawing, point1, 5, Scalar(0,255,255), -1,8);
						//Point point2 = hull1[endIdx];
						//circle(drawing, point2, 5, Scalar(255,0,255), -1,8);
						Point point3 = hull1[defectPtIdx];
						circle(drawing, point3, 5, Scalar(255,255,0), -1,8);

						cout<<hullP.size()<<endl;
					}
*/					
        			drawContours( drawing, contours, i, greencolor, 1, 8, vector<Vec4i>(), 0, Point() );
        			drawContours( drawing, hullP, i, redcolor, 1, 8, vector<Vec4i>(), 0, Point() );
				}
      		}
	return drawing;
}
