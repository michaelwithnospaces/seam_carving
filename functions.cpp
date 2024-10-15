#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"

using std::cout, std::endl, std::string;

void initializeImage(Pixel image[][MAX_HEIGHT]) {
  // iterate through columns
  for (unsigned int col = 0; col < MAX_WIDTH; col++) {
    // iterate through rows
    for (unsigned int row = 0; row < MAX_HEIGHT; row++) {
      // initialize pixel
      image[col][row] = {0, 0, 0};
    }
  }
}

void loadImage(string filename, Pixel image[][MAX_HEIGHT], unsigned int& width, unsigned int& height) {
  std::ifstream s(filename);

  if (!s.is_open())
  {
    throw std::runtime_error("Failed to open " + filename);
  }

  std::string s_preamble;
  int f_width, f_height, max_val;

  s >> s_preamble;
  // std::cout << "file type read: " << s_preamble << std::endl;
 
  if (s_preamble != "P3" && s_preamble != "p3")
  {
    throw std::runtime_error("Invalid type " + s_preamble );
  }

  s >> f_width >> f_height;

  if (f_width < 0 || f_width > static_cast<int>(MAX_WIDTH) || f_height < 0 || f_height > static_cast<int>(MAX_HEIGHT) || s.fail())
  {
    throw std::runtime_error("Invalid dimensions");
  }

  width = f_width;
  height = f_height;

  s >> max_val;

  if (max_val != 255)
  {
    throw std::runtime_error("Invalid max color value");
  }

  // collection is in row major
  for (unsigned int row = 0; row < height; row++)
  {
    for (unsigned int col = 0; col < width; col++)
    {
      short r, g, b;
      if (!(s >> r >> g >> b)) throw std::runtime_error("Not enough values");

      if (r < 0 || r > max_val || g < 0 || g > max_val || b < 0 || b > max_val) throw std::runtime_error("Invalid color value");

      image[col][row] = {r, g, b}; // converts to column major
    }
  }

  short extra;
  if (s >> extra) throw std::runtime_error("Too many values");
}

void outputImage(string filename, Pixel image[][MAX_HEIGHT], unsigned int width, unsigned int height) {
  std::ofstream s(filename);

  if (!s.is_open())
  {
    throw std::runtime_error("Failed to open " + filename);
  }

  s << "P3\n" << width << " " << height << "\n255" << std::endl;

  for (unsigned int row = 0; row < height; row++)
  {
    for (unsigned int col = 0; col < width; col++)
    {
      s << image[col][row].r << std::endl;
      s << image[col][row].g << std::endl;
      s << image[col][row].b << std::endl;
    }
  }
}

unsigned int energy(Pixel image[][MAX_HEIGHT], unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
  
  struct vec2 {
    int x;
    int y;
  };

  vec2 left, right, top, bottom;

  left.y = y;
  right.y = y;
  if (x == 0) 
  {
    left.x = width - 1;
    right.x = x + 1;
  }
  else if (x == width - 1)
  {
    left.x = x - 1;
    right.x = 0;
  }
  else
  {
    left.x = x - 1;
    right.x = x + 1;
  }

  top.x = x;
  bottom.x = x;
  if (y == 0) 
  {
    top.y = height - 1;
    bottom.y = y + 1;
  }
  else if (y == height - 1)
  {
    top.y = y - 1;
    bottom.y = 0;
  }
  else
  {
    bottom.y = y + 1;
    top.y = y - 1;
  }

  int xGradientR = image[right.x][right.y].r - image[left.x][left.y].r;
  int xGradientG = image[right.x][right.y].g - image[left.x][left.y].g;
  int xGradientB = image[right.x][right.y].b - image[left.x][left.y].b;

  int yGradientR = image[bottom.x][bottom.y].r - image[top.x][top.y].r;
  int yGradientG = image[bottom.x][bottom.y].g - image[top.x][top.y].g;
  int yGradientB = image[bottom.x][bottom.y].b - image[top.x][top.y].b;

  unsigned int energy = (xGradientR * xGradientR) + (xGradientG * xGradientG) + (xGradientB * xGradientB)
                      + (yGradientR * yGradientR) + (yGradientG * yGradientG) + (yGradientB * yGradientB);

  return energy;
}

// uncomment functions as you implement them (part 2)

unsigned int loadVerticalSeam(Pixel image[][MAX_HEIGHT], unsigned int start_col, unsigned int width, unsigned int height, unsigned int seam[]) {
  // TODO: implement (part 2)
  unsigned int curr_col = start_col;
  unsigned int curr_row = 0;
  unsigned int accumulated_energy = 0;

  while (curr_row < height)
  {
    seam[curr_row] = curr_col;
    accumulated_energy += energy(image, curr_col, curr_row, width, height);

    if (curr_row == height - 1) break;

    unsigned int min_energy = energy(image, curr_col, curr_row + 1, width, height);
    unsigned int min_col = curr_col;

    if (curr_col < width - 1)
    {
      unsigned int left_energy = energy(image, curr_col + 1, curr_row + 1, width, height);
      if (left_energy < min_energy)
      {
        min_energy = left_energy;
        min_col = curr_col + 1;
      }
    }

    if (curr_col > 0)
    {
      unsigned int right_energy = energy(image, curr_col - 1, curr_row + 1, width, height);
      if (right_energy < min_energy)
      {
        min_energy = right_energy;
        min_col = curr_col - 1;
      }
    }

    curr_col = min_col;
    curr_row++;
  }
  return accumulated_energy;
}


// unsigned int loadHorizontalSeam(Pixel image[][MAX_HEIGHT], unsigned int start_row, unsigned int width, unsigned int height, unsigned int seam[]) {
//   // TODO: implement (part 2)
//   return 0;
// }

// void findMinVerticalSeam(Pixel image[][MAX_HEIGHT], unsigned int width, unsigned int height, unsigned int seam[]) {
//   // TODO: implement (part 2)
// }

// void findMinHorizontalSeam(Pixel image[][MAX_HEIGHT], unsigned int width, unsigned int height, unsigned int seam[]) {
//   // TODO: implement (part 2)
// }

// void removeVerticalSeam(Pixel image[][MAX_HEIGHT], unsigned int& width, unsigned int height, unsigned int verticalSeam[]) {
//   // TODO: implement (part 2)
// }

// void removeHorizontalSeam(Pixel image[][MAX_HEIGHT], unsigned int width, unsigned int& height, unsigned int horizontalSeam[]) {
//   // TODO: implement (part 2)
// }
