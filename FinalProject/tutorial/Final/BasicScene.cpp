#include "BasicScene.h"
#include <Eigen/src/Core/Matrix.h>
#include <memory>
#include "SceneWithImGui.h"
#include "GLFW/glfw3.h"
#include "Mesh.h"
#include "PickVisitor.h"
#include "Renderer.h"
#include "IglMeshLoader.h"
#include "imgui.h"
#include "ModelsFactory.h"

using namespace cg3d;

void BasicScene::Init(float fov, int width, int height, float near, float far)
{
    //init game objects
    camera = Camera::Create( "camera", fov, float(width) / height, near, far);
    init_objects();
    statistics = GameStatistics::getInstance();
    data = Data::getInstance();
    init_helpers();
    this->animate = false;


}

BasicScene::BasicScene(std::string name, Display* display) : SceneWithImGui(std::move(name), display)
{
    ImGui::GetIO().IniFilename = nullptr;
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 5.0f;
}
void BasicScene::BuildImGui(){
    startMenu();
    PausedMenu();
    NextLevelMenu();
    WinMenu();
    LoseMenu();
    StoreMenu();
    LeadersMenu();
    SettingsMenu();
}
void BasicScene::Update(const Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    Scene::Update(program, proj, view, model);
    program.SetUniform4f("lightColor", 0.8f, 0.3f, 0.0f, 0.5f);
    program.SetUniform4f("Kai", 1.0f, 0.3f, 0.6f, 1.0f);
    program.SetUniform4f("Kdi", 0.5f, 0.5f, 0.0f, 1.0f);
    program.SetUniform1f("specular_exponent", 5.0f);
    program.SetUniform4f("light_position", 0.0, 15.0f, 0.0, 1.0f);
}

void BasicScene::MouseCallback(Viewport* viewport, int x, int y, int button, int action, int mods, int buttonState[])
{
    // note: there's a (small) chance the button state here precedes the mouse press/release event

    if (action == GLFW_PRESS && animate) { // default mouse button press behavior
        PickVisitor visitor;
        visitor.Init();
        renderer->RenderViewportAtPos(x, y, &visitor); // pick using fixed colors hack
        auto modelAndDepth = visitor.PickAtPos(x, renderer->GetWindowHeight() - y);
        renderer->RenderViewportAtPos(x, y); // draw again to avoid flickering
        pickedModel = modelAndDepth.first ? std::dynamic_pointer_cast<Model>(modelAndDepth.first->shared_from_this()) : nullptr;
        pickedModelDepth = modelAndDepth.second;
        camera->GetRotation().transpose();
        xAtPress = x;
        yAtPress = y;

        // if (pickedModel)
        //     debug("found ", pickedModel->isPickable ? "pickable" : "non-pickable", " model at pos ", x, ", ", y, ": ",
        //           pickedModel->name, ", depth: ", pickedModelDepth);
        // else
        //     debug("found nothing at pos ", x, ", ", y);

        if (pickedModel && !pickedModel->isPickable)
            pickedModel = nullptr; // for non-pickable models we need only pickedModelDepth for mouse movement calculations later

        if (pickedModel)
            pickedToutAtPress = pickedModel->GetTout();
        else
            cameraToutAtPress = camera->GetTout();
    }
}

void BasicScene::ScrollCallback(Viewport* viewport, int x, int y, int xoffset, int yoffset, bool dragging, int buttonState[])
{
    // note: there's a (small) chance the button state here precedes the mouse press/release event
    auto system = camera->GetRotation().transpose();
    if (pickedModel) {
        pickedModel->TranslateInSystem(system, {0, 0, -float(yoffset)});
        pickedToutAtPress = pickedModel->GetTout();
    } else {
        camera->TranslateInSystem(system, {0, 0, -float(yoffset)});
        cameraToutAtPress = camera->GetTout();
    }
}

