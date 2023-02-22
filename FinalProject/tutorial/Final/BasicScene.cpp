#include "BasicScene.h"
#include <Eigen/src/Core/Matrix.h>
#include <memory>
#include "SceneWithImGui.h"
#include "GLFW/glfw3.h"
#include <random>
#include "Mesh.h"
#include "PickVisitor.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "Renderer.h"
#include "IglMeshLoader.h"
#include "ModelsFactory.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>

using namespace cg3d;



void BasicScene::Init(float fov, int width, int height, float near, float far) {

    camera = Camera::Create("camera", fov, float(width) / height, near, far);
    initProperties(width, height);
    init_objects();
    init_helpers();
    setFonts();
    setStartPos();
    setImage();

}

BasicScene::BasicScene(std::string name, Display* display) : SceneWithImGui(std::move(name), display)
{
    ImGui::GetIO().IniFilename = nullptr;
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 5.0f;
}

void BasicScene::initProperties( int width, int height){
    windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
                  ImGuiWindowFlags_NoResize |ImGuiWindowFlags_NoBringToFrontOnFocus  |
                  ImGuiWindowFlags_NoScrollbar;

    screen_height= height;
    screen_width = width;
    statistics = GameStatistics::getInstance();
    data = Data::getInstance();
    windowSize = ImVec2(window_width, window_height);
}

void BasicScene::init_helpers(){

    this->highScores = new HighScores();
    this->statistics->menu_flags[MainMenu_OP] = true;
    this->soundManager = SoundManager::getInstance();
    soundManager->play_game_music();
    this->animate = false;

}

void BasicScene::setImage(){
    backgroundImage = stbi_load("../tutorial/Final/images/7x7.jpg", &image_width , &image_height, &channels, STBI_rgb_alpha);
    if (backgroundImage == NULL) {
        // Error handling if the image couldn't be loaded
        std::cout << "image not loaded! " << std::endl;
    }
    else {
        glGenTextures(1, &backgroundImageTexture);
        glBindTexture(GL_TEXTURE_2D, backgroundImageTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, backgroundImage);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        stbi_image_free(backgroundImage);
        std::cout << "image loaded successfully! " << std::endl;
    }
}

void BasicScene::setFonts() {
    ImGuiIO &io = ImGui::GetIO();
    ImFontConfig font_config;
    font_config.GlyphExtraSpacing = ImVec2(0.0f, 1.0f); // Set extra spacing for each glyph
    font_config.GlyphOffset = ImVec2(0.0f, -1.0f); // Set glyph offset to adjust for extra spacing
    backgroundImage = stbi_load("../tutorial/Final/images/image.png", &image_width, &image_height, &channels, STBI_rgb_alpha);
    headerFont = io.Fonts->AddFontFromFileTTF("../tutorial/Final/fonts/Broxford.otf", 18.0f, &font_config);
    messageFont = io.Fonts->AddFontFromFileTTF("../tutorial/Final/fonts/Amena.otf", 14.0f, &font_config);
    regularFont = io.Fonts->AddFontFromFileTTF("../tutorial/Final/fonts/Castron.otf", 14.0f, &font_config);
}

void BasicScene::setStartPos() {

    int height = (screen_height - window_height)/2.0f  ;
    int width = (screen_width - window_width)/2.0f ;
    startPos = ImVec2(width,height);
     height = (screen_height - image_height)/2.0f  ;
     width = (screen_width - image_width)/2.0f ;

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
    //PlayMenu();
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
                animate = !animate;
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
    background->Scale(1000, Axis::XYZ);
    background->SetPickable(false);
    background->SetStatic();
    auto program = std::make_shared<Program>("shaders/phongShader");
    material =  std::make_shared<Material>("material", program); // empty material
    material->AddTexture(0, "textures/box0.bmp", 2);
    snake = new Snake(material,root,camera);
    camera->Translate(50,Movable::Axis::Z);
    camera->Translate(5, Movable::Axis::Y);
}

