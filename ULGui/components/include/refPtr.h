//
// Created by XW on 2024/6/9.
//

#ifndef _REF_PTR_H
#define _REF_PTR_H

namespace ULGui::components {

template<typename T>
class RefPtr {
public:
    RefPtr() = default;
    RefPtr(const RefPtr<T>& other)
        : _rawPtr(other._rawPtr) {}
    explicit RefPtr(T* ptr)
        : _rawPtr(ptr) {}

    T& operator*() { return *_rawPtr; }

private:
    T* _rawPtr {nullptr};
};

} // namespace ULGui::components

#endif // _REF_PTR_H
