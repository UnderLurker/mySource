//
// Created by XW on 2024/5/17.
//

#ifndef _PICTURE_ALGORITHM_H
#define _PICTURE_ALGORITHM_H

#include "Matrix.h"
#include "Util.h"

namespace myUtil {

enum RotateAngle {
    Angle_90,
    Angle_180,
    Angle_270
};

/**
 * 放大缩小插值算法
 */
enum Interpolation {
    Nearest,  // 最近邻法
    Bilinear, // 双线性插值(default)
    // Bicubic   // 双三次插值
};

class PictureAlgorithm {
public:
    /**
     * 垂直翻转
     * @tparam T
     * @param matrix input matrix
     */
    template<typename T>
    static Matrix<T>* verticalImage(const Matrix<T>& matrix);
    /**
     * 水平翻转
     * @tparam T
     * @param matrix input matrix
     */
    template<typename T>
    static Matrix<T>* horizontalImage(const Matrix<T>& matrix);
    /**
     * 旋转
     * @tparam T
     * @param matrix input matrix
     * @param angle 只有90 180 270
     * @return
     * result
     */
    template<typename T>
    static Matrix<T>* rotate(const Matrix<T>& matrix, RotateAngle angle);

    /**
     * 放缩图像的矩阵，比例大于0
     * @tparam T
     * @param rgb
     * @param width
     * @param height
     * @param scale 大于0
     * @param interpolation 插值算法
     * @return
     */
    template<typename T>
    static Matrix<T>* ShrinkMatrix(const T** matrix,
                                   uint32_t width,
                                   uint32_t height,
                                   double scale                = 1.0f,
                                   Interpolation interpolation = Bilinear);

    /**
     * 放缩图像的矩阵，比例大于0
     * @tparam T
     * @param rgb
     * @param scale 大于0
     * @param interpolation 插值算法
     * @return
     */
    template<typename T>
    static Matrix<T>* ShrinkMatrix(const Matrix<T>& matrix,
                                   double scale                = 1.0f,
                                   Interpolation interpolation = Bilinear);
};
} // namespace myUtil

#include "picture_algorithm.inl"

#endif // _PICTURE_ALGORITHM_H
