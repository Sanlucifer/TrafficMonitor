#pragma once
#include <stdexcept>
#include <wrl/client.h> //ComPtr
#include <d2d1.h>
#include <dwrite.h>

#define SAFE_RELEASE(p)                                                                                                \
    {                                                                                                                  \
        if (p)                                                                                                         \
        {                                                                                                              \
            (p)->Release();                                                                                            \
            (p) = (NULL);                                                                                              \
        }                                                                                                              \
    }

namespace FunctionChecker
{
    bool CheckLibraryExist(LPCTSTR p_library_name) noexcept;
    bool CheckFunctionExist(LPCTSTR p_library_name, LPCSTR p_function_name) noexcept;
}

class HResultException : public std::runtime_error
{
public:
    HResultException(HRESULT hr, const char* p_message);
    ~HResultException() override = default;

    auto GetError()
        -> Microsoft::WRL::ComPtr<IErrorInfo>;
    bool HasError() const noexcept;

private:
    Microsoft::WRL::ComPtr<IErrorInfo> m_p_error;
    HRESULT m_hr;
};

template <class T>
class DefaultStaticVariableWrapperDtor
{
public:
    void operator()(T*){};
};
template <class T, class DTOR = DefaultStaticVariableWrapperDtor<T>>
class StaticVariableWrapper : private DTOR
{
public:
    template <class CTOR>
    StaticVariableWrapper(CTOR ctor, DTOR dtor = {})
        : DTOR{dtor}
    {
        ctor(std::addressof(m_content));
    }
    ~StaticVariableWrapper()
    {
        DTOR::operator()(std::addressof(m_content));
    }
    T& Get()
    {
        return m_content;
    }

private:
    T m_content;
};
template <class T, class CTOR, class DTOR = DefaultStaticVariableWrapperDtor<T>>
auto MakeStaticVariableWrapper(CTOR ctor, DTOR dtor = {})
    -> StaticVariableWrapper<T, DTOR>
{
    return {ctor, dtor};
}

void ThrowIfFailed(HRESULT hr, const char* p_message = "Error occurred when call COM function.");

class D2D1Support
{
public:
    static bool CheckSupport();
    static ID2D1Factory* GetFactory();
};

class D2D1DCSupport
{
public:
    D2D1DCSupport();
    ~D2D1DCSupport() = default;
    auto GetRenderTarget() const
        -> Microsoft::WRL::ComPtr<ID2D1DCRenderTarget>;

private:
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> m_p_render_target;
};

class DWriteSupport
{
public:
    static bool CheckSupport();
    static IDWriteFactory* GetFactory();
};
