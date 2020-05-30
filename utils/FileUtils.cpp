//
// Created by mingj on 2020/5/30.
//

#include "FileUtils.h"
#include <fstream>
#include <sstream>

std::string FileUtils::readFile2String(const std::string &filename) {
    std::ifstream ifile(filename);
    std::stringstream ss;
    ss << ifile.rdbuf();
    return ss.str();
}
