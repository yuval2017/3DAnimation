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
#include "ObjLoader.h"

using namespace cg3d;



void BasicScene::Init(float fov, int width, int height, float near, float far) {

    initProperties(width, height);
    this->statistics->menu_flags[LoadingMenu_OP] = true;
    done = 0;
    init_objects();
    init_cameras(fov,width,height,near,far);
    init_helpers();
    setFonts();
    setStartPos();
    setImage();

}


void BasicScene::init_cameras(float fov, int width, int height,float near, float far) {

    //Set camera list
    cameras.resize(cameras.capacity());
    cameras[0] = Camera::Create("global view", fov, float(width) / height, near, far);
    cameras[1] = Camera::Create("snake front view", fov, float(width) / height, near, far);
    cameras[2] = Camera::Create("snake back view", fov, float(width) / height, near, far);
    cameras[3] = Camera::Create("snake back view", fov, float(width) / height, near, far);
    number_of_cameras = int(cameras.size());


    // x axis is for move step to left and right,
    // y axis moves down and up, z axis is for front and back.


    // Set front
    snake->GetSnakeBones()[snake->GetSnakeBones().size()-1]->AddChild(cameras[0]);
    cameras[0]->Translate(Eigen::Vector3f(1.f, 1.f, -6.f));
    cameras[0]->Rotate(M_PI, Movable::Axis::Y);
    cameras[0]->Rotate(-M_PI/18, Movable::Axis::X);


    // Set mid snake view
    snake->GetSnakeBones()[snake->GetSnakeBones().size()-1]->AddChild(cameras[1]);
    cameras[1]->Translate(Eigen::Vector3f(-3.f, 2.f, -15.f));
    cameras[1]->Rotate(M_PI, Movable::Axis::Y);
   // cameras[1]->Rotate(M_PI/8, Movable::Axis::X);

    //Set over-view
    snake->GetSnakeBones()[snake->GetSnakeBones().size()-1]->AddChild(cameras[2]);
    cameras[2]->Translate(Eigen::Vector3f(-5.f, 10.f, -20.f));
    cameras[2]->Rotate(M_PI, Movable::Axis::Y);
    cameras[2]->RotateByDegree(-15.f, Movable::Axis::X);

    //Set over-view
    snake->GetSnakeBones()[snake->GetSnakeBones().size()-1]->AddChild(cameras[3]);
    cameras[3]->Translate(Eigen::Vector3f(-5.f, 10.f, -50.f));
    cameras[3]->Rotate(M_PI, Movable::Axis::Y);
    cameras[3]->RotateByDegree(-15.f, Movable::Axis::X);

    camera = cameras[0];

}

void BasicScene::resetCameras(){
    //set the cameras on the new snake after level up.
    snake->GetSnakeBones()[snake->GetSnakeBones().size()-1]->AddChild(cameras[0]);

    snake->GetSnakeBones()[snake->GetSnakeBones().size()-1]->AddChild(cameras[1]);

    snake->GetSnakeBones()[snake->GetSnakeBones().size()-1]->AddChild(cameras[2]);

    snake->GetSnakeBones()[snake->GetSnakeBones().size()-1]->AddChild(cameras[3]);
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
    loadingMenu();
    startMenu();
    PausedMenu();
    NextLevelMenu();
    WinMenu();
    LoseMenu();
    StoreMenu();
    LeadersMenu();
    SettingsMenu();
    PlayMenu();
}

