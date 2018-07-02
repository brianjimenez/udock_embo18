#ifndef SPACESHIP_CONTROLLER_H
#define SPACESHIP_CONTROLLER_H

// GLM headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>


class spaceship_controller
{
public:
	glm::mat4 position;
	glm::vec3 camPos; //where the cam following should be
	
    //internal vector for orientation
	glm::vec3 up; //up vector of the ship
    glm::vec3 worldUp; //up vector reference for the world
    glm::vec3 center; //the point the ship currently look at;
    glm::vec3 direction; //direction of the ship (vector from pos to center)
    glm::vec3 normVec; //give the right axis of the ship
    glm::vec3 newdirection; //direction of the ship (vector from pos to center)

    //for mouse rotation
    // glm::vec2 screenCenter;
    glm::vec2 screenSize;
	float speed,baseSpeed; //how fast the spaceship goes
	float maneuverability; //how much easily the space ship turn
	float sensitivity; // the mouse sensitivity
	bool forceCam;
	spaceship_controller()
    {	
    	position = glm::mat4();
    	// position = glm::translate(position,glm::vec3(5.5,0.5,0.0) );//correction for model !! 
    	speed = 1.2;
    	baseSpeed = 80.0;
    	maneuverability = 3.0;
    	camPos = glm::vec3 (0.0,3.5,12.0);
    	forceCam = true;
    	//internal vector for orientation
    	center = glm::vec3(0.0f, 0.0f, 0.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        direction = glm::normalize(glm::vec3(0.0,0.0,-1.0));
        newdirection = direction;
        updateVecs();

	}

		spaceship_controller(glm::vec2 screenSize)
    {	
    	screenSize = screenSize;

    	position = glm::mat4();
    	// position = glm::translate(position,glm::vec3(5.5,0.5,0.0) );//correction for model !! 
    	speed = 1.2;
    	baseSpeed = 80.0;
    	maneuverability = 3.0;
    	camPos = glm::vec3 (0.0,3.5,12.0);
    	forceCam = true;
    	
    	//internal vector for orientation
    	center = glm::vec3(0.0f, 0.0f, 0.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        direction = glm::normalize(glm::vec3(0.0,0.0,-1.0));
        newdirection = direction;
        updateVecs();

	}

	void updateSpeed(float elapsed)
	{
		speed = baseSpeed * elapsed;
	}
	void translate(float sign){
		// glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

		position = glm::translate(position,glm::normalize(glm::cross(direction, worldUp)) * sign*speed);
		updateVecs();
	}
    void forward(){
    	position = glm::translate(position,direction * speed);
    	forceCam = true;
    	updateVecs();
    }

    void backward(){
    	position = glm::translate(position,-direction * speed);
    	forceCam = true;
    	updateVecs();
    }

    void rotateAround(float angle)
    {
		// glm::mat4 rotation = glm::rotate(glm::mat4(), angle,getPos()+worldUp);
		// position =  position * rotation;
        // position = glm::rotate(position, angle, worldUp);
        position = glm::rotate(position, angle, up );//up seems better !
        updateVecs();
 
    }



    void rotate(){
    	position = glm::rotate(position,1.0f,glm::vec3(0.0f,1.0f,0.0f));
    	forceCam = true;
    	updateVecs();
    	
    }

	void mouse_callback( double xpos, double ypos)
	{

		static bool firstMouse = true;
		static float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
		static float pitch =  0.0f;
		static float lastX =  screenSize.x / 2.0;
		static float lastY =  screenSize.y / 2.0;

	    if (firstMouse)
	    {
	        lastX = xpos;
	        lastY = ypos;
	        firstMouse = false;
	    }


	    // float xoffset =(float)( xpos - screenSize.x/2);
	    // float yoffset =(float)( ypos - screenSize.y/2);
	    float xoffset = xpos - lastX;
	    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	    lastX = xpos;
	    lastY = ypos;

	    sensitivity = 0.8f; // change this value to your liking
	    xoffset *= sensitivity;
	    yoffset *= sensitivity;

	    yaw += xoffset;
	    pitch += yoffset;

	    // make sure that when pitch is out of bounds, screen doesn't get flipped
	    if (pitch > 89.0f)
	        pitch = 89.0f;
	    if (pitch < -89.0f)
	        pitch = -89.0f;

	    glm::vec3 front;
	    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	    front.y = sin(glm::radians(pitch));
	    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	    newdirection = glm::normalize(front);
	}


    // /**
     // * Rotation droite gauche en troisième personne
     // */
    // void rotateAround(float angle)
    // {
    //     // pos -= center;
    //     // pos = glm::rotate(pos, angle, worldUp);
    //     // pos = glm::rotate(pos, angle, up);//seems better !
    //     position = glm::rotate(position,1.0f,glm::vec3(0.0f,1.0f,0.0f));
    // 	forceCam = true;
    //     // pos += center;
    //     updateVecs();
    // }

    /**
     * Rotation haut bas en troisième personne
     */
    void rotateUpAround(float angle)
    {
        // pos -= center;

        //On ne monte pas trop haut pour ne pas passer de l'autre coté
        glm::mat4 previousPos = position;
        // pos = glm::rotate(pos, angle, normVec);
         position = glm::rotate(position, angle,normVec);
		// glm::mat4 rotation = glm::rotate(glm::mat4(), angle,normVec);
		// position =  position * rotation;

        // position = glm::rotate(position, angle,glm::vec3(-1.0,0.0,0.0));

        glm::vec3 normPos = glm::vec3(position * glm::vec4(direction,1.0));
        normPos = glm::normalize(normPos);

        // std::cout<<glm::to_string(normPos)<<std::endl;

        float newAngle = glm::dot(normPos, worldUp);
        // std::cout << "angle "<<newAngle <<" radians "<< glm::radians(newAngle)<<std::endl;
		// newAngle = glm::radians(newAngle);
        // if (newAngle > 0.99 || newAngle < -0.99)
        if (newAngle > 0.99 || newAngle < -0.99)
        {
	        // std::cout << " block !"<< std::endl;

            position = previousPos;
        }

        // pos += center;
        updateVecs();
    }

    void arcBallRotation(int x, int y)
    {

        static float lastx = -1; //remember, the static keyword indicate a persistent variable (live during the whole program)
        static float lasty = -1;


        if (lastx == -1 && lasty == -1)
        {
            lastx = x;
            lasty = y;
        }

        float dx,dy;

        // if (std::abs (x - screenSize.x/2) >screenSize.x/3){
         	dx =(float)( x - screenSize.x/2);
        // }else{
        	// dx = x - lastx;
        // }

        if (dx > 0.0f){
        	dx = std::min (maneuverability, dx /10.0f );
        }
        else{
        	dx = std::max (-maneuverability, dx /10.0f );
        }

        rotateAround(glm::radians(-dx));



        // if (std::abs (y - screenSize.y/2) >screenSize.y/3){
         	dy =(float)( y - screenSize.y/2);
        // }else{
        	// dy = y - lasty;
        // }

        if (dy > 0.0f){
			dy = std::min (maneuverability, dy /10.0f );        }
        else{
        	dy = std::max (-maneuverability, dy /10.0f );
        }
        
        rotateUpAround(glm::radians(-dy));

        
        lastx = x;
        lasty = y;




     //    float xoffset = dx;
     //    float yoffset = dy;
	    // sensitivity = 0.8f; // change this value to your liking

	    // xoffset *= sensitivity;
	    // yoffset *= sensitivity;

	    // float yaw = xoffset;
	    // float pitch = yoffset;

	    // // make sure that when pitch is out of bounds, screen doesn't get flipped
	    // if (pitch > 89.0f)
	    //     pitch = 89.0f;
	    // if (pitch < -89.0f)
	    //     pitch = -89.0f;


	 //    glm::mat4 rotation = glm::eulerAngleYXZ 	( 	
	 //    	cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
		// 	sin(glm::radians(pitch)),
		// 	sin(glm::radians(yaw)) * cos(glm::radians(pitch)) 
		// );

		// position = position * rotation;
    }



    void updateVecs(void)
    {
        // direction = center - getPos();
        // direction = glm::normalize(glm::vec3(glm::mat4(glm::mat3(position)) *  glm::vec4(direction,1.0f)));
 		// direction =	glm::normalize(glm::vec3(glm::translate(position,direction)*glm::vec4(getPos(),1.0)));

        normVec = glm::cross(direction, worldUp);
        normVec = glm::normalize(normVec);

        up = glm::cross(normVec, direction);
        up = glm::normalize(up);
    }







    glm::vec3 getPos(){
    	return glm::vec3(position*glm::vec4(0.0f,0.0f,0.0f,1.0f));
    }

    glm::vec3 getCamPointToLookAT(){
    	return getPos()+ glm::vec3(0.0f,2.5f,0.0f);
    }

    glm::vec3 getCamPos(){
    	return glm::vec3(position* glm::vec4(camPos,1.0f));
    }
};


#endif