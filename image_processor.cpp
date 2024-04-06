#include "image.h"
#include "parser.h"
#include <iostream>
#define RESET "\033[0m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"

int main(int argc, char** argv) {
    Parser a = Parse(argc, argv);
    Image image(0, 0);
    image.Read(a.info_in.c_str());
    for (const auto& f : a.filters) {
        if (f.first == "-neg") {
            Filters::Neg(image);
        } else if (f.first == "-gs") {
            Filters::Grayscale(image);
        } else if (f.first == "-sharp") {
            Filters::Sharpening(image);
        } else if (f.first == "-blur") {
            Filters::Blur(image, f.second[0]);
        } else if (f.first == "-crop") {
            Filters::Crop(image, static_cast<size_t>(f.second[0]), static_cast<size_t>(f.second[1]));
        } else if (f.first == "-edge") {
            Filters::Edge(image, f.second[0]);
        } else if (f.first == "-nokia") {
            Filters::Nokia(image);
        }
    }
    if (a.filters.empty()) {
        std::cout << BLUE << "IMAGE PROCESSOR" << RESET << std::endl;
        std::cout << "A simple image manipulation utility" << std::endl;
        std::cout << std::endl;
        std::cout << BLUE << "USAGE:" << RESET << std::endl;
        std::cout << "buld/image_processor " << YELLOW << "input_path output_path " << MAGENTA << "*filters*" << RESET
                  << std::endl;
        std::cout << std::endl;
        std::cout << BLUE << "FILTERS:" << RESET << std::endl;
        std::cout << "Negative\t" << MAGENTA << "-neg\t" << RESET << std::endl;
        std::cout << "Grayscale\t" << MAGENTA << "-gs\t" << RESET << std::endl;
        std::cout << "Sharpening\t" << MAGENTA << "-sharp\t" << RESET << std::endl;
        std::cout << "Edge Detection\t" << MAGENTA << "-edge\t" << RESET << YELLOW << "trashhold\t" << RESET
                  << std::endl;
        std::cout << "Gaussian Blur\t" << MAGENTA << "-blur\t" << RESET << YELLOW << "sigma\t" << RESET << std::endl;
        std::cout << "Crop the image\t" << MAGENTA << "-crop\t" << RESET << YELLOW << "width\t"
                  << "height\t" << RESET << std::endl;
        std::cout << "Nokia\t" << MAGENTA << "-nokia\t" << RESET << BLUE
                  << "How an old Nokia would show this image. P. S. there are only 2 pixel's colors: black and white\t"
                  << RESET << std::endl;
    }
    image.Export(a.info_out.c_str());
}
