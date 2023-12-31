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
        if (_gray) {
            // fill the data area
            for (int32_t i = 0; i < _height; i++) {
                // compute the offset of destination _bitmap and source image
                int32_t idx = _height - 1 - i, j = 0;
                int32_t offsetDst = idx * _rowSize + BMP_HEADER_LEN + (_gray ? 4 * 256 : 0);
                // fill data
                for (j = 0; j < _width; j++) {
                    _bitmap[offsetDst + j] = _grayBuf->getValue(i, j);
                }
                // fill 0x0, this part can be ignored
                for (; j < _rowSize; j++) {
                    _bitmap[offsetDst + j] = 0x0;
                }
            }
        } else {
            int32_t i = 0;
            // fill the data area
            for (i = 0; i < _height; i++) {
                // compute the offset of destination _bitmap and source image
                int32_t idx = _height - 1 - i, j = 0;
                int32_t offsetDst = idx * _rowSize + BMP_HEADER_LEN + (_gray ? 4 * 256 : 0);
                // fill data
                for (j = 0; j < _width * 3; j++) {
                    RGB temp = _buf.getValue(i, j / 3);
                    if (j % 3 == 0 && (_flag == 0 || _flag == 1))
                        _bitmap[offsetDst + j] = temp.blue;
                    else if (j % 3 == 1 && (_flag == 0 || _flag == 2))
                        _bitmap[offsetDst + j] = temp.green;
                    else if (j % 3 == 2 && (_flag == 0 || _flag == 3))
                        _bitmap[offsetDst + j] = temp.red;
                }
                // fill 0x0, this part can be ignored
                for (; j < _rowSize; j++) {
                    _bitmap[offsetDst + j] = 0x0;
                }
            }
        }
        FILE* fp = fopen(filePath, "wb+");
        fwrite(_bitmap, 1, _dataSize, fp);
        fclose(fp);
    } catch (...) {
        return ERROR_WRITE;
    }
    return SUCCESS;
}

void BMPData::GrayEncoder(double (*convert)(double), double (*GrayAlgorithm)(RGB))
{
    // fill the data area
    for (int32_t i = 0; i < _height; i++) {
        // compute the offset of destination _bitmap and source image
        int32_t j = 0;
        // fill data
        for (j = 0; j < _width; j++) {
            _grayBuf->setValue(i, j, (uint8_t)convert(GrayAlgorithm(_buf.getValue(i, j))));
        }
        // fill 0x0, this part can be ignored
        for (; j < _rowSize; j++) {
            _grayBuf->setValue(i, j, 0x0);
        }
    }
}

