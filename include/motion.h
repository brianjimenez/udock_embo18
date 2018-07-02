
#ifndef __MOTION__
#define __MOTION__

#include "pickupRay.h"
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "Camera.h"




void setBodyPosition(btRigidBody* body, glm::vec3 position)
{
	body->setActivationState(ACTIVE_TAG);
	btDefaultMotionState* myMotionState = (btDefaultMotionState*)body->getMotionState();
	btTransform trans;
	myMotionState->getWorldTransform(trans);
	trans.setOrigin(btVector3(position.x,position.y,position.z));
	myMotionState->setWorldTransform(trans);
	body->setWorldTransform(trans);
	body->setMotionState(myMotionState);
}


void setBodyRotation(btRigidBody* body, glm::quat rotation)
{
	body->setActivationState(ACTIVE_TAG);
	btDefaultMotionState* myMotionState = (btDefaultMotionState*)body->getMotionState();
	btTransform trans;
	myMotionState->getWorldTransform(trans);
	trans.setRotation(btQuaternion(rotation.x,rotation.y,rotation.z,rotation.w));
	myMotionState->setWorldTransform(trans);
	body->setWorldTransform(trans);
	body->setMotionState(myMotionState);
}

// void addToBodyRotation(btRigidBody* body, glm::quat rotation)
// {
// 	body->setActivationState(ACTIVE_TAG);
// 	btDefaultMotionState* myMotionState = (btDefaultMotionState*)body->getMotionState();
// 	btTransform trans;
// 	myMotionState->getWorldTransform(trans);
// 	trans.getRotation();
// 	trans.setRotation(btQuaternion(rotation.x,rotation.y,rotation.z,rotation.w));
// 	myMotionState->setWorldTransform(trans);
// 	body->setWorldTransform(trans);
// 	body->setMotionState(myMotionState);
// }

///Applique une force sur le centre de gravite
void applyForceCenter(btRigidBody* body,const glm::vec3 & force)
{
	body->setActivationState(ACTIVE_TAG);
	body->applyCentralForce(btVector3(force.x,force.y,force.z));
}

///Applique une force en un point donné
///Attention, la position relative est la position world - la position du body
void applyForce(btRigidBody* body,const glm::vec3 & force, const glm::vec3 & relPos)
{
	body->setActivationState(ACTIVE_TAG);
	body->applyForce(btVector3(force.x,force.y,force.z),btVector3(relPos.x,relPos.y,relPos.z));
}


///Applique une force sur le centre de gravite
void applyImpulseCenter(btRigidBody* body,const glm::vec3 & force)
{
	body->setActivationState(ACTIVE_TAG);
	body->applyCentralImpulse(btVector3(force.x,force.y,force.z));
}

///Applique une force en un point donné
void applyImpulse(btRigidBody* body,glm::vec3 & force,glm::vec3 & pos)
{
	body->setActivationState(ACTIVE_TAG);
	body->applyImpulse(btVector3(force.x,force.y,force.z),btVector3(pos.x,pos.y,pos.z));
}

void applyTorque(btRigidBody* body,glm::vec3 force)
{
	body->setActivationState(ACTIVE_TAG);
	body->applyTorque(btVector3(force.x,force.y,force.z));
}

void applyTorqueImpulse(btRigidBody* body,glm::vec3 force)
{
	body->setActivationState(ACTIVE_TAG);
	body->applyTorqueImpulse(btVector3(force.x,force.y,force.z));
}


///Donne le vecteur vitesse du body
glm::vec3 getLinearVelocity(btRigidBody* body)
{
	btVector3 actSpeed = body->getLinearVelocity();
	return glm::vec3(actSpeed.m_floats[0],actSpeed.m_floats[1],actSpeed.m_floats[2]);
}

void setLinearVelocity(btRigidBody* body,glm::vec3 speed)
{
	body->setLinearVelocity(btVector3(speed.x,speed.y,speed.z));
}

glm::vec3 getAngularVelocity(btRigidBody* body)
{
	btVector3 angSpeed = body->getAngularVelocity();
	return glm::vec3(angSpeed.m_floats[0],angSpeed.m_floats[1],angSpeed.m_floats[2]);
}

void setAngularVelocity(btRigidBody* body,glm::vec3 speed)
{
	body->setAngularVelocity(btVector3(speed.x,speed.y,speed.z));
}



///Fait qu'un objet ne bouge plus
///Attention !!!  ecrase les linear et angular factors
void setFreeze(btRigidBody* body,bool freeze)
{
	if(freeze)
	{
		body->setLinearFactor(btVector3(0.0f,0.0f,0.0f));
		body->setAngularFactor(btVector3(0.0f,0.0f,0.0f));
	}
	else
	{
		body->setLinearFactor(btVector3(1.0f,1.0f,1.0f));
		body->setAngularFactor(btVector3(1.0f,1.0f,1.0f));
	}
}

