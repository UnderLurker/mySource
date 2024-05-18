//
// Created by XW on 2024/5/17.
//
#include <cmath>

#include "ImageUtil.h"
#include "picture_algorithm.h"

namespace myUtil {
namespace {
template<typename T>
void rotate90(const Matrix<T>& input, Matrix<T>* output) {
    int32_t x = input.col - 1, y = 0;
    for (uint32_t i = 0; i < input.row; i++) {
        if (x < 0 || y >= input.row) return;
        for (uint32_t j = 0; j < input.col; j++) {
            output->setValue(y++, x, input.getValue(i, j));
        }
        x--;
        y = 0;
    }
}

template<typename T>
void rotate180(const Matrix<T>& input, Matrix<T>* output) {
    for (uint32_t i = 0; i < input.row / 2; i++) {
        for (uint32_t j = 0; j < input.col; j++) {
            uint32_t x = input.row - 1 - i, y = input.col - 1 - j;
            output->setValue(i, j, input.getValue(x, y));
            output->setValue(x, y, input.getValue(i, j));
        }
    }
    if (input.row & 1) {
        uint32_t row = input.row / 2;
        for (uint32_t i = 0; i < input.col; i++) {
            output->setValue(row, i, input.getValue(row, input.col - 1 - i));
        }
    }
}

template<typename T>
void rotate270(const Matrix<T>& input, Matrix<T>* output) {
    int32_t x = 0, y = input.row - 1;
    for (uint32_t i = 0; i < input.row; i++) {
        for (uint32_t j = 0; j < input.col; j++) {
            output->setValue(y--, x, input.getValue(i, j));
        }
        x++;
        y = input.row - 1;
    }
}

template<typename T>
T getValueByNearest(const T** matrix, uint32_t row, uint32_t col) {
    return matrix[row][col];
}

template<typename T>
T getValueByBilinear(const T** matrix, double row, double col) {
    bool rowEqual = (uint32_t)row == row;
    bool colEqual = (uint32_t)col == col;
    int32_t r[]   = {(int32_t)row, (int32_t)row + 1};
    int32_t c[]   = {(int32_t)col, (int32_t)col + 1};
    auto x = (int32_t)row, y = (int32_t)col;
    if (rowEqual && colEqual) return matrix[x][y];
    else if (rowEqual) return (matrix[x][c[1]] - matrix[x][c[0]]) * (y - c[0]) + matrix[x][c[0]];
    else if (colEqual) return (matrix[r[1]][y] - matrix[r[0]][y]) * (x - r[0]) + matrix[r[0]][y];
    else
        return matrix[r[1]][c[0]] * (c[1] - y) * (r[0] - x) + matrix[r[1]][c[1]] * (y - c[0]) * (r[0] - x) +
               matrix[r[0]][c[0]] * (c[1] - y) * (x - r[1]) + matrix[r[0]][c[1]] * (y - c[0]) * (x - r[1]);
}

template<>
RGB getValueByBilinear<RGB>(const RGB** matrix, double row, double col) {
    bool rowEqual = (uint32_t)row == row;
    bool colEqual = (uint32_t)col == col;
    int32_t r[]   = {(int32_t)row, (int32_t)row + 1};
    int32_t c[]   = {(int32_t)col, (int32_t)col + 1};
    auto x = (int32_t)row, y = (int32_t)col;
    if (rowEqual && colEqual) return matrix[x][y];
    else if (rowEqual) {
        CalcRGB result = ((CalcRGB)matrix[x][c[1]] - (CalcRGB)matrix[x][c[0]]) * (y - c[0]) + (CalcRGB)matrix[x][c[0]];
        return result.RGB();
    } else if (colEqual) {
        CalcRGB result = ((CalcRGB)matrix[r[1]][y] - (CalcRGB)matrix[r[0]][y]) * (x - r[0]) + (CalcRGB)matrix[r[0]][y];
        return result.RGB();
    } else {
        CalcRGB result = (CalcRGB)matrix[r[1]][c[0]] * (c[1] - y) * (r[0] - x) +
                         (CalcRGB)matrix[r[1]][c[1]] * (y - c[0]) * (r[0] - x) +
                         (CalcRGB)matrix[r[0]][c[0]] * (c[1] - y) * (x - r[1]) +
                         (CalcRGB)matrix[r[0]][c[1]] * (y - c[0]) * (x - r[1]);
        return result.RGB();
    }
}
} // namespace

template<typename T>
Matrix<T>* PictureAlgorithm::verticalImage(const Matrix<T>& matrix) {
    auto* result = new Matrix<T>(matrix);
    for (uint32_t i = 0; i < result->row / 2; i++) {
        for (uint32_t j = 0; j < result->col; j++) {
            T temp = result->getValue(i, j);
            result->setValue(i, j, result->getValue(result->row - 1 - i, j));
            result->setValue(result->row - 1 - i, j, temp);
        }
    }
    return result;
}

template<typename T>
Matrix<T>* PictureAlgorithm::horizontalImage(const Matrix<T>& matrix) {
    auto* result = new Matrix<T>(matrix);
    for (uint32_t i = 0; i < result->row; i++) {
        for (uint32_t j = 0; j < result->col / 2; j++) {
            T temp = result->getValue(i, j);
            result->setValue(i, j, result->getValue(i, result->col - 1 - j));
            result->setValue(i, result->col - 1 - j, temp);
        }
    }
    return result;
}

template<typename T>
Matrix<T>* PictureAlgorithm::rotate(const Matrix<T>& matrix, RotateAngle angle) {
    uint32_t row = angle % 2 ? matrix.row : matrix.col;
    uint32_t col = angle % 2 ? matrix.col : matrix.row;
    auto* result = new Matrix<T>(row, col);
    switch (angle) {
        case Angle_90:
            rotate90(matrix, result);
            break;
        case Angle_180:
            rotate180(matrix, result);
            break;
        case Angle_270:
            rotate270(matrix, result);
            break;
        default:
            break;
    }
    return result;
}

template<typename T>
Matrix<T>* PictureAlgorithm::ShrinkMatrix(const T** matrix,
                                          uint32_t width,
                                          uint32_t height,
                                          double scale,
                                          Interpolation interpolation) {
    if (scale <= 0) return nullptr;
    uint32_t resWidth = width * scale, resHeight = height * scale;
    auto* res = new Matrix<T>(resHeight, resWidth);
    for (uint32_t i = 0; i < resHeight; i++) {
        for (uint32_t j = 0; j < resWidth; j++) {
            double row = round(i * width * 1.0 / resWidth);
            double col = round(j * height * 1.0 / resHeight);
            if (row >= width) row = width - 1;
            if (col >= height) col = height - 1;
            if (interpolation == Nearest)
                res->setValue(i, j, getValueByNearest<T>(const_cast<const T**>(matrix), row, col));
            else if (interpolation == Bilinear)
                res->setValue(i, j, getValueByBilinear<T>(const_cast<const T**>(matrix), row, col));
        }
    }
    return res;
}

template<typename T>
Matrix<T>* PictureAlgorithm::ShrinkMatrix(const Matrix<T>& matrix, double scale, Interpolation interpolation) {
    if (scale <= 0) return nullptr;
    uint32_t resWidth = matrix.col * scale, resHeight = matrix.row * scale;
    auto* res = new Matrix<T>(resHeight, resWidth);
    for (uint32_t i = 0; i < resHeight; i++) {
        for (uint32_t j = 0; j < resWidth; j++) {
            double row = round(i * matrix.col * 1.0 / resWidth);
            double col = round(j * matrix.row * 1.0 / resHeight);
            if (row >= matrix.row) row = matrix.row - 1;
            if (col >= matrix.col) col = matrix.col - 1;
            if (interpolation == Nearest)
                res->setValue(i, j, getValueByNearest<T>(const_cast<const T**>(matrix.matrix), row, col));
            else if (interpolation == Bilinear)
                res->setValue(i, j, getValueByBilinear<T>(const_cast<const T**>(matrix.matrix), row, col));
        }
    }
    return res;
}

} // namespace myUtil