void BasicScene::CursorPosCallback(Viewport* viewport, int x, int y, bool dragging, int* buttonState)
{
    if (dragging && animate) {
        auto system = camera->GetRotation().transpose() * GetRotation();
        auto moveCoeff = camera->CalcMoveCoeff(pickedModelDepth, viewport->width);
        auto angleCoeff = camera->CalcAngleCoeff(viewport->width);
        if (pickedModel) {
            //pickedModel->SetTout(pickedToutAtPress);
            if (buttonState[GLFW_MOUSE_BUTTON_RIGHT] != GLFW_RELEASE)
                pickedModel->TranslateInSystem(system, {-float(xAtPress - x) / moveCoeff, float(yAtPress - y) / moveCoeff, 0});
            if (buttonState[GLFW_MOUSE_BUTTON_MIDDLE] != GLFW_RELEASE)
                pickedModel->RotateInSystem(system, float(xAtPress - x) / angleCoeff, Axis::Z);
            if (buttonState[GLFW_MOUSE_BUTTON_LEFT] != GLFW_RELEASE) {
                pickedModel->RotateInSystem(system, float(xAtPress - x) / angleCoeff, Axis::Y);
                pickedModel->RotateInSystem(system, float(yAtPress - y) / angleCoeff, Axis::X);
            }
        } else {
            // camera->SetTout(cameraToutAtPress);
            if (buttonState[GLFW_MOUSE_BUTTON_RIGHT] != GLFW_RELEASE)
                root->TranslateInSystem(system, {-float(xAtPress - x) / moveCoeff/10.0f, float( yAtPress - y) / moveCoeff/10.0f, 0});
            if (buttonState[GLFW_MOUSE_BUTTON_MIDDLE] != GLFW_RELEASE)
                root->RotateInSystem(system, float(x - xAtPress) / 180.0f, Axis::Z);
            if (buttonState[GLFW_MOUSE_BUTTON_LEFT] != GLFW_RELEASE) {
                root->RotateInSystem(system, float(x - xAtPress) / angleCoeff, Axis::Y);
                root->RotateInSystem(system, float(y - yAtPress) / angleCoeff, Axis::X);
            }
        }
        xAtPress =  x;
        yAtPress =  y;
    }
}

void BasicScene::KeyCallback(Viewport* viewport, int x, int y, int key, int scancode, int action, int mods)
{
    auto system = camera->GetRotation().transpose();

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) // NOLINT(hicpp-multiway-paths-covered)
        {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
            case GLFW_KEY_UP:
                snake->MoveUp();
                break;
            case GLFW_KEY_DOWN:
                snake->MoveDone();
                break;
            case GLFW_KEY_LEFT:
                snake->MoveLeft();

                break;
            case GLFW_KEY_RIGHT:
                snake->MoveRight();
                break;
            case GLFW_KEY_W:
                camera->TranslateInSystem(system, {0, 0.1f, 0});
                break;
            case GLFW_KEY_P:
                animate = false;
                statistics->menu_flags[PauseMenu_OP] = true;
                break;
            case GLFW_KEY_S:
                camera->TranslateInSystem(system, {0, -0.1f, 0});
                break;
            case GLFW_KEY_A:
                camera->TranslateInSystem(system, {-0.1f, 0, 0});
                break;
            case GLFW_KEY_D:
                camera->TranslateInSystem(system, {0.1f, 0, 0});
                break;
            case GLFW_KEY_B:
                camera->TranslateInSystem(system, {0, 0, 0.1f});
                break;
            case GLFW_KEY_F:
                camera->TranslateInSystem(system, {0, 0, -0.1f});
                break;
            case GLFW_KEY_1:
                if( pickedIndex > 0)
                    pickedIndex--;
                break;
            case GLFW_KEY_2:
                done_inite = true;
                break;
            case GLFW_KEY_3:
                animate = true;
                break;
            case GLFW_KEY_4:
                snake->with_skinning = !snake->with_skinning;
                break;
        }
    }
}

Eigen::Vector3f BasicScene::GetSpherePos()
{
    Eigen::Vector3f l = Eigen::Vector3f(1.6f,0,0);
    Eigen::Vector3f res;
    return res;
}

void BasicScene::init_objects() {
    AddChild(root = Movable::Create("root")); // a common (invisible) parent object for all the shapes
    auto background=ModelsFactory::getInstance()->CreateModel(DAYLIGHT_MATERIAL,CUBE,"background");
    AddChild(background);
    background->Scale(120, Axis::XYZ);
    background->SetPickable(false);
    background->SetStatic();
    auto program = std::make_shared<Program>("shaders/phongShader");
    material =  std::make_shared<Material>("material", program); // empty material
    material->AddTexture(0, "textures/box0.bmp", 2);
    snake = new Snake(material,root,camera);
    camera->Translate(44,Movable::Axis::Z);
    camera->Translate(10, Movable::Axis::Y);
}

