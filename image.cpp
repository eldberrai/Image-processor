#include "image.h"
#include <iostream>
#include <fstream>

Color::Color() : r(0), g(0), b(0) {
}
Color::Color(float r, float g, float b) : r(r), g(g), b(b) {
}
Color::~Color() {
}
Image::Image(size_t width, size_t height)
    : colors_vec_(std::vector<Color>(width * height)), width_(width), height_(height) {
}
Image::~Image() {
}
Color Image::GetColor(size_t x, size_t y) const {
    return colors_vec_[y * width_ + x];
}
void Image::SetColor(const Color& color, size_t x, size_t y) {
    colors_vec_[y * width_ + x].r = color.r;
    colors_vec_[y * width_ + x].b = color.b;
    colors_vec_[y * width_ + x].g = color.g;
}
void Image::Export(const char* path) {
    std::ofstream f;
    f.open(path, std::ios::out | std::ios::binary);

    u_char pad[3] = {0, 0, 0};
    const int pad_amount = ((4 - (static_cast<int>(width_) * 3) % 4) % 4);
    const int file_size = FILE_HEADER_SIZE + INFORMATION_HEADER_SIZE +
                          static_cast<int>(width_) * static_cast<int>(height_) * 3 +
                          pad_amount * static_cast<int>(height_);

    //    BMP
    file_header[0] = 'B';
    file_header[1] = 'M';
    //    File size
    const int table_header = 2;
    file_header[table_header] = file_size;
    file_header[table_header + 1] = file_size >> bites_in_byte_;
    file_header[table_header + 2] = file_size >> bites_in_byte_ * 2;
    file_header[table_header + 3] = file_size >> bites_in_byte_ * 3;
    //    Not used
    const int table_header2 = 6;
    file_header[table_header2] = 0;
    file_header[table_header2 + 1] = 0;
    file_header[table_header2 + 2] = 0;
    file_header[table_header2 + 3] = 0;
    //    Pixel data
    const int table_pixel_data = 10;
    file_header[table_pixel_data] = FILE_HEADER_SIZE + INFORMATION_HEADER_SIZE;
    file_header[table_pixel_data + 1] = 0;
    file_header[table_pixel_data + 2] = 0;
    file_header[table_pixel_data + 3] = 0;

    // information header size
    info_header[0] = INFORMATION_HEADER_SIZE;
    info_header[1] = 0;
    info_header[2] = 0;
    info_header[3] = 0;
    // width
    const int table_width = 4;
    info_header[table_width] = width_;
    info_header[table_width + 1] = width_ >> bites_in_byte_;
    info_header[table_width + 2] = width_ >> bites_in_byte_ * 2;
    info_header[table_width + 3] = width_ >> bites_in_byte_ * 3;
    // height
    const int table_height = 8;
    info_header[table_height] = height_;
    info_header[table_height + 1] = height_ >> bites_in_byte_;
    info_header[table_height + 2] = height_ >> bites_in_byte_ * 2;
    info_header[table_height + 3] = height_ >> bites_in_byte_ * 3;
    // planes
    const int table_planes = 12;
    info_header[table_planes] = 1;
    info_header[table_planes + 1] = 0;
    // bpp
    const int table_bpp = 14;
    const int bits_per_pixel = 24;
    info_header[table_bpp] = bits_per_pixel;
    info_header[table_bpp + 1] = 0;
    // other
    const int table_a = 16;
    info_header[table_a] = 0;
    info_header[table_a + 1] = 0;
    info_header[table_a + 2] = 0;
    info_header[table_a + 3] = 0;
    info_header[table_a + 4] = 0;
    const int table_b = 21;
    info_header[table_b] = 0;
    info_header[table_b + 1] = 0;
    info_header[table_b + 2] = 0;
    info_header[table_b + 3] = 0;
    info_header[table_b + 4] = 0;
    const int table_c = 26;
    info_header[table_c] = 0;
    info_header[table_c + 1] = 0;
    info_header[table_c + 2] = 0;
    info_header[table_c + 3] = 0;
    info_header[table_c + 4] = 0;
    const int table_d = 31;
    info_header[table_d] = 0;
    info_header[table_d + 1] = 0;
    info_header[table_d + 2] = 0;
    info_header[table_d + 3] = 0;
    info_header[table_d + 4] = 0;
    const int table_e = 36;
    info_header[table_e] = 0;
    info_header[table_e + 1] = 0;
    info_header[table_e + 2] = 0;
    info_header[table_e + 3] = 0;
    //    info_header1 = info_header;

    f.write(reinterpret_cast<char*>(file_header), FILE_HEADER_SIZE);
    f.write(reinterpret_cast<char*>(info_header), INFORMATION_HEADER_SIZE);
    for (size_t y = 0; y < height_; ++y) {
        for (size_t x = 0; x < width_; ++x) {
            u_char r = static_cast<u_char>(GetColor(x, y).r * max_color_);
            u_char g = static_cast<u_char>(GetColor(x, y).g * max_color_);
            u_char b = static_cast<u_char>(GetColor(x, y).b * max_color_);
            u_char color[] = {b, g, r};
            f.write(reinterpret_cast<char*>(color), 3);
        }
        f.write(reinterpret_cast<char*>(pad), pad_amount);
    }
    f.close();
}
void Image::Read(const char* path) {
    std::ifstream f;
    f.open(path, std::ios::in | std::ios::binary);
    if (!f.is_open()) {
        throw std::runtime_error("Incorrect directory");
    }
    f.read(reinterpret_cast<char*>(file_header), FILE_HEADER_SIZE);
    f.read(reinterpret_cast<char*>(info_header), INFORMATION_HEADER_SIZE);
    const int table_w = 4;
    const int table_h = 8;
    width_ = info_header[table_w] + (info_header[table_w + 1] << bites_in_byte_) +
             (info_header[table_w + 2] << (bites_in_byte_ * 2)) + (info_header[table_w + 3] << (bites_in_byte_ * 3));
    height_ = info_header[table_h] + (info_header[table_h + 1] << bites_in_byte_) +
              (info_header[table_h + 2] << (bites_in_byte_ * 2)) + (info_header[table_h + 3] << (bites_in_byte_ * 3));
    colors_vec_.resize(width_ * height_);
    const int pad_amount = ((4 - (static_cast<int>(width_) * 3) % 4) % 4);
    for (size_t y = 0; y < height_; ++y) {
        for (size_t x = 0; x < width_; ++x) {
            u_char color[3];
            f.read(reinterpret_cast<char*>(color), 3);

            colors_vec_[y * width_ + x].r = static_cast<float>(color[2]) / max_color_;
            colors_vec_[y * width_ + x].g = static_cast<float>(color[1]) / max_color_;
            colors_vec_[y * width_ + x].b = static_cast<float>(color[0]) / max_color_;
        }
        f.ignore(pad_amount);
    }
    f.close();
}
void Filters::Neg(Image& im) {
    for (auto& col : im.colors_vec_) {
        col.r = 1 - col.r;
        col.b = 1 - col.b;
        col.g = 1 - col.g;
    }
}
void Filters::Grayscale(Image& im) {
    float r = 0;
    float g = 0;
    float b = 0;
    const float coef_r = 0.299;
    const float coef_g = 0.587;
    const float coef_b = 0.114;
    for (auto& col : im.colors_vec_) {
        r = col.r;
        g = col.g;
        b = col.b;
        col.r = coef_r * r + coef_g * g + coef_b * b;
        col.b = coef_r * r + coef_g * g + coef_b * b;
        col.g = coef_r * r + coef_g * g + coef_b * b;
    }
}
void Filters::Sharpening(Image& im) {
    const float five = 5;
    Matrix(0, -1, 0, -1, five, -1, 0, -1, 0).Multiply(im);
}
void Filters::Edge(Image& im, float threshold) {
    Grayscale(im);
    Matrix(0, -1, 0, -1, 4, -1, 0, -1, 0).Multiply(im);
    for (size_t y = 0; y < im.height_; ++y) {
        for (size_t x = 0; x < im.width_; ++x) {
            if (im.GetColor(x, y).r <= threshold) {
                im.colors_vec_[y * im.width_ + x].r = 0;
                im.colors_vec_[y * im.width_ + x].g = 0;
                im.colors_vec_[y * im.width_ + x].b = 0;
            } else {
                im.colors_vec_[y * im.width_ + x].r = 1;
                im.colors_vec_[y * im.width_ + x].g = 1;
                im.colors_vec_[y * im.width_ + x].b = 1;
            }
        }
    }
}

