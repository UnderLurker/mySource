#include "Image.h"

#include <bitset>
#include <exception>
#include <fstream>

#include "Matrix.h"
#include "ImageConst.h"

NAME_SPACE_START(myUtil)

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

void BMPData::readBMP(const string& filePath)
{
    fstream file(filePath.c_str(), ios::in | ios::binary);
    if (file.fail())
        return;
    file.seekg(2, ios::beg);
    try {
        // header
        file.read((char*)&bmpHeader, sizeof(bmpHeader));
        // palette
        file.seekg(54, ios::beg);
        char* p = new char[bmpHeader.bfOffBits - 54];
        file.read(p, bmpHeader.bfOffBits - 54);
        for (int i = 0; i < (bmpHeader.bfOffBits - 54) / 4; i++) {
            Palette pal;
            pal.rgbRed = p[i * 4 + 0];
            pal.rgbGreen = p[i * 4 + 1];
            pal.rgbBlue = p[i * 4 + 2];
            pal.rgbAlpha = p[i * 4 + 3];
            palettes.push_back(pal);
        }
        // image data
        rgb = new Matrix<RGB>(bmpHeader.biHeight, bmpHeader.biWidth);
        file.seekg(bmpHeader.bfOffBits, ios::beg);
        int readSize = bmpHeader.biBitCount / 8;
        while (!file.eof()) {
            uint32_t t;
            file.read((char*)&t, readSize);
            int offset = readSize == 4 ? 8 : 0;
            RGB info;
            info.red = (t >> (0 + offset)) & 0xff;
            info.green = (t >> (8 + offset)) & 0xff;
            info.blue = (t >> (16 + offset)) & 0xff;
        }
    } catch (...) {
        file.close();
        return;
    }
    file.close();
    return;
}

void BMPData::Init()
{
    width += (4 - width % 4) % 4;
    rowSize = ceil((gray ? 8 : 24) * width / 32) * 4;
    // rowSize = max(rowSize, 3 * width+(4-width%4)%4);
    dataSize = rowSize * height + 54 + (gray ? 4 * 256 : 0);
    bitmap = new uint8_t[dataSize];
    offbits = 54 + (gray ? 4 * 256 : 0);
    SetBitmapInfo(dataSize, height, width);
    if (gray) {
        BmpHeader[28] = 0x08;
        // fill the header area
        for (int i = 0; i < 54; i++)
            bitmap[i] = BmpHeader[i];
        // init Palette
        for (int i = 0; i < 256; i++) {
            bitmap[i * 4 + 0 + 54] = i;
            bitmap[i * 4 + 1 + 54] = i;
            bitmap[i * 4 + 2 + 54] = i;
            bitmap[i * 4 + 3 + 54] = 0;
        }
    }
    for (int i = 0; i < 54; i++) {
        bitmap[i] = BmpHeader[i];
    }
}

