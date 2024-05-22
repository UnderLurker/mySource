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
 * �Ŵ���С��ֵ�㷨
 */
enum Interpolation {
    Nearest,  // ����ڷ�
    Bilinear, // ˫���Բ�ֵ(default)
    // Bicubic   // ˫���β�ֵ
};

class PictureAlgorithm {
public:
    /**
     * ��ֱ��ת
     * @tparam T
     * @param matrix input matrix
     */
    template<typename T>
    static Matrix<T>* verticalImage(const Matrix<T>& matrix);
    /**
     * ˮƽ��ת
     * @tparam T
     * @param matrix input matrix
     */
    template<typename T>
    static Matrix<T>* horizontalImage(const Matrix<T>& matrix);
    /**
     * ��ת
     * @tparam T
     * @param matrix input matrix
     * @param angle ֻ��90 180 270
     * @return
     * result
     */
    template<typename T>
    static Matrix<T>* rotate(const Matrix<T>& matrix, RotateAngle angle);

    /**
     * ����ͼ��ľ��󣬱�������0
     * @tparam T
     * @param rgb
     * @param width
     * @param height
     * @param scale ����0
     * @param interpolation ��ֵ�㷨
     * @return
     */
    template<typename T>
    static Matrix<T>* ShrinkMatrix(const T** matrix,
                                   uint32_t width,
                                   uint32_t height,
                                   double scale                = 1.0f,
                                   Interpolation interpolation = Bilinear);

    /**
     * ����ͼ��ľ��󣬱�������0
     * @tparam T
     * @param rgb
     * @param scale ����0
     * @param interpolation ��ֵ�㷨
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
