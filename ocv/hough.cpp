#include <iostream>
#include <cstdint>
#include <opencv/cv.hpp>

using namespace std;
using namespace cv;

int main( int argc, char** argv )
{
	VideoCapture	cap(0);
	if (!cap.isOpened()) {
		cerr << "Failed to open camera" << endl;
		exit(-1);
	}

	size_t	angleBins = 512;
	size_t rhoBins = 256;
	vector<float> ss(angleBins);
	vector<float> cs(angleBins);
	for (size_t i = 0; i < angleBins; i++) {
		cs[i] = cosf(i * M_PI / angleBins);
		ss[i] = sinf(i * M_PI / angleBins);
	}

	while (1) {
		Mat	clr, gry;
		cap >> clr;
		cvtColor(clr, gry, COLOR_BGR2GRAY);
		Mat_<uint8_t> clp(gry, Rect(384, 104, 512, 512));
		imshow("clp", clp);
		Mat_<uint8_t> bin;
		adaptiveThreshold(clp, bin, 255, ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 19, 5);
		imshow("bin", bin);

		float xc = (bin.cols  - 1) / 2.0;
		float yc = (bin.rows  - 1) / 2.0;

		Mat_<int32_t> hough = Mat::zeros(Size(angleBins, 512), CV_32S);

		for (size_t row = 0; row < bin.rows; row++) {
			for (size_t col = 0; col < bin.cols; col++) {
				if (!bin(col, row)) continue;
				for (size_t i = 0; i < ss.size(); i++) {
					float rho = (col - xc) * cs[i] + (row - yc) * ss[i];
					int bin = hough.rows/2 + roundf(rho / 1.415);
					hough(bin, i)++;
				}
			}
		}

		int32_t mx = 0;
		for (size_t row = 0; row < hough.rows; row++) {
			for (size_t col = 0; col < hough.cols; col++) {
				if (mx < hough(row, col)) mx = hough(row, col);
			}
		}

		if (mx > 0) {
			Mat_<uint8_t> nhough = Mat::zeros(hough.size(), CV_8UC1);
			for (size_t row = 0; row < hough.rows; row++) {
				for (size_t col = 0; col < hough.cols; col++) {
					nhough(row, col) = hough(row, col) * 255 / mx;
				}
			}
			imshow("hough", nhough);
		}

		waitKey(1);
	}
}
