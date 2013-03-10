 #include "cv.h"
#include "highgui.h"
#include "opencv2/imgproc/imgproc.hpp"

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

    	Mat depthMap = cvCreateImage(size,IPL_DEPTH_16U,0);
		Mat bgrImage ;
		Mat dispImage ;
		Mat filtered;
        while ( 1 )
		{
			capture.grab();
			capture.retrieve( depthMap, CV_CAP_OPENNI_DEPTH_MAP );
			capture.retrieve( bgrImage, CV_CAP_OPENNI_BGR_IMAGE );
			capture.retrieve( dispImage, CV_CAP_OPENNI_DISPARITY_MAP );
		
			inRange(depthMap,25,900,filtered);
			
			/*for(int i=0;i<depthMap.rows;i++)
			{
				for(int j=0;j<depthMap.cols;j++)
				{
					if(Get2D()depthMap,i,j)>25 
						&& depthMap.at<unsigned short>(i,j)<1000)
					{
						filtered.at<unsigned short>(i,j) = 255;
					}
					else{
					filtered.at<unsigned short>(i,j) = 0;
					}
				}
			}*/

			//filtered.convertTo(filteredbw, CV_8U, 1.0/400);
			//filtered.convertTo(filtered8,CV_8U,1.0/256);

			circle(bgrImage,cvPoint(200,200),2,cvScalar(0,255,0),5);
			circle(filtered,cvPoint(200,200),2,cvScalar(0,255,0),5);

			writer.write(depthMap);
            imshow("DISP",dispImage);
			imshow("FILT",filtered);
			imshow("COLOR",bgrImage);

			cout<<capture.get(CV_CAP_PROP_FPS)<<endl;
			cout  << "depth: " << depthMap.at<unsigned short>(200,200);
			cout  << "  disp: " << dispImage.at<unsigned short>(200,200);
			cout  << "  filt: " << filtered.at<unsigned short>(200,200) << endl;

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


