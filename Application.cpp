#include "glew.h"

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

#include "TestClearColor.h"
#include "TestTexture.h"
#include "TestRotation.h"
#include "TestBlock.h"
#include "ModelViewer.h"
#include "TestBatchRendering.h"
#include "TestBinarySpacePartition.h"
#include "TestGJK.h"
#include "TestProcGeo.h"
#include "TestMouseSelect.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"



int main( void )
{

	GLFWwindow* window;

	// Initialize the library 

	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );
	glfwWindowHint( GLFW_SAMPLES, 4 );

	// Create a windowed mode window and its OpenGL context 
	window = glfwCreateWindow(1536, 1152, "OpenGL Project", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Make the window's context current 
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	if (glewInit() == GLEW_OK)
		std::cout << "OK" << std::endl;

	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	{

		const char* glsl_version = "#version 130";
		
		ImGui::CreateContext(  );
		ImGui_ImplGlfw_InitForOpenGL( window, true );
		ImGui_ImplOpenGL3_Init( glsl_version );
		ImGuiIO& io = ImGui::GetIO(  ); ( void )io;
		ImGui::StyleColorsDark(  );

		Test::Test* currentTest{ nullptr };
		Test::TestMenu* testMenu = new Test::TestMenu( currentTest );
		currentTest = testMenu;

		testMenu->registerTest<Test::TestClearColor>( "Clear Color" );
		testMenu->registerTest<Test::TestTexture>( "Textures" );
		testMenu->registerTest<Test::TestRotation>( "Rotate" );
		testMenu->registerTest<Test::TestBlock>( "Block Maker" );
		testMenu->registerTest<Test::ModelViewer>( "Model Viewer" );
		testMenu->registerTest<Test::TestBatchRendering>( "Batch Rendering" );
		testMenu->registerTest<Test::TestBinarySpacePartition>( "Binary Space Partitioning" );
		testMenu->registerTest<Test::TestGJK>( "GJK Collision" );
		testMenu->registerTest<Test::TestProcGeo>( "Procedural Geometry" );
		testMenu->registerTest<Test::TestMouseSelect>( "Mouse Selection" );

		currentTest = testMenu;

		// Loop until the user closes the window 
		while (!glfwWindowShouldClose(window))
		{
			glCall(glClearColor(0.8f, 0.1f, 0.7f, 1.0f));
			
			Renderer::get().clear();

			//ImGui rendering
			ImGui_ImplOpenGL3_NewFrame(  );
			ImGui_ImplGlfw_NewFrame(  );
			ImGui::NewFrame( );

			if ( currentTest )
			{
				currentTest->onUpdate( io.DeltaTime * 1000 );
				currentTest->onRender( );

				ImGui::Begin( "Test" );
				if ( currentTest != testMenu && ImGui::Button( "<--" ) )
				{
					delete currentTest;
					currentTest = testMenu;
				}
				currentTest->onImGuiRender(  );
				ImGui::End(  );
			}

			//ImGui::ShowDemoWindow( );

			ImGui::Render(  );

			ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData(  ) );
			
			glfwMakeContextCurrent(window);
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		
		delete currentTest;
		if ( currentTest != testMenu )
			delete testMenu;
		currentTest = nullptr;
		testMenu = nullptr;
	}

	ImGui_ImplOpenGL3_Shutdown(  );
	ImGui_ImplGlfw_Shutdown(  );
	ImGui::DestroyContext(  );



	glfwDestroyWindow(window);
	glfwTerminate();

	


}
