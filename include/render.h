#ifndef RENDER_H
#define RENDER_H

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


#include <imgui.h>
#include "imgui_impl_sdl_gl3.h"

#include "MolecularHandler.h"
#include "Camera.h"

#include "scene_data.h"

void docking_render (scene_docking_data &docking_data){
	    
		static glm::mat4 previousViewProjection;

	     // first pass-------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, docking_data.framebuffer);

        glClearColor(docking_data.bgColor->x,docking_data.bgColor->y,docking_data.bgColor->z,docking_data.bgColor->w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
        glEnable(GL_DEPTH_TEST);
        // glBindTexture(GL_TEXTURE_2D, sphereTexture);

        //Change Camera
        glm::mat4 view = docking_data.cam->getView();
        glm::mat4 proj = docking_data.cam->getProj();


        // if (!surf)
        // {
        //     glEnable(GL_DEPTH_TEST);

        //     bbshader.use();
        //     bbshader.setMat4f("view", view);
        //     bbshader.setMat4f("proj", proj);
            

        //     glBindVertexArray(vao);
        //     for (unsigned int i=0; i<mh.vertbb.size();i++ )
        //     {
        //         bbshader.setMat4f("model", mh.getModelMatrix(i));
        //         vertices = mh.vertbb[i];
        //         nbvertices = vertices.size();

        //         //fill the buffer with the new data
                
        //         glBufferData(GL_ARRAY_BUFFER, nbvertices * sizeof (float), vertices.data(), GL_STATIC_DRAW);
        //         glDrawArrays(GL_POINTS, 0, nbvertices/3);
                
        //     }
        //     glBindVertexArray(0);


        // }

        // else
        // {   
            // glDisable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);  
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);  

        docking_data.surfaceShader.use();
        docking_data.surfaceShader.setMat4f("view", view);
        docking_data.surfaceShader.setMat4f("proj", proj);
        glBindVertexArray(docking_data.vao);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), BUFFER_OFFSET(0));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), BUFFER_OFFSET(3*sizeof(float)));
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), BUFFER_OFFSET(6*sizeof(float)));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        for (unsigned int i=0; i<docking_data.mh->vertsurf.size();i++ )
        {
            docking_data.surfaceShader.setMat4f("model", docking_data.mh->getModelMatrix(i));

            std::vector<float> surfVertices;
            surfVertices = docking_data.mh->vertsurf[i];
            int nbsurfvertices = surfVertices.size();
            // //fill the buffer with the data
            glBufferData(GL_ARRAY_BUFFER, nbsurfvertices * sizeof (float), surfVertices.data(), GL_STATIC_DRAW);



            glDrawArrays(GL_TRIANGLES,0,nbsurfvertices/3);
        }
        glBindVertexArray(0);
        glDisable(GL_CULL_FACE); 
        // }


 	//     docking_data.c->setView(view);
	 //    docking_data.c->setProjection(proj);
		// // docking_data.c->setUp(docking_data.cam->up);
		// // docking_data.c->setRight(docking_data.cam->normVec);
		
	 //    docking_data.c->render();
	    
        docking_data.cm->render(*docking_data.l,view,proj);

        for (unsigned int i=0; i<docking_data.cm->constraints.size();i++ )
        {


     		//draw sphere
        	docking_data.colorShader.use();
        	glBindVertexArray(docking_data.VAO_SPHERE);
        	docking_data.colorShader.setMat4f("view", view);
        	docking_data.colorShader.setMat4f("proj", proj);

        	glm::mat4 phymodel = glm::mat4(1);
        	docking_data.cm->constraints[i].parent->getWorldTransform().getOpenGLMatrix(glm::value_ptr(phymodel));
	        glm::vec3 pos= glm::vec3((phymodel)*glm::vec4(docking_data.cm->constraints[i].pos,1.0f));
	        // pos= pos -glm::vec3(0.5,0.5,0.5);//correct to place the sphere center on the point
        	docking_data.colorShader.setMat4f("model", glm::translate(glm::mat4(),pos));
        	docking_data.colorShader.setVec4f("color", glm::vec4(1.0,1.0,0.0,1.0));
        
        	glDrawArrays(GL_TRIANGLES, 0, docking_data.nbsphereVertices/3);
        	glBindVertexArray(0);
		}


		if(*docking_data.showSkybox){

	        // draw the skybox
	        glDepthFunc(GL_LEQUAL);// change depth function so depth test passes when values are equal to depth buffer's content

	        docking_data.skyboxShader.use();
	        docking_data.skyboxShader.setMat4f("view", glm::mat4(glm::mat3(view)));//only the rotation
	        docking_data.skyboxShader.setMat4f("proj", proj);
	        // ... set view and projection matrix
	        glBindVertexArray(docking_data.VAO_SKYBOX);
	        glActiveTexture(GL_TEXTURE0);
	        glBindTexture(GL_TEXTURE_CUBE_MAP, docking_data.cubemapTexture);
	        glDrawArrays(GL_TRIANGLES, 0, 36);
	        glBindVertexArray(0);
	        glDepthFunc(GL_LESS);// set depth function back to default

	    }
        



        // second pass
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT);
          
        docking_data.screenQuadShader.use();
        glUniform1i(glGetUniformLocation(docking_data.screenQuadShader.ID, "screenTexture"), 0); // set it manually
        glUniform1i(glGetUniformLocation(docking_data.screenQuadShader.ID, "depthTexture"), 1); // set it manually

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, docking_data.texColorBuffer);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, docking_data.texDepthBuffer); 
        
        glBindVertexArray(docking_data.quadVAO);
        glDisable(GL_DEPTH_TEST);



        // screenQuadShader.setFloat("window_width", window_width);
        // screenQuadShader.setFloat("window_height", window_height);

        docking_data.screenQuadShader.setFloat("window_width", docking_data.cam->screen_width);
        docking_data.screenQuadShader.setFloat("window_height", docking_data.cam->screen_height);
        //matrices for motion blur
        glm::mat4 viewProjectionInverse = glm::inverse ( proj * view);
        docking_data.screenQuadShader.setMat4f("viewProjectionInverse", viewProjectionInverse);

        glm::vec4 point (1.0f,1.0f,1.0f,1.0f);

        docking_data.screenQuadShader.setMat4f("previousViewProjection",  previousViewProjection);
        //update the matrix for next frame
        previousViewProjection =  proj * view  ;
        
        glDrawArrays(GL_TRIANGLES, 0, 6);  
}