void BasicScene::startMenu() {

    if(statistics->menu_flags[MainMenu_OP]) {
        setWindow("3D Snake");
        ImGui::PushFont(regularFont);
        buttonStyle();
        if (ImGui::Button("Start Game", ImVec2(-1, 0))) {
            std::cout << "new game button pressed in start menu ." << endl;
            statistics->menu_flags[MainMenu_OP] = false;
            animate = true;

        }
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }

        if (ImGui::Button("Tutorial", ImVec2(-1, 0))) {
            if (data->message_size() == 0)
                data->set_message(
                        "Press the up,down,right,left,\n Keys to move around.\nBe aware of obstacles.\n"
                        "Eat as much animals as you can.\nEach eat will gain you score points.\nGood Luck!");
            else
                data->set_message("");
        }
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }

        if (ImGui::Button("Store", ImVec2(-1, 0))) {
            std::cout << "store button pressed in start menu  ." << endl;

            //start menu check .
            //TODO: replace.
            Score* scor = generateRandomScore();
            int pos = highScores->nextLeaderPos();
            if( pos == -1) {
                std::vector<int> scores = this->highScores->extractScores();
                for (int i = 0; i < scores.size(); i++) {
                    if (scor->score > scores[i]) {
                        pos = i;
                    }
                }
            }
            if (pos != -1){
                highScores->saveToHighScores(scor,pos);
            }
//            statistics->menu_flags[MainMenu_OP] = false;
//            data->set_back(MainMenu_OP);
//            statistics->menu_flags[StoreMenu_OP] = true;
        }
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }

        if (ImGui::Button("Leaders", ImVec2(-1, 0))) {
            std::cout << "leaders button pressed in start menu  ." << endl;
            statistics->menu_flags[MainMenu_OP] = false;
            data->back_to_main.push_back(MainMenu_OP);
            statistics->menu_flags[LeadersMenu_OP] = true;
        }
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }

        if (ImGui::Button("Settings", ImVec2(-1, 0))) {
            if (data->message_size() == 0)
            {
                statistics->menu_flags[MainMenu_OP] = false;
                data->back_to_main.push_back(MainMenu_OP);
                statistics->menu_flags[SettingsMenu_OP] = true;
            }
            else {
                data->set_message("");
            }
        }
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }

        if (ImGui::Button("Exit", ImVec2(-1, 0))) {
            delete soundManager;
            exit(0);
        }
        ImGui::PopStyleColor(3);
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }
        ImGui::PopFont();
        ImGui::PushFont(messageFont);
        ImGui::Text("%s", data->msg_c_str());

        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }
        ImGui::PopFont();
        endWindow();

    }
}
void BasicScene::PausedMenu()
{
    if(statistics->menu_flags[PauseMenu_OP]) {
        setWindow("Pause");
        ImGui::PushFont(regularFont);
        ImGui::ProgressBar(statistics->get_progress(), ImVec2(0.0f, 0.0f));
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text("Progress Bar");
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }

        ImGui::Text("Total score: ");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        std::string tmp = std::to_string(statistics->score);
        ImGui::Text("%s", tmp.c_str());
        ImGui::Spacing();
        ImGui::Text("Level: ");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        std::string tmp2 = std::to_string(statistics->level);
        ImGui::Text("%s", tmp2.c_str());
        ImGui::Text("Snake speed: ");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        std::string tmp3 = std::to_string(statistics->speed);
        ImGui::Text("%s", tmp3.c_str());
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }
        buttonStyle();
        if (ImGui::Button("Continue", ImVec2(-1, 0))) {
            std::cout << "continue button pressed in pause menu ." << endl;
            statistics->menu_flags[PauseMenu_OP] = false;
            animate = true;
        }
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }

        if (ImGui::Button("Store", ImVec2(-1, 0))) {
            std::cout << "store menu button pressed in pause menu." << endl;
            statistics->menu_flags[PauseMenu_OP] = false;
            data->back_to_main.push_back(PauseMenu_OP);
            statistics->menu_flags[StoreMenu_OP] = true;
        }
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }

        if (ImGui::Button("Settings", ImVec2(-1, 0))) {

            data->set_message("");
            std::cout << "setting menu button pressed in pause menu." << endl;
            statistics->menu_flags[PauseMenu_OP] = false;
            data->back_to_main.push_back(PauseMenu_OP);
            statistics->menu_flags[SettingsMenu_OP] = true;


        }
        if (ImGui::Button("Surrender", ImVec2(-1, 0))) {
            data->set_message("");
            std::cout << "Surrender button pressed in pause menu." << endl;
            statistics->menu_flags[PauseMenu_OP] = false;
            statistics->menu_flags[MainMenu_OP] = true;
            data->back_to_main.clear();
            //TODO:init statistic and game.
            statistics->reset_game();


        }
        ImGui::PopStyleColor(3);
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }

        ImGui::PopFont();
        endWindow();
    }
}
void BasicScene::SettingsMenu()
{
    if(statistics->menu_flags[SettingsMenu_OP]) {
        setWindow("Settings");
        ImGui::PushFont(regularFont);
        setBoxes();
        ImVec4 black = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // set color to black
        ImGui::PushStyleColor(ImGuiCol_CheckMark, black); // set checkbox check color
        // Checkbox for sound on/off
        if ( data->gameMusic){
            if (ImGui::Checkbox("Music On", &data->gameMusic)) {
                data->gameMusic = false;
                soundManager->stop_game_music();
            }
        }
        else{
            if (ImGui::Checkbox("Music Off", &data->gameMusic)) {
                data->gameMusic = true;
                soundManager->play_game_music();
            }
        }
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }
        if (data->gameSound){
            if (ImGui::Checkbox("Sound On", &data->gameSound)) {
                data->gameSound = false;
                soundManager->stop_all_game_sounds();
            }
        }
        else{
            if (ImGui::Checkbox("Sound Off", &data->gameSound)) {
                data->gameSound = true;
                soundManager->restart_game_sounds();
            }
        }
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }
        // Slider for music volume control
        if(ImGui::SliderFloat("Music Volume", &data->musicVolume, 0.0f, 1.0f) ){
            std::cout<< "Music Volume value changed to : " << data->musicVolume <<endl;
            soundManager->set_game_play_music(std::to_string(data->musicVolume));
        }
        // Slider for sound volume control
        if(ImGui::SliderFloat("Game Sounds Volume", &data->soundVolume, 0.0f, 1.0f) ){
            std::cout<< "Sounds Volume value changed to : " << data->musicVolume <<endl;
            soundManager->set_sounds_volume(std::to_string(data->soundVolume));
        }
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }

        ImGui::PopStyleColor();
        static const char* music_names[] = { "Song 1", "Song 2", "Song 3"};
        static int current_music_index = 0;
        if (ImGui::BeginCombo("Music", music_names[current_music_index])) {
            for (int i = 0; i < IM_ARRAYSIZE(music_names); i++) {
                bool is_selected = (current_music_index == i);
                if (ImGui::Selectable(music_names[i], is_selected)) {
                    current_music_index = i;
                    soundManager->change_game_music(current_music_index+1);
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        buttonStyle();
        if (ImGui::Button("Fail sound", ImVec2(-1, 0))) {
            std::cout << "Fail sound button pressed in win menu." << endl;
            soundManager->play_sound(std::to_string(FAIL_SOUND));
        }
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }
        if (ImGui::Button("Health sound", ImVec2(-1, 0))) {
            std::cout << "Health sound button pressed in win menu." << endl;
            soundManager->play_sound(std::to_string(HEALTH_SOUND));
        }
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }
        if (ImGui::Button("Hit sound", ImVec2(-1, 0))) {
            std::cout << "Hit sound button pressed in win menu." << endl;
            soundManager->play_sound(std::to_string(HIT_SOUND));
        }
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }
        if (ImGui::Button("Progress sound", ImVec2(-1, 0))) {
            std::cout << "Progress sound button pressed in win menu." << endl;
            soundManager->play_sound(std::to_string(PROGRESS_SOUND));
        }
        for(int i = 0; i< 5 ; i++){
            ImGui::Spacing();
        }
        if (ImGui::Button("Back", ImVec2(-1, 0))) {
            std::cout << "Back button pressed in win menu." << endl;
            if(data->back_to_main.size() == 0 ){
                statistics->menu_flags[SettingsMenu_OP] =false;
                animate = true;
            }
            else{
                statistics->menu_flags[SettingsMenu_OP] =false;
                int ret = data->back_to_main.back();
                data->back_to_main.pop_back();
                statistics->menu_flags[ret] =true;
            }
        }
        ImGui::PopStyleColor(3);
        ImGui::PopFont();
        endWindow();
    }
}
void BasicScene::NextLevelMenu() {

    if (statistics->menu_flags[LevelMenu_OP]) {
        setWindow("Level Up");
        ImGui::PushFont(regularFont);
        std::string tmp = std::to_string(statistics->level);
        ImGui::Text("%s", tmp.c_str());
        ImGui::Spacing();
        ImGui::Text("Your total score until now is ");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        tmp = std::to_string(statistics->score);
        ImGui::Text("%s", tmp.c_str());
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }
        buttonStyle();
        if (ImGui::Button("Next level", ImVec2(-1, 0))) {
            //TODO: START NEXT LEVEL.
            std::cout << "Next level button pressed in next level menu." << endl;
        }
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }
        if (ImGui::Button("Back ", ImVec2(-1, 0))) {
            init();
            std::cout << "Back button pressed in next level menu." << endl;
            if(data->back_to_main.size() == 0 ){
                statistics->menu_flags[SettingsMenu_OP] =false;
                animate = true;
            }
            else{
                statistics->menu_flags[SettingsMenu_OP] =false;
                int ret = data->back_to_main.back();
                data->back_to_main.pop_back();
                statistics->menu_flags[ret] =true;
            }
        }
        ImGui::PopStyleColor(3);
        ImGui::PopFont();
        endWindow();
    }
}
void BasicScene::WinMenu() {
    if (statistics->menu_flags[WinMenu_OP]) {
        setWindow("You Won!");
        ImGui::PushFont(regularFont);
        ImGui::Text("Congratulations You finished the game!!");
        ImGui::Spacing();
        ImGui::Text("Your total score until now is ");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        std::string tmp = std::to_string(statistics->score);
        ImGui::Text("%s", tmp.c_str());
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }
        buttonStyle();
        int pos = highScores->nextLeaderPos();
        if( pos == -1) {
            std::vector<int> scores = this->highScores->extractScores();
            for (int i = 0; i < scores.size(); i++) {
                if (statistics->score > scores[i]) {
                    pos = i;
                }
            }
        }
        if(pos != -1){
            ImGui::Text("Please type your name to save in\nleader board: ");
            char buffer[1024] = "";  // buffer to store the input string
            ImGui::InputText("Input", buffer, sizeof(buffer));
            ImGui::Spacing();
            ImGui::Spacing();
            if (ImGui::Button("Save", ImVec2(-1, 0))) {
                Score* scor = new Score ();
                scor->name = buffer;
                scor->score = statistics->score;
                highScores->saveToHighScores(scor,pos);
            }

        }
        for(int i = 0 ; i< 3 ; i++){
            ImGui::Spacing();
        }
        if (ImGui::Button("Back ", ImVec2(-1, 0))) {
            std::cout << "Back button pressed in win menu." << endl;
            if(data->back_to_main.size() == 0 ){
                statistics->menu_flags[SettingsMenu_OP] =false;
                animate = true;
            }
            else{
                statistics->menu_flags[SettingsMenu_OP] =false;
                int ret = data->back_to_main.back();
                data->back_to_main.pop_back();
                statistics->menu_flags[ret] =true;
            }
        }
        ImGui::PopStyleColor(3);
        ImGui::PopFont();
        endWindow();
    }
}
void BasicScene::LoseMenu() {

    if (statistics->menu_flags[GameOverMenu_OP]) {
        setWindow("Game Over!");
        ImGui::PushFont(regularFont);
        ImGui::Text("ohhh you died...");
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }
        buttonStyle();
        int pos = highScores->nextLeaderPos();
        if( pos == -1) {
            std::vector<int> scores = this->highScores->extractScores();
            for (int i = 0; i < scores.size(); i++) {
                if (statistics->score > scores[i]) {
                    pos = i;
                }
            }
        }
        if(pos != -1) {
            ImGui::Text("Please type your name to save in\nleader board: ");
            char buffer[1024] = "";  // buffer to store the input string
            ImGui::InputText("Input", buffer, sizeof(buffer));
            ImGui::Spacing();
            ImGui::Spacing();
            if (ImGui::Button("Save", ImVec2(-1, 0))) {
                Score* scor = new Score ();
                scor->name = buffer;
                scor->score = statistics->score;
                highScores->saveToHighScores(scor,pos);
            }

            for(int i = 0 ; i< 3 ; i++){
                ImGui::Spacing();
            }
        }
        if (ImGui::Button("Back ", ImVec2(-1, 0))) {
            std::cout << "Back button pressed in loose menu." << endl;
            //highScores->saveToHighScores(characterName, game->getTotalMoney());
            if(data->back_to_main.size() == 0 ){
                statistics->menu_flags[SettingsMenu_OP] =false;
                animate = true;
            }
            else{
                statistics->menu_flags[SettingsMenu_OP] =false;
                int ret = data->back_to_main.back();
                data->back_to_main.pop_back();
                statistics->menu_flags[ret] =true;
            }
        }
        ImGui::PopStyleColor(3);
        ImGui::PopFont();
        endWindow();
    }
}
void BasicScene::StoreMenu() {

    if (statistics->menu_flags[StoreMenu_OP]) {
        setWindow("Store");
        ImGui::PushFont(regularFont);
        ImGui::Text("You have total money of ");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        std::string tmp = std::to_string(data->get_total_money());
        ImGui::Text("%s", tmp.c_str());
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }
        if (ImGui::Button("extra speed - 50 coins", ImVec2(-1, 0))) {
            statistics->speed += 1;

        }
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }
        buttonStyle();
        if (ImGui::Button("refill health - 30 coins", ImVec2(-1, 0))) {
            data->inc_life_bought();
            data->set_message(std::to_string(data->get_life_bought()));
        }

        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }
        if (ImGui::Button("Back", ImVec2(-1, 0))) {
            std::cout << "Back button pressed in store menu." << endl;
            statistics->menu_flags[StoreMenu_OP] = false;
            if(data->back_to_main.size() == 0 ){
                animate =true;
            }
            else{
                statistics->menu_flags[data->back_to_main.back()] = true;
                data->back_to_main.pop_back();
            }
        }
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }
        ImGui::PopFont();
        ImGui::PushFont(messageFont);
        ImGui::Text("%s", data->msg_c_str());
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }
        ImGui::PopStyleColor(3);
        ImGui::PopFont();
        endWindow();
    }
}
void BasicScene::LeadersMenu() {

    if (statistics->menu_flags[LeadersMenu_OP]) {
        setWindow("Leader Board");
        ImGui::PushFont(regularFont);
        this->highScores->loadHighScores();
        std::vector<std::string> names = highScores->extractPlayerNames();
        std::vector<int> scores = highScores->extractScores();
        if (names.size() == 0) {
            ImGui::BulletText("No Saved Scores!");
        } else {
            for (int i = 0; i <= names.size() - 1; i++) {
                std::string text = names[i] + "\t|\t" + std::to_string(scores[i]) + "\n";
                ImGui::BulletText("%s", text.c_str());
            }
        }
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }
        buttonStyle();
        if (ImGui::Button("Back", ImVec2(-1, 0))) {
            std::cout << "Back button pressed in store menu." << endl;
            statistics->menu_flags[LeadersMenu_OP] = false;
            if(data->back_to_main.size() == 0){
                animate =true;
            }
            else{
                statistics->menu_flags[data->back_to_main.back()] = true;
                data->back_to_main.pop_back();
            }
        }
        ImGui::PopStyleColor(3);
        for(int i = 0; i< 200 ; i++){
            ImGui::Spacing();
        }
        ImGui::PopFont();
        endWindow();
    }
}
void BasicScene::PlayMenu()
{
    if(animate) {
        int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
        bool* pOpen = nullptr;
        ImGui::Begin("Menu", pOpen, flags);
        ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetWindowSize(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::Text("Camera: ");
        //TODO: replace cameras.
//        for (int i = 0; i < camList.size(); i++) {
//            ImGui::SameLine(0);
//            bool selectedCamera = camList[i] == camera;
//            if (selectedCamera)
//                ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
//            if (ImGui::Button(std::to_string(i + 1).c_str()))
//                SetCamera(i);
//            if (selectedCamera)
//                ImGui::PopStyleColor();
//        }


        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }
        ImGui::End();
    }
}



