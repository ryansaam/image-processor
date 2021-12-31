// #include <filesystem>
#include <iostream>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"
#include "CImg.h"

template <class T>
constexpr
std::string_view
type_name();

size_t truncate(size_t value);

int main(int argc, char** argv) {
  cimg_library::CImg<float> skittleImg("./colors/lenna_soderberg.jpeg");

  int width = skittleImg.width();
  int height = skittleImg.height();

  // std::cout << "decltype(i) is " << type_name<decltype(skittleImg(0, 0, 0, 0))>() << '\n';

  // convert to BGR
  // for (int row = 0; row < height; row++)
  //       for (int col = 0; col < width; col++) {
  //
  //           const auto R = skittleImg(col, row, 0, 0);
  //           const auto G = skittleImg(col, row, 0, 1);
  //           const auto B = skittleImg(col, row, 0, 2);
  //
  //           imgBuffer(col, row, 0, 0) = B;
  //           imgBuffer(col, row, 0, 1) = G;
  //           imgBuffer(col, row, 0, 2) = R;
  //
  //       }

  // enhancing contrast
  cv::CommandLineParser parser( argc, argv, "{@input | ./colors/lenna_soderberg.jpeg | input image}" );
  cv::Mat image = cv::imread( cv::samples::findFile( parser.get<cv::String>( "@input" ) ) );
  if( image.empty() )
  {
    std::cout << "Could not open or find the image!\n" << std::endl;
    std::cout << "Usage: " << argv[0] << " <Input image>" << std::endl;
    return -1;
  }

  cv::Mat imageBuffer = cv::Mat::zeros( image.size(), image.type() );

  double alpha = 1.8; /*< Simple contrast control */
  int beta = 0;     /*< Simple brightness control */

  for( int row = 0; row < image.rows; row++ ) {
        for( int col = 0; col < image.cols; col++ ) {
            for( int channel = 0; channel < image.channels(); channel++ ) {
                imageBuffer.at<cv::Vec3b>(row,col)[channel] =
                  cv::saturate_cast<uchar>( alpha * image.at<cv::Vec3b>(row,col)[channel] + beta );
            }
        }
  }

  imshow("Original Image", image);
  imshow("New Image", imageBuffer);
  cv::waitKey();

  // convert to gray scale
  //imgBuffer.RGBtoYCbCr().channel(0);



  // Debugging

  std::getchar();

  return 0;
}

template <class T>
constexpr
std::string_view
type_name()
{
    using namespace std;
#ifdef __clang__
    string_view p = __PRETTY_FUNCTION__;
    return string_view(p.data() + 34, p.size() - 34 - 1);
#elif defined(__GNUC__)
    string_view p = __PRETTY_FUNCTION__;
#  if __cplusplus < 201402
    return string_view(p.data() + 36, p.size() - 36 - 1);
#  else
    return string_view(p.data() + 49, p.find(';', 49) - 49);
#  endif
#elif defined(_MSC_VER)
    string_view p = __FUNCSIG__;
    return string_view(p.data() + 84, p.size() - 84 - 7);
#endif
}

size_t truncate(size_t value)
{
    if(value < 0) return 0;
    if(value > 255) return 255;

    return value;
}

// g++ -std=c++17 -I/opt/X11/include -L/opt/X11/lib -lX11 main.cpp -o build
