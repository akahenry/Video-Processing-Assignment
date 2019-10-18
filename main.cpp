#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <string>
#include <vector>

#define ESC 27
#define ENTER 13

#define NAMEWINDOW "Video Processing"
#define BLURTRACKBAR 0
#define LOWTHRESHOLD 100
#define CANNYKERNELSIZE 3
#define SOBELDEPTH 3
#define SOBELKERNELSIZE 3
#define SOBELSCALE 1
#define SOBELDELTA 128
#define BRIGHTNESSINCREASE 10
#define BRIGHTNESSDECREASE 10
#define CONTRASTINCREASE 1.1
#define CONTRASTDECREASE 0.9

using namespace cv;
using namespace std;

class Interface
{
private:
	vector<int> values;

public:
	void printCommands()
	{
		cout << "ESC   - Exit" << endl;
		cout << "ENTER - Default" << endl;
		cout << "G     - Gaussian Blur" << endl;
		cout << "C     - Canny (edge detection)" << endl;
		cout << "S     - Sobel" << endl;
		cout << "N     - Negative" << endl;
		cout << "I     - Increase Brightness" << endl;
		cout << "D     - Decrease Brightness" << endl;
		cout << "U     - Increase Contrast" << endl;
		cout << "X     - Decrease Contrast" << endl;
		cout << "T     - To gray scale" << endl;
		cout << "R     - Resize to N/2xM/2" << endl;
		cout << "L     - Rotate left 90 degrees" << endl;
		cout << "H     - Horizontal flip" << endl;
		cout << "V     - Vertical flip" << endl;
		cout << "CTRL  - Start record" << endl;
		cout << "ALT   - Stop record" << endl;
	}

	void setValue(int newValue, int trackbarNumber)
	{
		if (trackbarNumber >= 0 && trackbarNumber < values.size())
			values[trackbarNumber] = newValue;
	}

	int getValue(int trackbarNumber)
	{
		return values[trackbarNumber];
	}

	void createTrackbar(string tName, string wName, int count, int trackbarNumber)
	{
		if (trackbarNumber == values.size())
		{
			values.push_back(count);
			cv::createTrackbar(tName, wName, &values[trackbarNumber], count, NULL);
		}
	}

	int runOperation(Mat* frame, int *lastKey)
	{
		int key;
		Mat grad_x, grad_y;

		key = waitKey(1);

		if (key != -1)
			*lastKey = key;

		switch (*lastKey)
		{
		case ENTER:
			break;
		case 'G':
			GaussianBlur(*frame, *frame, Size(values[int(BLURTRACKBAR)], values[int(BLURTRACKBAR)]), 0, 0);
			break;
		case 'C':
			Canny(*frame, *frame, LOWTHRESHOLD, 3*int(LOWTHRESHOLD), CANNYKERNELSIZE);
			break;
		case 'S':
			Sobel(*frame, grad_x, SOBELDEPTH, 1, 0, SOBELKERNELSIZE, SOBELSCALE, SOBELDELTA, BORDER_DEFAULT);
			Sobel(*frame, grad_y, SOBELDEPTH, 0, 1, SOBELKERNELSIZE, SOBELSCALE, SOBELDELTA, BORDER_DEFAULT);

			convertScaleAbs(grad_x, grad_x);
			convertScaleAbs(grad_y, grad_y);

			addWeighted(grad_x, 0.5, grad_y, 0.5, 0, *frame);
			break;
		case 'N':
			frame->convertTo(*frame, -1, 0, -255);
			break;
		case 'I':
			frame->convertTo(*frame, -1, 0, BRIGHTNESSINCREASE);
			break;
		case 'D':
			frame->convertTo(*frame, -1, 0, BRIGHTNESSDECREASE);
			break;
		case 'U':
			frame->convertTo(*frame, -1, CONTRASTINCREASE, 0);
			break;
		case 'X':
			frame->convertTo(*frame, -1, CONTRASTDECREASE, 0);
			break;
		case 'T':
			cvtColor(*frame, *frame, COLOR_RGB2GRAY);
			break;
		default:
			break;
		}

		return *lastKey;
	}
};


int main(int argc, char** argv)
{
	int camera = 0;
	int lastKey = ENTER;
	VideoCapture cap;
	Interface interf;

	if (!cap.open(camera))
		return 0;

	interf.printCommands();
	namedWindow(NAMEWINDOW, WINDOW_AUTOSIZE);
	interf.createTrackbar("Trackbar", NAMEWINDOW, 3, BLURTRACKBAR);


	for (;;)
	{
		Mat frame;
		cap >> frame;
		if (frame.empty()) break;
		if (interf.runOperation(&frame, &lastKey) == ESC) break;
		imshow("Cam", frame);
	}
}


