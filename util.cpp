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
 *Function to find centroids of all blobs in an image.
 *May want to expand this to also draw and output the centers of each blob on a new image.
 */
vector<Point2f> getBlobCenters(Vector<Point> contours){
   
    //Find the moments of each contour
    vector<Moments> mmnts(contours.size());
    for(int i=0;i<contours.size();i++){
        mmnts[i] = moments(contours[i],false);
    }
    
    vector<Point2f> blobCenters(contours.size());
    for(int i=0;i<contours.size();i++){
        blobCenters[i] = Point2f(mmnts[i].m10/mmnts[i].m00,mmnts[i].m01/mmnts[i].m00);
    }
    
    return blobCenters;
    
}

/**
 *
 */
Mat thresholdDistance(Mat input, short lowBound, short highBound){
    
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

//Probably a good idea to only keep track of motionhistory once a gesture is started.

/**
 *Function to track and draw the tracked path of an object using moments
 *NOTE: This function will only work if there is one object in the image.
 */
IplImage* drawTracking(IplImage* threshed, IplImage* trackedImage){
    
    //Find the moments of the incoming image
    CvMoments* moments = (CvMoments*)malloc(sizeof(CvMoments));
    cvMoments(threshed,moments,1);
    double momentFirst = cvGetSpatialMoment(moments,1,0);
    double momentSecond = cvGetSpatialMoment(moments,0,1);
    double area = cvGetCentralMoment(moments,0,0);
    
    //Check the area to get rid of noise
    if(area<500){
        int Xpos = momentFirst/area;
        int Ypos = momentSecond/area;
        if(prevX>=0 && prevY>=0 && Xpos>=0 && Ypos>=0){
            
            //Draw the tracking line
            cvLine(trackedImage,cvPoint(Xpos,Ypos),cvPoint(prevX,prevY),cvScalar(255,255,255),5);
        }
        //Set the new position to be the old position
        prevX=Xpos;
        prevY=Ypos;
        printf("position (%d,%d)\n",Xpos,Ypos);
    }
    //Free used memory
    free(moments);
    return trackedImage;
}

/*
 *Function to remove all objects in an image smaller than a given size. Similar functionality to Matlab's bwareaopen.
 */
int eraseSmallObjects(IplImage *image, int size)
{
    
    CvMemStorage *storage;
    CvSeq *contour = NULL;
    CvScalar white, black;
    IplImage *input = NULL; // cvFindContours changes the input
    double area;
    int foundCountours = 0;
    
    black = CV_RGB( 0, 0, 0 );
    white = CV_RGB( 255, 255, 255 );
    
    if(image == NULL || size == 0)
        return(foundCountours);
    
    input = cvCloneImage(image);
    
    storage = cvCreateMemStorage(0);
    
    cvFindContours(input, storage, &contour, sizeof (CvContour),
                   CV_RETR_LIST,
                   CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
    int i=0;
    while(contour)
    {
        area = cvContourArea(contour, CV_WHOLE_SEQ );
        if( -size <= area && area <= 0)
        { // removes white dots
            cvDrawContours( image, contour, black, black, -1, CV_FILLED, 8 );
            foundCountours++;
        }
        else
        {
            //if( 0 < area && area <= size) // fills in black holes
            cvDrawContours( image, contour, white, white, -1, CV_FILLED, 8 );
            foundCountours++;
        }
        contour = contour->h_next;
        i++;
    }
    
    cvReleaseMemStorage( &storage ); // desallocate CvSeq as well.
    cvReleaseImage(&input);
    return(foundCountours);
    
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