void BMPData::GaussianHandle(bool isRGB, double (*convert)(double), int32_t flag)
{
    // 高斯变换矩阵
    double** gaussian = getTwoDimGaussianDistrbute(GAUSSIAN_RADIUS, GAUSSIAN_TEMPLATE_RADIUS);
    if (!isRGB) {
        auto* temp = new Matrix<uint8_t>(_height, _width);
        for (int32_t i = 0; i < _height; i++) {
            for (int32_t j = 0; j < _rowSize; j++) {
                double sum = 0;
                for (int32_t x = i - GAUSSIAN_TEMPLATE_RADIUS; x <= i + GAUSSIAN_TEMPLATE_RADIUS; x++) {
                    for (int32_t y = j - GAUSSIAN_TEMPLATE_RADIUS; y <= j + GAUSSIAN_TEMPLATE_RADIUS; y++) {
                        if (x < 0 || y < 0 || y >= _width || x >= _height)
                            continue;
                        int32_t row = x + GAUSSIAN_TEMPLATE_RADIUS - i, col = y + GAUSSIAN_TEMPLATE_RADIUS - j;
                        sum += (_grayBuf->getValue(x, y) * gaussian[row][col]);
                    }
                }
                temp->setValue(i, j, (uint8_t)convert(sum));
            }
        }
        delete _grayBuf;
        _grayBuf = temp;
    } else {
        auto* temp = new Matrix<RGB>(_height, _width);
        for (int32_t i = 0; i < _height; i++) {
            for (int32_t j = 0; j < _width; j++) {
                double sum[3] = { 0 };
                for (int32_t x = i - GAUSSIAN_TEMPLATE_RADIUS; x <= i + GAUSSIAN_TEMPLATE_RADIUS; x++) {
                    for (int32_t y = j - GAUSSIAN_TEMPLATE_RADIUS; y <= j + GAUSSIAN_TEMPLATE_RADIUS; y++) {
                        if (x < 0 || y < 0 || y >= _width || x >= _height)
                            continue;
                        int32_t row = x + GAUSSIAN_TEMPLATE_RADIUS - i, col = y + GAUSSIAN_TEMPLATE_RADIUS - j;
                        sum[0] += (_buf.getValue(x, y).blue * gaussian[row][col]);
                        sum[1] += (_buf.getValue(x, y).green * gaussian[row][col]);
                        sum[2] += (_buf.getValue(x, y).red * gaussian[row][col]);
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
        _buf.~Matrix();
        _buf = *temp;
    }
    FREE_LP_2(gaussian, 2 * GAUSSIAN_TEMPLATE_RADIUS + 1)
}

void BMPData::EdgeDetect(double matrix1[3][3], double matrix2[3][3], int32_t row, uint8_t (*algorithm)(double, double))
{
    auto* temp = new Matrix<uint8_t>(_height, _width);
    for (int32_t i = 1; i < _height - 1; i++) {
        for (int32_t j = 1; j < _width - 1; j++) {
            double g_x = 0, g_y = 0;
            for (int32_t x = 0; x < row; x++) {
                for (int32_t y = 0; y < row; y++) {
                    g_x += matrix1[x][y] * _grayBuf->getValue(i + x - 1, j + y - 1);
                    g_y += matrix2[x][y] * _grayBuf->getValue(i + x - 1, j + y - 1);
                }
            }
            // g_x+=_grayBuf->getValue(i-1,j-1)+_grayBuf->getValue(i-1,j)+_grayBuf->getValue(i-1,j+1)
            //     -_grayBuf->getValue(i+1,j-1)-_grayBuf->getValue(i+1,j)-_grayBuf->getValue(i+1,j+1);
            // g_y+=_grayBuf->getValue(i-1,j+1)+_grayBuf->getValue(i,j+1)+_grayBuf->getValue(i+1,j+1)
            //     -_grayBuf->getValue(i-1,j-1)-_grayBuf->getValue(i,j-1)-_grayBuf->getValue(i+1,j-1);
            // 如果加入此行，因为之前已经灰度化了，所以会出现灰色
            // g_x/=3,g_y/=3;
            temp->setValue(i, j, algorithm(g_x, g_y));
        }
    }
    delete _grayBuf;
    _grayBuf = temp;
}

Matrix<RGB> BMPData::getRGBMatrix() const
{
    if (_gray) return {};
    return _buf;
}

void BMPData::Init()
{
    _width += (4 - _width % 4) % 4;
    _rowSize = static_cast<int32_t>(ceil((_gray ? 8 : 24) * _width / 32) * 4);
    // _rowSize = max(_rowSize, 3 * _width+(4-_width%4)%4);
    _dataSize = _rowSize * _height + BMP_HEADER_LEN + (_gray ? 4 * 256 : 0);
    _bitmap = new uint8_t[_dataSize];
    _offBits = BMP_HEADER_LEN + (_gray ? 4 * 256 : 0);
    SetBitmapInfo(_dataSize, _height, _width);
    if (_gray) {
        BmpHeader[28] = 0x08;
        // fill the header area
        for (int32_t i = 0; i < BMP_HEADER_LEN; i++)
            _bitmap[i] = BmpHeader[i];
        // init Palette
        for (int32_t i = 0; i < 256; i++) {
            _bitmap[i * 4 + 0 + BMP_HEADER_LEN] = i;
            _bitmap[i * 4 + 1 + BMP_HEADER_LEN] = i;
            _bitmap[i * 4 + 2 + BMP_HEADER_LEN] = i;
            _bitmap[i * 4 + 3 + BMP_HEADER_LEN] = 0;
        }
    }
    for (int32_t i = 0; i < BMP_HEADER_LEN; i++) {
        _bitmap[i] = BmpHeader[i];
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
        BmpHeader[10 + i] = _offBits & 0xff;
        _offBits >>= 8;
    }
}

bool BMPData::HeadProcess(fstream& file)
{
    try{
        file.read((char*)&_bmpHeader, sizeof(_bmpHeader));
        _height = _bmpHeader.biHeight;
        _width = _bmpHeader.biWidth;
        _width += (4 - _width % 4) % 4;
        _rowSize = static_cast<int32_t>(ceil((_gray ? 8 : 24) * _width / 32) * 4);
        _dataSize = _rowSize * _height + BMP_HEADER_LEN + (_gray ? 4 * 256 : 0);
        _bitmap = new uint8_t[_dataSize];
        return true;
    }catch (...){
        return false;
    }
}

bool BMPData::PaletteProcess(fstream& file)
{
    try{
        file.seekg(BMP_HEADER_LEN, ios::beg);
        char* p = new char[_bmpHeader.bfOffBits - BMP_HEADER_LEN];
        file.read(p, _bmpHeader.bfOffBits - BMP_HEADER_LEN);
        for (int32_t i = 0; i < (_bmpHeader.bfOffBits - BMP_HEADER_LEN) / 4; i++) {
            Palette pal {};
            pal.rgbRed = p[i * 4 + 0];
            pal.rgbGreen = p[i * 4 + 1];
            pal.rgbBlue = p[i * 4 + 2];
            pal.rgbAlpha = p[i * 4 + 3];
            _palettes.push_back(pal);
        }
        return true;
    }catch(...) {
        return false;
    }
}

bool BMPData::DataProcess(fstream& file)
{
    try{
        _buf = Matrix<RGB>(_bmpHeader.biHeight, _bmpHeader.biWidth);
        file.seekg(_bmpHeader.bfOffBits, ios::beg);
        int32_t readSize = _bmpHeader.biBitCount / 8;
        int32_t row = _bmpHeader.biHeight - 1, col = 0;
        while (!file.eof() && row >= 0) {
            uint32_t t;
            file.read((char*)&t, readSize);
            int offset = readSize == 4 ? 8 : 0;
            RGB info;
            info.blue = (t >> (0 + offset)) & 0xff;
            info.green = (t >> (8 + offset)) & 0xff;
            info.red = (t >> (16 + offset)) & 0xff;
            _buf.setValue(row, col, info);
            if (col >= _bmpHeader.biWidth - 1) {
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
        _dataSize <<= 8;
        _dataSize += head[2 + i];
        // offset of image data
        _offBits <<= 8;
        _offBits += head[0xa + i];
        // _width of image
        _width <<= 8;
        _width += head[18 + i];
        // _height of image
        _height <<= 8;
        _height += head[22 + i];
    }
}

NAME_SPACE_END()
