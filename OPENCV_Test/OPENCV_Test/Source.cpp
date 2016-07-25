#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <list>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>

# define M_PI 3.14159265358979323846 

enum EdgeType { STRAIGHT = 0, CONVEX =1, CONCAVE =2 };
using namespace std;
using namespace cv;

EdgeType determineEdgeType(double gradient[], int num_elements)
{
	double threshold = 1;
	double gradient_change = 0;
	EdgeType edge = STRAIGHT;

	for (int i = 0; i < num_elements-1; i++)
	{
		gradient_change = gradient[i] - gradient[i + 1];

		if (abs(gradient_change) > threshold)
		{
			if (gradient_change < 0)
			{
				edge = CONVEX;
				return edge;
			}
			else if(gradient_change > 0)
			{
				edge = CONCAVE;
				return edge;
			}
		}
	}

	return edge;
}

//void drawEdge(EdgeType edge, CvPoint*[], int startPt, int endPt)
//{
//	CvPoint *pt1;
//	CvPoint *pt2;
//
//	for (int i = 0; i < num_elements; i++)
//	{
//		pt1 = (CvPoint*)cvGetSeqElem(result, i);
//		if (i + 1 >= result->total)
//			pt2 = (CvPoint*)cvGetSeqElem(result, 0);
//		else
//			pt2 = (CvPoint*)cvGetSeqElem(result, i + 1);
//		
//		if (edge = CONVEX)
//		{
//
//		}
//		else if (edge = CONCAVE)
//		{
//
//		}
//		else
//		{
//
//		}
//	}
//}

bool checkDistance(CvPoint* pt1, CvPoint* pt2, double approxArchLength, double threshhold)
{
	
	double changeInX = (pt1->x - pt2->x);
	double changeInY = (pt1->y - pt2->y);
	double dist = sqrt(changeInX*changeInX + changeInY*changeInY);

	if (dist < approxArchLength + threshhold && dist > approxArchLength - threshhold)
		return true;
	return false;
}

void identifyTrueCorners(list<CvPoint*> potentialCorners, double areaOfPiece)
{
	return;
	//Convert list to array
	double approximateArchLength = sqrt(areaOfPiece * 2); //approximate distance from one corner to the opposite corner
	double threshholdLength = 15;
	int numPoints = potentialCorners.size();
	printf("%f\n",approximateArchLength);
	
	CvPoint *pt1;
	CvPoint *pt2;
	CvPoint *pt3;
	CvPoint *pt4;

	list<CvPoint*>::iterator it1 = potentialCorners.begin();
	list<CvPoint*>::iterator it2 = potentialCorners.begin();
	
	for (int i = 0; i < numPoints; i++)
	{
		pt1 = *(it1++);;
		
		for (int j = 0; j < numPoints; j++)
		{
			pt2 = *(it2++);

			if (!checkDistance(pt1, pt2, approximateArchLength, threshholdLength)) //Distance to next point is too small, so it won't be a corner, skip
				continue;
			
			
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			
			//list<CvPoint*>::iterator it4 = potentialCorners.begin();

			for (int k = 0; k < numPoints; k++)
			{
				list<CvPoint*>::iterator it3 = potentialCorners.begin();
				if (*it3 == *it2 || *it3 == *it1)
				{
					it3++;
				}
					
					
			}
		}
	}
}