void spaceship_render (scene_spaceship_data &spaceship_data){
	    
		static glm::mat4 previousViewProjection;

	     // first pass-------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, spaceship_data.framebuffer);

        // glClearColor(0.3f, 0.7f, 0.95f, 1.0f);
                glClearColor(spaceship_data.bgColor->x,spaceship_data.bgColor->y,spaceship_data.bgColor->z,spaceship_data.bgColor->w);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
        glEnable(GL_DEPTH_TEST);
        // glBindTexture(GL_TEXTURE_2D, sphereTexture);

        //Change Camera
        // glm::mat4 view = spaceship_data.followcam->getView();
        // glm::mat4 proj = spaceship_data.followcam->getProj();
		glm::mat4 view = spaceship_data.fpsCam->GetViewMatrix();
        glm::mat4 proj = glm::perspective(glm::radians(spaceship_data.fpsCam->Zoom),(float)spaceship_data.fpsCam->screen_width/ (float)spaceship_data.fpsCam->screen_height,0.1f,500.0f);


        glEnable(GL_CULL_FACE);  
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);  

        spaceship_data.surfaceShader.use();
        spaceship_data.surfaceShader.setMat4f("view", view);
        spaceship_data.surfaceShader.setMat4f("proj", proj);
        glBindVertexArray(spaceship_data.vao);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), BUFFER_OFFSET(0));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), BUFFER_OFFSET(3*sizeof(float)));
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), BUFFER_OFFSET(6*sizeof(float)));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        for (unsigned int i=0; i<spaceship_data.mh->vertsurf.size();i++ )
        {
            spaceship_data.surfaceShader.setMat4f("model", spaceship_data.mh->getModelMatrix(i));

            std::vector<float> surfVertices;
            surfVertices = spaceship_data.mh->vertsurf[i];
            int nbsurfvertices = surfVertices.size();
            // //fill the buffer with the data
            glBufferData(GL_ARRAY_BUFFER, nbsurfvertices * sizeof (float), surfVertices.data(), GL_STATIC_DRAW);



            glDrawArrays(GL_TRIANGLES,0,nbsurfvertices/3);
        }
        glBindVertexArray(0);
        glDisable(GL_CULL_FACE); 
        // }
        spaceship_data.l->setColor(glm::vec4(0.0,0.0,0.0,1.0));//color of the constraint
        spaceship_data.cm->render(*spaceship_data.l,view,proj);

        if (spaceship_data.fire){


        	if (spaceship_data.viewMode == thirdPerson){
        	spaceship_data.l->setColor(glm::vec4(1.0,1.0,0.0,1.0)); //color of the ray
	        spaceship_data.l->setView(view);
			spaceship_data.l->setProjection(proj);

			glm::vec3 pos = spaceship_data.spaceship.getPos();
			glm::vec3 direction = spaceship_data.spaceship.direction;
			glm::mat4 mat = spaceship_data.spaceship.position;

			spaceship_data.l->setPoints(pos,glm::vec3(mat*glm::vec4(direction*100.0,1.0)));

			spaceship_data.l->render();
			}

			if (spaceship_data.viewMode == firstPerson){
						
				spaceship_data.l->setColor(glm::vec4(0.0,1.0,0.0,1.0)); //color of the ray
		       spaceship_data.l->setView(view);
				spaceship_data.l->setProjection(proj);
				glm::vec3 pos = spaceship_data.spaceship.getPos();

				// glm::vec3 direction = glm::normalize(spaceship_data.spaceship.direction);
				// glm::mat4 mat = spaceship_data.spaceship.position;

				spaceship_data.l->setPoints(pos+glm::vec3(0.0,-1.0,0.1),pos + glm::normalize(spaceship_data.fpsCam->Front) * 1000.0);

				spaceship_data.l->render();
				
			}
		}

		// {

	 //    spaceship_data.l->setColor(glm::vec4(1.0,0.0,0.0,1.0)); //color of the ray
  //       spaceship_data.l->setView(view);
		// spaceship_data.l->setProjection(proj);

		// // glm::vec3 pos = spaceship_data.spaceship.getPos();
		// glm::vec3 pos =spaceship_data.fpsCam->Position;
		// glm::vec3 direction = pos+ (spaceship_data.fpsCam->Front* 1000.0);

		// // std::cout << glm::to_string(direction)<<std::endl;
		// // glm::mat4 mat = spaceship_data.spaceship.position;

		// spaceship_data.l->setPoints(pos,direction);

		// spaceship_data.l->render();
		// }

		// {
		//         		glm::vec3 direction = glm::vec3(spaceship_data.spaceship.position* glm::vec4(spaceship_data.spaceship.direction,1.0));
  //       		direction = glm::normalize(direction);
		//         glm::vec3 rotationAxis = glm::cross(direction,glm::normalize(spaceship_data.fpsCam->Front));
		//         rotationAxis = glm::normalize(spaceship_data.spaceship.getPos() + rotationAxis);
	 //    spaceship_data.l->setColor(glm::vec4(1.0,0.0,0.0,1.0)); //color of the ray
  //       spaceship_data.l->setView(view);
		// spaceship_data.l->setProjection(proj);

		// glm::vec3 pos = spaceship_data.spaceship.getPos();
		// glm::mat4 mat = spaceship_data.spaceship.position;

		// spaceship_data.l->setPoints(pos,glm::vec3(mat*glm::vec4(rotationAxis*100.0,1.0)));

		// spaceship_data.l->render();
		// }

		// {
		// spaceship_data.l->setColor(glm::vec4(0.0,1.0,0.0,1.0)); //color of the ray
  //       spaceship_data.l->setView(view);
		// spaceship_data.l->setProjection(proj);

		// glm::vec3 pos = spaceship_data.spaceship.getPos();
		// glm::vec3 direction = spaceship_data.spaceship.direction;
		// glm::mat4 mat = spaceship_data.spaceship.position;

		// spaceship_data.l->setPoints(pos,glm::vec3(mat*glm::vec4(direction*100.0,1.0)));

		// spaceship_data.l->render();
		// }

