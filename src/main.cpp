//============================================================================
// Name        : tetcutter.cpp
// Author      : Pourya Shirazian
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "base/FileDirectory.h"
#include "base/Logger.h"
#include "graphics/SceneGraph.h"
#include "graphics/Gizmo.h"
#include "graphics/AppScreen.h"
#include "graphics/selectgl.h"
#include "graphics/SGBox.h"

using namespace PS;
using namespace PS::SG;
using namespace PS::FILESTRINGUTILS;

using namespace std;

void draw() {
	TheSceneGraph::Instance().draw();
	TheGizmoManager::Instance().draw();

	glutSwapBuffers();

}

void timestep() {
	TheSceneGraph::Instance().timestep();
}

void MousePress(int button, int state, int x, int y)
{
    TheGizmoManager::Instance().mousePress(button, state, x, y);
    TheSceneGraph::Instance().mousePress(button, state, x, y);

    // Wheel reports as button 3(scroll up) and button 4(scroll down)
	if ((button == 3) || (button == 4)) {
		// Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
		if (state == GLUT_UP)
			return; // Disregard redundant GLUT_UP events

		TheSceneGraph::Instance().mouseWheel(button, (button==3)? 1 : -1, x, y);
	}

    //Update selection
	glutPostRedisplay();
}


void MousePassiveMove(int x, int y)
{
}

void MouseMove(int x, int y)
{
	TheGizmoManager::Instance().mouseMove(x, y);
	TheSceneGraph::Instance().mouseMove(x, y);

	glutPostRedisplay();
}

void MouseWheel(int button, int dir, int x, int y)
{
	TheSceneGraph::Instance().mouseWheel(button, dir, x, y);
	glutPostRedisplay();
}

void NormalKey(unsigned char key, int x, int y)
{
	switch(key)
	{
	case('g'):{
		TheGizmoManager::Instance().setType(gtTranslate);
	}
	break;

	case('s'):{
		TheGizmoManager::Instance().setType(gtScale);
		break;
	}

	case('r'):{
		TheGizmoManager::Instance().setType(gtRotate);
		break;
	}

	case('x'):{
		TheGizmoManager::Instance().setAxis(axisX);
	}
	break;
	case('y'):{
		TheGizmoManager::Instance().setAxis(axisY);
	}
	break;
	case('z'):{
		TheGizmoManager::Instance().setAxis(axisZ);
	}
	break;

	case('p'): {
		TheSceneGraph::Instance().print();
	}
	break;

	case('['):{
		TheSceneGraph::Instance().camera().incrZoom(0.5f);
	}
	break;
	case(']'):{
		TheSceneGraph::Instance().camera().incrZoom(-0.5f);
	}
	break;

	case(27):
	{
		//Saving Settings and Exit
		LogInfo("Saving settings and exit.");
		exit(1);
		//glutLeaveMainLoop();
	}
	break;


	}


	//Update Screen
	glutPostRedisplay();
}

void SpecialKey(int key, int x, int y)
{
	switch(key)
	{
		case(GLUT_KEY_F4):
		{
			//Set UIAxis
			int axis = (int)TheGizmoManager::Instance().axis();
			axis = (axis + 1) % axisCount;
			TheGizmoManager::Instance().setAxis((GizmoAxis)axis);
			LogInfoArg1("Change haptic axis to %d", TheGizmoManager::Instance().axis());
			break;
		}

		case(GLUT_KEY_F5):
		{
			GizmoAxis axis = TheGizmoManager::Instance().axis();
			vec3f inc(0,0,0);
			if(axis < axisFree)
				inc.setElement(axis, -0.1);
			else
				inc = vec3f(-0.1, -0.1, -0.1);
			inc = inc * 0.5;
			TheGizmoManager::Instance().transform()->scale(inc);
			break;
		}


		case(GLUT_KEY_F6):
		{
			GizmoAxis axis = TheGizmoManager::Instance().axis();
			vec3f inc(0,0,0);
			if(axis < axisFree)
				inc.setElement(axis, 0.1);
			else
				inc = vec3f(0.1, 0.1, 0.1);
			inc = inc * 0.5;
			TheGizmoManager::Instance().transform()->scale(inc);
			break;
		}

	}

	//Modifier
	TheSceneGraph::Instance().setModifier(glutGetModifiers());

	glutPostRedisplay();
}


void closeApp() {
	LogInfo("closing app");
}


int main(int argc, char* argv[]) {
    cout << "OpenGL Framework - Pourya Shirazian" << endl;

	//Initialize app
	glutInit(&argc, argv);
#ifdef PS_OS_MAC
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
#else
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
#endif
	glutInitWindowSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
	glutCreateWindow("OpenGL Framework");
	glutDisplayFunc(draw);
	glutReshapeFunc(def_resize);
	glutMouseFunc(MousePress);
	glutPassiveMotionFunc(MousePassiveMove);
	glutMotionFunc(MouseMove);
	//glutMouseWheelFunc(MouseWheel);
	glutKeyboardFunc(NormalKey);
	glutSpecialFunc(SpecialKey);
#ifdef PS_OS_MAC
	glutWMCloseFunc(closeApp);
#else
	glutCloseFunc(closeApp);
#endif	
	glutIdleFunc(timestep);

	//init gl
	def_initgl();

	//Build Shaders for drawing the mesh
	AnsiStr strRoot = ExtractOneLevelUp(ExtractFilePath(GetExePath()));
	AnsiStr strShaderRoot = strRoot + "data/shaders/";

	//Load Shaders
    LogInfoArg1("Look for shaders in: %s", strShaderRoot.cptr());
    int count = TheShaderManager::Instance().addFromFolder(strShaderRoot.cptr());
    if(count == 0) {
        LogError("Unable to find any shaders in that path");
    }

	//Ground and Room
	TheSceneGraph::Instance().addFloor(32, 32, 0.5f);
	TheSceneGraph::Instance().addSceneBox(AABB(vec3f(-10, -10, -16), vec3f(10, 10, 16)));

	//box
	Geometry g1;
	g1.addCube(vec3f(0.0, 0.0, 0.0), 1.0);
	g1.colors().clear();

	//color
	float r = RandRangeT<float>(0.0, 1.0);
	float g = RandRangeT<float>(0.0, 1.0);
	float b = RandRangeT<float>(0.0, 1.0);
	g1.addPerVertexColor(vec4f(r, g, b, 1.0f));
	SGMesh* acube = new SGMesh(g1);
	if(TheShaderManager::Instance().has("phong"))
		acube->setEffect(SmartPtrSGEffect(new SGEffect(TheShaderManager::Instance().get("phong"))));

	acube->transform()->setTranslate(vec3f(0, 1, 0));


	TheSceneGraph::Instance().add(acube);
	TheGizmoManager::Instance().setFocusedNode(acube);




	glutMainLoop();

	return 0;
}
