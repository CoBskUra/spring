﻿// Dear ImGui: standalone example application for DirectX 9

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include <tchar.h>
#include "Utilites/Grid.h"
#include "Simulation/Spring.h"
#include "Simulation/Plots.h"
#include "Extend_ImGui/Text.h"
#include "implot/implot.h"
# define M_PI           3.14159265358979323846 
#define MORE_THEN_ZERO 1/(2^32)
//#include "Extend_ImGui/ExtendImVec2.h"

// Data
static LPDIRECT3D9              g_pD3D = nullptr;
static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};


int main(int, char**);

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
void SetSizeAndPosition(Grid* data);
void  SelectedFunction(float (*f)(float, float, float, float), int id);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



// Main code
int main(int, char**)
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Sprezyna", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);


    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // Our state
    bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    char buf[100];
    float f;
    bool animationActive = false;
    bool reset = false;
    ImVec2 buttonSize = ImVec2(100, 30);

    // set global window flags
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;

    // split window 
    Grid father = Grid(1, "head");
    Grid columns[3] = { Grid(0.2f,"parameters"), Grid(0.2f, "Animations"), Grid(0.6f)};
    father.Split(columns, 3, Grid_CR::Colum);
    //Grid rows1[2] = { Grid(0.65f, "Animations"), Grid(0.35f,"pva")};
    Grid rows2[3] = { Grid(0.3f, "force"), Grid(0.3f,"pva"),  Grid(0.4f, "position")};
    //columns[1].Split(rows1, 2, Grid_CR::Row);
    columns[2].Split(rows2, 3, Grid_CR::Row);

    Grid *parameters = father.FindIndex("parameters");
    Grid *animations = father.FindIndex("Animations");
    Grid *pva = father.FindIndex("pva");
    Grid *graphOfForce = father.FindIndex("force");
    Grid *graphOfPosition = father.FindIndex("position");
    
    
    // spring simulation values
    Simulation::SimulationState simulationState;
    simulationState.delta = 0.1;
    simulationState.elasticityScalar = 1;
    simulationState.h = 0;
    simulationState.dampingScalar = 0.1;
    simulationState.mass = 1;
    simulationState.position = 20;
    simulationState.velocity = 0;
    simulationState.w = 1;
    simulationState.positionScalar = 10;

    // spring appearance
    Simulation::SpringsAppearance appearance;
    appearance.springColor = ImVec4(0.59f, 0.664f, 0.88f, 1.00f);
    appearance.weightColor = ImVec4(0.2f, 0.5f, 0.3f, 1.00f);
    appearance.weightSize = ImVec2(55, 75);
    appearance.area = 2000;
    appearance.minWidth = 2;
    appearance.maxWidth = 75;
    Simulation::SpringSimulation springSimulation = Simulation::SpringSimulation(simulationState, appearance);

    float minShift = -M_PI;
    float maxShift = M_PI;

   



    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            g_d3dpp.BackBufferWidth = g_ResizeWidth;
            g_d3dpp.BackBufferHeight = g_ResizeHeight;
            g_ResizeWidth = g_ResizeHeight = 0;
            ResetDevice();
        }
        father.SetSize(io.DisplaySize.x, io.DisplaySize.y);

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();



       //  parameters
        {
            ImGui::Begin("Spring Information", 0,  window_flags);                          // Create a window called "Hello, world!" and append into it.
            SetSizeAndPosition(parameters);

            // info
            ImGui::Text("Basic information");
            ImGui::LabelText("FPS","%.2f",io.Framerate);

            ImGui::Text("Simulation information");
            ImGui::Text("   Kinematics");
            ImGui::LabelText("x", "%.2f", springSimulation.state.position);
            ImGui::LabelText("x1", "%.2f", springSimulation.state.velocity);
            ImGui::LabelText("x(t)", "%.2f", springSimulation.state.acceleration);

            ImGui::Text("   Forces");
            ImGui::LabelText("f(t)", "%.2f", springSimulation.state.force);
            ImGui::LabelText("g(t)", "%.2f", springSimulation.state.dampingForce);
            ImGui::LabelText("h(t)", "%.2f", springSimulation.state.h);

            ImGui::Text("   Other");
            ImGui::LabelText("w(t)", "%.2f", springSimulation.state.w);

            // Inputs
            ImGui::Text("Simulation properties");
            DragFloatWithDisable(!animationActive, "x0", &simulationState.position,0.1f, FLT_MIN, FLT_MAX);
            DragFloatWithDisable(!animationActive, "v0", &simulationState.velocity, 0.1f, FLT_MIN, FLT_MAX);
            DragFloatWithDisable(!animationActive, "delta", &simulationState.delta, 0.1f, FLT_MIN, FLT_MAX);
            DragFloatWithDisable(!animationActive, "mass", &simulationState.mass, 0.1f, MORE_THEN_ZERO, FLT_MAX);
            DragFloatWithDisable(!animationActive, "damping", &simulationState.dampingScalar, 0.1f, FLT_MIN, FLT_MAX);
            DragFloatWithDisable(!animationActive, "elasticity", &simulationState.elasticityScalar, 0.1f, FLT_MIN, FLT_MAX);
            ImGui::NewLine();

            ImGui::Combo("h fun", &simulationState.fun_h_Id, Simulation::functionDescriptions, IM_ARRAYSIZE(Simulation::functionDescriptions), IM_ARRAYSIZE(Simulation::functionDescriptions));
            DragFloatWithDisable(!animationActive, "h_amplitude", &simulationState.h_amplitude, 0.1f, FLT_MIN, FLT_MAX);
            DragFloatWithDisable(!animationActive, "h_frequency", &simulationState.h_frequency, 0.1f, FLT_MIN, FLT_MAX);
            DragFloatWithDisable(!animationActive, "h_shift", &simulationState.h_shift, 0.01f,  minShift, maxShift);
            ImGui::NewLine();

            ImGui::Combo("w fun", &simulationState.fun_w_Id, Simulation::functionDescriptions, IM_ARRAYSIZE(Simulation::functionDescriptions), IM_ARRAYSIZE(Simulation::functionDescriptions));
            DragFloatWithDisable(!animationActive, "w_amplitude", &simulationState.w_amplitude, 0.1f, FLT_MIN, FLT_MAX);
            DragFloatWithDisable(!animationActive, "w_frequency", &simulationState.w_frequency, 0.1f, FLT_MIN, FLT_MAX);
            DragFloatWithDisable(!animationActive, "w_shift", &simulationState.w_shift, 0.01f,  minShift, maxShift);

            if (animationActive && ImGui::Button("Stop", buttonSize))
            {
                animationActive = false;
            }
            else if(!animationActive && ImGui::Button("Start", buttonSize))
            {
                animationActive = true;
                if (reset)
                {
                    simulationState.inverseMass = 1 / simulationState.mass;
                    springSimulation.state = simulationState;
                }
                reset = false;
            }

            if (ImGui::Button("Reset", buttonSize))
            {
                reset = true;
                animationActive = false;
            }

            ImGui::End();
        }

        // Animations
        {
            ImGui::Begin("Spring visualization!",0 , window_flags);                          // Create a window called "Hello, world!" and append into it.
            SetSizeAndPosition(animations);
            if (animationActive) {
                springSimulation.CalculateNextState();
            }
            springSimulation.DrawSpring();

            ImGui::End();
        }

        // position/ velocity. acceleration
        {
            ImGui::Begin("Graphs of position/ velocity/ acceleration", 0, window_flags);                          // Create a window called "Hello, world!" and append into it.
            SetSizeAndPosition(pva);
            Simulation::GraphOfPositionVelocityAcceleration(
                springSimulation.state.time, springSimulation.state.position, springSimulation.state.velocity, 
                springSimulation.state.acceleration,animationActive, reset);
            ImGui::End();
        }

        // grap of force
        {
            ImGui::Begin("Graphs of forces", 0, window_flags);                      // Create a window called "Hello, world!" and append into it.
            SetSizeAndPosition(graphOfForce);
            Simulation::GraphOfForces(
                springSimulation.state.time, springSimulation.state.springForce, 
                springSimulation.state.dampingForce, springSimulation.state.w, 
                springSimulation.state.h, animationActive, reset);
            ImGui::End();
        }

        // graphs of position
        {
            ImGui::Begin("Trajectory graph", 0, window_flags);                         // Create a window called "Hello, world!" and append into it.
            SetSizeAndPosition(graphOfPosition);
            Simulation::GraphOfTrajectory(springSimulation.state.position, springSimulation.state.velocity, 
                animationActive, reset);
            ImGui::End();
        }


        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

void SetSizeAndPosition(Grid* data) {
    ImGui::SetWindowSize(data->GetSize());
    ImGui::SetWindowPos(data->GetPosition());
}

