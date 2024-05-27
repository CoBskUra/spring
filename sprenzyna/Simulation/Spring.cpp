#include "Spring.h"


namespace Simulation {

    void SpringSimulation::DrawSpring() {
        float springArea = appearance.area;
        const auto& style = ImGui::GetStyle();
        const auto& draw_list = ImGui::GetWindowDrawList();
        const auto& screenPos = ImGui::GetCursorScreenPos();
        const auto& windowSize = ImGui::GetWindowSize();

        float scaledPosition = state.position * state.positionScalar;
        float length = scaledPosition + windowSize.y / 3.0f;
        float springWidth = springArea / length;
        springWidth = std::min(appearance.maxWidth, springWidth);
        ImVec2 springSize = ImVec2(springWidth, length);
        ImVec2 windowTopCenter = ImVec2(screenPos.x + windowSize.x / 2.0f, screenPos.y);
        ImVec2 springStart = ImVec2(windowTopCenter.x - springWidth / 2.0f, windowTopCenter.y);
        ImVec2 weightStart = windowTopCenter + ImVec2(-appearance.weightSize.x / 2.0f, length);
        ImVec2 weightEnd = weightStart + appearance.weightSize;


        draw_list->AddRectFilled(springStart, springStart + springSize, ImGui::ColorConvertFloat4ToU32(appearance.springColor), style.FrameRounding);
        draw_list->AddRectFilled(weightStart, weightEnd, ImGui::ColorConvertFloat4ToU32(appearance.weightColor), style.FrameRounding);
    }

    void SpringSimulation::CalculateNextState() {

        // obliczanie si³
        state.w = CalculateSelectedFunction(state.fun_w_Id, state.time, state.w_amplitude, state.w_frequency, state.w_shift);
        state.h = CalculateSelectedFunction(state.fun_h_Id, state.time, state.h_amplitude, state.h_frequency, state.h_shift);

        state.dampingForce = -state.dampingScalar * state.velocity;
        state.springForce = state.elasticityScalar * (state.w - state.position);
        state.force = state.mass * state.acceleration;


        // obliczanie pochodnych po³o¿enia 
        state.acceleration = (state.springForce + state.dampingForce + state.h) * state.inverseMass;
        state.position = state.position + state.delta * (state.velocity /*+ state.delta * state.acceleration * 0.5f*/);
        state.velocity = state.velocity + state.delta * state.acceleration;


        // aktualny czas symulacji
        state.time += state.delta;
    }







   
    float Constant(float time, float amplitude, float frequency, float shift)
    {
        return amplitude;
    }

    float EdgeFunction(float time, float amplitude, float frequency, float shift)
    {
        if (time > 0)
            return amplitude;
        else
            return 0;
    }

    float Sinusoidal(float time, float amplitude, float frequency, float shift) {
        return amplitude * sinf(frequency * time + shift);
    }

    float SgnSinusoidal(float time, float amplitude, float frequency, float shift) {
        float tmp = Sinusoidal(time, amplitude, frequency, shift);
        return (tmp > 0) - (tmp < 0);
    }


    float  CalculateSelectedFunction(int id, float time, float amplitude, float frequency, float shift)
    {
        switch (id)
        {
        case 0:
            return Constant(time, amplitude, frequency, shift);
            break;
        case 1:
            return EdgeFunction(time, amplitude, frequency, shift);
            break;
        case 2:
            return SgnSinusoidal(time, amplitude, frequency, shift);
            break;
        case 3:
            return Sinusoidal(time, amplitude, frequency, shift);
            break;
        default:
            break;
        }
    }
}


