#ifndef _COOKIE_H
#define _COOKIE_H
#include <string>
#include "Util.h"

NAME_SPACE_START(myUtil)
using namespace std;
class Cookie
{
public:
	Cookie() = default;
	Cookie(const string& name, const string& value) :
		_name(name), _value(value) {
		_comment = "";
		_path = "";
		_domain = "";
		_version = "";
		_maxAge = 0;
	}
	string getNameValue() const;
	void setNameValue(const string& name, const string& value);
	void setComment(const string& comment);
	void setPath(const string& path);
	void setDomain(const string& domain);
	void setVersion(const string& version);
	void setMaxAge(const int& maxAge);
	string getComment() const;
	string getPath() const;
	string getDomain() const;
	string getVersion() const;
	int getMaxAge() const;
	string toString() const;
private:
	string _name;
	string _value;
	//注释
	string _comment;
	//路径，若要访问的url startwith（path）此cookie携带
	string _path;
	//网站域名
	string _domain;
	string _version;
	//生存时间
	int _maxAge{ 0 };
};
NAME_SPACE_END()
#endif //!_COOKIE_H