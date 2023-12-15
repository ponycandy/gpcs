#pragma once
#include <boost/serialization/vector.hpp>
namespace gpcs
{
    struct mat 
    {
        int rows;
        int cols;
        std::vector<std::vector<double>> data;
        template<class Archive>
        void serialize(Archive& ar, const unsigned int version) {
            ar& rows;
            ar& cols;
            ar& data;
        }
        std::vector<double>& operator[] (int i) {
            return data[i];
        }
        const std::vector<double>& operator[] (int i) const {
            return data[i];
        }
    };
}