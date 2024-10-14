#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <vector>
#include <string>

struct LogMessage
{
    int category;
    SDL_LogPriority priority;
    std::string message;
};

namespace
{
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    bool showDemoWindow = true;
    ImVec4 clearColor{0.45f, 0.55f, 0.60f, 1.00f};
    std::vector<LogMessage> logMessages;
}

void LogOutputMessage(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
    logMessages.push_back({category,
                           priority,
                           message});
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetLogOutputFunction(&LogOutputMessage, nullptr);

    if (!SDL_CreateWindowAndRenderer("My SDL App", 800, 600, SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE, &window, &renderer))
    {
        SDL_Log("Failed to create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_Log("SDL initialized");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    return SDL_APP_CONTINUE;
}

// Function to get the string representation of the log priority
const char *GetPriorityString(SDL_LogPriority priority)
{
    switch (priority)
    {
    case SDL_LOG_PRIORITY_TRACE:
        return "TRACE";
    case SDL_LOG_PRIORITY_VERBOSE:
        return "VERBOSE";
    case SDL_LOG_PRIORITY_DEBUG:
        return "DEBUG";
    case SDL_LOG_PRIORITY_INFO:
        return "INFO";
    case SDL_LOG_PRIORITY_WARN:
        return "WARNING";
    case SDL_LOG_PRIORITY_ERROR:
        return "ERROR";
    case SDL_LOG_PRIORITY_CRITICAL:
        return "CRITICAL";
    default:
        return "UNKNOWN";
    }
}

// Function to get the color associated with each log priority
ImVec4 GetLogLevelColor(SDL_LogPriority priority)
{
    switch (priority)
    {
    case SDL_LOG_PRIORITY_TRACE:
    case SDL_LOG_PRIORITY_VERBOSE:
        return {0.6f, 0.6f, 0.6f, 1.0f}; // Gray
    case SDL_LOG_PRIORITY_DEBUG:
        return {0.0f, 0.75f, 1.0f, 1.0f}; // Cyan
    case SDL_LOG_PRIORITY_INFO:
        return ImGui::GetStyleColorVec4(ImGuiCol_Text); // Default text color
    case SDL_LOG_PRIORITY_WARN:
        return {1.0f, 1.0f, 0.0f, 1.0f}; // Yellow
    case SDL_LOG_PRIORITY_ERROR:
        return {1.0f, 0.5f, 0.5f, 1.0f}; // Light Red
    case SDL_LOG_PRIORITY_CRITICAL:
        return {1.0f, 0.0f, 0.0f, 1.0f}; // Red
    default:
        return ImGui::GetStyleColorVec4(ImGuiCol_Text); // Default text color
    }
}

void ShowLogWindow()
{
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_Appearing);
    ImGui::Begin("Log");

    ImGuiListClipper clipper;
    clipper.Begin(logMessages.size());
    while (clipper.Step())
    {
        for (int lineNo = clipper.DisplayStart; lineNo < clipper.DisplayEnd; ++lineNo)
        {
            const auto &logEntry = logMessages[lineNo];
            ImVec4 color = GetLogLevelColor(logEntry.priority);
            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::Text("[%s] %s", GetPriorityString(logEntry.priority), logEntry.message.c_str());
            ImGui::PopStyleColor();
        }
    }
    clipper.End();

    // Auto-scroll to bottom if already at the bottom
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::End();
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    if (showDemoWindow)
        ImGui::ShowDemoWindow(&showDemoWindow);

    ShowLogWindow();

    ImGui::Render();
    // SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    SDL_SetRenderDrawColorFloat(renderer, clearColor.x, clearColor.y, clearColor.z, clearColor.w);
    SDL_RenderClear(renderer);

    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

bool IsMouseEvent(Uint32 type)
{
    return type == SDL_EVENT_MOUSE_MOTION ||
           type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
           type == SDL_EVENT_MOUSE_BUTTON_UP ||
           type == SDL_EVENT_MOUSE_WHEEL;
};

bool IsKeyboardEvent(Uint32 type)
{
    return type == SDL_EVENT_KEY_DOWN ||
           type == SDL_EVENT_KEY_UP ||
           type == SDL_EVENT_TEXT_INPUT ||
           type == SDL_EVENT_TEXT_EDITING;
};

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    ImGui_ImplSDL3_ProcessEvent(event);
    auto io = ImGui::GetIO();

    if (io.WantCaptureMouse && IsMouseEvent(event->type))
    {
        return SDL_APP_CONTINUE;
    }

    if (io.WantCaptureKeyboard && IsKeyboardEvent(event->type))
    {
        return SDL_APP_CONTINUE;
    }

    switch (event->type)
    {
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    case SDL_EVENT_KEY_DOWN:
    {
        if (event->key.key == SDLK_RETURN && event->key.mod & SDL_KMOD_ALT)
        {
            SDL_SetWindowFullscreen(window, !(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN));
        }
        break;
    }
    default:
        break;
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}