//met a zero toutes les vitesses
void stopMol (btRigidBody* body){
		body->setLinearVelocity(btVector3(0.0,0.0,0.0));
		body->setAngularVelocity(btVector3(0.0,0.0,0.0));
	}

void mousePollFunction(bool rotate,btDiscreteDynamicsWorld * dynamicsWorld, Camera  cam,float elapsed)
{
	static int lastx = -1; //une variable static perdure tout le long du programe
	static int lasty = -1;
	static float elaspedSinceLastMove = 0;


	int x = 0;
	int y = 0;

	SDL_GetMouseState(&x,&y);

	elaspedSinceLastMove += elapsed;

	if(!rotate)
	{
		lastx = x;
		lasty = y;
	}
	else
	{
		if(lastx == -1 && lasty == -1)
		{
			lastx = x;
			lasty = y;
		}

		glm::vec3 hitPoint;
        btRigidBody* body = getPointedBody(dynamicsWorld,cam,hitPoint);

        
        //if the mouse hit something
        if (body)
        {
        	// std::cout <<hitPoint.x << " " << hitPoint.y << " " << hitPoint.z << std::endl;
            // std::cout <<body << std::endl;
            // std::cout <<body->getInvMass() << std::endl;

			//on convertit en position monde les coordoné de la souris,plus les coordoné de l'etape d'avant
			glm::vec3 now = unProjectMousePos(cam,glm::vec3(x,y,0.0));
			glm::vec3 before = unProjectMousePos(cam,glm::vec3(lastx,lasty,0.0));


			//calculate angularVelocity
			btVector3 angSpeed = body->getAngularVelocity();
			btVector3 damping = (angSpeed/5.0f) * 60.0f * elapsed;

			if(damping.length2() < angSpeed.length2())
			{
				angSpeed -= damping;
			}
			else
			{
				angSpeed = btVector3 ();
			}
			
			body->setAngularVelocity(angSpeed);
			body->setLinearVelocity(btVector3(0.0,0.0,0.0));



			//calculate the point where the rotation occur
						
			btTransform trans = body->getWorldTransform();
			// std::cout <<trans.getOrigin().getX() << " " << trans.getOrigin().getY() << " " << trans.getOrigin().getZ() << std::endl;

			btVector3 rotatePoint = btVector3(hitPoint.x,hitPoint.y,hitPoint.z) - trans.getOrigin();
			rotatePoint = rotatePoint * trans.getBasis().inverse();//basis is the rotation matrix
				

			// //On utilise thales

			float distCamRpw = glm::length(cam.pos - hitPoint);
			float distCamNow = glm::length(cam.pos - now);
			float rapportThales = distCamRpw / distCamNow;

			glm::vec3 vertCamOld = before - cam.pos;
			glm::vec3 posOldNearMol = cam.pos + (vertCamOld * rapportThales);


			// glm::vec3 force = posOldNearMol - hitPoint; //the rigidbody goes counter to the pointer movement
			glm::vec3 force =  hitPoint- posOldNearMol ; //the rigidbody follow the pointer movement
			btVector3 relPos = btVector3(hitPoint.x,hitPoint.y,hitPoint.z) - trans.getOrigin();


			// force = force * 100000.0f;
			// std::cout <<"force length2 "<<glm::length2(force) << std::endl;

			if(glm::length2(force) > 0.01)
			{
				//On va vérifier si on pointe vers le barycentre, auquel cas on va tricher un peu pour aider le déplacement
				glm::vec3 relPosNorm = glm::vec3( relPos.getX(),relPos.getY(),relPos.getZ());
				relPosNorm = normalize(relPosNorm);
				glm::vec3 forceNorm = force;
				forceNorm = normalize(forceNorm);
				// float scalProd = relPosNorm.scalProd(forceNorm);
				float scalProd = glm::dot(relPosNorm,forceNorm);

				if(scalProd < 0)
				{
					//on le ramène un peu vers nous
					//Pour éviter les hésitation de rotation quand on tire vers le barycentre

					vertCamOld =normalize(vertCamOld);
					vertCamOld*= -1.0f * glm::length(force) * abs(scalProd) ;
					posOldNearMol += vertCamOld;
					force = hitPoint- posOldNearMol ;

					//// relPos = hitPoint - _SelectedMol->_Transform._Pos;// important ?
				}

				if(glm::length(force) > 1)
				{
					force = normalize(force);
					force *= 1;
				}
				// glm::vec3 torque = glm::cross(relPos,force*400);
				force = force * 350.0f;///NOMBRE MAGIQUE ! (400)

				btVector3 torque =  relPos.cross(btVector3(force.x,force.y,force.z));
				
				// std::cout <<"torque "<<torque.getX() << " " <<torque.getY() << " " << torque.getZ() << std::endl;
				body->setActivationState(ACTIVE_TAG);
				body->applyTorque(torque);

			//_SelectedMol->_Body->applyTorque(glm::vec3(0,400,0));
			}

		}

	}

	lastx = x;
	lasty = y;

}
		

