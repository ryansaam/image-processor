// #include <filesystem>
#include <iostream>
#include "CImg.h"

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

size_t truncate(size_t value);

int main() {
  cimg_library::CImg<float> skittleImg("./colors/lena_condec.jpeg");
  // cimg_library::CImg<float> imgBuffer("./colors/original_green.jpg");

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
  // typeof skittleImg(col, row, 0, 0) is unsigned char &
  float minIntensity = 1.0f;
  float maxIntensity = 0.0f;
  cimg_library::CImg<float> imgBuffer = skittleImg.get_RGBtoHSI();
  for (int row = 0; row < height; row++)
        for (int col = 0; col < width; col++) {
            const auto I = imgBuffer(col, row, 0, 2);
            minIntensity = std::min((float)I, minIntensity);
            maxIntensity = std::max((float)I, maxIntensity);
        }

  for (int row = 0; row < height; row++)
        for (int col = 0; col < width; col++) {
          const auto I = imgBuffer(col, row, 0, 2);
          const auto newIntensity = (((float)I - minIntensity) / (maxIntensity - minIntensity));
          std::cout << "old Intensity " << (float)I << std::endl;
          std::cout << "new Intensity " << newIntensity << std::endl;
          imgBuffer(col, row, 0, 2) = newIntensity;
        }

  std::cout << "min " << minIntensity << std::endl;
  std::cout << "max " << maxIntensity << std::endl;

  cimg_library::CImg<float> outputImg = imgBuffer.get_HSItoRGB();

  // for (int row = 0; row < height; row++)
  //       for (int col = 0; col < width; col++) {
  //
  //         const int contrast = 5;
  //         float factor = (259.0f * (contrast + 255.0f)) / (255.0f * (259.0f - contrast));
  //
  //         const auto R = skittleImg(col, row, 0, 0);
  //         const auto G = skittleImg(col, row, 0, 1);
  //         const auto B = skittleImg(col, row, 0, 2);
  //
  //         // std::cout << (int)R << std::endl;
  //
  //         size_t newRed   = truncate((size_t)(factor * ((int)R - 128) + 128));
  //         size_t newGreen = truncate((size_t)(factor * ((int)G - 128) + 128));
  //         size_t newBlue  = truncate((size_t)(factor * ((int)B - 128) + 128));
  //
  //         imgBuffer(col, row, 0, 0) = (unsigned char)newRed;
  //         imgBuffer(col, row, 0, 1) = (unsigned char)newGreen;
  //         imgBuffer(col, row, 0, 2) = (unsigned char)newBlue;
  //
  //       }

  // convert to gray scale
  //imgBuffer.RGBtoYCbCr().channel(0);






  // Debugging
  cimg_library::CImgDisplay main_disp(skittleImg, "Green Skittle");
  cimg_library::CImgDisplay alt_disp(outputImg, "Edited");
  main_disp.display(skittleImg);
  alt_disp.display(outputImg);

  std::getchar();

  return 0;
}

size_t truncate(size_t value)
{
    if(value < 0) return 0;
    if(value > 255) return 255;

    return value;
}

// g++ -std=c++17 -I/opt/X11/include -L/opt/X11/lib -lX11 main.cpp -o build
