#include <opencv2/opencv.hpp>
#include <ctime>

/*
OpenCV는 영상처리나 컴퓨터 비전을 위한 굉장히 좋은 라이브러리 입니다.
따라서 웬만한 기능은 다 있어, 이런 기능이 있을까 하는 기능있을까 하면 그 기능이 있습니다.
그러나 학생이기 때문에 직접 짜보는 것이 중요합니다.
*/

// cv namespace를 사용하셔도 됩니다.
using namespace cv;


int uniform_distribution(int* x, int* y) {
	srand(time(NULL));
	float xp, yp;
	for (int i = 0; i < 100; i++)
	{
		xp = (float)(rand()) / (float)(RAND_MAX);
		yp = (float)(rand()) / (float)(RAND_MAX);
		x[i] = (int)(640.0 * xp);
		y[i] = (int)(480.0 * yp);
	}
	return 0;
}

void dataFitting(int *x, int *y, int *rx, int *ry)
{
	double tmp, xp, yp;
	for (int i = 0; i < 95; i++)
	{
		tmp = 0.02*x[i] + 0.01*y[i] + 1.0;
		xp = (0.91*x[i] + 0.02*y[i] + 0.6) / tmp;
		yp = (0.05*x[i] + 0.95*y[i] + 1.2) / tmp;
		rx[i] = (int)(xp);
		ry[i] = (int)(yp);
	}
	//case outlier 5%
	rx[95] = 40;
	rx[96] = 60;
	rx[97] = 30;
	rx[98] = 70;
	rx[99] = 10;
	ry[95] = 70;
	ry[96] = 30;
	ry[97] = 80;
	ry[98] = 90;
	ry[99] = 40;
}

void saveFittedData(Mat &img1, Mat &img2, int *x, int *y, int *rx, int *ry)
{
	if (img1.channels() == 1)
	{
		for (int i = 0; i < 100; i++)
		{
			uchar P1 = img1.at<uchar>(y[i], x[i]);
			img2.at<uchar>(ry[i], rx[i]) = P1;
		}
	}
	else if (img1.channels() == 3)
	{
		for (int i = 0; i < 100; i++)
		{
			Vec3b P1 = img1.at<Vec3b>(y[i], x[i]);
			img2.at<Vec3b>(ry[i], rx[i]) = P1;
		}
	}
}



int main(int argc, char *argv[])
{
	int x[100];
	int y[100];
	int rx[100];
	int ry[100];
	// jpg, png, bmp과 같은 이미지 관련 확장자들은 모두 사용할 수 있습니다.
	cv::Mat image = cv::imread("lena2.png", CV_LOAD_IMAGE_COLOR);
	if (image.empty())
	{
		printf("No Image Found\n");
		return -1;
	}
	namedWindow("Original Image", WINDOW_AUTOSIZE);
	cv::imshow("Original image", image);

	int h = image.rows;
	int w = image.cols;

	int imgstate = (image.channels() == 1) ? CV_8UC1 : CV_8UC3;
	Mat newimage(h, w, imgstate, Scalar(0));

	uniform_distribution(x, y);
	dataFitting(x, y, rx, ry);
	saveFittedData(image, newimage, x, y, rx, ry);

	// 이미지 띄우기
	
	cv::imshow("new image", newimage);
	// cv::waitKey()는 imshow를 위해서 반드시 존재해야합니다. 
	//안그러면 image를 그리고 보여주기 전에 창이 사라집니다.
	cv::waitKey();
	// cv::waitKey(); --> 아무 파라미터도 넣지 않으면 키입력이 있을 때 까지 무한히 기다립니다.
	// cv::waitKey(mills); --> 넣은 숫자ms(밀리세컨드)만큼 기다립니다.
	destroyAllWindows();
	// 저장 또한 어느 확장자를 사용하셔도 무방합니다.
	//cv::imwrite("changed image", newimage);

	return 0;
}



