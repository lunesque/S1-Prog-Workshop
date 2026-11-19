#include <sil/sil.hpp>
#include "random.hpp"
#include <vector>
#include <algorithm>
#include <complex>

void keep_green_only(sil::Image& image) 
{
  for (glm::vec3& color : image.pixels())
  {
    color.r = 0.f;
    color.b = 0.f;
  }
}

void channels_swap(sil::Image& image)
{
  for (glm::vec3& color : image.pixels())
  {
    std::swap(color.r, color.b);
  }
}

void grayscale(sil::Image& image)
{
  for (glm::vec3& color : image.pixels())
  {
    float lum = color.r * 0.2126 + color.g * 0.7152 + color.b * 0.0722;
    color.r = lum;
    color.g = lum;
    color.b = lum;
  }
}

void invert(sil::Image& image) 
{
  for (glm::vec3& color : image.pixels())
  {
    color.r = 1 - color.r;
    color.g = 1 - color.g;
    color.b = 1 - color.b;
  }
}

void gradient(sil::Image& image)
{
  for (int x{0}; x < image.width(); ++x)
  {
    for (int y{0}; y < image.height(); ++y)
    {
      image.pixel(x, y) = glm::vec3{static_cast<float>(x) / static_cast<float>(image.width() - 1)};
    }
  }
}

// return later
void vertical_flip(sil::Image& image)
{
  sil::Image flipped {image.width(), image.height()};
  for (int x{0}; x < image.width(); ++x)
  {
    for (int y{0}; y < image.height(); ++y)
    {
      flipped.pixel(x, y) = image.pixel(image.width() - 1 - x, y);
    }
  }
  image = flipped;
}

void add_noise(sil::Image& image, float amount) 
{
  std::size_t pixel_count {image.pixels().size()};
  for (int i{0}; i < amount*pixel_count; i++) {
    glm::vec3& random_pixel = image.pixels()[random_int(0,pixel_count)];
    random_pixel.r = random_float(0.f, 1.f);
    random_pixel.g = random_float(0.f, 1.f);
    random_pixel.b = random_float(0.f, 1.f);
  }
}

void rotate(sil::Image& image, sil::Image& new_image)
{
  for (int x{0}; x < image.width(); ++x)
  {
    for (int y{0}; y < image.height(); ++y)
    {
      new_image.pixel(y,x) = image.pixel(x,y);
    }
  }
}

void chromatic_abberation(sil::Image& image, sil::Image& new_image, int step)
{
  for (int x{0}; x < image.width(); ++x)
  {
    for (int y{0}; y < image.height(); ++y)
    {
      new_image.pixel(x,y) = image.pixel(x,y);
      if (x > step) {
        new_image.pixel(x,y).r = image.pixel(x-step,y).r;
      }
      if (x < image.width()-step) {
        new_image.pixel(x,y).b = image.pixel(x+step,y).b;
      }
    }
  }
}

void luminosity(sil::Image& image, float lightness)
{
  for (glm::vec3& color : image.pixels())
  {
    color.r = pow(color.r, lightness);
    color.g = pow(color.g, lightness);
    color.b = pow(color.b, lightness);
  }
}

void disk(sil::Image& image, int r)
{
  int center_x = {image.width()/2};
  int center_y = {image.height()/2};
  for (int x{center_x - r}; x < center_x + r; ++x)
  {
    for (int y{center_y - r}; y < center_y + r; ++y)
    {
      int distance = sqrt(pow(center_x - x, 2) + pow(center_y - y,2));
      if (distance < r )
      image.pixel(x,y) = glm::vec3{1};
    }
  }
}

void circle(sil::Image& image, int r, int thickness, int center_x, int center_y)
{
  // int center_x = {image.width()/2};
  // int center_y = {image.height()/2};
  for (int x{center_x - r}; x < center_x + r; ++x)
  {
    for (int y{center_y - r}; y < center_y + r; ++y)
    {
      int distance = sqrt(pow(center_x - x, 2) + pow(center_y - y,2));
      if (distance < r && distance > r - thickness)
      image.pixel(x,y) = glm::vec3{1};
    }
  }
}

void animation()
{

}

void rosette(sil::Image& image)
{
  int r = 100;
  int thickness = 6;
  circle(image, r, thickness, image.width()/2, image.height()/2);
  circle(image, r, thickness, image.width()/2 - r + thickness/2, image.height()/2);
  circle(image, r, thickness, image.width()/2 + r - thickness/2, image.height()/2);

  circle(image, r, thickness, 2*image.width()/5, image.height()/3);
  circle(image, r, thickness, 3*image.width()/5, 2*image.height()/3);

  circle(image, r, thickness, 2*image.width()/5, 2*image.height()/3);
  circle(image, r, thickness, 3*image.width()/5, image.height()/3);

}

void mosaic(sil::Image& image)
{
  for (int x{0}; x < image.width(); ++x)
  {
    for (int y{0}; y < image.height(); ++y)
    {
      if (x%100 == 0)
      {
        image.pixel(x,y) = image.pixel(1,1);
      } else image.pixel(x,y) = image.pixel(x,y);
      
    }
  }
}

void mirror_mosaic(sil::Image& image)
{

}

void glitch(sil::Image& image)
{

}