void BasicScene::startMenu() {

    if(statistics->menu_flags[MainMenu_OP]) {
        ImGui::SetNextWindowPos(ImVec2(200, 200));
        ImGui::SetNextWindowSize(ImVec2(400, 400));
        ImGui::SetNextWindowSizeConstraints(ImVec2(400, -1.0f), ImVec2(400, -1.0f));
        ImGui::Begin("Snake Game - 3D Animation Course", nullptr, ImGuiWindowFlags_NoMove);
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        if (ImGui::Button("Start Game", ImVec2(-1, 0))) {
            std::cout << "new game button pressed in start menu ." << endl;
            statistics->menu_flags[MainMenu_OP] = false;
            animate = true;

        }
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        if (ImGui::Button("Tutorial", ImVec2(-1, 0))) {
            if (data->message_size() == 0)
                data->set_message(
                        "Press the up,down,right,left,\n Keys to move around.\nBe aware of obstacles.\n"
                        "Eat as much animals as you can.\nEach eat will gain you score points.\nGood Luck!");
            else
                data->set_message("");
        }
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        if (ImGui::Button("Store", ImVec2(-1, 0))) {
            std::cout << "store button pressed in start menu  ." << endl;
            statistics->menu_flags[MainMenu_OP] = false;
            data->set_back(MainMenu_OP);
            statistics->menu_flags[StoreMenu_OP] = true;
        }
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        if (ImGui::Button("Leaders", ImVec2(-1, 0))) {
            std::cout << "leaders button pressed in start menu  ." << endl;
            statistics->menu_flags[MainMenu_OP] = false;
            data->set_back(MainMenu_OP);
            statistics->menu_flags[LeadersMenu_OP] = true;
        }
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        if (ImGui::Button("Settings", ImVec2(-1, 0))) {
            if (data->message_size() == 0)
            {
                statistics->menu_flags[MainMenu_OP] = false;
                data->set_back(MainMenu_OP);
                statistics->menu_flags[SettingsMenu_OP] = true;
            }
            else {
                data->set_message("");
            }
        }
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Text("%s", data->msg_c_str());
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::BulletText("Snake Game By Yuval Hitter & Bar Damri. \n3D Animation Course\n");
        ImGui::End();
    }
}


void BasicScene::init_helpers(){

    this->highScores = new HighScores();
    this->statistics->menu_flags[MainMenu_OP] = true;
    this->soundManager = SoundManager::getInstance();
    soundManager->play_game_music();

}

void BasicScene::PausedMenu()
{
    if(statistics->menu_flags[PauseMenu_OP]) {
        ImGui::Begin("Snake Game - 3D Animation Course", nullptr, ImGuiWindowFlags_NoMove);
        ImGui::SetNextWindowPos(ImVec2(200, 200));
        ImGui::SetNextWindowSize(ImVec2(400, 400));
        ImGui::SetNextWindowSizeConstraints(ImVec2(400, -1.0f), ImVec2(400, -1.0f));
        ImGui::Text("%s", statistics->characterName);
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::ProgressBar(statistics->get_progress(), ImVec2(0.0f, 0.0f));
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text("Progress Bar");
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::Text("Total score ");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        std::string tmp = std::to_string(statistics->score);
        ImGui::Text("%s", tmp.c_str());
        ImGui::Spacing();
        ImGui::Text("Level ");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        std::string tmp2 = std::to_string(statistics->level);
        ImGui::Text("%s", tmp2.c_str());
        ImGui::Text("Snake speed ");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        tmp = std::to_string(statistics->speed);
        ImGui::Text("%s", tmp.c_str());
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        if (ImGui::Button("Continue", ImVec2(-1, 0))) {
            std::cout << "continue button pressed in pause menu ." << endl;
            statistics->menu_flags[PauseMenu_OP] = false;
            animate = true;
        }
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        if (ImGui::Button("Store", ImVec2(-1, 0))) {
            std::cout << "store menu button pressed in pause menu." << endl;
            statistics->menu_flags[PauseMenu_OP] = false;
            data->set_back(PauseMenu_OP);
            statistics->menu_flags[StoreMenu_OP] = true;
        }
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        if (ImGui::Button("Settings", ImVec2(-1, 0))) {
            if (data->message_size() == 0) {
                std::cout << "setting menu button pressed in pause menu." << endl;
                statistics->menu_flags[PauseMenu_OP] = false;
                data->set_back(PauseMenu_OP);
                statistics->menu_flags[SettingsMenu_OP] = true;
            }
            else
                data->set_message("");
        }
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::BulletText("Snake Game By Yuval Hitter & Bar Damri. \n3D Animation Course\n");
        ImGui::End();
    }
}


