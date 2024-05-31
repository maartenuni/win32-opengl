
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <epoxy/gl.h>
#include <epoxy/wgl.h>


LRESULT CALLBACK
window_proc(HWND window, uint32_t msg_id, WPARAM wparam, LPARAM lparam);

void
print_last_error() {
    DWORD error = GetLastError();

    if (error == 0)
        return;
    
    LPSTR message = NULL;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&message,
        0,
        NULL
        );
    
    fprintf(stderr, "The last error was: %s\n", message);

    LocalFree(message);
}

void setup_open_gl(HWND window) {

    PIXELFORMATDESCRIPTOR pix_fmt_descriptor = {
        .nSize = sizeof(PIXELFORMATDESCRIPTOR),
        .nVersion = 1,
        .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER ,
        .iPixelType = PFD_TYPE_RGBA,
        .cColorBits = 32,
        .cDepthBits = 24,
        .iLayerType = PFD_MAIN_PLANE
    };
    PIXELFORMATDESCRIPTOR copy;
    memcpy (&copy, &pix_fmt_descriptor, sizeof(pix_fmt_descriptor));

    HDC handle_device_context = GetDC(window);
    fprintf(stdout, "handle_device_context : %p\n", (void*) handle_device_context);
    assert(handle_device_context != NULL);

    int pix_fmt_id = 0;
    pix_fmt_id = ChoosePixelFormat(handle_device_context, &pix_fmt_descriptor);
    if (pix_fmt_id == 0) {
        print_last_error();
    }
    if (memcmp(&copy, &pix_fmt_descriptor, sizeof(PIXELFORMATDESCRIPTOR)) != 0) {
        fprintf(stdout, "ChoosePixelFormat update the pixel format descriptor\n");
    }
    fprintf(stdout, "pix_fmt_id = %d\n", pix_fmt_id);
    if (SetPixelFormat(handle_device_context, pix_fmt_id, &pix_fmt_descriptor) != TRUE) {
        print_last_error();
        fprintf(stderr, "unable to set pixlel format\n");
    }

    HGLRC open_gl_context = wglCreateContext(handle_device_context);
    if (!open_gl_context) {
        print_last_error();
        fprintf(stdout, "gl_context : %p\n", (void*) open_gl_context);
    }
    wglMakeCurrent(handle_device_context, open_gl_context);

    fprintf(stdout, "OpenGL version: %s\n", glGetString(GL_VERSION));
}

int main(void) {

    MSG message;

    const char class_name[] = "OpenGL test window class";
    const char window_name[] = "OpenGL test window";

    HINSTANCE instance = GetModuleHandle(NULL);

    WNDCLASS window_class = {
        .lpfnWndProc = window_proc,
        .hInstance = instance,
        .lpszClassName = class_name,
        .style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW,
    };

    RegisterClass(&window_class);

    HWND window_handle = CreateWindow(
        class_name,
        window_name,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        NULL,
        NULL
    );

    if (window_handle == NULL) {
        return EXIT_FAILURE;
    }

    ShowWindow(window_handle, SW_SHOW);
    UpdateWindow(window_handle);

    while (GetMessage(&message, window_handle, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
}

LRESULT CALLBACK
window_proc(HWND window, uint32_t msg_id, WPARAM wparam, LPARAM lparam)
{
    switch(msg_id) {
    case WM_CREATE:
        setup_open_gl(window);
        break;
    case WM_SIZE:
        {
            int width = LOWORD(lparam);
            int height = HIWORD(lparam);
            fprintf(stdout, "wm_size = %d %d\n", width, height);
        }
        break;
    default:
        return DefWindowProc(window, msg_id, wparam, lparam);
    }
    return 0;
}