// to return
enum SortMode {
  all,
  line,
  any
};
float brightness(glm::vec3 const& color) 
{
  return color.r * 0.2126 + color.g * 0.7152 + color.b * 0.0722;
}
// float brightness(std::vector<glm::vec3> row)
// {
//   for (glm::vec3 color : row)
//   {
//     return color.r * 0.2126 + color.g * 0.7152 + color.b * 0.0722;
//   }
// }
void sort_pixels(sil::Image& image, SortMode mode = all)
{
  // for (glm::vec3& color : image.pixels())
  // {
  //   std::sort(v.begin(), v.end());
  // }

  // if (mode == line)
  // {
  //   std::sort(image.pixels().begin(), image.pixels().end(), [](std::vector<glm::vec3> const& row1, std::vector<glm::vec3> const& row2)
  //   {
  //     // return color1.r < color2.r; // Trie selon la composante rouge
  //     return brightness(row1) < brightness(row2);
  //   });
  // } else if (mode == any)
  // {

  // } else 
  // {
    std::sort(image.pixels().begin(), image.pixels().end(), [](glm::vec3 const& color1, glm::vec3 const& color2)
    {
      // return color1.r < color2.r; // Trie selon la composante rouge
      return brightness(color1) < brightness(color2);
    });
  // }
}

void fractal(sil::Image& image) // to return
{
  for (int x{0}; x < image.width(); ++x)
  {
    for (int y{0}; y < image.height(); ++y)
    {
      std::complex<float> z{0.f, 0.f};
      std::complex<float> c{x/125.f - 2, y/125.f - 2};
      while (std::abs(z) < 2) {
        image.pixel(x,y) = glm::vec3{1};
        z = z * z + c;
      } 
    }
  }
}

void color_gradient(sil::Image& image)
{

}

void dithering(sil::Image& image)
{
  const int bayer_r = 2;
  const int bayer_n = 4;
  const int NUM_VALUES = 3;
  float bayer_matrix_4x4[][bayer_n] = {
    {    -0.5,       0,  -0.375,   0.125 },
    {    0.25,   -0.25,   0.375, - 0.125 },
    { -0.3125,  0.1875, -0.4375,  0.0625 },
    {  0.4375, -0.0625,  0.3125, -0.1875 },
  };
  for (int sy = 0; sy < image.height(); sy++) {
        for (int sx = 0; sx < image.width(); sx++) {

            float orig_color = brightness(image.pixel(sx,sy));
            glm::vec3 color_result {1};
            float bayer_value = bayer_matrix_4x4[sy % bayer_n][sx % bayer_n];
            float output_color = orig_color + (bayer_r * bayer_value);

            if (output_color < (NUM_VALUES / 2)) {
              color_result = glm::vec3{0};
            }
            image.pixel(sx,sy) = color_result;
        }
    }
}

void normalize(sil::Image& image) 
{
  std::vector<float> lums {};
  glm::vec3& lightest {image.pixels()[0]};
  glm::vec3& darkest {image.pixels()[0]};
  for (glm::vec3& color : image.pixels())
  {
    if (brightness(color) > brightness(lightest)) lightest = color;
    if (brightness(color) < brightness(darkest)) darkest = color;
  }
  // float r_trans = (1 - lightest.r) - darkest.r;
  // float g_trans = (1 - lightest.g) - darkest.g;
  // float b_trans = (1 - lightest.b) - darkest.b;
  float r_trans = (1 - lightest.r);
  float g_trans = (1 - lightest.g);
  float b_trans = (1 - lightest.b);
  for (glm::vec3& color : image.pixels())
  {
    color.r = (color.r + r_trans)/darkest.r;
    color.g = (color.g + g_trans)/darkest.g;
    color.b = (color.b + b_trans)/darkest.b;
  }
  
}

int main()
{
  sil::Image image{"images/logo.png"};
  sil::Image photo{"images/photo.jpg"};
  sil::Image low_contrast{"images/photo_faible_contraste.jpg"};


  // TODO: modifier l'image
  // keep_green_only(image);
  // image.save("output/keep_green_only.png");

  // channels_swap(image);
  // image.save("output/channels_swap.png");

  // grayscale(image);
  // image.save("output/grayscale.png");

  // invert(image);
  // image.save("output/invert.png");

  // sil::Image blank_image{300 /*width*/, 200 /*height*/}; // Creates a black image with the given size.
  // gradient(blank_image);
  // blank_image.save("output/gradient.png");

  vertical_flip(image); // not functional
  image.save("output/vertical_flip.png");

  // add_noise(image, 0.5);
  // image.save("output/add_noise.png");

  // sil::Image new_image {image.height(), image.width()};
  // rotate(image, new_image); // currently clockwise instead of counterclockwise
  // new_image.save("output/rotate.png"); 

  // sil::Image new_image {image.width(), image.height()};
  // chromatic_abberation(image, new_image, 25); // strange red pixels 
  // new_image.save("output/chromatic_abberation.png"); 

  // luminosity(photo, 2); // strange red pixels 
  // photo.save("output/darken.png"); 
  // luminosity(photo, 0.1);
  // photo.save("output/lighten.png"); 

  // sil::Image blank_image{500/*width*/, 500/*height*/};
  // disk(blank_image, 100);
  // blank_image.save("output/disk.png");

  // circle(blank_image, 100, 10, blank_image.width()/2, blank_image.height()/2);
  // blank_image.save("output/circle.png");

  //animation

  // rosette(blank_image);
  // blank_image.save("output/rosette.png");

  // mosaic(image);
  // image.save("output/mosaic.png");

  //mirror mosaic

  // glitch(image);
  // image.save("output/glitch.png");

  // sort_pixels(image); // not functional
  // image.save("output/sort_all.png");
  // sort_pixels(image, line);
  // image.save("output/sort_line.png");
  // sort_pixels(image, random);
  // image.save("output/sort_random.png");

  // sil::Image blank_image{500/*width*/, 500/*height*/};
  // fractal(blank_image);
  // blank_image.save("output/fractal.png");

  // dithering(photo);
  // photo.save("output/dithering.png");

  // normalize(low_contrast);
  // low_contrast.save("output/normalize.png");

}