void BasicScene::SettingsMenu()
{
    if(statistics->menu_flags[SettingsMenu_OP]) {
        ImGui::Begin("Snake Game - 3D Animation Course", nullptr, ImGuiWindowFlags_NoMove);
        ImGui::SetNextWindowPos(ImVec2(200, 200));
        ImGui::SetNextWindowSize(ImVec2(400, 400));
        ImGui::SetNextWindowSizeConstraints(ImVec2(400, -1.0f), ImVec2(400, -1.0f));
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Begin("Settings");
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        // Checkbox for sound on/off
        if (ImGui::Checkbox("Sound On", &data->soundOn)) {
            data->soundOn = false;
            soundManager->stop_game_music();
        }
        else {
            data->soundOn = true;
            soundManager->play_game_music();
        }

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();


        // Slider for volume control
        float tmp = data->volume ;
        if(ImGui::SliderFloat("Volume", &data->volume, 0.0f, 1.0f) ){
           //TODO: CHAnge volume.
        }
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::End();

        ImGui::BulletText("Snake Game By Yuval Hitter & Bar Damri. \n3D Animation Course\n");
        ImGui::End();
    }
}
void BasicScene::NextLevelMenu() {

    if (statistics->menu_flags[LevelMenu_OP]) {
        ImGui::Begin("Snake Game - 3D Animation Course", nullptr, ImGuiWindowFlags_NoMove);
        ImGui::SetNextWindowPos(ImVec2(200, 200));
        ImGui::SetNextWindowSize(ImVec2(400, 400));
        ImGui::SetNextWindowSizeConstraints(ImVec2(400, -1.0f), ImVec2(400, -1.0f));
        ImGui::Text("%s", statistics->characterName);
        ImGui::Spacing();
        ImGui::Text("Congratulations You passed level");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        std::string tmp = std::to_string(statistics->level);
        ImGui::Text("%s", tmp.c_str());
        ImGui::Spacing();
        ImGui::Text("Your total score until now is ");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        tmp = std::to_string(statistics->score);
        ImGui::Text("%s", tmp.c_str());
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        if (ImGui::Button("Next level", ImVec2(-1, 0))) {
            //TODO: START NEXT LEVEL.
            std::cout << "Next level button pressed in next level menu." << endl;
            //highScores->saveToHighScores(characterName, game->getTotalMoney());

        }
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        if (ImGui::Button("Back to menu", ImVec2(-1, 0))) {
            //TODO: END GAME.
            std::cout << "Next level button pressed in next level menu." << endl;
            //highScores->saveToHighScores(characterName, game->getTotalMoney());
            statistics->menu_flags[LevelMenu_OP] = false;
            statistics->menu_flags[MainMenu_OP] = true;
        }
        ImGui::BulletText("Snake Game By Yuval Hitter & Bar Damri. \n3D Animation Course\n");
        ImGui::End();
    }
}
void BasicScene::WinMenu() {
    if (statistics->menu_flags[WinMenu_OP]) {
        ImGui::Begin("Snake Game - 3D Animation Course", nullptr, ImGuiWindowFlags_NoMove);
        ImGui::SetNextWindowPos(ImVec2(200, 200));
        ImGui::SetNextWindowSize(ImVec2(400, 400));
        ImGui::SetNextWindowSizeConstraints(ImVec2(400, -1.0f), ImVec2(400, -1.0f));
        ImGui::Text("%s", statistics->characterName);
        ImGui::Spacing();
        ImGui::Text("Congratulations You ended the game!!");
        ImGui::Spacing();
        ImGui::Text("Your total score until now is ");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        std::string tmp = std::to_string(statistics->score);
        ImGui::Text("%s", tmp.c_str());
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        if (ImGui::Button("Back to menu", ImVec2(-1, 0))) {
            //TODO: GO TO MENU.
            std::cout << "Back to menu button pressed in win menu." << endl;
            //highScores->saveToHighScores(characterName, game->getTotalMoney());
            statistics->menu_flags[WinMenu_OP] =false;
            statistics->menu_flags[MainMenu_OP] =true;
        }
        ImGui::BulletText("Snake Game By Yuval Hitter & Bar Damri. \n3D Animation Course\n");
        ImGui::End();
    }
}