void BasicScene::buttonStyle() {
    // Set button color to dark green
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.5f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.7f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.3f, 0.0f, 1.0f));

}
void BasicScene::setBoxes() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 0.5f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.0f, 0.5f, 0.0f, 1.0f);
}
void BasicScene::endWindow() {
    ImGui::PushFont(headerFont);
    ImVec2 cursor_pos = ImGui::GetCursorPos();
    ImGui::SetCursorPos(ImVec2(cursor_pos.x, cursor_pos.y + ImGui::GetTextLineHeightWithSpacing()+200));
    ImGui::BulletText("Snake Game By Yuval Hitter & Bar Damri. \n3D Animation Course\n");

    //ImGui::BulletText("Snake Game By Yuval Hitter & Bar Damri. \n3D Animation Course\n");
    ImGui::PopFont();
    ImGui::PopStyleColor();
    ImGui::End();
}
void BasicScene::setWindow(const char* header) {
    ImGui::SetNextWindowPos(startPos);
    ImGui::SetNextWindowSize(windowSize);
    ImGui::SetNextWindowSizeConstraints(ImVec2(window_width, -1.0f), ImVec2(window_height, -1.0f));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    ImGui::Begin("Snake Game - 3D Animation Course", nullptr, windowFlags);
    // Draw the texture as a rectangle in the window
    ImVec2 image_size = ImVec2(image_width, image_height);
    ImVec2 window_pos = ImGui::GetWindowPos();
    ImVec2 image_pos = window_pos + ImGui::GetCursorPos();
    ImGui::GetWindowDrawList()->AddImage(
            (void*)(intptr_t)backgroundImageTexture,
            image_pos,
            image_pos + image_size,
            ImVec2(0, 0),
            ImVec2(1, 1),
            ImColor(255, 255, 255, 255)
    );

    for(int i = 0; i< 3 ; i++){
        ImGui::Spacing();
    }
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Centered Text").x) / 2.0f);
    ImGui::PushFont(headerFont);
    ImGui::Text("%s", header);

    for (int i = 0; i < 3; ++i) {
        ImGui::Spacing();
    }
    ImGui::Separator();
    for (int i = 0; i < 3; ++i) {
        ImGui::Spacing();
    }
    ImGui::PopFont();
}
BasicScene::~BasicScene(){
    delete soundManager;
}
Score* BasicScene::generateRandomScore() {
    // Generate random score between 0 and 100
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> scoreDist(0, 10000);
    int score = scoreDist(gen);

    // Generate random player name
    std::uniform_int_distribution<> nameDist(0, 25);
    std::string playerName;
    for (int i = 0; i < 8; ++i) {
        playerName += static_cast<char>('a' + nameDist(gen));
    }
    Score * ret = new Score ();
    ret->name = playerName;
    ret->score = score;
    return ret;
}