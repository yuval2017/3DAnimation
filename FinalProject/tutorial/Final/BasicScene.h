#pragma once
#include "AutoMorphingModel.h"
#include "SceneWithImGui.h"
#include "Snake.h"
#include "CamModel.h"
#include "Data.h"
#include "GameStatistics.h"
#include "HighScores.h"
#include <memory>
#include "imgui.h"
#include <utility>
#include "SoundManager.h"

class BasicScene : public cg3d::SceneWithImGui
{
public:
    BasicScene(std::string name, cg3d::Display* display);
    void BuildImGui() override;
    void dropEngine();
    bool isLeader(int score);
    void Init(float fov, int width, int height, float near, float far);
    void Update(const cg3d::Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model) override;
    void MouseCallback(cg3d::Viewport* viewport, int x, int y, int button, int action, int mods, int buttonState[]) override;
    void ScrollCallback(cg3d::Viewport* viewport, int x, int y, int xoffset, int yoffset, bool dragging, int buttonState[]) override;
    void CursorPosCallback(cg3d::Viewport* viewport, int x, int y, bool dragging, int* buttonState)  override;
    void KeyCallback(cg3d::Viewport* viewport, int x, int y, int key, int scancode, int action, int mods) override;
    std::shared_ptr<Movable> GetRoot(){return root;};
    Eigen::Vector3f GetSpherePos();
    Snake *snake;
    //added for engine
    std::vector<std::shared_ptr<cg3d::Model>> game_models;
    void create_menu();
    bool done_inite = false;
    ~BasicScene();
private:
    Data* data;
    GameStatistics* statistics;
    HighScores* highScores;
    SoundManager* soundManager;
    std::shared_ptr<Movable> root;
    std::shared_ptr<Material> material;
    std::shared_ptr<Program> program;
    std::shared_ptr<cg3d::Model> sphere1, sphere2, sphere3;
    std::shared_ptr<cg3d::AutoMorphingModel> autoCube;
    std::vector<std::shared_ptr<cg3d::Model>> cyls, axis;
    int pickedIndex = 0;
    int tipIndex = 0;
    Eigen::VectorXi EMAP;
    Eigen::MatrixXi F,E,EF,EI;
    Eigen::VectorXi EQ;
    // If an edge were collapsed, we'd collapse it to these points:
    Eigen::MatrixXd V, C, N, T, points,edges,colors;
    void init_objects();
    void startMenu();
    void PausedMenu();
    void NextLevelMenu();
    void WinMenu();
    void LoseMenu();
    void StoreMenu();
    void LeadersMenu();
    void init_helpers();
    void setFonts();
    void setImage();
    void PlayMenu();
    //SoundManager* sound_manager;
    ImFont* headerFont = nullptr;
    ImFont* regularFont = nullptr;
    ImFont* messageFont = nullptr;
    unsigned char* backgroundImage ;
    int window_width = 700;
    int window_height = 700 ;
    ImVec2 windowSize;
    int image_width;
    int image_height;
    int screen_width;
    int screen_height;
    int channels;
    void SettingsMenu();
    void setStartPos();
    void setWindow(const char* header);
    void endWindow();
    void buttonStyle();
    void setBoxes();
    Score* generateRandomScore();
    GLuint backgroundImageTexture;
    ImVec2 startPos ;
    int windowFlags;
};
