#pragma once
#include "igl/opengl/glfw/Display.h"
#include "igl/opengl/glfw/Renderer.h"
#include "sandBox.h"
#include "Game.h"
#include <igl/PI.h>
#include "igl/look_at.h"
//#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
//#include <igl/opengl/glfw/imgui/ImGuiHelpers.h>
//#include <../imgui/imgui.h>
  
#ifndef cameraOfCube
Camera* cameraOfCube;
#endif

static void glfw_mouse_press(GLFWwindow* window, int button, int action, int modifier)
{
 
	Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
	Game* scn = (Game*)rndr->GetScene();

	if (action == GLFW_PRESS&&scn->getGameState()==GameState::PLAY) {
		double x2, y2;
		glfwGetCursorPos(window, &x2, &y2);
		 
		bool found = false;
		double depth, closestZ = 1;
		int i = 0, savedIndx = scn->selected_data_index, lastIndx = scn->selected_data_index;

		for (; i < scn->data_list.size(); i++)
		{
			scn->selected_data_index = i;
			depth = rndr->Picking(x2, y2);
			if (depth < 0 && (closestZ > 0 || closestZ < depth))
			{
				savedIndx = i;
				closestZ = depth;
				found = true;
				std::cout << "id " << i << std::endl;
				std::cout << "found " << depth << std::endl;
			}
		}
		if (found) {
			scn->rotateScene = false;
			scn->selected_data_index = savedIndx;
			scn->data().set_colors(Eigen::RowVector3d(0.9, 0.1, 0.1));
			if (lastIndx != savedIndx)
				scn->data_list[lastIndx].set_colors(Eigen::RowVector3d(255.0 / 255.0, 228.0 / 255.0, 58.0 / 255.0));
			rndr->UpdatePosition(x2, y2);
		}
		else {
			scn->selected_data_index = savedIndx;
			scn->rotateScene = true;
			scn->data_list[lastIndx].set_colors(Eigen::RowVector3d(255.0 / 255.0, 228.0 / 255.0, 58.0 / 255.0));

		}
	}
	else {
		rndr->GetScene()->isPicked = false;

	}

}


//static void glfw_char_mods_callback(GLFWwindow* window, unsigned int codepoint, int modifier)
//{
//  __viewer->key_pressed(codepoint, modifier);
//}

float lastX = -1;
float lastY = -1;
bool  firstMouse = true;

void moveCameraProper(GLFWwindow* window, double xposIn, double yposIn) {

	if (lastX == -1 || lastY == -1) {
		int width;
		int height;
		glfwGetWindowSize(window, &width, &height);
		lastX = width / 2;
		lastY = height / 2;
	}
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	cameraOfCube->ProcessMouseMovement(xoffset, yoffset);
}
void glfw_mouse_move(GLFWwindow* window, double x, double y)
{
	
	Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
	rndr->UpdatePosition(x, y);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
			moveCameraProper(window, x, y);
		rndr->MouseProcessing(GLFW_MOUSE_BUTTON_RIGHT);
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		rndr->MouseProcessing(GLFW_MOUSE_BUTTON_LEFT);
	}
}

static void glfw_mouse_scroll(GLFWwindow* window, double x, double y)
{
	cameraOfCube->ProcessMouseScroll(y);
	Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
	Game* scn = (Game*)rndr->GetScene();
	if (scn->getGameState() == GameState::PLAY) {
		if (rndr->IsPicked()) {

			int id = scn->selected_data_index;
			if (scn->parents[id] != -1)
				id = scn->firstLinkId;
			scn->data_list[id].TranslateInSystem(scn->GetRotation(), Eigen::Vector3d(0, 0, 0.1 * y));

		}
		else
			rndr->GetScene()->MyTranslate(Eigen::Vector3d(0, 0, -y * 0.03), true);
	}
}

void glfw_window_size(GLFWwindow* window, int width, int height)
{
	Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
	
	//igl::opengl::glfw::Viewer* scn = rndr->GetScene();
	 
	rndr->post_resize(window, width, height); 
}


//static void glfw_drop_callback(GLFWwindow *window,int count,const char **filenames)
//{
//
//}

