#include <sil/sil.hpp>
#include "random.hpp"
#include <vector>
#include <algorithm>
#include <complex>
#include <glm/gtx/matrix_transform_2d.hpp>

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

void mirror(sil::Image& image)
{
  sil::Image mirrored {image.width(), image.height()};
  for (int x{0}; x < image.width(); ++x)
  {
    for (int y{0}; y < image.height(); ++y)
    {
      mirrored.pixel(x, y) = image.pixel(image.width() - 1 - x, y);
    }
  }
  image = mirrored;
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

void rotate(sil::Image& image)
{
  sil::Image new_image {image.height(), image.width()};
  for (int x{0}; x < image.width(); ++x)
  {
    for (int y{0}; y < image.height(); ++y)
    {
      new_image.pixel(y,x) = image.pixel(x,y);
    }
  }
  image = new_image;
}

void chromatic_abberation(sil::Image& image, int step)
{
  sil::Image new_image {image.width(), image.height()};
  for (int x{0}; x < image.width(); ++x)
  {
    for (int y{0}; y < image.height(); ++y)
    {
      new_image.pixel(x,y).g = image.pixel(x,y).g;
      if (x < image.width()-step) {
        new_image.pixel(x,y).b = image.pixel(x+step,y).b;
      }
      if (x > step) {
        new_image.pixel(x,y).r = image.pixel(x-step,y).r;
      }
    }
  }
  image = new_image;
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

void disk(sil::Image& image, int r, int center_x, int center_y)
{
  for (int x{0}; x < image.width(); ++x)
  {
    for (int y{0}; y < image.height(); ++y)
    {
      int distance = sqrt(pow(center_x - x, 2) + pow(center_y - y,2));
      if (distance < r) image.pixel(x,y) = glm::vec3{1};
    }
  }
}

void circle(sil::Image& image, int r, int thickness, int center_x, int center_y)
{
  for (int x{0}; x < image.width(); ++x)
  {
    for (int y{0}; y < image.height(); ++y)
    {
      int distance = sqrt(pow(center_x - x, 2) + pow(center_y - y,2));
      if (distance < r && distance > r - thickness) image.pixel(x,y) = glm::vec3{1};
    }
  }
}

void rosette(sil::Image& image, int r, int thickness, int number)
{
  circle(image, r, thickness, image.width()/2, image.height()/2);

  for (int i{0}; i < number; ++i)
  {
    int new_center_x = image.width()/2 + r*cos(i*2*M_PI/number);
    int new_center_y = image.height()/2 + r*sin(i*2*M_PI/number);

    circle(image, r, thickness, new_center_x, new_center_y);
  }

}

// mosaic v1 : same dimensions as orginal image --> worse quality/more pixelated
// not functional
void small_mosaic(sil::Image& image, int number)
{
  sil::Image new_image {image.width(), image.height()};
  for (int x{0}; x < image.width(); ++x)
  {
    for (int y{0}; y < image.height(); ++y)
    {
      for (int i{0}; i < image.width()/number; ++i)
      {
        if (x % i == 0 && y % i == 0)
        {
          new_image.pixel(x,y) = image.pixel(i, i);
        }
      }
    }    
  }
  image = new_image;
}
// mosaic v2 : create new image with dimensions = a multiple of original dimensions
void mosaic(sil::Image& image, int number)
{
  sil::Image new_image {image.width()*number, image.height()*number};
  for (int x{0}; x < new_image.width(); ++x)
  {
    for (int y{0}; y < new_image.height(); ++y)
    {
      new_image.pixel(x,y) = image.pixel(x%image.width(), y%image.height());
    }    
  }
  image = new_image;
}

void mirror_mosaic(sil::Image& image, int number)
{
  sil::Image new_image {image.width()*number, image.height()*number};
  for (int x{0}; x < new_image.width(); ++x)
  {
    for (int y{0}; y < new_image.height(); ++y)
    {
      if ((y/image.height())%2 == 1) 
      {
        new_image.pixel(x,y) = image.pixel(x%image.width(), image.height() - 1 - y%image.height());
      }
      else 
      {
        new_image.pixel(x,y) = image.pixel(x%image.width(), y%image.height());
      }
    }    
  }
  image = new_image;
}

void glitch(sil::Image& image)
{
  int number = random_int(20,50);
  for (int i{0}; i < number; i++)
  {
    int width {random_int(5,50)}; 
    int height {random_int(5,10)};

    int orig_x {random_int(1,image.width()-width)}; 
    int orig_y {random_int(1,image.height()-height)};
    
    int dest_x {random_int(1,image.width()-width)}; 
    int dest_y {random_int(1,image.height()-height)};

    for (int i{0}; i < width; i++)
    {

      for (int j{0}; j < height; j++)
      {
        std::swap(image.pixel(orig_x+i,orig_y+j), image.pixel(dest_x+i,dest_y+j));
      }    
    }
  }
}

float brightness(glm::vec3 const& color) 
{
  return color.r * 0.2126 + color.g * 0.7152 + color.b * 0.0722;
}
void sort_pixels(sil::Image& image)
{
  std::sort(image.pixels().begin(), image.pixels().end(), [](glm::vec3 const& color1, glm::vec3 const& color2)
  {
    return brightness(color1) < brightness(color2);
  });
}
void sort_pixels_by_line(sil::Image& image)
{
  for (int y{0}; y < image.height(); ++y)
  {
    std::sort(image.pixels().begin() + image.width()*y, image.pixels().begin() + image.width()*(y+1), [](glm::vec3 const& color1, glm::vec3 const& color2)
      {
        return brightness(color1) < brightness(color2);
      });
  }
}
void sort_pixels_random(sil::Image& image)
{

  for (int i{0}; i < 150; ++i)
  {
    int start = random_int(1, image.pixels().size());

    int step = random_int(50,150);

    std::sort(image.pixels().begin() + start, image.pixels().begin() + start + step, [](glm::vec3 const& color1, glm::vec3 const& color2)
    {
      return brightness(color1) < brightness(color2);
    });
  }
}

void fractal(sil::Image& image)
{
  int iterations = 50;
  for (int x{0}; x < image.width(); ++x)
  {
    for (int y{0}; y < image.height(); ++y)
    {
      std::complex<float> z{0.f, 0.f};
      std::complex<float> c{x/125.f - 2, y/125.f - 2};
      for (int i{0}; i < iterations; i++)
      {
        z = z * z + c;
        if (std::abs(z) > 2) {
          break;
        }
        if (i < iterations/16) {
          image.pixel(x,y) = glm::vec3{0.1};
        } else if (i < iterations/8) {
          image.pixel(x,y) = glm::vec3{0.2};
        } else if (i < iterations/4) {
          image.pixel(x,y) = glm::vec3{0.5};
        } else if (i < iterations/2) {
          image.pixel(x,y) = glm::vec3{0.8};
        } else {
          image.pixel(x,y) = glm::vec3{1};
        }
      }
    }
  }
}

glm::vec3 linear_srgb_to_oklab(glm::vec3 c) 
{
  float l = 0.4122214708f * c.r + 0.5363325363f * c.g + 0.0514459929f * c.b;
	float m = 0.2119034982f * c.r + 0.6806995451f * c.g + 0.1073969566f * c.b;
	float s = 0.0883024619f * c.r + 0.2817188376f * c.g + 0.6299787005f * c.b;

  float l_ = cbrtf(l);
  float m_ = cbrtf(m);
  float s_ = cbrtf(s);

  return {
    0.2104542553f*l_ + 0.7936177850f*m_ - 0.0040720468f*s_,
    1.9779984951f*l_ - 2.4285922050f*m_ + 0.4505937099f*s_,
    0.0259040371f*l_ + 0.7827717662f*m_ - 0.8086757660f*s_,
  };
}

glm::vec3 oklab_to_linear_srgb(glm::vec3 c) 
{
    float l_ = c[0] + 0.3963377774f * c[1] + 0.2158037573f * c[2];
    float m_ = c[0] - 0.1055613458f * c[1] - 0.0638541728f * c[2];
    float s_ = c[0] - 0.0894841775f * c[1] - 1.2914855480f * c[2];

    float l = l_*l_*l_;
    float m = m_*m_*m_;
    float s = s_*s_*s_;

    return {
		+4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s,
		-1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s,
		-0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s,
    };
}

glm::vec3 srgb_to_linear_rgb(glm::vec3 color) {
  if (color.r <= 0.04045) 
  {
    color.r /= 12.92;
  } else {
    color.r = pow((color.r + 0.055)/1.055, 2.4);
  } 
  if (color.g <= 0.04045) 
  {
    color.g /= 12.92;
  } else {
    color.g = pow((color.g + 0.055)/1.055, 2.4);
  } 
  if (color.b <= 0.04045) 
  {
    color.b /= 12.92;
  } else {
    color.b = pow((color.b + 0.055)/1.055, 2.4);
  } 
  return {
    color.r, color.g, color.b
  };
}

void srgb_gradient(sil::Image& image, glm::vec3 color1, glm::vec3 color2)
{
  for (int x{0}; x < image.width(); ++x)
  {
    for (int y{0}; y < image.height(); ++y)
    {
      image.pixel(x, y) = glm::mix(color1, color2, static_cast<float>(x) / static_cast<float>(image.width() - 1));
    }
  }
}

void linear_rgb_gradient(sil::Image& image, glm::vec3 color1, glm::vec3 color2)
{
  glm::vec3 linear_color1 {srgb_to_linear_rgb(color1)};
  glm::vec3 linear_color2 {srgb_to_linear_rgb(color2)};
  for (int x{0}; x < image.width(); ++x)
  {
    for (int y{0}; y < image.height(); ++y)
    {
      image.pixel(x, y) = glm::mix(linear_color1, linear_color2, static_cast<float>(x) / static_cast<float>(image.width() - 1));
    }
  }
}

// not functional
void oklab_gradient(sil::Image& image, glm::vec3 color1, glm::vec3 color2)
{
  glm::vec3 oklab_color1 {linear_srgb_to_oklab(srgb_to_linear_rgb(color1))};
  glm::vec3 oklab_color2 {linear_srgb_to_oklab(srgb_to_linear_rgb(color2))};
  for (int x{0}; x < image.width(); ++x)
  {
    for (int y{0}; y < image.height(); ++y)
    {
      image.pixel(x, y) = glm::mix(oklab_color1, oklab_color2, static_cast<float>(x) / static_cast<float>(image.width() - 1));
      image.pixel(x, y) = oklab_to_linear_srgb(image.pixel(x, y));
    }
  }
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

// not functional
void normalize(sil::Image& image) 
{
  glm::vec3& lightest {image.pixels()[0]};
  glm::vec3& darkest {image.pixels()[0]};
  for (glm::vec3& color : image.pixels())
  {
    if (brightness(color) > brightness(lightest)) lightest = color;
    if (brightness(color) < brightness(darkest)) darkest = color;
  }
  float r_trans = (1 - lightest.r) - darkest.r;
  float g_trans = (1 - lightest.g) - darkest.g;
  float b_trans = (1 - lightest.b) - darkest.b;

  for (glm::vec3& color : image.pixels())
  {
    color.r = (color.r + r_trans);
    color.g = (color.g + g_trans);
    color.b = (color.b + b_trans);
  }
  
}

glm::vec2 rotated(glm::vec2 point, glm::vec2 center_of_rotation, float angle)
{
  return glm::vec2{glm::rotate(glm::mat3{1.f}, angle) * glm::vec3{point - center_of_rotation, 0.f}} + center_of_rotation;
}

// not fuctional
void vortex(sil::Image& image, int center_x, int center_y, float angle)
{
  for (int x{0}; x < image.width(); ++x)
  {
    for (int y{0}; y < image.height(); ++y)
    {
      glm::vec2 center {center_x, center_y};
      glm::vec2 new_position {rotated(image.pixel(x,y), center, angle)};

      // if (glm::distance(center, new_position) < )
      image.pixel(new_position.x, new_position.y) = image.pixel(x,y);
    }
  }
}

int main()
{
  sil::Image image{"images/logo.png"};
  sil::Image photo{"images/photo.jpg"};
  sil::Image low_contrast{"images/photo_faible_contraste.jpg"};

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

  // mirror(image);
  // image.save("output/mirror.png");

  // add_noise(image, 0.5);
  // image.save("output/add_noise.png");

  // rotate(image);
  // image.save("output/rotate.png"); 

  // chromatic_abberation(image, 25);
  // image.save("output/chromatic_abberation.png"); 

  // luminosity(photo, 2); // strange red pixels 
  // photo.save("output/darken.png"); 
  // luminosity(photo, 0.1);
  // photo.save("output/lighten.png"); 

  // sil::Image blank_image{500/*width*/, 500/*height*/};
  // disk(blank_image, 100, blank_image.width()/2, blank_image.height()/2);
  // blank_image.save("output/disk.png");

  // sil::Image blank_image{500/*width*/, 500/*height*/};
  // circle(blank_image, 100, 10, blank_image.width()/2, blank_image.height()/2);
  // blank_image.save("output/circle.png");

  // animation
  // int step {10}; // the distance the disk travels between two frames
  // for (int frame{0}; frame < 50; ++frame)
  // {
  //   sil::Image blank_image{500, 500};
  //   int center_x {frame*step};
  //   disk(blank_image, 100, center_x, blank_image.height()/2);
  //   blank_image.save("output/animation/" + std::to_string(frame) + ".jpg");
  // }

  // sil::Image blank_image{500/*width*/, 500/*height*/};
  // rosette(blank_image, 100, 6, 7);
  // blank_image.save("output/rosette.png");

  // small_mosaic(image, 5);
  // image.save("output/small_mosaic.png");
  // mosaic(image, 5);
  // image.save("output/mosaic.png");

  // mirror_mosaic(image, 5);
  // image.save("output/mirror_mosaic.png");

  // glitch(image);
  // image.save("output/glitch.png");

  // sort_pixels(image); // not functional
  // image.save("output/sort_all.png");
  // sort_pixels_by_line(image);
  // image.save("output/sort_line.png");
  // sort_pixels_random(image);
  // image.save("output/sort_random.png");

  // sil::Image blank_image{500/*width*/, 500/*height*/};
  // fractal(blank_image);
  // blank_image.save("output/fractal.png");

  // sil::Image blank_image{500/*width*/, 500/*height*/};
  // glm::vec3 pink{1,0.8,0.8};
  // glm::vec3 blue{0.5,0.8,0.9};
  // srgb_gradient(blank_image, pink, blue);
  // blank_image.save("output/srgb_gradient.png");
  // linear_rgb_gradient(blank_image, pink, blue);
  // blank_image.save("output/linear_rgb_gradient.png");
  // oklab_gradient(blank_image, pink, blue);
  // blank_image.save("output/oklab_gradient.png");

  // dithering(photo);
  // photo.save("output/dithering.png");

  // normalize(low_contrast);
  // low_contrast.save("output/normalize.png");

  // vortex(image, image.width()/2, image.height()/2, 50);
  // image.save("output/vortex.png");

}
