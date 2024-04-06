#include "parser.h"

Parser Parse(int argc, char** argv) {
    Parser input;
    std::string tmp;
    for (size_t i = 0; i < static_cast<size_t>(argc); i++) {
        tmp = argv[i];
        if (input.info_in.empty() && (tmp.back() == 'p' || tmp.back() == 'P')) {
            input.info_in = tmp;
        } else if (!input.info_in.empty() && (tmp.back() == 'p' || tmp.back() == 'P') && input.info_out.empty()) {
            input.info_out = tmp;
        } else if (tmp[0] == '-') {
            input.filters.push_back(std::pair(tmp, std::vector<float>()));
        } else if (isdigit(tmp[0])) {
            input.filters.back().second.push_back(stof(tmp));
        }
    }
    return input;
}