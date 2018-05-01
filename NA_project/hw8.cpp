#include <opencv2/opencv.hpp>

/*
OpenCV는 영상처리나 컴퓨터 비전을 위한 굉장히 좋은 라이브러리 입니다.
따라서 웬만한 기능은 다 있어, 이런 기능이 있을까 하는 기능있을까 하면 그 기능이 있습니다.
그러나 학생이기 때문에 직접 짜보는 것이 중요합니다.
*/

// cv namespace를 사용하셔도 됩니다.
using namespace cv;

void bilinear_interpolation(Mat &img1, Mat &img2, float xrate, float yrate)
{
	for (int y = 0; y < img2.rows; y++)
	{
		for (int x = 0; x < img2.cols; x++)
		{
			int px = (int)(x / xrate);
			int py = (int)(y / yrate);

			double fx1 = (double)x / (double)xrate - (double)px;
			double fx2 = 1 - fx1;
			double fy1 = (double)y / (double)yrate - (double)py;
			double fy2 = 1 - fy1;

			double w1 = fx2*fy2;
			double w2 = fx1*fy2;
			double w3 = fx2*fy1;
			double w4 = fx1*fy1;

			if (img1.channels() == 1)
			{
				uchar P1 = img1.at<uchar>(py, px);
				uchar P2 = img1.at<uchar>(py, px+1);
				uchar P3 = img1.at<uchar>(py+1, px);
				uchar P4 = img1.at<uchar>(py+1, px+1);
				img2.at<uchar>(y, x) = w1*P1 + w2*P2 + w3*P3 + w4*P4;
			}
			else if (img1.channels() == 3)
			{
				Vec3b P1 = img1.at<Vec3b>(py, px);
				Vec3b P2 = img1.at<Vec3b>(py, px+1);
				Vec3b P3 = img1.at<Vec3b>(py+1, px);
				Vec3b P4 = img1.at<Vec3b>(py+1, px+1);
				img2.at<Vec3b>(y, x) = w1*P1 + w2*P2 + w3*P3 + w4*P4;
			}
		}
	}
}

int main(int argc, char *argv[])
{
	// jpg, png, bmp과 같은 이미지 관련 확장자들은 모두 사용할 수 있습니다.
	cv::Mat image = cv::imread("lena.png", CV_LOAD_IMAGE_COLOR);
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

	float n, m;

	std::cout << "input x scale and y scale" << std::endl;
	std::cin >> m;
	std::cin >> n;
	
	bilinear_interpolation(image, newimage, m, n);


	// 이미지 띄우기
	//cv::imshow("Original image", image);
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



