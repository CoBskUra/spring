#pragma once
#include <imgui.h>
#include <string>
#include <format>
//#include "ExtendImVec2.h"

namespace Simulation {
    struct SimulationState {
        float position = 0;
        float positionScalar = 10;
        float velocity = 0;
        float acceleration = 0;
        float force = 0;
        float springForce = 0;
        float dampingForce = 0;
        float dampingScalar = 0;
        float delta = 0;
        float mass = 1;
        float inverseMass = 1;
        float elasticityScalar = 0;
        float time = 0;

        float w = 0;
        float w_amplitude = 1;
        float w_frequency = 1;
        float w_shift = 1;
        int fun_w_Id = 0;

        float h = 0;
        float h_amplitude =1;
        float h_frequency = 1;
        float h_shift = 1;
        int fun_h_Id = 0;


    };

    struct SpringsAppearance {
        float area = 0;
        float minWidth = 0;
        float maxWidth = 0;
        ImVec2 weightSize;
        ImVec4 springColor;
        ImVec4 weightColor;
    };

    class SpringSimulation {
    public:
        SimulationState state;
        SpringsAppearance appearance;

        SpringSimulation(SimulationState state, SpringsAppearance appearance)
        {
            this->state = state;
            this->appearance = appearance;
        }

        void DrawSpring();
        void CalculateNextState();
    };


    static const char* functionDescriptions[4] = { "amplitude", "0  t <  0\namplitude t >= 0", "sgn (amplitude*sin(frequency*time +shift))",  "amplitude*sin(frequency*time +shift)" };

    float Constant(float time, float amplitude, float frequency, float shift);

    float EdgeFunction(float time, float amplitude, float frequency, float shift);

    float Sinusoidal(float time, float amplitude, float frequency, float shift);

    float SgnSinusoidal(float time, float amplitude, float frequency, float shift);

    float  CalculateSelectedFunction(int id, float time, float amplitude, float frequency, float shift);
};