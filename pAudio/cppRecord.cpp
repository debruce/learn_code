#include <iostream>
#include <vector>
#include <list>
#include <cmath>
#include <memory>
#include <stdint.h>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Cholesky>
#include <eigen3/Eigen/Eigenvalues>
#include <opencv/cv.hpp>
#include <opencv/cxeigen.hpp>
#include <boost/ref.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <portaudiocpp/PortAudioCpp.hxx>

using namespace	std;
using namespace	cv;
using namespace	Eigen;
using namespace	boost;

void myEig(const char* name, MatrixXd em)
{
	Mat	m, n;
	eigen2cv(em, m);
	normalize(m, n, 0, 1, NORM_MINMAX);
	imshow(name, n);
}

void myCorr(const char* name, Mat m)
{
	Mat	z, n;
	normalize(m, n, 0, 1, NORM_MINMAX);
	// resize(n, z, Size(m.cols*4, m.rows*4), 0, 0, INTER_NEAREST);
	imshow(name, n);
}

void myShow(const char* name, Mat m)
{
	Mat	z, n;
	normalize(m, n, 0, 1, NORM_MINMAX);
	resize(n, z, Size(m.cols/4, m.rows*16), 0, 0, INTER_NEAREST);
	imshow(name, z);
}

const int	numBufs = 4;

class AudioBuffer : public thread {
	uint16_t	numChannels;
	uint16_t	maxFrames;
	bool		ready;
	uint64_t	writePtr;
	uint64_t	readPtr;
	vector<float>*	bufs[numBufs];
	Mat_<float>	hann;
public:
	AudioBuffer(uint16_t _numChannels, uint16_t _maxFrames) : numChannels(_numChannels), maxFrames(_maxFrames) { }

	~AudioBuffer()
	{
	}

	void operator()()
	{
		namedWindow("data");
		namedWindow("psi");
		namedWindow("phase");
		hann = Mat::zeros(1, maxFrames, CV_32F);
		for (int i = 0; i < maxFrames; i++) {
			hann(0,i) = .5*(1 - cosf(2*M_PI*i/(maxFrames - 1)));
		}
		for (int i = 0; i < numBufs; i++)
			bufs[i] = new vector<float>(numChannels*maxFrames);
		writePtr = 0;
		readPtr = 0;
		ready = true;

		while (true) {
			if (readPtr < writePtr) {
				vector<float>& buf = *bufs[readPtr % numBufs];

				Mat_<float> data = Mat::zeros(numChannels, maxFrames, CV_32F);
				for (int c = 0; c < numChannels; c++) {
					for (int f = 0; f < maxFrames; f++) {
						data(c, f) = buf[f * numChannels + c];
					}
				}
				myShow("data", data);

				Mat_<float> left = Mat::zeros(1, maxFrames, CV_32F);
				Mat_<float> right = Mat::zeros(1, maxFrames, CV_32F);
				for (int f = 0; f < maxFrames; f++) {
					left(0, f) = buf[f * numChannels + 0] * hann(0, f);
					right(0, f) = buf[f * numChannels + 2] * hann(0, f);
				}
				Point2d pt = phaseCorrelate(left, right, hann);

				int bsize = 256;
				MatrixXd r00(bsize, bsize);
				MatrixXd r20(bsize, bsize);

				for (int i = 0; i < maxFrames; i++) {
					for (int j = 0; j < maxFrames; j++) {
						r00(i % bsize, j % bsize) = buf[i * numChannels + 0] * buf[j * numChannels + 0];
						r20(i % bsize, j % bsize) = buf[i * numChannels + 2] * buf[j * numChannels + 0];
					}
				}
				MatrixXd L = r00.llt().matrixL();
				MatrixXd Li = L.inverse();
				MatrixXd Lih = Li.conjugate().transpose();
				MatrixXd psi = Li * r20 * Lih;
				myEig("psi", psi);

				EigenSolver<MatrixXd>	es(psi);
				vector<double>	szs;
				for (int i = 0; i < 256; i++) {
					szs.push_back(abs(es.eigenvalues()[i]));
				}
				sort(szs.begin(), szs.end());
				double lim = szs[256-4];
				for (int i = 0; i < 256; i++) {
					double len = abs(es.eigenvalues()[i]);
					if (len < lim) continue;
					cout << i << ": " << es.eigenvalues()[i] << endl;
				}
				cout << endl;
				cout << "pt=" << pt << endl;

				Mat drw = Mat::zeros(32, 1024, CV_8UC3);
				circle(drw, Point(512+pt.x, 16), 3, Scalar(0,0,255));
				imshow("phase", drw);

				readPtr++;
			}
			waitKey(1);
		}
	}

	int recordCallback(const void* inBuf, void* outBuf, unsigned long framesPerBuf, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags flags)
	{
		if (!ready) return paContinue;
		if (writePtr > readPtr+numBufs-1) return paContinue;
		const float* data = (const float*)inBuf;
		vector<float>& buf = *bufs[writePtr % numBufs];
		memcpy(&buf[0], inBuf, sizeof(buf[0])*numChannels*maxFrames);
		writePtr++;
		return paContinue;
	}
};

int main(int argc, char *argv[])
{
	try {
		portaudio::AutoSystem	autosys;
		portaudio::System&	sys = portaudio::System::instance();

		int	inputIndex;
		int	maxChannels = 0;
		for (portaudio::System::DeviceIterator i = sys.devicesBegin(); i != sys.devicesEnd(); ++i) {
			cout << (*i).index() << ": " << (*i).name() << ", "
				 << "in=" << (*i).maxInputChannels() << " "
				 << "out=" << (*i).maxOutputChannels() << ", "
				 << (*i).hostApi().name();
			if ((*i).isSystemDefaultInputDevice()) cout << ", default input";
			if ((*i).isSystemDefaultOutputDevice()) cout << ", default output";
			cout << endl;

			if ((*i).maxInputChannels() > maxChannels) {
				maxChannels = (*i).maxInputChannels();
				inputIndex = (*i).index();
			}
		}
		cout << "inputIndex=" << inputIndex << " maxChannels=" << maxChannels << endl;

		AudioBuffer	buf(maxChannels, 4096);

		cout << "Opening recording input stream on " << sys.deviceByIndex(inputIndex).name() << endl;
		portaudio::DirectionSpecificStreamParameters inParamsRecord(sys.deviceByIndex(inputIndex), maxChannels, portaudio::FLOAT32, true, sys.deviceByIndex(inputIndex).defaultLowInputLatency(), NULL);
		portaudio::StreamParameters paramsRecord(inParamsRecord, portaudio::DirectionSpecificStreamParameters::null(), 48000, 4096, paClipOff);
		portaudio::MemFunCallbackStream<AudioBuffer> streamRecord(paramsRecord, buf, &AudioBuffer::recordCallback);
		streamRecord.start();
		cout << "after start" << endl;
		buf();
		streamRecord.stop();
		cout << "after stop" << endl;
		streamRecord.close();
		cout << "after close" << endl;
		sys.terminate();
		cout << "after terminate" << endl;
	}
	catch (const portaudio::PaException& e) {
		cout << "A PortAudio error occured: " << e.paErrorText() << endl;
	}
	catch (const portaudio::PaCppException& e) {
		cout << "A PortAudioCpp error occured: " << e.what() << endl;
	}
	catch (const std::exception& e) {
		cout << "A generic exception occured: " << e.what() << endl;
	}
	catch (...) {
		cout << "A unknown exception occured: " << endl;
	}
	return 0;
}