void BMPData::GrayEncoder(double (*convert)(double), double (*GrayAlgorithm)(RGB))
{
    // fill the data area
    for (int i = 0; i < height; i++) {
        // compute the offset of destination bitmap and source image
        int idx = height - 1 - i, j = 0;
        int offsetDst = idx * rowSize + 54 + (gray ? 4 * 256 : 0); // 54 means the header length
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

void BMPData::GaussianHandle(bool isRGB, double (*convert)(double), int flag)
{
    // 高斯变换矩阵
    double** gaussian = getTwoDimGaussianDistrbute(GAUSSIAN_RADIUS, GAUSSIAN_TEMPLATE_RADIUS);
    if (!isRGB) {
        Matrix<uint8_t>* temp = new Matrix<uint8_t>(height, width);
        for (int i = 0; i < height; i++) {
            int offsetDst = (height - 1 - i) * rowSize + 54 + (gray ? 4 * 256 : 0);
            for (int j = 0; j < rowSize; j++) {
                double sum = 0;
                for (int x = i - GAUSSIAN_TEMPLATE_RADIUS; x <= i + GAUSSIAN_TEMPLATE_RADIUS; x++) {
                    for (int y = j - GAUSSIAN_TEMPLATE_RADIUS; y <= j + GAUSSIAN_TEMPLATE_RADIUS; y++) {
                        if (x < 0 || y < 0 || y >= width || x >= height)
                            continue;
                        int row = x + GAUSSIAN_TEMPLATE_RADIUS - i, col = y + GAUSSIAN_TEMPLATE_RADIUS - j;
                        sum += (grayBuf->getValue(x, y) * gaussian[row][col]);
                    }
                }
                temp->setValue(i, j, (uint8_t)convert(sum));
            }
        }
        delete grayBuf;
        grayBuf = temp;
    } else {
        Matrix<RGB>* temp = new Matrix<RGB>(height, width);
        for (int i = 0; i < height; i++) {
            int offsetDst = (height - 1 - i) * rowSize + 54 + (gray ? 4 * 256 : 0);
            for (int j = 0; j < width; j++) {
                double sum[3] = { 0 };
                for (int x = i - GAUSSIAN_TEMPLATE_RADIUS; x <= i + GAUSSIAN_TEMPLATE_RADIUS; x++) {
                    for (int y = j - GAUSSIAN_TEMPLATE_RADIUS; y <= j + GAUSSIAN_TEMPLATE_RADIUS; y++) {
                        if (x < 0 || y < 0 || y >= width || x >= height)
                            continue;
                        int row = x + GAUSSIAN_TEMPLATE_RADIUS - i, col = y + GAUSSIAN_TEMPLATE_RADIUS - j;
                        sum[0] += (buf.getValue(x, y).blue * gaussian[row][col]);
                        sum[1] += (buf.getValue(x, y).green * gaussian[row][col]);
                        sum[2] += (buf.getValue(x, y).red * gaussian[row][col]);
                    }
                }
                RGB t;
                if (flag == 0 || flag == 1)
                    t.blue = convert(sum[0]);
                if (flag == 0 || flag == 2)
                    t.green = convert(sum[1]);
                if (flag == 0 || flag == 3)
                    t.red = convert(sum[2]);
                temp->setValue(i, j, t);
            }
        }
        buf.~Matrix();
        buf = *temp;
    }
    FREE_LP_2(gaussian, 2 * GAUSSIAN_TEMPLATE_RADIUS + 1)
}

void BMPData::EdgeDetect(double matrix1[3][3], double matrix2[3][3], int row, uint8_t (*algorithm)(double, double))
{
    Matrix<uint8_t>* temp = new Matrix<uint8_t>(height, width);
    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            double g_x = 0, g_y = 0;
            for (int x = 0; x < row; x++) {
                for (int y = 0; y < row; y++) {
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

void BMPData::saveBMP(const string& filename)
{
    if (gray) {
        // fill the data area
        for (int i = 0; i < height; i++) {
            // compute the offset of destination bitmap and source image
            int idx = height - 1 - i, j = 0;
            int offsetDst = idx * rowSize + 54 + (gray ? 4 * 256 : 0); // 54 means the header length
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
        int i = 0;
        // fill the data area
        for (i = 0; i < height; i++) {
            // compute the offset of destination bitmap and source image
            int idx = height - 1 - i, j = 0;
            int offsetDst = idx * rowSize + 54 + (gray ? 4 * 256 : 0); // 54 means the header length
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
    FILE* fp = fopen(filename.c_str(), "wb+");
    fwrite(bitmap, 1, dataSize, fp);
    fclose(fp);
}

// RGB BMPData::getRGB(int mcu_height,int mcu_width,int row,int col){
// 	int offsetHeight = (int)floor(row*1.0/mcu_height)*(int)ceil(width*1.0/mcu_width);
// 	int pos=col/mcu_width+offsetHeight;
// 	if(pos>=buf.size()) pos=buf.size()-1;
// 	return buf[pos][row%mcu_height][col%mcu_height];
// }

void BMPData::SetBitmapInfo(int dataSize, int height, int width)
{
    for (int i = 0; i < 4; i++) {
        // size of image ( header + data )
        BmpHeader[2 + i] = dataSize & 0xff;
        dataSize >>= 8;

        // width of image
        BmpHeader[18 + i] = width & 0xff;
        width >>= 8;

        // height of image
        BmpHeader[22 + i] = height & 0xff;
        height >>= 8;

        // offset of image data
        BmpHeader[10 + i] = offbits & 0xff;
        offbits >>= 8;
    }
}

void BMPData::analysisHead(uint8_t* head)
{
    for (int i = 3; i >= 0; i--) {
        // size of image ( all data )
        dataSize <<= 8;
        dataSize += head[2 + i];
        // offset of image data
        offbits <<= 8;
        offbits += head[0xa + i];
        // width of image
        width <<= 8;
        width += head[18 + i];
        // height of image
        height <<= 8;
        height += head[22 + i];
    }
}

NAME_SPACE_END()