//static void glfw_error_callback(int error, const char* description)
//{
//	fputs(description, stderr);
//}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int modifier)
{
	Renderer* rndr = (Renderer*) glfwGetWindowUserPointer(window);
	Game* scn = (Game*)rndr->GetScene();
	//Eigen::Vector3d O = (scn->data_list[scn->firstLinkId].MakeTransd() * Eigen::Vector4d(0, 0, 0, 1)).head(3);
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	else if(scn->getGameState()==GameState::PLAY&&(action == GLFW_PRESS || action == GLFW_REPEAT))
		switch (key)
		{
		case 'A':
		case 'a':
		{
			rndr->core().is_animating = !rndr->core().is_animating;
			//scn->data(0).MyRotate(Eigen::Vector3d(1, 0, 0), -0.05);
			break;
		}
		case 'B':
		case 'b':
		{
			scn->ikSolverConstrainDegree = !scn->ikSolverConstrainDegree;
			if (scn->ikSolverConstrainDegree)
				std::cout << "Constrain Degree is activated" << std::endl;
			else 	std::cout << "Constrain Degree is deactivated" << std::endl;
			break;
		}
		case 'F':
		case 'f':
		{
			scn->data().set_face_based(!scn->data().face_based);
			break;
		}
		case 'I':
		case 'i':
		{
			scn->data().dirty |= igl::opengl::MeshGL::DIRTY_NORMAL;
			scn->data().invert_normals = !scn->data().invert_normals;
			break;
		}
		case 'L':
		case 'l':
		{
			rndr->core().toggle(scn->data().show_lines);
			break;
		}
		case 'O':
		case 'o':
		{
			rndr->core().orthographic = !rndr->core().orthographic;
			break;
		}
		case 'T':
		case 't':
		{
			if (scn->lastLinkId >= 0) { 
				Eigen::Vector3d tip_pos =(scn->data_list[0].MakeTransd() *
					Eigen::Vector4d(scn->data_list[0].V.colwise().mean()[0],
						scn->data_list[0].V.colwise().mean()[1], scn->data_list[0].V.colwise().maxCoeff()[2], 1)).head(3);
				std::cout << "tip position: (" << tip_pos.transpose() << ")" << std::endl;

			}
			else {
				std::cout << "no links" << std::endl;
			}
			break;
		}
		case 'P':
		case 'p':
		{
			Eigen::Matrix3d mat = scn->selected_data_index == -1 ?
				scn->MakeTransd().block(0, 0, 3, 3) :
				scn->data().MakeTransd().block(0, 0, 3, 3);
			std::cout << "the rotation matrix for  " << scn->selected_data_index << ": " << std::endl;
			std::cout << mat << std::endl;
			break;
		
		}
		case 'D':
		case 'd':
		{
			Eigen::Vector3d target_des = scn->data_list[scn->targetId].MakeTransd().col(3).head(3);
			std::cout << "destination: (" << target_des.transpose() << ")" << std::endl;
			//scn->data(0).MyRotate(Eigen::Vector3d(0, 1, 0), 0.05);
			break;

		}
		case '[':
		case ']':
		{
			//rndr->ChangeCamera(key);
			scn->shouldSwitchCamera = !scn->shouldSwitchCamera;
			break;

		}
		case ';':
			scn->data().show_vertid = !scn->data().show_vertid;
			break;
		case ':':
			scn->data().show_faceid = !scn->data().show_faceid;
			break;
		case 'w':
		case 'W':
			//rndr->TranslateCamera(Eigen::Vector3f(0, 0, 0.03f));
			scn->shouldMoveSanke = !scn->shouldMoveSanke;
			scn->direction = Eigen::Vector3d(0, 0, scn->getSpeed());
			break;
		case 's':
		case 'S':
			//rndr->TranslateCamera(Eigen::Vector3f(0, 0, -0.03f));
			scn->shouldMoveSanke = !scn->shouldMoveSanke;
			scn->direction = Eigen::Vector3d(0, 0, -1* scn->getSpeed());
			break;
		case GLFW_KEY_UP:
			scn->shouldMoveSanke = !scn->shouldMoveSanke;
			scn->direction = Eigen::Vector3d(0, scn->getSpeed(), 0);
			break;
		case GLFW_KEY_DOWN:
			scn->shouldMoveSanke = !scn->shouldMoveSanke;
			scn->direction = Eigen::Vector3d(0, -1*scn->getSpeed(), 0);
			break;
		case GLFW_KEY_LEFT:
			scn->shouldMoveSanke = !scn->shouldMoveSanke;
			scn->direction = Eigen::Vector3d(-1* scn->getSpeed(), 0, 0);
			break;
		case GLFW_KEY_RIGHT:
			scn->shouldMoveSanke = !scn->shouldMoveSanke;
			scn->direction = Eigen::Vector3d(scn->getSpeed(), 0, 0);
			break;
		case ' ':
			//scn->getSnake()->FabrikFoward();
			//scn->shouldAnimateFabrik = !scn->shouldAnimateFabrik;
			//scn->shouldAnimateCCD = !scn->shouldAnimateCCD;
			//scn->simplification();
		
			break;	
		default: 
			Eigen::Vector3f shift;
			float scale;
			rndr->core().get_scale_and_shift_to_fit_mesh(scn->data().V, scn->data().F, scale, shift);
			
			std::cout << "near " << rndr->core().camera_dnear << std::endl;
			std::cout << "far " << rndr->core().camera_dfar << std::endl;
			std::cout << "angle " << rndr->core().camera_view_angle << std::endl;
			std::cout << "base_zoom " << rndr->core().camera_base_zoom << std::endl;
			std::cout << "zoom " << rndr->core().camera_zoom << std::endl;
			std::cout << "shift " << shift << std::endl;
			std::cout << "translate " << rndr->core().camera_translation << std::endl;

			break;//do nothing
		}
}


void Init(Display& display, igl::opengl::glfw::imgui::ImGuiMenu *menu)
{ 
	cameraOfCube = display.getCamera(); 
	display.AddKeyCallBack(glfw_key_callback);
	display.AddMouseCallBacks(glfw_mouse_press, glfw_mouse_scroll, glfw_mouse_move);
	display.AddResizeCallBack(glfw_window_size);
	menu->init(&display);
}



