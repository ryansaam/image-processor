#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <array>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"

namespace fs = std::filesystem;

template <class T>
constexpr
std::string_view
type_name();

size_t truncate(size_t value);
void saveImage(const std::string &filename, const cv::Mat &image);
void bgrConversion(int argc, char** argv, const cv::Mat &image, const std::string &filename);
void bumpContrast(int argc, char** argv, const cv::Mat &image, const std::string &filename);
void grayConvert(int argc, char** argv, const cv::Mat &image, const std::string &filename);

int main(int argc, char** argv) {
  // create output directory
  fs::remove_all("processed");
  fs::create_directory("processed");

  // store all filenames in a vector
  std::vector<std::string> files;
  for (const auto &entry : fs::recursive_directory_iterator("./originals")) {
      if (entry.path() == "./originals/.DS_Store")
        continue;

      if (entry.path().extension() == ".jpeg" || entry.path().extension() == ".jpg") {

          std::string imgPath = entry.path().string();
          std::string delimiter = "/";

          size_t pos = 0;
          std::string token;
          while ((pos = imgPath.find(delimiter)) != std::string::npos) {
              token = imgPath.substr(0, pos);
              imgPath.erase(0, pos + delimiter.length());
          }

          std::array<std::string, 4> imgPrefix = { "org_", "bgr_", "con_", "gra_" };
          for (int i = 0; i < 4; i++)
              files.emplace_back(imgPrefix[i]+imgPath);

      }
      else {
        std::cout << "Fatal err: Image codec was not jpeg! Image: " << entry.path().string() << std::endl;
        return -1;
      }
  }

  // Do conversions: bgr, contrast, grayscale
  for (const auto &filename : files) {

      std::cout << filename << std::endl;

      cv::CommandLineParser parser( argc, argv, "{@input | ./originals/" + filename.substr(4) + " | input image}" );
      cv::Mat image = cv::imread( cv::samples::findFile( parser.get<cv::String>( "@input" ) ) );
      if( image.empty() )
      {
        std::cout << "Could not open or find the image!\n" << std::endl;
        std::cout << "Usage: " << argv[0] << " <Input image>" << std::endl;
        return -1;
      }

      // keep original
      if (filename.find("org") != std::string::npos) {
          saveImage(filename, image);
      }

      // convert to BGR
      else if (filename.find("bgr") != std::string::npos) {
          bgrConversion(argc, argv, image, filename);
      }

      // enhancing contrast
      else if (filename.find("con") != std::string::npos) {
          bumpContrast(argc, argv, image, filename);
      }

      // convert to gray scale
      else if (filename.find("gra") != std::string::npos) {
          grayConvert(argc, argv, image, filename);
      }

      else {
          std::cout << "ERROR: Invaild image prefix in"
                    << filename
                    << "; image must be prefix with 'org', 'bgr', 'con', or 'gra'!"
                    << std::endl;
      }

  }

  return 0;
}

void saveImage(const std::string &filename, const cv::Mat &image) {
    bool result = false;
    try
    {
        result = imwrite("./processed/"+filename, image);
    }
    catch (const cv::Exception& ex)
    {
        fprintf(stderr, "Exception converting image to JPEG format: %s\n", ex.what());
    }

    if (result)
        printf("Saved JPEG file\n");
    else
        printf("ERROR: Can't save JPEG file.\n");

    return;
}

void bgrConversion(int argc, char** argv, const cv::Mat &image, const std::string &filename) {
    cv::Mat imageBuffer = cv::Mat::zeros( image.size(), image.type() );

    cv::cvtColor(image, imageBuffer, cv::COLOR_BGR2RGB);

    saveImage(filename, imageBuffer);

    return;
}

void bumpContrast(int argc, char** argv, const cv::Mat &image, const std::string &filename) {
    cv::Mat imageBuffer = cv::Mat::zeros( image.size(), image.type() );

    const double alpha = 1.3; /*< Simple contrast control */
    const int beta = -32;     /*< Simple brightness control */

    for( int row = 0; row < image.rows; row++ ) {
          for( int col = 0; col < image.cols; col++ ) {
              for( int channel = 0; channel < image.channels(); channel++ ) {
                  imageBuffer.at<cv::Vec3b>(row,col)[channel] =
                    cv::saturate_cast<uchar>( alpha * image.at<cv::Vec3b>(row,col)[channel] + beta );
              }
          }
    }

    saveImage(filename, imageBuffer);

    return;
}

void grayConvert(int argc, char** argv, const cv::Mat &image, const std::string &filename) {
    cv::Mat imageBuffer = cv::Mat::zeros( image.size(), image.type() );

    cv::cvtColor(image, imageBuffer, cv::COLOR_BGR2GRAY);

    saveImage(filename, imageBuffer);

    return;
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

// g++ -std=c++17 main.cpp -lopencv_imgcodecs -lopencv_highgui -lopencv_core -lopencv_imgproc -o build -I/usr/local/include/opencv4