void BasicScene::Update(const Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    Scene::Update(program, proj, view, model);
    char * green = "green";
    char* grey = "grey";
    char* gold = "gold";
    int resgreen = (program.name).compare(green);
    int resgrey = (program.name).compare(grey);
    int resgold = (program.name).compare(gold);
    if( resgreen == 0 ){
        program.SetUniform4f("lightColor", 0.8f, 0.9f, 0.6f, 1.0f);
        program.SetUniform4f("Kai", 0.2f, 0.4f, 0.1f, 1.0f);
        program.SetUniform4f("Kdi", 0.2f, 0.4f, 0.1f, 1.0f);
        program.SetUniform1f("specular_exponent", 10.0f);
        program.SetUniform4f("light_position", 0.0, 15.0f, 0.0, 1.0f);
    }
    else if(resgrey == 0){
        program.SetUniform4f("lightColor", 0.7f, 0.7f, 0.7f, 1.0f);
        program.SetUniform4f("Kai", 0.3f, 0.3f, 0.3f, 1.0f);
        program.SetUniform4f("Kdi", 0.3f, 0.3f, 0.3f, 1.0f);
        program.SetUniform1f("specular_exponent", 20.0f);
        program.SetUniform4f("light_position", 0.0, 15.0f, 0.0, 1.0f);
    }
    else if(resgold == 0){
        program.SetUniform4f("lightColor", 1.0f, 0.8f, 0.1f, 1.0f);
        program.SetUniform4f("Kai", 0.8f, 0.6f, 0.1f, 1.0f);
        program.SetUniform4f("Kdi", 0.8f, 0.6f, 0.1f, 1.0f );
        program.SetUniform1f("specular_exponent", 50.0f);
        program.SetUniform4f("light_position", 0.0, 15.0f, 0.0, 1.0f);
    }
    else{
        program.SetUniform4f("lightColor", 0.8f, 0.3f, 0.0f, 0.5f);
        program.SetUniform4f("Kai", 1.0f, 0.3f, 0.6f, 1.0f);
        program.SetUniform4f("Kdi", 0.5f, 0.5f, 0.0f, 1.0f);
        program.SetUniform1f("specular_exponent", 5.0f);
        program.SetUniform4f("light_position", 0.0, 15.0f, 0.0, 1.0f);
    }


}

void BasicScene::MouseCallback(Viewport* viewport, int x, int y, int button, int action, int mods, int buttonState[])
{
//    // note: there's a (small) chance the button state here precedes the mouse press/release event
//
//    if (action == GLFW_PRESS && animate) { // default mouse button press behavior
//        PickVisitor visitor;
//        visitor.Init();
//        renderer->RenderViewportAtPos(x, y, &visitor); // pick using fixed colors hack
//        auto modelAndDepth = visitor.PickAtPos(x, renderer->GetWindowHeight() - y);
//        renderer->RenderViewportAtPos(x, y); // draw again to avoid flickering
//        pickedModel = modelAndDepth.first ? std::dynamic_pointer_cast<Model>(modelAndDepth.first->shared_from_this()) : nullptr;
//        pickedModelDepth = modelAndDepth.second;
//        camera->GetRotation().transpose();
//        xAtPress = x;
//        yAtPress = y;
        // if (pickedModel)
        //     debug("found ", pickedModel->isPickable ? "pickable" : "non-pickable", " model at pos ", x, ", ", y, ": ",
        //           pickedModel->name, ", depth: ", pickedModelDepth);
        // else
        //     debug("found nothing at pos ", x, ", ", y);

//        if (pickedModel && !pickedModel->isPickable)
//            pickedModel = nullptr; // for non-pickable models we need only pickedModelDepth for mouse movement calculations later
//
//        if (pickedModel)
//            pickedToutAtPress = pickedModel->GetTout();
//        else
//            cameraToutAtPress = camera->GetTout();
//    }
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
            case GLFW_KEY_R:
                animate = false;
                snake->reset_sake();
                animate = true;
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
            case GLFW_KEY_C:
                statistics->inc_Score(500);
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
    level1 = ModelsFactory::getInstance()->CreateModel(DAYLIGHT_MATERIAL,CUBE,"background");
    AddChild(level1);
    level1->Scale(200, Axis::XYZ);
    level1->SetPickable(false);
    level1->SetStatic();
    auto program = std::make_shared<Program>("shaders/phongShader");
    material =  std::make_shared<Material>("material", program); // empty material
    material->AddTexture(0, "textures/box0.bmp", 2);
    snake = new Snake(material,root,camera);
    init_flags[0]= true;
    done++;

}
void BasicScene::init_helpers(){

    this->highScores = new HighScores();
    this->soundManager = SoundManager::getInstance();
    //soundManager->play_game_music();
    this->animate = false;
    init_flags[2]= true;
    done++;

}

void BasicScene::loadingMenu() {

    if(start_time == 0.0 ) {
        start_time = ImGui::GetTime();
    }
    if(statistics->menu_flags[LoadingMenu_OP]){
        animate = false;
        setWindow("Loading");
        ImGui::PushFont(regularFont);
        // Get the current time in seconds

        // Get the current time in seconds
        float current_time = ImGui::GetTime();

        // Calculate the progress as a value between 0 and 1
        float progress =  (done == 3) ? std::min((current_time - start_time) / 30.f, 1.0f) : start_time;

        // Display the progress as a percentage
        ImGui::Text("Percentage of Time Passed: %.2f%%", progress * 100.0f);

        // Display the progress bar
        ImGui::ProgressBar(progress, ImVec2(-1, 0), "");
        // Call the callback function when progress reaches 100%
        if (progress >= 1.0f) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            statistics->menu_flags[LoadingMenu_OP] = false;
            statistics->menu_flags[MainMenu_OP] =true;
        }
        ImGui::PopFont();
        endWindow();
    }
}


