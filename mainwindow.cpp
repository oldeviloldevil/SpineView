#include "StdAfx.h"
#include "mainwindow.h"

namespace utils {

///////////////////////////////////////////////////////////////////////////////
// Win32Window
///////////////////////////////////////////////////////////////////////////////

static const TCHAR kWindowBaseClassName[] = "WindowBaseClass";
HINSTANCE Win32Window::instance_ = NULL;
ATOM Win32Window::window_class_ = 0;

Win32Window::Win32Window() : wnd_(NULL) {
}

Win32Window::~Win32Window() {
//  ATLASSERT(NULL == wnd_);
}

bool Win32Window::Create(HWND parent, LPCTSTR title, DWORD style,
                         DWORD exstyle, int x, int y, int cx, int cy, LPTSTR classname/* = NULL*/) {
  if (wnd_) {
    // Window already exists.
    return false;
  }

  if (!window_class_) {
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCTSTR>(&Win32Window::WndProc),
                           &instance_)) {
//      LOG_GLE(LS_ERROR) << "GetModuleHandleEx failed";
      return false;
    }

    // Class not registered, register it.
    WNDCLASSEX wcex;
    memset(&wcex, 0, sizeof(wcex));
    wcex.cbSize = sizeof(wcex);
    wcex.hInstance = instance_;
	wcex.style = CS_DBLCLKS;
	wcex.cbWndExtra = sizeof(LONG_PTR) + 2; //保存自己的数据
    wcex.lpfnWndProc = &Win32Window::WndProc;
	wcex.lpszClassName = classname ? classname : kWindowBaseClassName;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	window_class_ = ::RegisterClassEx(&wcex);
    if (!window_class_) {
      //TraceLog("RegisterClassEx failed");
      return false;
    }
  }
  wnd_ = ::CreateWindowEx(exstyle, classname ? classname : kWindowBaseClassName, title, style,
                          x, y, cx, cy, parent, NULL, instance_, this);
  return (NULL != wnd_);
}

void Win32Window::Destroy() {
  //ATLVERIFY(::DestroyWindow(wnd_) != FALSE);
}

void Win32Window::Shutdown() {
  if (window_class_) {
    ::UnregisterClass(MAKEINTATOM(window_class_), instance_);
    window_class_ = 0;
  }
}

bool Win32Window::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam,
                            LRESULT& result) {
  switch (uMsg) {
  case WM_CLOSE:
    if (!OnClose()) {
      result = 0;
      return true;
    }
    break;
  }
  return false;
}

LRESULT Win32Window::WndProc(HWND hwnd, UINT uMsg,
                             WPARAM wParam, LPARAM lParam) {
  Win32Window* that = reinterpret_cast<Win32Window*>(
      ::GetWindowLongPtr(hwnd, GWLP_USERDATA));
  if (!that && (WM_CREATE == uMsg)) {
    CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
    that = static_cast<Win32Window*>(cs->lpCreateParams);
    that->wnd_ = hwnd;
    ::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(that));
  }
  if (that) {
    LRESULT result;
    bool handled = that->OnMessage(uMsg, wParam, lParam, result);
    if (WM_DESTROY == uMsg) {
      for (HWND child = ::GetWindow(hwnd, GW_CHILD); child;
           child = ::GetWindow(child, GW_HWNDNEXT)) {
//        LOG(LS_INFO) << "Child window: " << static_cast<void*>(child);
      }
    }
    if (WM_NCDESTROY == uMsg) {
      ::SetWindowLongPtr(hwnd, GWLP_USERDATA, NULL);
      that->wnd_ = NULL;
      that->OnNcDestroy();
    }
    if (handled) {
      return result;
    }
  }
  return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
}

}  // namespace talk_base
