#ifndef INPUT_H
#define INPUT_H

#if defined(_WIN32)
// SDL2 Headers
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#elif defined(__APPLE__)
// SDL2 Headers
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#else
// SDL2 Headers
#include <SDL.h>
#include <SDL_opengl.h>
#endif

#include <glm/gtx/matrix_interpolation.hpp>

#include <imgui.h>
#include "imgui_impl_sdl_gl3.h"

#include "MolecularHandler.h"
#include "Camera.h"


#include "scene_data.h"
void docking_input(scene_docking_data &docking_data)
{
	SDL_Event windowEvent;
	static btRigidBody* selectedBody;
	static bool mousePressed = false; //true down

    //event handling
    while (SDL_PollEvent(&windowEvent))
    {
        ImGui_ImplSdlGL3_ProcessEvent(&windowEvent);
        switch (windowEvent.type)

        {

            // case SDL_WINDOWEVENT:
            // {
            //     switch (windowEvent.window.event)
            //     {
            //         case SDL_WINDOWEVENT_SIZE_CHANGED:
            //         {

            //             docking_data.cam->setWindowSize(windowEvent.window.data1, windowEvent.window.data2);
            //             glViewport(0, 0, windowEvent.window.data1, windowEvent.window.data2);
            //             break;
            //         }
            //     }
            //     break;
            // }

                // you can exit the program by clicking the cross button or pressing escape
            case SDL_QUIT: docking_data.nextscene = END;
                break;


                //mouse wheel control zoom
            case SDL_MOUSEWHEEL: docking_data.cam->traveling( windowEvent.wheel.y*2);
                ;
                break;
                //you can rotate with the left click
            case SDL_MOUSEMOTION:
            {
                if (windowEvent.motion.state == SDL_BUTTON_RMASK)
                {
                    docking_data.cam->arcBallRotation(true,windowEvent.motion.x, windowEvent.motion.y);
                }
                else
                {
                    docking_data.cam->arcBallRotation(false,windowEvent.motion.x, windowEvent.motion.y);
                }
                
                if (windowEvent.motion.state == SDL_BUTTON_LMASK)
                {   float updatePhy = glm::min(docking_data.elapsed,1.0f/60.0f); //Bullet fait de la merde si sous 60hz...
                    glm::vec3 hitPoint =  glm::vec3();
                    btRigidBody* currentBody = getPointedBody(docking_data.mh->dynamicsWorld,*docking_data.cam,hitPoint);
                    if (selectedBody == currentBody){
                        mousePollFunction(true,docking_data.mh->dynamicsWorld,*docking_data.cam,updatePhy);
                    }else{
                    mousePollFunction(false,docking_data.mh->dynamicsWorld,*docking_data.cam,updatePhy);
                    }
                }else{
                    float updatePhy = glm::min(docking_data.elapsed,1.0f/60.0f); //Bullet fait de la merde si sous 60hz...
                    mousePollFunction(false,docking_data.mh->dynamicsWorld,*docking_data.cam,updatePhy);
                }
                break;
            }

            case SDL_MOUSEBUTTONDOWN:
            { 	
            	mousePressed =true;
                if (windowEvent.button.button == SDL_BUTTON_LMASK)
                {
                    //Clavier
                    
                    // mousePollFunction(true,mh.dynamicsWorld,cam,elapsed);
                    const Uint8 *keybState = SDL_GetKeyboardState(NULL);
                    //si on click gauche + crtl, on place une contrainte
                    if (keybState[SDL_SCANCODE_LCTRL]){
                        
                        glm::vec3 hitPoint =  glm::vec3();
                        btRigidBody* body = getPointedBody(docking_data.mh->dynamicsWorld,*docking_data.cam,hitPoint);
                        if (body ){
                            glm::mat4 parentphymodel = glm::mat4(1);
                            body->getWorldTransform().getOpenGLMatrix(glm::value_ptr(parentphymodel ));
                            parentphymodel = glm::inverse(parentphymodel);
                            //on multiplie par l'inverse de la matrice physique pour avoir la position relative du point
                            hitPoint =   glm::vec3((parentphymodel)*glm::vec4(hitPoint,1.0f));
                            docking_data.cm->addConstraint(hitPoint,body);
                            // std::cout<<cm.constraints.size()<<std::endl;
                        }
                    }
                    // else if (keybState[SDL_SCANCODE_LSHIFT]){

                    //sinon on selectionne une molecule pour la faire tourner
                    else if (!keybState[SDL_SCANCODE_LSHIFT]){
                    	
                        glm::vec3 hitPoint =  glm::vec3();
                        selectedBody = getPointedBody(docking_data.mh->dynamicsWorld,*docking_data.cam,hitPoint);

                    }
                }

                if (windowEvent.button.button == SDL_BUTTON_RIGHT)
                {
                    //Clavier
                    // std::cout<<"harh"<<std::endl;

                    const Uint8 *keybState = SDL_GetKeyboardState(NULL);
                    if (keybState[SDL_SCANCODE_LCTRL])
                    {
                        glm::vec3 hitPoint =  glm::vec3();
                        btRigidBody* body = getPointedBody(docking_data.mh->dynamicsWorld,*docking_data.cam,hitPoint);
                        //if we touch a mol
                        if (body )
                        {
                        	// std::cout<<"hit"<<std::endl;
                        	//and we touch a constraint
                        	int constraintID = docking_data.cm->getConstraintID(hitPoint);
                        	std::cout<<"ID "<<constraintID<<std::endl;
                        	if (constraintID != -1)
                        	{
                        		//remove the constraint
                        		// if there is more than one element
                        		if (docking_data.cm->constraints.size()>1){
                        			if (constraintID %2 ==0){
                        				//if the id point on the last element of the constraint list and that id is even, then it's an isolated constraint
                        				if ((unsigned int )constraintID ==docking_data.cm->constraints.size()-1){
											docking_data.cm->constraints.erase(docking_data.cm->constraints.begin()+constraintID);
                        				}else{
                        				docking_data.cm->constraints.erase(docking_data.cm->constraints.begin()+constraintID,docking_data.cm->constraints.begin()+constraintID+2);
                        			}
                        			}else{
                        				docking_data.cm->constraints.erase(docking_data.cm->constraints.begin()+constraintID -1 ,docking_data.cm->constraints.begin()+constraintID+1);
                        			}
                        		}
                        		else{
                        			//if there only one element
                        			docking_data.cm->constraints.erase(docking_data.cm->constraints.begin()+constraintID);
                        		}
                        	}	
                        }
                    }
                }
                break;
            
                //user drag and drop a file
            }
            case SDL_MOUSEBUTTONUP:
            {
            	mousePressed = false;
            	break;
            }
            case SDL_DROPFILE:
            {
                char* dropped_filedir = windowEvent.drop.file;
                Molecule mol = readMOL2(dropped_filedir);

                //use molecularHandler to manage new data
                docking_data.mh->addMol(mol);
                //move camera to new center of scene
                docking_data.cam->lookAtPoint(docking_data.mh->getCenterofScene());
                docking_data.cam->pos = docking_data.mh->getCenterofScene()+glm::vec3(0.0,0.0,150.0);
               
                // Free dropped_filedir memory
                SDL_free(dropped_filedir);
                break;
            }
                // a key is pressed
            case SDL_KEYDOWN:
            {
                switch (windowEvent.key.keysym.sym)
                {
                    case SDLK_SPACE:
                    {
                        docking_data.cm->applyDockingForces(docking_data.mh->getCenterofScene());
                        break;
                    }
                    case SDLK_RIGHT: docking_data.cam->rotateAround(glm::radians(5.0f));
                        break;
                    case SDLK_LEFT: docking_data.cam->rotateAround(glm::radians(-5.0f));
                        break;


                }
                break;
            }
                // a key is released
            case SDL_KEYUP:
            {
                switch (windowEvent.key.keysym.sym)
                {

                    case SDLK_ESCAPE: 
                    {
                        docking_data.nextscene = END;
                        break;
                    }

                    case SDLK_TAB: 
                    {
                        docking_data.surf = !docking_data.surf;
                        docking_data.nextscene = spaceship;
                        break;
                    }


                    case SDLK_o: 
                    {
                        docking_data.optim = !docking_data.optim;
                        docking_data.freeze = true;
                        break;
                    }
                
                }
                break;
            }

        }
    } 

    if (mousePressed)
    {
    	// std::cout<<"pressed"<<std::endl;
    	if (windowEvent.button.button == SDL_BUTTON_LMASK)
        {
        	const Uint8 *keybState = SDL_GetKeyboardState(NULL);
            if (keybState[SDL_SCANCODE_LSHIFT])
            {
                glm::vec3 hitPoint =  glm::vec3();
                btRigidBody* body = getPointedBody(docking_data.mh->dynamicsWorld,*docking_data.cam,hitPoint);
                if (body ){
                    int constraintID = docking_data.cm->getConstraintID(hitPoint);
                	std::cout<<"ID "<<constraintID<<std::endl;
                	if (constraintID != -1)
                	{
                		glm::mat4 parentphymodel = glm::mat4(1);
                        body->getWorldTransform().getOpenGLMatrix(glm::value_ptr(parentphymodel ));
                        parentphymodel = glm::inverse(parentphymodel);
                        //on multiplie par l'inverse de la matrice physique pour avoir la position relative du point
                		docking_data.cm->constraints[constraintID].pos = glm::vec3((parentphymodel)*glm::vec4(hitPoint,1.0f)) ;
                	}
                }
            }
        }
    }  
}





