#pragma once
#include <boost/serialization/vector.hpp>
#include <stdexcept>
namespace gpcs
{
    struct mat
    {
        int rows;
        int cols;
        std::vector<double> data;
        template<class Archive>
        void serialize(Archive& ar, const unsigned int version) {
            ar& rows;
            ar& cols;
            ar& data;
        }
        double* operator[] (int i) {
            // check if the index is valid
            if (i < 0 || i >= rows)
            {
                throw std::out_of_range("Invalid row index");
            }
            // return the pointer to the row
            return data.data() + i * cols;
        }
        const double* operator[] (int i) const {
            // check if the index is valid
            if (i < 0 || i >= rows) {
                throw std::out_of_range("Invalid row index");
            }
            // return the pointer to the row
            return data.data() + i * cols;
        }
        void resize(int nrows, int ncols)
        {
            rows = nrows;
            cols = ncols;
            int totalsize = rows * cols;
            data.reserve(totalsize);
            for (int i = 1; i <= totalsize; i++)
            {
                double v = 0;
                data.push_back(v);
            }
        }
    };
}