void BasicScene::startMenu() {

    if(statistics->menu_flags[MainMenu_OP]) {
        animate = false;
        setWindow("3D Snake");
        ImGui::PushFont(regularFont);
        buttonStyle();
        if (ImGui::Button("Start Game", ImVec2(-1, 0))) {
            std::cout << "new game button pressed in start menu ." << endl;
            statistics->menu_flags[MainMenu_OP] = false;
            animate = true;
            statistics->objectCollisionStopper.start(15);
            statistics->selfCollisionStopper.start(30);
            if(data->double_score >0){
                data->dec_double_score();
                statistics->double_score=true;
            }
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
            statistics->menu_flags[MainMenu_OP] = false;
            data->back_to_main.push_back(MainMenu_OP);
            statistics->menu_flags[StoreMenu_OP] = true;

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
        animate = false;
        setWindow("Pause");
        ImGui::PushFont(regularFont);
        float progress = statistics->score;
        progress= progress/data->scores[statistics->level];
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
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
        ImGui::Text("Life remain: ");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        std::string tmp11 = std::to_string(data->life_bought);
        ImGui::Text("%s", tmp11.c_str());
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
            data->add_total_money(statistics->score/10);
            soundManager->play_sound(std::to_string(BOO_SOUND));
            statistics->restart =true;
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
        animate = false;
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
        animate = false;
        if(start_time == 0.0 ) {
            start_time = ImGui::GetTime();
        }
        setWindow("Level Up");
        ImGui::PushFont(regularFont);
        std::string tmp = std::to_string(statistics->level);
        ImGui::Text(" Current level: %s", tmp.c_str());
        ImGui::Spacing();
        ImGui::Text("Your total score until now is ");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        tmp = std::to_string(statistics->score);
        ImGui::Text("%s", tmp.c_str());
        for (int i = 0; i < 3; i++) {
            ImGui::Spacing();
        }

        buttonStyle();
        // Get the current time in seconds
        float current_time = ImGui::GetTime();

        // Calculate the progress as a value between 0 and 1
        float progress =  (done == 3) ? std::min((current_time - start_time) / 10.f, 1.0f) : start_time;

        // Display the progress as a percentage
        ImGui::Text("Percentage of Time Passed: %.2f%%", progress * 100.0f);

        // Display the progress bar
        ImGui::ProgressBar(progress, ImVec2(-1, 0), "");
        // Call the callback function when progress reaches 100%
        if (progress >= 1.0f) {
            if (ImGui::Button("Next level", ImVec2(-1, 0))) {
                std::cout << "Next level button pressed in next level menu." << endl;
                statistics->menu_flags[LevelMenu_OP] = false;
                statistics->level++;
                statistics->objectCollisionStopper.reset();
                statistics->selfCollisionStopper.reset();
                statistics->objectCollisionStopper.start(5);
                statistics->selfCollisionStopper.start(5);
                animate = true;
            }
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
        animate = false;
        if(start_time == 0.0 ) {
            start_time = ImGui::GetTime();
        }
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
       static  bool saved = false;
        if(pos != -1){
            ImGui::Text("Please type your name to save in\nleader board: ");
            static char name[256] = ""; // buffer to store the input string
            ImGui::InputText("Input", name,IM_ARRAYSIZE(name));
            ImGui::Spacing();
            ImGui::Spacing();
            if (ImGui::Button("Save", ImVec2(-1, 0)) && !saved) {
                Score *scor = new Score();
                scor->name = name;
                scor->score = statistics->score;
                highScores->saveToHighScores(scor, pos);
                data->set_message("your score was saved!");
                saved = true;

            }
            else if (saved){
                data->set_message("your score was already saved");
            }

        }

        for (int i = 0; i < 3; i++) {
            ImGui::Spacing();
        }
        // Get the current time in seconds
        float current_time = ImGui::GetTime();

        // Calculate the progress as a value between 0 and 1
        float progress =  (done == 3) ? std::min((current_time - start_time) / 30.f, 1.0f) : start_time;

        // Display the progress as a percentage
        ImGui::Text("Percentage of Time Passed: %.2f%%", progress * 100.0f);

        // Display the progress bar
        ImGui::ProgressBar(progress, ImVec2(-1, 0), "");
        // Call the callback function when progress reaches 100%
        if (progress >= 1.0f) {
            if (ImGui::Button("Back ", ImVec2(-1, 0))) {
                std::cout << "Back button pressed in win menu." << endl;
                data->back_to_main.clear();
                statistics->menu_flags[WinMenu_OP] =false;
                statistics->menu_flags[MainMenu_OP] =true;
                data->add_total_money(statistics->score/10);
                statistics->reset_game();

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
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
        endWindow();
    }
}


void BasicScene::LoseMenu() {

    if (statistics->menu_flags[GameOverMenu_OP]) {
        animate = false;
        if(start_time == 0.0 ) {
            start_time = ImGui::GetTime();
        }
        setWindow("Game Over!");
        ImGui::PushFont(regularFont);
        ImGui::Text("You lost..\nMaybe you will succeed next time.");
        for (int i = 0; i < 8; i++) {
            ImGui::Spacing();
        }
        ImGui::Text("Your total score until now is ");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        std::string tmp = std::to_string(statistics->score);
        ImGui::Text("%s", tmp.c_str());
        for (int i = 0; i < 3; i++) {
            ImGui::Spacing();
        }
        buttonStyle();
        int pos = highScores->nextLeaderPos();
        if (pos == -1) {
            std::vector<int> scores = this->highScores->extractScores();
            for (int i = 0; i < scores.size(); i++) {
                if (statistics->score > scores[i]) {
                    pos = i;
                }
            }
        }
        static bool saved = false;
        if(pos != -1){
        ImGui::Text("Please type your name to save in\nleader board: ");
        static char name[256] = ""; // buffer to store the input string
        ImGui::InputText("Input", name,IM_ARRAYSIZE(name));
        ImGui::Spacing();
        ImGui::Spacing();
        if (ImGui::Button("Save", ImVec2(-1, 0)) && !saved) {
            Score *scor = new Score();
            scor->name = name;
            scor->score = statistics->score;
            highScores->saveToHighScores(scor, pos);
            data->set_message("your score was saved!");
            saved = true;
        }
        else if (saved){
            data->set_message("your score was already saved");
        }

         }
        for (int i = 0; i < 3; i++) {
            ImGui::Spacing();
        }
        // Get the current time in seconds
        float current_time = ImGui::GetTime();

        // Calculate the progress as a value between 0 and 1
        float progress =  (done == 3) ? std::min((current_time - start_time) / 10.f, 1.0f) : start_time;

        // Display the progress as a percentage
        ImGui::Text("Percentage of Time Passed: %.2f%%", progress * 100.0f);

        // Display the progress bar
        ImGui::ProgressBar(progress, ImVec2(-1, 0), "");
        // Call the callback function when progress reaches 100%
        if (progress >= 1.0f) {
            if (ImGui::Button("Back ", ImVec2(-1, 0))) {
                std::cout << "Back button pressed in lose menu." << endl;
                data->back_to_main.clear();
                statistics->menu_flags[GameOverMenu_OP] = false;
                statistics->menu_flags[MainMenu_OP] = true;
                data->add_total_money(statistics->score/10);
                statistics->reset_game();

            }
        }

        for (int i = 0; i < 3; i++) {
            ImGui::Spacing();
        }
        ImGui::PopFont();
        ImGui::PushFont(messageFont);
        ImGui::Text("%s", data->msg_c_str());
        for (int i = 0; i < 3; i++) {
            ImGui::Spacing();
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
        endWindow();

    }
}
void BasicScene::StoreMenu() {

    if (statistics->menu_flags[StoreMenu_OP]) {
        animate = false;
        setWindow("Store");
        ImGui::PushFont(regularFont);
        buttonStyle();
        ImGui::Text("You have total money of ");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        std::string tmp = std::to_string(data->total_money);
        ImGui::Text("%s", tmp.c_str());
        for(int i = 0; i< 5 ; i++){
            ImGui::Spacing();
        }
        ImGui::Text("Total score: ");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        std::string tmp2 = std::to_string(statistics->score);
        ImGui::Text("%s", tmp2.c_str());
        ImGui::Spacing();
        ImGui::Text("Life remain: ");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        std::string tmp11 = std::to_string(data->life_bought);
        ImGui::Text("%s", tmp11.c_str());
        ImGui::Spacing();
        ImGui::Text("Level: ");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        std::string tmp22 = std::to_string(statistics->level);
        ImGui::Text("%s", tmp22.c_str());
        ImGui::Text("Snake speed: ");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        std::string tmp33 = std::to_string(statistics->speed);
        ImGui::Text("%s", tmp33.c_str());

        if (ImGui::Button("Buy speed - 50 coins", ImVec2(-1, 0)) ) {
            if(data->total_money >=SPEED_COST) {
                statistics->inc_speed();
                soundManager->play_sound(std::to_string(KACHING_SOUND));
            }
            else{
                data->set_message("You have not enough money for that.");
            }
        }
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }

        if (ImGui::Button("Buy extra life - 30 coins", ImVec2(-1, 0))) {
            if(data->total_money >=LIFE_COST) {
            data->inc_life_bought();
            soundManager->play_sound(std::to_string(KACHING_SOUND));
        }
        else{
            data->set_message("You have not enough money for that.");
        }
        }

        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }
        if (ImGui::Button("Buy self collision invisibility - 10 coins", ImVec2(-1, 0))) {
            if (data->total_money >= SELF_COLLIDE_COST) {
                data->inc_self_collision();
                soundManager->play_sound(std::to_string(KACHING_SOUND));
            } else {
                data->set_message("You have not enough money for that.");
            }
        }
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }

        if (ImGui::Button("Buy obstacles collision invisibility - 20 coins", ImVec2(-1, 0))) {
            if (data->total_money >= OBJECT_COLLIDE_COST) {
                data->inc_object_collision();
                soundManager->play_sound(std::to_string(KACHING_SOUND));
            } else {
                data->set_message("You have not enough money for that.");
            }
        }
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }
        if (ImGui::Button("Buy double score in game- 60 coins", ImVec2(-1, 0))) {
            if (data->total_money>= OBJECT_COLLIDE_COST) {
            data->inc_double_score();
            soundManager->play_sound(std::to_string(KACHING_SOUND));
            } else {
                data->set_message("You have not enough money for that.");
            }
        }
        for(int i = 0; i< 3 ; i++){
            ImGui::Spacing();
        }
        if (ImGui::Button("Back", ImVec2(-1, 0))) {
            std::cout << "Back button pressed in store menu." << endl;
            statistics->menu_flags[StoreMenu_OP] = false;
            if(data->back_to_main.size() == 0 ){
                statistics->menu_flags[MainMenu_OP] = true;
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
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
        endWindow();
    }
}
void BasicScene::LeadersMenu() {

    if (statistics->menu_flags[LeadersMenu_OP]) {
        animate = false;
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
        ImGui::Begin("Game Menu", pOpen, flags);
        ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetWindowSize(ImVec2(screen_width, 100), ImGuiCond_Always);
        ImGui::Text("Camera: ");
        for (int i = 0; i < cameras.size(); i++) {
            ImGui::SameLine(0);
            bool selectedCamera = cameras[i] == camera;
            if (selectedCamera)
                ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
            if (ImGui::Button(std::to_string(i + 1).c_str()))
                SetCamera(i);
            if (selectedCamera)
                ImGui::PopStyleColor();
        }
        ImGui::SameLine();
        if (ImGui::Button("Center"))
            camera->SetTout(Eigen::Affine3f::Identity());
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Text("Total score: ");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        std::string tmp = std::to_string(statistics->score);
        ImGui::Text("%s", tmp.c_str());
        ImGui::Spacing();
        ImGui::Text("Level: ");
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        std::string tmp2 = std::to_string(statistics->level);
        ImGui::Text("%s", tmp2.c_str());
        ImGui::Spacing();
        ImGui::End();
    }
}


void BasicScene::SetCamera(int index)
{
    camera = cameras[index];
    viewport->camera = camera;
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
    data->set_message("");
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

SoundManager* BasicScene::getSoundManager() {
    return soundManager;
}

GameStatistics* BasicScene::getStatistics(){
    return this->statistics;
}
Data* BasicScene::getData(){
    return this->data;
}

void BasicScene::ViewportSizeCallback(Viewport* _viewport)
{
    for (auto& cam: cameras)
        cam->SetProjection(float(_viewport->width) / float(_viewport->height));

    // note: we don't need to call Scene::ViewportSizeCallback since we are setting the projection of all the cameras
}

void BasicScene::AddViewportCallback(Viewport* _viewport)
{
    viewport = _viewport;

    Scene::AddViewportCallback(viewport);
}