void identifyCorners(CvSeq* pieceContour, IplImage* img, double area)
{
	list <CvPoint*> potentialCorners;
	CvPoint *pt1;
	CvPoint *pt2;
	CvPoint *pt3;
	double gradient1Theta = 0;
	double gradient2Theta = 0;
	double angle;
	double angleThreshold = 19;

	for (int i = 0; i < pieceContour->total; i++)
	{
		 
		gradient2Theta = gradient1Theta;
		
		pt1 = (CvPoint*)cvGetSeqElem(pieceContour, i);
		if (i + 1 == pieceContour->total)
			pt2 = (CvPoint*)cvGetSeqElem(pieceContour, 0);
		else
			pt2 = (CvPoint*)cvGetSeqElem(pieceContour, i + 1);
		
		if (i + 2 == pieceContour->total)
			pt3 = (CvPoint*)cvGetSeqElem(pieceContour, 0);
		else if (i + 2 > pieceContour->total)
			pt3 = (CvPoint*)cvGetSeqElem(pieceContour, 1);
		else
			pt3 = (CvPoint*)cvGetSeqElem(pieceContour, i + 2);

		
		
		if (pt1->x - pt2->x != 0.0)
			gradient1Theta = atan((pt1->y - pt2->y) / (pt1->x - pt2->x));
		else
			gradient1Theta = M_PI / 2.0; //90 degrees
		
		
		
		if (pt2->x - pt3->x != 0.0)
			gradient2Theta = atan((pt2->y - pt3->y) / (pt2->x - pt3->x));
		else 
			gradient2Theta = M_PI / 2.0; //90 degrees

		
		
		angle = fabs(gradient2Theta - gradient1Theta) * 180.0 / M_PI;
		
		if (angle >= 90 - angleThreshold && angle <= 90 + angleThreshold)
		{
			cvCircle(img, *pt2, 10, cvScalar(255, 0, 255));
			potentialCorners.push_front(pt2);
		}
	}
	identifyTrueCorners(potentialCorners, area);
}

int main()
{

	IplImage* img = cvLoadImage("C:/Users/g11f0364/Desktop/Puzzles/BlackPuzzle.jpg");

	//show the original image
	cvNamedWindow("Original");
	cvShowImage("Original", img);

	//smooth the original image using Gaussian kernel to remove noise
	cvSmooth(img, img, CV_GAUSSIAN, 3, 3);

	//converting the original image into grayscale
	IplImage* imgGrayScale = cvCreateImage(cvGetSize(img), 8, 1);
	cvCvtColor(img, imgGrayScale, CV_BGR2GRAY);

	cvNamedWindow("GrayScale Image");
	cvShowImage("GrayScale Image", imgGrayScale);

	//thresholding the grayscale image to get better results
	cvThreshold(imgGrayScale, imgGrayScale, 20, 255, CV_THRESH_BINARY_INV);

	cvNamedWindow("Thresholded Image");
	cvShowImage("Thresholded Image", imgGrayScale);

	CvSeq* contour;  //hold the pointer to a contour
	CvSeq* result;   //hold sequence of points of a contour
	CvMemStorage *storage = cvCreateMemStorage(0); //storage area for all contours
	
	//finding all contours in the image
	cvFindContours(imgGrayScale, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0, 0));

	//iterating through each contour
	while (contour)
	{
		//obtain a sequence of points of the countour, pointed by the variable 'countour'
		result = cvApproxPoly(contour, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 5.0, 0);

		if (fabs(cvContourArea(result, CV_WHOLE_SEQ))>100 && fabs(cvContourArea(result, CV_WHOLE_SEQ))<500000) 
		{
			//iterating through each point
			double sumX = 0;
			double sumY = 0;
			CvPoint *pt1;
			CvPoint *pt2;
			
			for (int i = 0; i < contour->total; i++)
			{
				pt1 = (CvPoint*)cvGetSeqElem(contour, i);
				sumX += pt1->x;
				sumY += pt1->y;
			}
			
			
			for (int i = 0; i<result->total; i++)
			{
				pt1 = (CvPoint*)cvGetSeqElem(result, i);
				
				if (i + 1 >= result->total)
					pt2 = (CvPoint*)cvGetSeqElem(result, 0);
				else
					pt2 = (CvPoint*)cvGetSeqElem(result, i+1);
			
				//cvLine(img, *pt1, *pt2, cvScalar(255, 0, 0), 4);
				
			}
			int centroidX = sumX / contour->total;
			int centroidY = sumY / contour->total;
			cvCircle(img, CvPoint(centroidX, centroidY), 5, cvScalar(0, 255, 255),-1);
			double area = cvContourArea(contour, CV_WHOLE_SEQ, 0);
			identifyCorners(result,img,area);
		}

		//obtain the next contour
		contour = contour->h_next;
	}

	//show the image in which identified shapes are marked   
	cvNamedWindow("Tracked");
	cvShowImage("Tracked", img);

	cvWaitKey(0); //wait for a key press

	//cleaning up
	cvDestroyAllWindows();
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&img);
	cvReleaseImage(&imgGrayScale);

	return 0;
}