void spaceship_input(scene_spaceship_data &spaceship_data)
{
	SDL_Event windowEvent;
	// static btRigidBody* selectedBody;
	static bool mousePressed = false; //true down


    //event handling

	int x,y;
	SDL_GetMouseState(&x,&y);
    // spaceship_data.spaceship.arcBallRotation( x,y );
	// spaceship_data.spaceship.mouse_callback(x,y);
    while (SDL_PollEvent(&windowEvent))
    {
        ImGui_ImplSdlGL3_ProcessEvent(&windowEvent);
        switch (windowEvent.type)

        {
    
            case SDL_WINDOWEVENT:
            {
                switch (windowEvent.window.event)
                {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                    {

                        spaceship_data.cam->setWindowSize(windowEvent.window.data1, windowEvent.window.data2);
                        glViewport(0, 0, windowEvent.window.data1, windowEvent.window.data2);
                        break;
                    }
                }
                break;
            }

                // you can exit the program by clicking the cross button or pressing escape
            case SDL_QUIT: spaceship_data.nextscene = END;
                break;


                //mouse wheel control zoom
            case SDL_MOUSEWHEEL: spaceship_data.followcam->traveling( windowEvent.wheel.y*2);
                ;
                break;
                //you can rotate with the left click
            case SDL_MOUSEMOTION:
            {
            	// spaceship_data.spaceship.arcBallRotation(windowEvent.motion.x, windowEvent.motion.y);
            	// if(spaceship_data.viewMode == firstPerson){
                if (*spaceship_data.invertedAxis==false){
        		    spaceship_data.fpsCam->ProcessMouseMovement(windowEvent.motion.xrel,-windowEvent.motion.yrel);
                }else{
                    spaceship_data.fpsCam->ProcessMouseMovement(windowEvent.motion.xrel,windowEvent.motion.yrel);
                }
        // 		glm::vec3 direction = glm::vec3(spaceship_data.spaceship.position* glm::vec4(spaceship_data.spaceship.direction,1.0));
        // 		direction = glm::normalize(direction);
        // 		//TEMP ORTHO
		      //   glm::vec3 temp = glm::cross(direction,glm::normalize(spaceship_data.fpsCam->WorldUp));
		      //   direction = glm::cross(direction,glm::normalize(temp));


		      //   // glm::vec3 rotationAxis = glm::cross(direction,glm::normalize(spaceship_data.fpsCam->Front));
		      //   // rotationAxis = glm::normalize(spaceship_data.spaceship.getPos() + rotationAxis);

		      //   float rotationAngle = (glm::dot(glm::normalize(spaceship_data.fpsCam->Front),direction));
		      //   std::cout << rotationAngle << std::endl;
		      //   // spaceship_data.spaceship.position = glm::rotate(spaceship_data.spaceship.position,rotationAngle,rotationAxis);
		     	// spaceship_data.spaceship.position = glm::rotate(spaceship_data.spaceship.position,rotationAngle,spaceship_data.fpsCam->WorldUp);





        		// glm::vec3 campos = spaceship_data.fpsCam->Position;
        		// glm::vec3 direction = spaceship_data.spaceship.direction;
        		// glm::vec3 Front = spaceship_data.fpsCam->Front;

    //     		//generate the matrix to align v1 on v2

    //     		glm::vec3 v1 = direction;
    //     		glm::vec3 v2 = Front;
    //     		// glm::vec3 v1 = glm::vec3(1.0,0.0,0.0);
    //     		// glm::vec3 v2 = glm::vec3(0.0,0.5,0.5);
        		
    //     		v1 = glm::normalize(v1);
    //     		v2 = glm::normalize(v2);

    //     		glm::vec3 axis = glm::cross(v1,v2);

    //     		float d = glm::dot(v1,v2);
    //     		float angle = acos (d);

    //     		glm::mat4 m = axisAngleMatrix(campos+axis,angle);

				// glm::vec3 rotatedV1 = glm::vec3(m * glm::vec4(v1,1.0));

    //     		// std::cout << "expected "<< glm::to_string(v2) << " result " << glm::to_string(rotatedV1) << std::endl;
    //     		spaceship_data.spaceship.position = m *spaceship_data.spaceship.position;

		  //       spaceship_data.spaceship.direction= spaceship_data.fpsCam->Front;
		  //       spaceship_data.spaceship.updateVecs();



        		// glm::vec3 up = spaceship_data.fpsCam->Right;
    //     		glm::vec3 up = glm::vec3(0.0,1.0,0.0);

	   //    		glm::vec3 v1 = direction;
    //     		glm::vec3 v2 = Front;
				// // glm::vec3 v1 = glm::vec3(1.0,0.5,0.0);
    //     		// glm::vec3 v2 = glm::vec3(0.0,-0.5,1.0);
        		
    //     		v1 = glm::normalize(v1);
    //     		v2 = glm::normalize(v2);

    //     		glm::vec3 temp = glm::normalize(glm::cross(v1,up));
    //     		glm::vec3 newV1 = glm::normalize(glm::cross(temp,up));

    //     		temp = glm::normalize(glm::cross(v2,up));
    //     		glm::vec3 newV2 = glm::normalize(glm::cross(temp,up));


    //     		// float d = glm::dot(v1,v2);
    //     		// float angle = acos (d);

    //     		float d = glm::dot(newV1,newV2);
    //     		if (d > 1.0) d =1.0f;
    //     		else if (d < -1.0) d =-1.0f;

    //     		float newAngle = acos (d);
    //     		glm::mat4 m = glm::axisAngleMatrix(campos+up,newAngle);

				// glm::vec3 rotatedV1 = glm::vec3(m * glm::vec4(v1,1.0));


        		// std::cout <<"d "<< d <<" new angle "<< glm::degrees(newAngle) <<  std::endl;//"angle "<< glm::degrees(angle) <<
        		// spaceship_data.spaceship.position = m *spaceship_data.spaceship.position;




    //     		// glm::vec3 xv1 = glm::vec3(1.0,0.0,0.0);
				// // glm::vec3 yv1 = glm::vec3(0.0,1.0,0.0);
				// // glm::vec3 zv1 = glm::vec3(0.0,0.0,1.0);


				// glm::vec3 xv2 = glm::vec3(0.0,-1.0,0.0);
				// glm::vec3 yv2 = glm::vec3(1.0,0.0,0.0);
				// glm::vec3 zv2 = glm::vec3(0.0,0.0,-1.0);

				// // glm::vec3 xv2 = glm::normalize(spaceship_data.fpsCam->Right);
				// // glm::vec3 yv2 = glm::normalize(spaceship_data.fpsCam->Up);
				// // glm::vec3 zv2 = glm::normalize(spaceship_data.fpsCam->Front);

    // 			float content[16] = {
				//    xv2.x, xv2.y, xv2.z, 0.0f,
				//    yv2.x, yv2.y, yv2.z, 0.0f,
				//    zv2.x, zv2.y, zv2.z ,0.0f,
				//    0.0f,  0.0f,  0.0f,  1.0f
				// };
				// glm::mat4 M;

				// memcpy( glm::value_ptr( M ), content, sizeof( content ) );


        		// std::cout <<"expected "<< glm::to_string(xv2) <<" result "<<glm::to_string(glm::vec3(M * glm::vec4(xv1,1.0f))) <<  std::endl;//"angle "<< glm::degrees(angle) 
				// std::cout <<"expected "<< glm::to_string(Front) <<" result "<<glm::to_string(glm::vec3(M * glm::vec4(xv1,1.0f))) <<  std::endl;
				// spaceship_data.spaceship.position = M * spaceship_data.spaceship.position ;


		        spaceship_data.spaceship.direction= spaceship_data.fpsCam->Front;
		        spaceship_data.spaceship.updateVecs();








            		// spaceship_data.spaceship.direction = spaceship_data.fpsCam->Front;
            	// }



                // if (windowEvent.motion.state == SDL_BUTTON_RMASK)
                // {
                //     spaceship_data.followcam->arcBallRotation(true,windowEvent.motion.x, windowEvent.motion.y);
                //     spaceship_data.spaceship.forceCam = false;
                // }
                // else
                // {
                //     spaceship_data.followcam->arcBallRotation(false,windowEvent.motion.x, windowEvent.motion.y);
                // }
                
                // if (windowEvent.motion.state == SDL_BUTTON_LMASK)
                // {   float updatePhy = glm::min(spaceship_data.elapsed,1.0f/60.0f); //Bullet fait de la merde si sous 60hz...
                //     glm::vec3 hitPoint =  glm::vec3();
                //     btRigidBody* currentBody = getPointedBody(spaceship_data.mh->dynamicsWorld,*spaceship_data.cam,hitPoint);
                //     if (selectedBody == currentBody){
                //         mousePollFunction(true,spaceship_data.mh->dynamicsWorld,*spaceship_data.cam,updatePhy);
                //     }else{
                //     mousePollFunction(false,spaceship_data.mh->dynamicsWorld,*spaceship_data.cam,updatePhy);
                //     }
                // }else{
                //     float updatePhy = glm::min(spaceship_data.elapsed,1.0f/60.0f); //Bullet fait de la merde si sous 60hz...
                //     mousePollFunction(false,spaceship_data.mh->dynamicsWorld,*spaceship_data.cam,updatePhy);
                // }
                break;
            }

            case SDL_MOUSEBUTTONDOWN:
            {
            	mousePressed =true;
                

                if (windowEvent.button.button == SDL_BUTTON_RMASK)
                {
                    //Clavier
                    
                    // mousePollFunction(true,mh.dynamicsWorld,cam,elapsed);
                    const Uint8 *keybState = SDL_GetKeyboardState(NULL);
                    if (keybState[SDL_SCANCODE_LCTRL]){
                        
                        // glm::vec3 hitPoint =  glm::vec3();
                        // btRigidBody* body = getPointedBody(mh.dynamicsWorld,cam,hitPoint);
                        // cm.addConstraint(hitPoint,body);
                        // std::cout<<cm.constraints.size()<<std::endl;
                    }
                }
                break;
            
                //user drag and drop a file
            }
            case SDL_MOUSEBUTTONUP:
            {
            	mousePressed = false;
            	spaceship_data.fire = false;
            	break;
            }
            case SDL_DROPFILE:
            {
                char* dropped_filedir = windowEvent.drop.file;
                Molecule mol = readMOL2(dropped_filedir);

                //use molecularHandler to manage new data
                spaceship_data.mh->addMol(mol);
                //move camera to new center of scene
                spaceship_data.cam->lookAtPoint(spaceship_data.mh->getCenterofScene());
                spaceship_data.cam->pos = spaceship_data.mh->getCenterofScene()+glm::vec3(0.0,0.0,150.0);
               
                // Free dropped_filedir memory
                SDL_free(dropped_filedir);
                break;
            }
                // a key is pressed
            case SDL_KEYDOWN:
            {
                switch (windowEvent.key.keysym.sym)
                {
      //               case SDLK_SPACE:
      //               {
      //               	spaceship_data.spaceship.forward();
						// spaceship_data.spaceship.forward();
						// spaceship_data.spaceship.forward();
      //                   break;
      //               }
                    // case SDLK_RIGHT: 
                    // 	spaceship_data.followcam->rotateAround(glm::radians(5.0f));
                    // 	spaceship_data.spaceship.forceCam = false;
                    //     break;
                    // case SDLK_LEFT: 
                    // 	spaceship_data.followcam->rotateAround(glm::radians(-5.0f));
                    // 	spaceship_data.spaceship.forceCam = false;
                    //     break;

                    case SDLK_w://beware QWERTY
                    {
                        if (*spaceship_data.wasd == true){
                            spaceship_data.spaceship.forward();
                            spaceship_data.spaceshipMoving = true;
                        }
                        break;
                    }   

                    case SDLK_a: //beware QWERTY
                    {
                        // spaceship_data.spaceship.rotateAround(glm::radians(3.0f));
                        if (*spaceship_data.wasd == true){
                            spaceship_data.spaceship.translate(-1.0);
                        }
                        break;
                    }

                    case SDLK_z://beware QWERTY
                    {
                        if (*spaceship_data.wasd == false){
                            spaceship_data.spaceship.forward();
                            spaceship_data.spaceshipMoving = true;
                        }
                        break;
                    }   

                    case SDLK_q: //beware QWERTY
                    {
                        // spaceship_data.spaceship.rotateAround(glm::radians(3.0f));
                        if (*spaceship_data.wasd == false){
                            spaceship_data.spaceship.translate(-1.0);
                        }
                        break;
                    }


                    case SDLK_d: //beware QWERTY
                    {
                        // spaceship_data.spaceship.rotateAround(glm::radians(-3.0f));
                        spaceship_data.spaceship.translate(1.0);
                        break;
                    }

                    case SDLK_s: //beware QWERTY
                    {
                            spaceship_data.spaceship.backward();

                        break;
                    }

                }
                break;
            }
                // a key is released
            case SDL_KEYUP:
            {
            	spaceship_data.spaceshipMoving = false;
                switch (windowEvent.key.keysym.sym)
                {

                    case SDLK_ESCAPE: 
                    {
                        // spaceship_data.running = false;
                        spaceship_data.nextscene = END;
                        break;
                    }

                    case SDLK_TAB: 
                    {
                        // spaceship_data.surf = !spaceship_data.surf;
                        spaceship_data.nextscene = docking;
                        break;
                    }



                    case SDLK_o: 
                    {
                        spaceship_data.optim = !spaceship_data.optim;
                        spaceship_data.freeze = true;
                        break;
                    }

                    // case SDLK_v: 
                    // {
                    //     spaceship_data.viewMode = (spaceship_data.viewMode+1)%NB_VIEW;
                    //     break;
                    // }
                	
                	// spaceship_data.spaceship.forward();
                }
                break;
            }

        }
    } 
    if (mousePressed)
    {
    	//deplacer les contraintes	
    	if (windowEvent.button.button == SDL_BUTTON_LMASK)
        {
        	spaceship_data.fire = true;

        	glm::vec3 hitPoint =  glm::vec3();

        	glm::vec3 shipPos = spaceship_data.spaceship.getPos();
        	glm::vec3 rayEnd = glm::vec3(spaceship_data.spaceship.position*glm::vec4(spaceship_data.spaceship.direction*50.0,1.0));
        	btRigidBody* body = rayPickup(spaceship_data.mh->dynamicsWorld,shipPos,rayEnd,hitPoint);

            // btRigidBody* body = getPointedBody(spaceship_data.mh->dynamicsWorld,*spaceship_data.cam,hitPoint);
            
            if (body ){
                int constraintID = spaceship_data.cm->getConstraintID(hitPoint);
            	if (constraintID != -1)
            	{
            		glm::mat4 parentphymodel = glm::mat4(1);
                    body->getWorldTransform().getOpenGLMatrix(glm::value_ptr(parentphymodel ));
                    parentphymodel = glm::inverse(parentphymodel);
                    //on multiplie par l'inverse de la matrice physique pour avoir la position relative du point
            		spaceship_data.cm->constraints[constraintID].pos = glm::vec3((parentphymodel)*glm::vec4(hitPoint,1.0f)) ;
            	}
            }
        }
    }     
}

#endif