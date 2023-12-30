#include "bmp.h"

#include <bitset>
#include <exception>
#include <fstream>

#include "ImageConst.h"
#include "Matrix.h"

NAME_SPACE_START(myUtil)

constexpr int32_t BMP_HEADER_LEN = 54;

// 获取Two-dimensional Gaussian distribution
double** getTwoDimGaussianDistrbute(int32_t GaussianRadius, int32_t GaussianTemplateRadius)
{
    int32_t n = 2 * GaussianTemplateRadius + 1;
    auto** res = new double*[n];
    for (int32_t i = 0; i < n; i++)
        res[i] = new double[n];
    double t = 1.0 / (2 * M_PI * GaussianRadius * GaussianRadius);
    for (int32_t i = -1 * GaussianTemplateRadius; i <= GaussianTemplateRadius; i++) {
        for (int32_t j = -1 * GaussianTemplateRadius; j <= GaussianTemplateRadius; j++) {
            res[i + GaussianTemplateRadius][j + GaussianTemplateRadius] =
                t * exp((-1) * (i * i + j * j) / (2 * GaussianRadius * GaussianRadius));
        }
    }
    return res;
}

ImageStatus BMPData::read(const char* filePath)
{
    CHECK_NULL_RETURN(filePath, ERROR_NULLPTR)
    fstream file(filePath, ios::in | ios::binary);
    if (file.fail()) return ERROR_FILE_OPERATOR;
    file.seekg(2, ios::beg);
    try {
        // header
        if (!HeadProcess(file)) return ERROR_BMP_HEAD;
        // palette
        if (!PaletteProcess(file)) return ERROR_BMP_PALETTE;
        // image data
        if (!DataProcess(file)) return ERROR_FILE_DECODE;
    } catch (...) {
        file.close();
        return ERROR_UNKNOWN;
    }
    file.close();
    return SUCCESS;
}

ImageStatus BMPData::write(const char* filePath)
{
    CHECK_NULL_RETURN(filePath, ERROR_NULLPTR)
    try {
        if (gray) {
            // fill the data area
            for (int32_t i = 0; i < height; i++) {
                // compute the offset of destination bitmap and source image
                int32_t idx = height - 1 - i, j = 0;
                int32_t offsetDst = idx * rowSize + BMP_HEADER_LEN + (gray ? 4 * 256 : 0);
                // fill data
                for (j = 0; j < width; j++) {
                    bitmap[offsetDst + j] = grayBuf->getValue(i, j);
                }
                // fill 0x0, this part can be ignored
                for (; j < rowSize; j++) {
                    bitmap[offsetDst + j] = 0x0;
                }
            }
        } else {
            int32_t i = 0;
            // fill the data area
            for (i = 0; i < height; i++) {
                // compute the offset of destination bitmap and source image
                int32_t idx = height - 1 - i, j = 0;
                int32_t offsetDst = idx * rowSize + BMP_HEADER_LEN + (gray ? 4 * 256 : 0);
                // fill data
                for (j = 0; j < width * 3; j++) {
                    RGB temp = buf.getValue(i, j / 3);
                    if (j % 3 == 0 && (flag == 0 || flag == 1))
                        bitmap[offsetDst + j] = temp.blue;
                    else if (j % 3 == 1 && (flag == 0 || flag == 2))
                        bitmap[offsetDst + j] = temp.green;
                    else if (j % 3 == 2 && (flag == 0 || flag == 3))
                        bitmap[offsetDst + j] = temp.red;
                }
                // fill 0x0, this part can be ignored
                for (; j < rowSize; j++) {
                    bitmap[offsetDst + j] = 0x0;
                }
            }
        }
        FILE* fp = fopen(filePath, "wb+");
        fwrite(bitmap, 1, dataSize, fp);
        fclose(fp);
    } catch (...) {
        return ERROR_WRITE;
    }
    return SUCCESS;
}

void BMPData::GrayEncoder(double (*convert)(double), double (*GrayAlgorithm)(RGB))
{
    // fill the data area
    for (int32_t i = 0; i < height; i++) {
        // compute the offset of destination bitmap and source image
        int32_t j = 0;
        // fill data
        for (j = 0; j < width; j++) {
            grayBuf->setValue(i, j, (uint8_t)convert(GrayAlgorithm(buf.getValue(i, j))));
        }
        // fill 0x0, this part can be ignored
        for (; j < rowSize; j++) {
            grayBuf->setValue(i, j, 0x0);
        }
    }
}

