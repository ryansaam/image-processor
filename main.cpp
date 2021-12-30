// #include <filesystem>
#include <iostream>
#include "CImg.h"

int main() {
  cimg_library::CImg<float> lenaCondec("./colors/lena_condec.jpeg");

  int width = lenaCondec.width();
  int height = lenaCondec.height();

  // enhancing contrast
  float minIntensity = 1.0f;
  float maxIntensity = 0.0f;
  cimg_library::CImg<float> imgBuffer = lenaCondec.get_RGBtoHSI();
  for (int row = 0; row < height; row++)
        for (int col = 0; col < width; col++) {
            const auto I = imgBuffer(col, row, 0, 2);
            minIntensity = std::min((float)I, minIntensity);
            maxIntensity = std::max((float)I, maxIntensity);
        }

  for (int row = 0; row < height; row++)
        for (int col = 0; col < width; col++) {
          auto I = imgBuffer(col, row, 0, 2);
          const auto newIntensity = (((float)I - minIntensity) / (maxIntensity - minIntensity));
          std::cout << "old Intensity " << (float)I << std::endl;

          imgBuffer(col, row, 0, 2) = newIntensity;
          I = imgBuffer(col, row, 0, 2);
          std::cout << "new Intensity " << (float)I << std::endl;
        }

  std::cout << "min " << minIntensity << std::endl;
  std::cout << "max " << maxIntensity << std::endl;

  cimg_library::CImg<float> outputImg = imgBuffer.get_HSItoRGB();

  // Debugging
  outputImg.save_jpeg("./colors/output-image.jpeg");

  std::getchar();

  return 0;
}

// g++ -std=c++17 -I/opt/X11/include -L/opt/X11/lib -lX11 main.cpp -o build