void BasicScene::LoseMenu() {

    if (statistics->menu_flags[GameOverMenu_OP]) {
        ImGui::Begin("Snake Game - 3D Animation Course", nullptr, ImGuiWindowFlags_NoMove);
        ImGui::SetNextWindowPos(ImVec2(200, 200));
        ImGui::SetNextWindowSize(ImVec2(400, 400));
        ImGui::SetNextWindowSizeConstraints(ImVec2(400, -1.0f), ImVec2(400, -1.0f));
        ImGui::Text("%s", statistics->characterName);
        ImGui::Spacing();
        ImGui::Text("ohhh you died...");
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        if (ImGui::Button("Back to menu", ImVec2(-1, 0))) {
            std::cout << "Back to menu button pressed in loose menu." << endl;
            //highScores->saveToHighScores(characterName, game->getTotalMoney());
            statistics->menu_flags[WinMenu_OP] =false;
            statistics->menu_flags[MainMenu_OP] =true;
        }
        ImGui::BulletText("Snake Game By Yuval Hitter & Bar Damri. \n3D Animation Course\n");
        ImGui::End();
    }
}
void BasicScene::StoreMenu() {

    if (statistics->menu_flags[StoreMenu_OP]) {
        ImGui::Begin("Snake Game - 3D Animation Course", nullptr, ImGuiWindowFlags_NoMove);
        ImGui::SetNextWindowPos(ImVec2(200, 200));
        ImGui::SetNextWindowSize(ImVec2(400, 400));
        ImGui::SetNextWindowSizeConstraints(ImVec2(400, -1.0f), ImVec2(400, -1.0f));
        ImGui::Text("%s", statistics->characterName);
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text("welcome to the store");
        ImGui::Spacing();
        ImGui::Text("You have total money of ");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        std::string tmp = std::to_string(data->get_total_money());
        ImGui::Text("%s", tmp.c_str());
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        if (ImGui::Button("extra speed - 50 coins", ImVec2(-1, 0))) {
            statistics->speed += 1;

        }
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        if (ImGui::Button("refill health - 30 coins", ImVec2(-1, 0))) {
            data->inc_life_bought();
            data->set_message(std::to_string(data->get_life_bought()));
        }

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        if (ImGui::Button("Back to game", ImVec2(-1, 0))) {
            std::cout << "Back to game button pressed in store menu." << endl;
            statistics->menu_flags[StoreMenu_OP] = false;
            if(data->get_back_to_main() ==-1){
                animate =true;
            }
            else{
                statistics->menu_flags[data->get_back_to_main()] = true;
                data->set_back(-1);
            }
        }
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Text("%s", data->msg_c_str());
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::BulletText("Snake Game By Yuval Hitter & Bar Damri. \n3D Animation Course\n");
        ImGui::End();
    }
}
void BasicScene::LeadersMenu() {

    if (statistics->menu_flags[LeadersMenu_OP]) {
        ImGui::Begin("Snake Game - 3D Animation Course", nullptr, ImGuiWindowFlags_NoMove);
        ImGui::SetNextWindowPos(ImVec2(200, 200));
        ImGui::SetNextWindowSize(ImVec2(400, 400));
        ImGui::SetNextWindowSizeConstraints(ImVec2(400, -1.0f), ImVec2(400, -1.0f));
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text("Leaders board");
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        std::vector<std::string> names = highScores->getTopHighScoreNames();
        std::vector<int> scores = highScores->getTopHighScores();
        if (names.size() == 0) {
            ImGui::BulletText("No Saved Scores!");
        } else {
            for (int i = names.size() - 1; i >= 0; i--) {
                std::string text = names[i] + "\t|\t" + std::to_string(scores[i]) + "\n";
                ImGui::BulletText("%s", text.c_str());
            }
        }

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        if (ImGui::Button("Back", ImVec2(-1, 0))) {
            std::cout << "Back button pressed in store menu." << endl;
            statistics->menu_flags[LeadersMenu_OP] = false;
            if(data->get_back_to_main() ==-1){
                animate =true;
            }
            else{
                statistics->menu_flags[data->get_back_to_main()] = true;
                data->set_back(-1);
            }
        }
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::BulletText("Snake Game By Yuval Hitter & Bar Damri. \n3D Animation Course\n");
        ImGui::End();
    }
}

void BasicScene::dropEngine() {

    if (this->soundManager != NULL){
        this->soundManager->drop();
    }

}
BasicScene::~BasicScene(){
    delete soundManager;
}