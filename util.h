/*util.h*/



/**
 *
 */
Mat thresholdDistance(Mat input, unsigned short lowBound, unsigned short highBound);

//short findHandDist(Mat input);

/**
 *Function to track and draw the tracked path of an object using moments
 *NOTE: This function will only work if there is one object in the image.
 */
//IplImage* drawTracking(IplImage* threshed, IplImage* trackedImage);

/*
 *Function to remove all objects in an image smaller than a given size. Similar functionality to Matlab's bwareaopen.
 */
//int eraseSmallObjects(IplImage *image, int size);
