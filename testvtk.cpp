#include "stdafx.h"

#include "vtkAutoInit.h" 
VTK_MODULE_INIT(vtkRenderingOpenGL2); // VTK was built with vtkRenderingOpenGL2
VTK_MODULE_INIT(vtkInteractionStyle);
#define vtkRenderingCore_AUTOINIT 2(vtkRenderingOpenGL2, vtkInteractionStyle)

#include <vtkCylinderSource.h>
#include <vtkCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCubeSource.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkCoordinate.h>
#include <vtkCellPicker.h>
#include <vtkAbstractPicker.h>

vtkSmartPointer<vtkActor> sprayCanActor;
vtkActor *cubeActor;
vtkActor *cubeTestActor;
vtkActor *floorActor;
vtkRenderer *ren1;
vtkRenderWindowInteractor *iren;
vtkRenderWindow *renWin;

float xS=0, yS=-2, zS=10;

// Mouse Interactor
class customMouseInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
	static customMouseInteractorStyle* New();
	vtkTypeMacro(customMouseInteractorStyle, vtkInteractorStyleTrackballCamera);
	
	int arfad = 0;
	virtual void OnLeftButtonDown()
	{
		float mouseX = this->Interactor->GetEventPosition()[0], mouseY = this->Interactor->GetEventPosition()[1];
		vtkSmartPointer<vtkCellPicker> pickerActor = vtkSmartPointer<vtkCellPicker>::New();
		pickerActor->SetTolerance(0.0005);
		
		// Pick from this location.		
		pickerActor->Pick(mouseX, mouseY, zS, ren1);
	
		
		double picked[3];
		pickerActor->GetPickPosition(picked);

		if (pickerActor->GetActor() == sprayCanActor) {
			sprayCanActor->SetPosition(picked[0]-1.8, picked[1]-0.5, zS);
			arfad = 1;
		}

		std::cout << "Picked value: " << picked[0] << " " << picked[1] << " " << picked[2] << pickerActor->GetCellId() <<"\n";

		
		// Forward events
		vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
		renWin->Render();
	}

	virtual void OnMouseMove()
	{
		
		if (arfad == 1) {
			sprayCanActor->SetPosition(0 - 1.8, 1 - 0.5, zS);
		}
		// Forward events
		vtkInteractorStyleTrackballCamera::OnMiddleButtonDown();
		renWin->Render();
	}

	

	virtual void OnMiddleButtonDown()
	{
		std::cout << "Pressed middle mouse button." << std::endl;
		// Forward events
		vtkInteractorStyleTrackballCamera::OnMiddleButtonDown();
		renWin->Render();
	}

	virtual void OnRightButtonDown()
	{
		std::cout << "Pressed right mouse button." << std::endl;
		// Forward events
		vtkInteractorStyleTrackballCamera::OnRightButtonDown();
		renWin->Render();
	}

	virtual void OnKeyPress()
	{
		// Get the keypress
		vtkRenderWindowInteractor *rwi = this->Interactor;
		std::string key = rwi->GetKeySym();

		// Output the key that was pressed
		std::cout << "Pressed " << key << std::endl;

		// Handle an arrow key
		if (key == "Up")
		{
			yS += 0.1;
			sprayCanActor->SetPosition(xS, yS, zS);

			std::cout << "The up arrow was pressed." << std::endl;
		}

		// Handle a "normal" key
		if (key == "a")
		{
			std::cout << "The a key was pressed." << std::endl;
		}
		renWin->Render();
		// Forward events
		vtkInteractorStyleTrackballCamera::OnKeyPress();
	}
	

};
vtkStandardNewMacro(customMouseInteractorStyle);


int main(int, char *[])
{	
	//SprayCan
	std::string inputFilename = "spraycan.vtk";
	vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
	reader->SetFileName(inputFilename.c_str());
	reader->Update();
	vtkSmartPointer<vtkPolyDataMapper> sprayCanMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	sprayCanMapper->SetInputConnection(reader->GetOutputPort());
	sprayCanActor = vtkSmartPointer<vtkActor>::New();
	sprayCanActor->SetScale(14);
	sprayCanActor->SetPosition(xS,yS,zS);
	sprayCanActor->SetMapper(sprayCanMapper);

	//AnotherCube
	vtkCubeSource *cubeTest = vtkCubeSource::New();
	cubeTest->SetXLength(5);
	cubeTest->SetYLength(5);
	cubeTest->SetZLength(5);
	vtkPolyDataMapper *cubeTestMapper = vtkPolyDataMapper::New();
	cubeTestMapper->SetInputConnection(cubeTest->GetOutputPort());
	cubeTestActor = vtkActor::New();
	cubeTestActor->SetMapper(cubeTestMapper);

	//wall
	vtkCubeSource *cube = vtkCubeSource::New();
	cube->SetXLength(15);
	cube->SetYLength(8);
	cube->SetZLength(1);
	vtkPolyDataMapper *cubeMapper = vtkPolyDataMapper::New();
	cubeMapper->SetInputConnection(cube->GetOutputPort());
	cubeActor = vtkActor::New();
	cubeActor->SetMapper(cubeMapper);


	//floor
	vtkCubeSource *floor = vtkCubeSource::New();
	floor->SetXLength(3000);
	floor->SetYLength(1);
	floor->SetZLength(3000);
	vtkPolyDataMapper *floorMapper = vtkPolyDataMapper::New();
	floorMapper->SetInputConnection(floor->GetOutputPort());
	floorActor = vtkActor::New();
	floorActor->SetMapper(floorMapper);
	floorActor->SetPosition(0,-10,0);

	
	//renderer
	ren1 = vtkRenderer::New();
	ren1->GetActiveCamera()->SetPosition(20,10,50);
	//ren1->GetActiveCamera()->Azimuth(180);
	//ren1->GetActiveCamera()->Elevation(20);
	ren1->SetBackground(0,0,1);
	
	ren1->AddActor(floorActor);
	ren1->AddActor(cubeActor);
	ren1->AddActor(sprayCanActor);
	//ren1->AddActor(cubeTestActor);
	
	//window renderer
	renWin = vtkRenderWindow::New();
	renWin->AddRenderer(ren1);
	
	//interactor
	iren = vtkRenderWindowInteractor::New();
	iren->SetRenderWindow(renWin);
	vtkSmartPointer<customMouseInteractorStyle> style =
		vtkSmartPointer<customMouseInteractorStyle>::New();
	
	
	iren->SetInteractorStyle(style);
	style->SetCurrentRenderer(ren1);

	renWin->SetSize(800, 600);
	renWin->Render();
	iren->Start();

}