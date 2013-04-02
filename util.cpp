/*
 util.cpp - Remade openCV utility functions.
 This C++ file contains utility functions for manipulating openCV images/videos. Some of these functions perform the same action as a corresponding openCV function and others are original functions, which manipulate data in order to detect gestures in an automobile infotainment environment.
 */

#include "cv.h"
#include "highgui.h"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;


/**
 *
 */
Mat thresholdDistance(Mat input, int lowBound, int highBound){
    
    Mat output;
    
    threshold(input,output,lowBound,lowBound,THRESH_TOZERO);
    threshold(output,output,highBound,highBound,THRESH_TOZERO_INV);
    
    return output;
}

//Use the following function to find the distance of the hand object in the frame. After this distance is found it will be stored for further use. It can then be used in calls to thresholdDistance so that the image only contains objects within a certain distance of the plane of interest. Use the values handDistance-150 and handDistance+50. (15cm in front of hand and 5cm in back.

short findHandDist(Mat input){
    
    short handDistance;
    //Need to calculate distance of hand object.

    return handDistance;
    
}




/*

double cv::threshold( InputArray _src, OutputArray _dst, double thresh, double maxval, int type )
{
    Mat src = _src.getMat();
    type &= THRESH_MASK;
    
    _dst.create( src.size(), src.type() );
    Mat dst = _dst.getMat();
    

    if( src.depth() == CV_16S )
    {
        int ithresh = cvFloor(thresh);
        thresh = ithresh;
        int imaxval = cvRound(maxval);
        imaxval = saturate_cast<short>(imaxval);
        
        if( ithresh < SHRT_MIN || ithresh >= SHRT_MAX )
        {
            if( type == THRESH_BINARY || type == THRESH_BINARY_INV ||
               ((type == THRESH_TRUNC || type == THRESH_TOZERO_INV) && ithresh < SHRT_MIN) ||
               (type == THRESH_TOZERO && ithresh >= SHRT_MAX) )
            {
                int v = type == THRESH_BINARY ? (ithresh >= SHRT_MAX ? 0 : imaxval) :
                type == THRESH_BINARY_INV ? (ithresh >= SHRT_MAX ? imaxval : 0) : 0;
                dst.setTo(v);
            }
            else
                src.copyTo(dst);
            return thresh;
        }
        thresh = ithresh;
        maxval = imaxval;
    }
    else
    {
        CV_Error( CV_StsUnsupportedFormat, "" );
    }
    
    parallel_for_(Range(0, dst.rows),
                  ThresholdRunner(src, dst, thresh, maxval, type),
                  dst.total()/(double)(1<<16));
    return thresh;
}

*/