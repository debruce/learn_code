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

	Mat	prev;
	while (1) {
		Mat	clr, gry;
		cap >> clr;
		cvtColor(clr, gry, COLOR_BGR2GRAY);
		Mat_<uint8_t> clp(gry, Rect(384, 104, 512, 512));
		imshow("clp", clp);

		Mat f, nf, pnf;
		clp.convertTo(f, CV_32F);
		f -= mean(f)(0);
		normalize(f, nf);
		pnf = Mat::zeros(Size(1024, 1024), CV_32FC1);
		nf.copyTo(pnf(Rect(256, 256, 512, 512)));
		imshow("pnf", pnf*50.0+.5);
		Mat ft;
		dft(pnf, ft);
		Mat_<float>	cr;
		mulSpectrums(ft, ft, cr, 0, 1);

		Mat_<float> spec(Size(512, 512), CV_32FC1);
		for (int r = 0; r < cr.rows/2; r++) {
			for (int c = 0; c < cr.cols/2; c++) {
				spec(r,c) = hypotf(cr(r, 2*c+1), cr(cr.rows-r-1, 2*c+1));
			}
		}
		Mat nspec;
		normalize(spec, nspec);
		imshow("nspec", nspec*255);
#if 0
		if (!prev.empty()) {
			mulSpectrums(ft, prev, cr, 0, 0);
			for (int r = 0; r < cr.rows/2; r++) {
				for (int c = 0; c < cr.cols/2; c++) {
					spec(r,c) = hypotf(cr(r, 2*c+1), cr(cr.rows-r-1, 2*c+1));
				}
			}
			normalize(spec, nspec);
			imshow("lag", nspec*255);
		}
#endif
		waitKey(1);

		prev = ft;
	}
}
