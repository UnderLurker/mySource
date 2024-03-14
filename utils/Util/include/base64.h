#ifndef __BASE64__
#define __BASE64__

#include "Util_const.h"
#include "Util.h"

NAME_SPACE_START(myUtil)

class Base64 {
public:
  static string encoding(const string& str)
  {
      string res = "";
      for (int i = 0; i < str.size(); i += 3) {
          string t = "";
          for (int j = 0; j < 3 && i + j < str.size(); j++) {
              t.append(bitset<8>(str[i + j]).to_string());
          }
          int pos = 0, count = 0;
          while (t.size() - pos >= 6) {
              int index = 0;
              for (int j = pos; j < pos + 6; j++) {
                  index = index * 2 + t[j] - '0';
              }
              res.append(string(1, BaseTable[index]));
              pos += 6;
              count++;
          }
          int zeroCount = 6 - t.size() + pos, index = 0;
          if (zeroCount < 6) {
              for (int j = pos; j < t.size(); j++) {
                  index = index * 2 + t[j] - '0';
              }
              index <<= zeroCount;
              res.append(string(1, BaseTable[index]));
              count++;
          }
          if (count != 4) {
              res.append(string(4 - count, '='));
          }
      }
      return res;
  }
  static string decoding(const string& str)
  {
      string res = "";
      for (int i = 0; i < str.size(); i += 4) {
          uint32_t temp = 0;
          int len = 0, zeroCount = 0;
          for (int j = 0; j < 4; j++) {
              if (str[i + j] == '=') {
                  zeroCount++;
                  continue;
              }
              temp <<= 6;
              temp |= AntiBaseTable[str[i + j]];
              len += 6;
          }
          if (zeroCount == 1) {
              temp >>= 2;
              len -= 2;
          } else if (zeroCount == 2) {
              temp >>= 4;
              len -= 4;
          }
          string str = "";
          int y = len % 8, x = y;
          while (len > 0) {
              str = string(1, (char)(temp & 0x000000ff)) + str;
              temp >>= 8;
              len -= 8;
          }
          res.append(str);
      }
      return res;
  }
};

NAME_SPACE_END()

#endif // !__BASE64__