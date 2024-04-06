#pragma once

#include <vector>

struct Color {
    float r;
    float g;
    float b;
    Color();
    Color(float r, float g, float b);
    ~Color();
};

class Image {
public:
    Image(size_t width, size_t height);
    ~Image();

    Color GetColor(size_t x, size_t y) const;
    void SetColor(const Color& color, size_t x, size_t y);
    void Export(const char* path);
    void Read(const char* path);
    std::vector<Color> colors_vec_;
    size_t width_;
    size_t height_;

private:
    const int8_t bites_in_byte_ = 8;
    const float max_color_ = 255.0f;
    static const int FILE_HEADER_SIZE = 14;
    static const int INFORMATION_HEADER_SIZE = 40;

public:
    u_char file_header[FILE_HEADER_SIZE];
    u_char info_header[INFORMATION_HEADER_SIZE];
};

class Matrix {
public:
    Matrix(float a1, float a2, float a3, float b1, float b2, float b3, float c1, float c2, float c3);
    void Multiply(Image&) const;

private:
    float a1_;
    float a2_;
    float a3_;
    float b1_;
    float b2_;
    float b3_;
    float c1_;
    float c2_;
    float c3_;
};

class Filters : public Matrix {
public:
    static void Neg(Image&);
    static void Grayscale(Image&);
    static void Sharpening(Image&);
    static void Edge(Image&, float);
    static void Crop(Image&, size_t, size_t);
    static void Blur(Image&, float);
    static void Nokia(Image&);
};