//		{
//		spaceship_data.l->setColor(glm::vec4(0.0,1.0,0.0,1.0)); //color of the ray
//        spaceship_data.l->setView(view);
//		spaceship_data.l->setProjection(proj);
//		glm::vec3 pos = spaceship_data.spaceship.getPos();

//		// glm::vec3 direction = glm::normalize(spaceship_data.spaceship.direction);
//		// glm::mat4 mat = spaceship_data.spaceship.position;

//		spaceship_data.l->setPoints(pos+glm::vec3(0.0,-1.0,0.1),pos + glm::normalize(spaceship_data.fpsCam->Front) * 1000.0);

//		spaceship_data.l->render();
//		}


		// {
		// spaceship_data.l->setColor(glm::vec4(0.0,1.0,0.0,1.0)); //color of the ray
  //       spaceship_data.l->setView(view);
		// spaceship_data.l->setProjection(proj);
		// glm::vec3 pos = spaceship_data.spaceship.getPos();

		// // glm::vec3 direction = glm::normalize(spaceship_data.spaceship.direction);
		// // glm::mat4 mat = spaceship_data.spaceship.position;

		// spaceship_data.l->setPoints(pos+glm::vec3(0.1),pos + spaceship_data.fpsCam->Up * 100.0);

		// spaceship_data.l->render();
		// }

    	if (spaceship_data.viewMode == thirdPerson)
    	{
    		
               //draw the spaceship
	        spaceship_data.spaceshipShader.use();
	        glBindVertexArray(spaceship_data.VAO_SPACESHIP);
	        spaceship_data.spaceshipShader.setMat4f("view", view);
	        spaceship_data.spaceshipShader.setMat4f("proj", proj);
	        spaceship_data.spaceshipShader.setMat4f("model", spaceship_data.spaceship.position);
	        glDrawArrays(GL_TRIANGLES, 0, spaceship_data.spaceshipVertices/3);
	        glBindVertexArray(0);

	        
		}



        for (unsigned int i=0; i<spaceship_data.cm->constraints.size();i++ )
        {


     		//draw sphere
        	spaceship_data.colorShader.use();
        	glBindVertexArray(spaceship_data.VAO_SPHERE);
        	spaceship_data.colorShader.setMat4f("view", view);
        	spaceship_data.colorShader.setMat4f("proj", proj);



        	glm::mat4 phymodel = glm::mat4(1);
        	spaceship_data.cm->constraints[i].parent->getWorldTransform().getOpenGLMatrix(glm::value_ptr(phymodel));
	        glm::vec3 pos= glm::vec3((phymodel)*glm::vec4(spaceship_data.cm->constraints[i].pos,1.0f));

        	spaceship_data.colorShader.setMat4f("model", glm::translate(glm::mat4(),pos));

        	spaceship_data.colorShader.setVec4f("color", glm::vec4(1.0,1.0,0.0,1.0));
        
        	glDrawArrays(GL_TRIANGLES, 0, spaceship_data.nbsphereVertices/3);
        	glBindVertexArray(0);
		}

		//draw the flame of engine for spaceship model
		if (spaceship_data.spaceshipMoving && spaceship_data.viewMode == thirdPerson)
		{
			spaceship_data.colorShader.use();
        	glBindVertexArray(spaceship_data.VAO_FLAME);
        	spaceship_data.colorShader.setMat4f("view", view);
        	spaceship_data.colorShader.setMat4f("proj", proj);

        	glm::mat4 flameModel= glm::rotate(glm::mat4(),0.0f,glm::vec3(0.0f,1.0f,0.0f)); 
        	flameModel = glm::translate(spaceship_data.spaceship.position * flameModel,glm::vec3(0.0,0.0,2.2)); // position of the flame
        	flameModel= glm::scale(flameModel,glm::vec3(0.4f+ 0.08f*cos(SDL_GetTicks()/150.0))+glm::vec3(0.0,0.0,1.0));

        	spaceship_data.colorShader.setMat4f("model", flameModel);

        	spaceship_data.colorShader.setVec4f("color", glm::vec4(1.0,1.0,1.0,1.0));
        
        	glDrawArrays(GL_TRIANGLES, 0, spaceship_data.nbflameVertices/3);
        	glBindVertexArray(0);
		}



		if (*spaceship_data.showSkybox){
	        // draw the skybox
	        glDepthFunc(GL_LEQUAL);// change depth function so depth test passes when values are equal to depth buffer's content

	        glBindVertexArray(spaceship_data.VAO_SKYBOX);
	        spaceship_data.skyboxShader.use();
	        spaceship_data.skyboxShader.setMat4f("view", glm::mat4(glm::mat3(view)));//only the rotation
	        spaceship_data.skyboxShader.setMat4f("proj", proj);
	        // ... set view and projection matrix
	        glActiveTexture(GL_TEXTURE0);
	        glBindTexture(GL_TEXTURE_CUBE_MAP, spaceship_data.cubemapTexture);
	        glDrawArrays(GL_TRIANGLES, 0, 36);
	        glBindVertexArray(0);
	        glDepthFunc(GL_LESS);// set depth function back to default
		}





        // second pass
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT);
          
        spaceship_data.screenQuadShader.use();
        glUniform1i(glGetUniformLocation(spaceship_data.screenQuadShader.ID, "screenTexture"), 0); // set it manually
        glUniform1i(glGetUniformLocation(spaceship_data.screenQuadShader.ID, "depthTexture"), 1); // set it manually

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, spaceship_data.texColorBuffer);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, spaceship_data.texDepthBuffer); 
        
        glBindVertexArray(spaceship_data.quadVAO);
        glDisable(GL_DEPTH_TEST);



        spaceship_data.screenQuadShader.setFloat("window_width", spaceship_data.cam->screen_width);
        spaceship_data.screenQuadShader.setFloat("window_height", spaceship_data.cam->screen_height);
        //matrices for motion blur
        glm::mat4 viewProjectionInverse = glm::inverse ( proj * view);
        spaceship_data.screenQuadShader.setMat4f("viewProjectionInverse", viewProjectionInverse);

        glm::vec4 point (1.0f,1.0f,1.0f,1.0f);

        spaceship_data.screenQuadShader.setMat4f("previousViewProjection",  previousViewProjection);
        //update the matrix for next frame
        previousViewProjection =  proj * view  ;
        
        glDrawArrays(GL_TRIANGLES, 0, 6);  
}





#endif