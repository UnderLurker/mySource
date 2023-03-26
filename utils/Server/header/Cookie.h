#pragma once
#ifndef _COOKIE_
#define _COOKIE_
#include <string>
using namespace std;
class Cookie
{
public:
	Cookie() {}
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
	//ע��
	string _comment;
	//·������Ҫ���ʵ�url startwith��path����cookieЯ��
	string _path;
	//��վ����
	string _domain;
	string _version;
	//����ʱ��
	int _maxAge{ 0 };
};

#endif //!_COOKIE_