void BMPData::GaussianHandle(bool isRGB, double (*convert)(double), int32_t flag)
{
    // 高斯变换矩阵
    double** gaussian = getTwoDimGaussianDistrbute(GAUSSIAN_RADIUS, GAUSSIAN_TEMPLATE_RADIUS);
    if (!isRGB) {
        auto* temp = new Matrix<uint8_t>(height, width);
        for (int32_t i = 0; i < height; i++) {
            for (int32_t j = 0; j < rowSize; j++) {
                double sum = 0;
                for (int32_t x = i - GAUSSIAN_TEMPLATE_RADIUS; x <= i + GAUSSIAN_TEMPLATE_RADIUS; x++) {
                    for (int32_t y = j - GAUSSIAN_TEMPLATE_RADIUS; y <= j + GAUSSIAN_TEMPLATE_RADIUS; y++) {
                        if (x < 0 || y < 0 || y >= width || x >= height)
                            continue;
                        int32_t row = x + GAUSSIAN_TEMPLATE_RADIUS - i, col = y + GAUSSIAN_TEMPLATE_RADIUS - j;
                        sum += (grayBuf->getValue(x, y) * gaussian[row][col]);
                    }
                }
                temp->setValue(i, j, (uint8_t)convert(sum));
            }
        }
        delete grayBuf;
        grayBuf = temp;
    } else {
        auto* temp = new Matrix<RGB>(height, width);
        for (int32_t i = 0; i < height; i++) {
            for (int32_t j = 0; j < width; j++) {
                double sum[3] = { 0 };
                for (int32_t x = i - GAUSSIAN_TEMPLATE_RADIUS; x <= i + GAUSSIAN_TEMPLATE_RADIUS; x++) {
                    for (int32_t y = j - GAUSSIAN_TEMPLATE_RADIUS; y <= j + GAUSSIAN_TEMPLATE_RADIUS; y++) {
                        if (x < 0 || y < 0 || y >= width || x >= height)
                            continue;
                        int32_t row = x + GAUSSIAN_TEMPLATE_RADIUS - i, col = y + GAUSSIAN_TEMPLATE_RADIUS - j;
                        sum[0] += (buf.getValue(x, y).blue * gaussian[row][col]);
                        sum[1] += (buf.getValue(x, y).green * gaussian[row][col]);
                        sum[2] += (buf.getValue(x, y).red * gaussian[row][col]);
                    }
                }
                RGB t;
                if (flag == 0 || flag == 1)
                    t.blue = static_cast<uint8_t>(convert(sum[0]));
                if (flag == 0 || flag == 2)
                    t.green = static_cast<uint8_t>(convert(sum[1]));
                if (flag == 0 || flag == 3)
                    t.red = static_cast<uint8_t>(convert(sum[2]));
                temp->setValue(i, j, t);
            }
        }
        buf.~Matrix();
        buf = *temp;
    }
    FREE_LP_2(gaussian, 2 * GAUSSIAN_TEMPLATE_RADIUS + 1)
}

void BMPData::EdgeDetect(double matrix1[3][3], double matrix2[3][3], int32_t row, uint8_t (*algorithm)(double, double))
{
    auto* temp = new Matrix<uint8_t>(height, width);
    for (int32_t i = 1; i < height - 1; i++) {
        for (int32_t j = 1; j < width - 1; j++) {
            double g_x = 0, g_y = 0;
            for (int32_t x = 0; x < row; x++) {
                for (int32_t y = 0; y < row; y++) {
                    g_x += matrix1[x][y] * grayBuf->getValue(i + x - 1, j + y - 1);
                    g_y += matrix2[x][y] * grayBuf->getValue(i + x - 1, j + y - 1);
                }
            }
            // g_x+=grayBuf->getValue(i-1,j-1)+grayBuf->getValue(i-1,j)+grayBuf->getValue(i-1,j+1)
            //     -grayBuf->getValue(i+1,j-1)-grayBuf->getValue(i+1,j)-grayBuf->getValue(i+1,j+1);
            // g_y+=grayBuf->getValue(i-1,j+1)+grayBuf->getValue(i,j+1)+grayBuf->getValue(i+1,j+1)
            //     -grayBuf->getValue(i-1,j-1)-grayBuf->getValue(i,j-1)-grayBuf->getValue(i+1,j-1);
            // 如果加入此行，因为之前已经灰度化了，所以会出现灰色
            // g_x/=3,g_y/=3;
            temp->setValue(i, j, algorithm(g_x, g_y));
        }
    }
    delete grayBuf;
    grayBuf = temp;
}

Matrix<RGB> BMPData::getRGBMatrix() const
{
    if (gray) return {};
    return buf;
}

