#include "Plots.h"

namespace Simulation {

    void GraphOfPositionVelocityAcceleration(float time, float position, float velocity, float acceleration, 
        bool pause, bool reset)
    {
        static ScrollingBuffer dataOfPVA[3];

        if (reset)
        {
            for (int i = 0; i < 3; i++) {
                dataOfPVA[i].Erase();
            }
        }

        const char* labels[3] = { "Position", "Velocity", "Acceleration" };

        //Analog signal values
        if (pause) {
            dataOfPVA[0].AddPoint(time, position);
            dataOfPVA[1].AddPoint(time, velocity);
            dataOfPVA[2].AddPoint(time, acceleration);
        }

        auto windowSize = ImGui::GetWindowSize();
        auto size = windowSize - ImVec2(30 ,50);



        if (ImPlot::BeginPlot("##GraphOfPositionVelocityAcceleration", size)) {
            ImPlot::SetupAxisLimits(ImAxis_X1, time - 10.0, time, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, -1, 1);
            for (int i = 0; i < 3; ++i) {
                    if (dataOfPVA[i].Data.size() > 0)
                        ImPlot::PlotLine(labels[i], &dataOfPVA[i].Data[0].x, &dataOfPVA[i].Data[0].y, dataOfPVA[i].Data.size(), 0, dataOfPVA[i].Offset, 2 * sizeof(float));
                
            }
            ImPlot::EndPlot();
        }
    }

    void GraphOfForces(float time, float springForce, float dampingForce, float w, float h,
        bool pause, bool reset)
    {
        static ScrollingBuffer dataOfForces[4];

        if (reset)
        {
            for (int i = 0; i < 4; i++) {
                dataOfForces[i].Erase();
            }
        }

        const char* labels[4] = { "springForce", "dampingForce", "w", "h"};


        if (pause) {
            dataOfForces[0].AddPoint(time, springForce);
            dataOfForces[1].AddPoint(time, dampingForce);
            dataOfForces[2].AddPoint(time, w);
            dataOfForces[3].AddPoint(time, h);
        }

        auto windowSize = ImGui::GetWindowSize();
        auto size = windowSize - ImVec2(30, 50);

        if (ImPlot::BeginPlot("##GraphOfForces", size)) {
            ImPlot::SetupAxisLimits(ImAxis_X1, time - 10.0, time, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, -1, 1);
            for (int i = 0; i < 4; ++i) {
                    if (dataOfForces[i].Data.size() > 0)
                        ImPlot::PlotLine(labels[i], &dataOfForces[i].Data[0].x, &dataOfForces[i].Data[0].y, dataOfForces[i].Data.size(), 0, dataOfForces[i].Offset, 2 * sizeof(float));
                
            }
            ImPlot::EndPlot();
        }
    }
    void GraphOfTrajectory(float position, float velocity, bool pause, bool reset)
    {
        static ScrollingBuffer trajectory;
        if (reset)
        {
            trajectory.Erase();
        }

        const char* label = "Trajectory";

        if(pause)
            trajectory.AddPoint(position, velocity);

        auto windowSize = ImGui::GetWindowSize();
        auto size = windowSize - ImVec2(30, 50);

        if (ImPlot::BeginPlot("##GraphOfForces", size)) {
            ImPlot::SetupAxisLimits(ImAxis_Y1, -10, 10);
            ImPlot::SetupAxisLimits(ImAxis_X1, -10, 10);
            if (trajectory.Data.size() > 0)
            ImPlot::PlotLine(label, &trajectory.Data[0].x, &trajectory.Data[0].y, trajectory.Data.size(), 0, trajectory.Offset, 2 * sizeof(float));
            
            ImPlot::EndPlot();
        }
    }
}