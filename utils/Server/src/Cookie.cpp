#include "Cookie.h"

string Cookie::getNameValue() const
{
	return _name + "=" + _value;
}

void Cookie::setNameValue(const string& name, const string& value)
{
	_name = name;
	_value = value;
}

void Cookie::setComment(const string& comment)
{
	_comment = comment;
}

void Cookie::setPath(const string& path)
{
	_path = path;
}

void Cookie::setDomain(const string& domain)
{
	_domain = domain;
}

void Cookie::setVersion(const string& version)
{
	_version = version;
}

void Cookie::setMaxAge(const int& maxAge)
{
	_maxAge = maxAge;
}

string Cookie::getComment() const
{
	return _comment;
}

string Cookie::getPath() const
{
	return _path;
}

string Cookie::getDomain() const
{
	return _domain;
}

string Cookie::getVersion() const
{
	return _version;
}

int Cookie::getMaxAge() const
{
	return _maxAge;
}

string Cookie::toString() const
{
	string res = getNameValue();
	if (_comment != "") res += ";comment=" + _comment;
	if (_path != "") res += ";Path=" + _path;
	if (_domain != "") res += ";Domain=" + _domain;
	if (_version != "") res += ";Version=" + _version;
	res += ";Max-Age=" + _maxAge;
	return res;
}