void BMPData::Init()
{
    width += (4 - width % 4) % 4;
    rowSize = static_cast<int32_t>(ceil((gray ? 8 : 24) * width / 32) * 4);
    // rowSize = max(rowSize, 3 * width+(4-width%4)%4);
    dataSize = rowSize * height + BMP_HEADER_LEN + (gray ? 4 * 256 : 0);
    bitmap = new uint8_t[dataSize];
    offBits = BMP_HEADER_LEN + (gray ? 4 * 256 : 0);
    SetBitmapInfo(dataSize, height, width);
    if (gray) {
        BmpHeader[28] = 0x08;
        // fill the header area
        for (int32_t i = 0; i < BMP_HEADER_LEN; i++)
            bitmap[i] = BmpHeader[i];
        // init Palette
        for (int32_t i = 0; i < 256; i++) {
            bitmap[i * 4 + 0 + BMP_HEADER_LEN] = i;
            bitmap[i * 4 + 1 + BMP_HEADER_LEN] = i;
            bitmap[i * 4 + 2 + BMP_HEADER_LEN] = i;
            bitmap[i * 4 + 3 + BMP_HEADER_LEN] = 0;
        }
    }
    for (int32_t i = 0; i < BMP_HEADER_LEN; i++) {
        bitmap[i] = BmpHeader[i];
    }
}

void BMPData::SetBitmapInfo(int32_t _dataSize, int32_t _height, int32_t _width)
{
    for (int32_t i = 0; i < 4; i++) {
        // size of image ( header + data )
        BmpHeader[2 + i] = _dataSize & 0xff;
        _dataSize >>= 8;

        // _width of image
        BmpHeader[18 + i] = _width & 0xff;
        _width >>= 8;

        // _height of image
        BmpHeader[22 + i] = _height & 0xff;
        _height >>= 8;

        // offset of image data
        BmpHeader[10 + i] = offBits & 0xff;
        offBits >>= 8;
    }
}

bool BMPData::HeadProcess(fstream& file)
{
    try{
        file.read((char*)&bmpHeader, sizeof(bmpHeader));
        height = bmpHeader.biHeight;
        width = bmpHeader.biWidth;
        width += (4 - width % 4) % 4;
        rowSize = static_cast<int32_t>(ceil((gray ? 8 : 24) * width / 32) * 4);
        dataSize = rowSize * height + BMP_HEADER_LEN + (gray ? 4 * 256 : 0);
        bitmap = new uint8_t[dataSize];
        return true;
    }catch (...){
        return false;
    }
}

bool BMPData::PaletteProcess(fstream& file)
{
    try{
        file.seekg(BMP_HEADER_LEN, ios::beg);
        char* p = new char[bmpHeader.bfOffBits - BMP_HEADER_LEN];
        file.read(p, bmpHeader.bfOffBits - BMP_HEADER_LEN);
        for (int32_t i = 0; i < (bmpHeader.bfOffBits - BMP_HEADER_LEN) / 4; i++) {
            Palette pal {};
            pal.rgbRed = p[i * 4 + 0];
            pal.rgbGreen = p[i * 4 + 1];
            pal.rgbBlue = p[i * 4 + 2];
            pal.rgbAlpha = p[i * 4 + 3];
            palettes.push_back(pal);
        }
        return true;
    }catch(...) {
        return false;
    }
}

bool BMPData::DataProcess(fstream& file)
{
    try{
        buf = Matrix<RGB>(bmpHeader.biHeight, bmpHeader.biWidth);
        file.seekg(bmpHeader.bfOffBits, ios::beg);
        int32_t readSize = bmpHeader.biBitCount / 8;
        int32_t row = bmpHeader.biHeight - 1, col = 0;
        while (!file.eof() && row >= 0) {
            uint32_t t;
            file.read((char*)&t, readSize);
            int offset = readSize == 4 ? 8 : 0;
            RGB info;
            info.blue = (t >> (0 + offset)) & 0xff;
            info.green = (t >> (8 + offset)) & 0xff;
            info.red = (t >> (16 + offset)) & 0xff;
            buf.setValue(row, col, info);
            if (col >= bmpHeader.biWidth - 1) {
                row--;
                col = 0;
            } else {
                col++;
            }
        }
    }catch(...){
        return false;
    }
    return true;
}

void BMPData::analysisHead(uint8_t* head)
{
    for (int32_t i = 3; i >= 0; i--) {
        // size of image ( all data )
        dataSize <<= 8;
        dataSize += head[2 + i];
        // offset of image data
        offBits <<= 8;
        offBits += head[0xa + i];
        // width of image
        width <<= 8;
        width += head[18 + i];
        // height of image
        height <<= 8;
        height += head[22 + i];
    }
}

NAME_SPACE_END()
