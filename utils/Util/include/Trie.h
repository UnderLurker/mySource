//
// Created by 常笑男 on 2024/9/7.
//

#ifndef _TRIE_H
#define _TRIE_H

#include <map>
#include <string>

namespace myUtil {

class Trie {
public:
    Trie() = default;
    virtual ~Trie() {
        for (auto& [key, value] : _table) {
            if (value) delete value;
        }
    }
    void insert(const std::string& word) {
        if (word.empty()) return;
        insert(word.c_str());
    }

    bool search(const std::string& word) {
        if (word.empty()) return true;
        bool flag = false;
        return search(word.c_str(), word.size(), flag);
    }

    bool startsWith(const std::string& prefix) {
        if (prefix.empty()) return true;
        bool flag = false;
        return startsWith(prefix.c_str(), prefix.size(), flag);
    }

private:
    void insert(const char* word) {
        if (!word || word[0] == '\0') {
            _end = true;
            return;
        } else if (_table.find(word[0]) == _table.end()) {
            _table[word[0]] = new Trie();
        }
        _table[word[0]]->insert(word + 1);
    }

    bool search(const char* word, size_t length, bool& flag) {
        if (!word || length <= 0) return true;
        else if (_table.find(word[0]) == _table.end()) return false;
        flag = _table[word[0]]->_end;
        return _table[word[0]]->search(word + 1, length - 1, flag) && flag;
    }

    bool startsWith(const char* word, size_t length, bool& flag) {
        if (!word || length <= 0) return true;
        else if (_table.find(word[0]) == _table.end()) return false;
        flag = _table[word[0]]->_end;
        return _table[word[0]]->startsWith(word + 1, length - 1, flag) && (flag || !_table[word[0]]->_table.empty());
    }

private:
    std::map<char, Trie*> _table;
    bool _end {false};
};

} // namespace myUtil
#endif // _TRIE_H
