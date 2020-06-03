//
// Created by mingj on 2020/6/2.
//

#include <iostream>
#include <ndn-cxx/interest.hpp>
#include <ndn-cxx/face.hpp>
#include <ndn-cxx/data.hpp>
#include "utils/FileUtils.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

using namespace std;
using namespace ndn;

int main(int argc, char **argv) {
    if (argc < 3) {
        cout << "Usage : " << argv[0] << " -f <json file path> " << endl;
        return -1;
    }
    Interest::setDefaultCanBePrefix(true);
    string jsonStr = FileUtils::readFile2String(argv[2]);;
    ndn::Face face;
    boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
    string uuid_string = boost::uuids::to_string(a_uuid);
    ndn::Interest interest("/localmanager/mircertification/" + uuid_string);
    interest.setMustBeFresh(true);
    interest.setApplicationParameters((uint8_t *) jsonStr.c_str(), jsonStr.size());
    face.expressInterest(interest, [=](const Interest &interest1, const Data &data) {
        // onData
        const auto &content = data.getContent();
        string result((const char *) content.value(), content.value_size());
        cout << "receive: " << endl << result << endl;
    }, [=](const Interest &interest1, const lp::Nack &nack) {
        // onNack
        cerr << "onNack: " << interest1.toUri() << endl << "reason: " << nack.getReason() << endl;
    }, [=](const Interest &interest1) {
        // onTimeout
        cerr << "onTimeout: " << interest1.toUri() << endl;
    });

    face.processEvents();
}