//transformer en objet dedier par rigidbody
// void dampMols(float elapsed, bool damp, Molecule * ignoreMol = NULL)
// {
// 	static glm::vec3 angDamping[2], linDamping[2];
// 	static float _DureeFade = 1.0f;
// 	static bool lastDamp = false;
// 	static float elapsedFade = 0;

// 	if(!damp)
// 	{
// 		lastDamp = false;
// 		return;
// 	}

// 	NYVert3Df angSpeed[2], linSpeed[2];

// 	if(_MoleculesToMatch[0]->_Body)
// 		angSpeed[0] = _MoleculesToMatch[0]->_Body->getAngularSpeed();
// 	if(_MoleculesToMatch[1]->_Body)
// 		angSpeed[1] = _MoleculesToMatch[1]->_Body->getAngularSpeed();

// 	if(_MoleculesToMatch[0]->_Body)
// 		linSpeed[0] = _MoleculesToMatch[0]->_Body->getSpeed();
// 	if(_MoleculesToMatch[1]->_Body)
// 		linSpeed[1] = _MoleculesToMatch[1]->_Body->getSpeed();

// 	if(lastDamp == false && damp == true)
// 	{
// 		angDamping[0] = angSpeed[0] / _DureeFade;
// 		angDamping[1] = angSpeed[1] / _DureeFade;
// 		linDamping[0] = linSpeed[0] / _DureeFade;
// 		linDamping[1] = linSpeed[1] / _DureeFade;
// 		lastDamp = true;
// 		elapsedFade = 0.0f;
// 	}

// 	if(abs(angSpeed[0].X) >= abs(angDamping[0].X)*elapsed)
// 		angSpeed[0] -= angDamping[0]*elapsed;
// 	else
// 		angSpeed[0] = NYVert3Df();

// 	if(abs(angSpeed[1].X) >= abs(angDamping[1].X)*elapsed)
// 		angSpeed[1] -= angDamping[1]*elapsed;
// 	else
// 		angSpeed[1] = NYVert3Df();

// 	if(abs(linSpeed[0].X) >= abs(linSpeed[0].X)*elapsed)
// 		linSpeed[0] -= linSpeed[0]*elapsed;
// 	else
// 		linSpeed[0] = NYVert3Df();

// 	if(abs(linSpeed[1].X) >= abs(linSpeed[1].X)*elapsed)
// 		linSpeed[1] -= linSpeed[1]*elapsed;
// 	else
// 		linSpeed[1] = NYVert3Df();

// 	if(_MoleculesToMatch[0]->_Body && ignoreMol != _MoleculesToMatch[0])
// 		 _MoleculesToMatch[0]->_Body->setAngularSpeed(angSpeed[0]);
// 	if(_MoleculesToMatch[1]->_Body && ignoreMol != _MoleculesToMatch[1])
// 		_MoleculesToMatch[1]->_Body->setAngularSpeed(angSpeed[1]);

// 	 if(_MoleculesToMatch[0]->_Body  && ignoreMol != _MoleculesToMatch[0])
// 		_MoleculesToMatch[0]->_Body->setSpeed(linSpeed[0]);
// 	 if(_MoleculesToMatch[1]->_Body && ignoreMol != _MoleculesToMatch[1])
// 		_MoleculesToMatch[1]->_Body->setSpeed(linSpeed[1]);

// 	 /*if(linSpeed[0].getMagnitude() == 0 && angSpeed[0].getMagnitude() == 0 && g_selected_mol !=  MoleculesToMatch[0])
// 		 MoleculesToMatch[0]->_Body->setFreeze(true);
// 	 else
// 		 if(MoleculesToMatch[0]->_Body->Frozen)
// 			MoleculesToMatch[0]->_Body->setFreeze(false);

// 	 if(linSpeed[1].getMagnitude() == 0 && angSpeed[1].getMagnitude() == 0 && g_selected_mol !=  MoleculesToMatch[1])
// 		 MoleculesToMatch[1]->_Body->setFreeze(true);
// 	 else
// 		 if(MoleculesToMatch[1]->_Body->Frozen)
// 			 MoleculesToMatch[1]->_Body->setFreeze(false);*/


// 	elapsedFade += elapsed;
// 	if(elapsedFade > _DureeFade)
// 	{
// 		elapsedFade = 0.0f;
// 		lastDamp = false;
// 	}
// }

#endif