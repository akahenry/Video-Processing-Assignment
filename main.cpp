#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <string>
#include <vector>

#define ESC 27
#define ENTER 13
#define CTRLR 18
#define CTRLP 16

#define NAMEWINDOW "Video Processing"
#define BLURTRACKBAR 0
#define LOWTHRESHOLD 40
#define CANNYKERNELSIZE 3
#define SOBELDEPTH 3
#define SOBELKERNELSIZE 3
#define SOBELSCALE 1
#define SOBELDELTA 128
#define BRIGHTNESSINCREASE 10
#define BRIGHTNESSDECREASE -10
#define CONTRASTINCREASE 1.1
#define CONTRASTDECREASE 0.9
#define FLIPVERTICAL 0
#define FLIPHORIZONTAL 1

using namespace cv;
using namespace std;

class Image
{
public:
	Mat img;

	Size2i size()
	{
		return img.size();
	}
	
	void GaussianBlur(int value)
	{
		if(value%2)
			cv::GaussianBlur(img, img, Size(value, value), 0, 0);
		else
			cv::GaussianBlur(img, img, Size(value+1, value+1), 0, 0);
	}

	void Canny()
	{
		Mat contours;
		cvtColor(img, img, COLOR_RGB2GRAY);
		cv::Canny(img, contours, LOWTHRESHOLD, (long int)3*(long int)(LOWTHRESHOLD), CANNYKERNELSIZE);
		cvtColor(contours, img, COLOR_GRAY2RGB);
	}

	void Sobel()
	{
		Mat grad_x, grad_y;
		cv::Sobel(img, grad_x, SOBELDEPTH, 1, 0, SOBELKERNELSIZE, SOBELSCALE, SOBELDELTA, BORDER_DEFAULT);
		cv::Sobel(img, grad_y, SOBELDEPTH, 0, 1, SOBELKERNELSIZE, SOBELSCALE, SOBELDELTA, BORDER_DEFAULT);

		convertScaleAbs(grad_x, grad_x);
		convertScaleAbs(grad_y, grad_y);

		addWeighted(grad_x, 0.5, grad_y, 0.5, 0, img);
	}

	void Negative()
	{
		bitwise_not(img, img);
	}

	void AdjustBrightness(int delta)
	{
		img.convertTo(img, -1, 1, delta);
	}

	void AdjustContrast(double alpha)
	{
		img.convertTo(img, -1, alpha, 0);
	}

	void GrayScale()
	{
		Mat gray;
		cvtColor(img, gray, COLOR_RGB2GRAY);
		cvtColor(gray, img, COLOR_GRAY2RGB);
	}

	void Resize(float fx, float fy)
	{
		resize(img, img, Size(), fx, fy, INTER_LINEAR);
	}

	void RotateLeft()
	{
		rotate(img, img, ROTATE_90_COUNTERCLOCKWISE);
	}

	void Flip(int fcode)
	{
		flip(img, img, fcode);
	}
};

class Interface
{
private:
	vector<int> values;

public:
	void printCommands()
	{
		cout << "ESC      - Exit" << endl;
		cout << "ENTER    - Default" << endl;
		cout << "G        - Gaussian Blur" << endl;
		cout << "C        - Canny (edge detection)" << endl;
		cout << "S        - Sobel" << endl;
		cout << "N        - Negative" << endl;
		cout << "I        - Increase Brightness" << endl;
		cout << "D        - Decrease Brightness" << endl;
		cout << "U        - Increase Contrast" << endl;
		cout << "X        - Decrease Contrast" << endl;
		cout << "T        - To gray scale" << endl;
		cout << "R        - Resize to N/2xM/2" << endl;
		cout << "L        - Rotate left 90 degrees" << endl;
		cout << "H        - Horizontal flip" << endl;
		cout << "V        - Vertical flip" << endl;
		cout << "CTRL + r - Start record" << endl;
		cout << "CTRL + p - Stop record" << endl;
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

	int runOperation(Image *frame, int *lastKey, VideoWriter *record, bool *isRecording)
	{
		int key;
		bool save = false;

		key = waitKey(1);

		if (key != -1)
			*lastKey = key;

		switch (*lastKey)
		{
		case ENTER:
			break;
		case 'G':
			frame->GaussianBlur(values[BLURTRACKBAR]);
			break;
		case 'C':
			frame->Canny();
			break;
		case 'S':
			frame->Sobel();
			break;
		case 'N':
			frame->Negative();
			break;
		case 'I':
			frame->AdjustBrightness(BRIGHTNESSINCREASE);
			break;
		case 'D':
			frame->AdjustBrightness(BRIGHTNESSDECREASE);
			break;
		case 'U':
			frame->AdjustContrast(CONTRASTINCREASE);
			break;
		case 'X':
			frame->AdjustContrast(CONTRASTDECREASE);
			break;
		case 'T':
			frame->GrayScale();
			break;
		case 'R':
			frame->Resize(0.5, 0.5);
			break;
		case 'L':
			frame->RotateLeft();
			break;
		case 'H':
			frame->Flip(FLIPHORIZONTAL);
			break;
		case 'V':
			frame->Flip(FLIPVERTICAL);
			break;
		case CTRLR:
			*isRecording = true;
			break;
		case CTRLP:
			*isRecording = false;
			save = true;
			break;
		default:
			break;
		}

		if (*isRecording)
		{
			record->write(frame->img);
		}
		else if (save)
		{
			record->release();
		}

		return *lastKey;
	}
};


int main(int argc, char** argv)
{
	int camera = 0;
	int lastKey = ENTER;
	VideoCapture cap;
	Image frame;
	Interface interf;

	if (!cap.open(camera))
		return 0;

	cap.read(frame.img);
	interf.printCommands();
	namedWindow(NAMEWINDOW, WINDOW_NORMAL);
	interf.createTrackbar("Trackbar", NAMEWINDOW, min(frame.size().width, frame.size().height), BLURTRACKBAR);
	VideoWriter record("outcpp.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 24, Size(frame.size().width, frame.size().height));
	bool isRecording = false;


	for (;;)
	{
		cap.read(frame.img);
		if (frame.img.empty()) break;
		if (interf.runOperation(&frame, &lastKey, &record, &isRecording) == ESC) break;
		imshow("Cam", frame.img);
	}
	cap.release();
	return 0;
}