void Filters::Blur(Image& im, float sigma) {
}
template <typename T>
T Max(T a, T b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}
template <typename T>
T Min(T a, T b) {
    if (a < b) {
        return a;
    } else {
        return b;
    }
}
void Filters::Crop(Image& im, size_t w, size_t h) {
    w = Min(w, im.width_);
    h = Min(h, im.height_);
    Image work(w, h);
    for (size_t x = 0; x < w; x++) {
        for (size_t y = 0; y < h; y++) {
            work.colors_vec_[y * work.width_ + x] = im.GetColor(x, y + im.height_ - h);
        }
    }
    const size_t fh = 14;
    const size_t ih = 40;
    for (size_t i = 0; i < fh; i++) {
        im.file_header[i] = work.file_header[i];
    }

    for (size_t i = 0; i < ih; i++) {
        im.info_header[i] = work.info_header[i];
    }
    im.width_ = work.width_;
    im.height_ = work.height_;
    im.colors_vec_ = work.colors_vec_;
}
void Filters::Nokia(Image& im) {
    Filters::Grayscale(im);
    Image work = im;
    const float col = 0.85;
    const float col_bl = 0.95;
    const float coef = 0.2;
    const float bord_max = 0.8;
    const float bord_top = 0.65;
    const float bord_av = 0.45;
    const float bord_min = 0.3;
    const Color white = {col, 1, col_bl};
    const Color black = {1 - col, 1 - col, 1 - col};
    for (size_t y = 0; y < work.height_; ++y) {
        for (size_t x = 0; x < work.width_; ++x) {
            if (x == 0 || y == 0 || x == work.width_ - 1 || y == work.height_ - 1 || x == 1 || y == 1 ||
                x == work.width_ - 2 || y == work.height_ - 2) {
                im.colors_vec_[y * work.width_ + x] = white;
            } else {
                if ((x * y) % 4 != 0) {
                    continue;
                }
                float tmp = coef * work.GetColor(x - 1, y).r + coef * work.GetColor(x + 1, y).r +
                            coef * work.GetColor(x, y - 1).r + coef * work.GetColor(x, y + 1).r +
                            coef * work.GetColor(x, y).r;
                if (tmp > bord_max) {
                    im.colors_vec_[y * work.width_ + x] = white;
                    im.colors_vec_[y * work.width_ + x - 1] = white;
                    im.colors_vec_[(y - 1) * work.width_ + x] = white;
                    im.colors_vec_[(y + 1) * work.width_ + x] = white;
                    im.colors_vec_[y * work.width_ + x + 1] = white;
                } else if (tmp > bord_top) {
                    im.colors_vec_[y * work.width_ + x] = black;
                    im.colors_vec_[y * work.width_ + x - 1] = white;
                    im.colors_vec_[(y - 1) * work.width_ + x] = white;
                    im.colors_vec_[(y + 1) * work.width_ + x] = white;
                    im.colors_vec_[y * work.width_ + x + 1] = white;
                } else if (tmp > bord_av) {
                    im.colors_vec_[y * work.width_ + x] = black;
                    im.colors_vec_[y * work.width_ + x - 1] = black;
                    im.colors_vec_[(y - 1) * work.width_ + x] = white;
                    im.colors_vec_[(y + 1) * work.width_ + x] = white;
                    im.colors_vec_[y * work.width_ + x + 1] = white;
                } else if (tmp > bord_min) {
                    im.colors_vec_[y * work.width_ + x] = white;
                    im.colors_vec_[y * work.width_ + x - 1] = white;
                    im.colors_vec_[(y - 1) * work.width_ + x] = black;
                    im.colors_vec_[(y + 1) * work.width_ + x] = black;
                    im.colors_vec_[y * work.width_ + x + 1] = black;
                } else {
                    im.colors_vec_[y * work.width_ + x] = black;
                    im.colors_vec_[y * work.width_ + x - 1] = black;
                    im.colors_vec_[(y - 1) * work.width_ + x] = black;
                    im.colors_vec_[(y + 1) * work.width_ + x] = black;
                    im.colors_vec_[y * work.width_ + x + 1] = black;
                }
            }
        }
    }
}
void Matrix::Multiply(Image& im) const {
    Image work = im;
    float r = 0;
    float g = 0;
    float b = 0;
    for (size_t y = 0; y < work.height_; ++y) {
        for (size_t x = 0; x < work.width_; ++x) {
            r = 0;
            g = 0;
            b = 0;
            r += this->a1_ * work.GetColor(Max(0, static_cast<int>(x) - 1), Max(0, static_cast<int>(y) - 1)).r;
            g += this->a1_ * work.GetColor(Max(0, static_cast<int>(x) - 1), Max(0, static_cast<int>(y) - 1)).g;
            b += this->a1_ * work.GetColor(Max(0, static_cast<int>(x) - 1), Max(0, static_cast<int>(y) - 1)).b;
            r += this->a2_ * work.GetColor(Max(0, static_cast<int>(x) - 1), Max(0, static_cast<int>(y))).r;
            g += this->a2_ * work.GetColor(Max(0, static_cast<int>(x) - 1), Max(0, static_cast<int>(y))).g;
            b += this->a2_ * work.GetColor(Max(0, static_cast<int>(x) - 1), Max(0, static_cast<int>(y))).b;
            r += this->a3_ * work.GetColor(Min(static_cast<int>(work.width_) - 1, static_cast<int>(x) + 1),
                                           Max(0, static_cast<int>(y) - 1))
                                 .r;
            g += this->a3_ * work.GetColor(Min(static_cast<int>(work.width_) - 1, static_cast<int>(x) + 1),
                                           Max(0, static_cast<int>(y) - 1))
                                 .g;
            b += this->a3_ * work.GetColor(Min(static_cast<int>(work.width_) - 1, static_cast<int>(x) + 1),
                                           Max(0, static_cast<int>(y) - 1))
                                 .b;
            r += this->c1_ * work.GetColor(Max(0, static_cast<int>(x) - 1),
                                           Min(static_cast<int>(work.height_) - 1, static_cast<int>(y) + 1))
                                 .r;
            g += this->c1_ * work.GetColor(Max(0, static_cast<int>(x) - 1),
                                           Min(static_cast<int>(work.height_) - 1, static_cast<int>(y) + 1))
                                 .g;
            b += this->c1_ * work.GetColor(Max(0, static_cast<int>(x) - 1),
                                           Min(static_cast<int>(work.height_) - 1, static_cast<int>(y) + 1))
                                 .b;
            r += this->c3_ * work.GetColor(Min(static_cast<int>(work.width_) - 1, static_cast<int>(x) + 1),
                                           Min(static_cast<int>(work.height_) - 1, static_cast<int>(y) + 1))
                                 .r;
            g += this->c3_ * work.GetColor(Min(static_cast<int>(work.width_) - 1, static_cast<int>(x) + 1),
                                           Min(static_cast<int>(work.height_) - 1, static_cast<int>(y) + 1))
                                 .g;
            b += this->c3_ * work.GetColor(Min(static_cast<int>(work.width_) - 1, static_cast<int>(x) + 1),
                                           Min(static_cast<int>(work.height_) - 1, static_cast<int>(y) + 1))
                                 .b;
            r += this->b1_ * work.GetColor(Max(0, static_cast<int>(x)), Max(0, static_cast<int>(y) - 1)).r;
            g += this->b1_ * work.GetColor(Max(0, static_cast<int>(x)), Max(0, static_cast<int>(y) - 1)).g;
            b += this->b1_ * work.GetColor(Max(0, static_cast<int>(x)), Max(0, static_cast<int>(y) - 1)).b;
            r += this->b3_ * work.GetColor(Max(0, static_cast<int>(x)),
                                           Min(static_cast<int>(work.height_) - 1, static_cast<int>(y) + 1))
                                 .r;
            g += this->b3_ * work.GetColor(Max(0, static_cast<int>(x)),
                                           Min(static_cast<int>(work.height_) - 1, static_cast<int>(y) + 1))
                                 .g;
            b += this->b3_ * work.GetColor(Max(0, static_cast<int>(x)),
                                           Min(static_cast<int>(work.height_) - 1, static_cast<int>(y) + 1))
                                 .b;
            r += this->c2_ * work.GetColor(Min(static_cast<int>(work.width_) - 1, static_cast<int>(x) + 1),
                                           Max(0, static_cast<int>(y)))
                                 .r;
            g += this->c2_ * work.GetColor(Min(static_cast<int>(work.width_) - 1, static_cast<int>(x) + 1),
                                           Max(0, static_cast<int>(y)))
                                 .g;
            b += this->c2_ * work.GetColor(Min(static_cast<int>(work.width_) - 1, static_cast<int>(x) + 1),
                                           Max(0, static_cast<int>(y)))
                                 .b;
            r += this->b2_ * work.GetColor(Max(0, static_cast<int>(x)), Max(0, static_cast<int>(y))).r;
            g += this->b2_ * work.GetColor(Max(0, static_cast<int>(x)), Max(0, static_cast<int>(y))).g;
            b += this->b2_ * work.GetColor(Max(0, static_cast<int>(x)), Max(0, static_cast<int>(y))).b;
            if (r < 0) {
                r = 0;
            } else if (r > 1) {
                r = 1;
            }
            if (g < 0) {
                g = 0;
            } else if (g > 1) {
                g = 1;
            }
            if (b < 0) {
                b = 0;
            } else if (b > 1) {
                b = 1;
            }
            im.colors_vec_[y * work.width_ + x].r = r;
            im.colors_vec_[y * work.width_ + x].g = g;
            im.colors_vec_[y * work.width_ + x].b = b;
        }
    }
}
Matrix::Matrix(float a1, float a2, float a3, float b1, float b2, float b3, float c1, float c2, float c3)
    : a1_(a1), a2_(a2), a3_(a3), b1_(b1), b2_(b2), b3_(b3), c1_(c1), c2_(c2), c3_(c3) {
}
