#ifndef MOLECULE_H
#define MOLECULE_H

#include <vector>
#include "Atom.h"
// GLM headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Molecule
{
public:
    std::vector<Atom> atoms; ///< Table des atomes composant la molécule
    glm::vec3 barycenter;
    // glm::mat4 model;

    Molecule()
    {
        atoms = std::vector<Atom>();
        // model = glm::mat4(1.0);
    }

    glm::uint size() const
    {
        return atoms.size();
    };

    void calcBarycenter(void)
    {
        barycenter = glm::vec3();
        int size = atoms.size();
        for (int i = 0; i < size; i++)
        {
            barycenter += atoms[i].pos;
        }
        barycenter /= (float) size;
    }

    void CenterToOrigin() 
    {
        calcBarycenter();


        int size = atoms.size();
        for(int i=0;i<size; i++){
            atoms[i].pos -=  barycenter;
        } 
        calcBarycenter();
    }

    // void translate(glm::vec3 t)
    // {
    //     //updates rotation/translation matrix:
    //     model[0][3]+=t.x;
    //     model[1][3]+=t.y;
    //     model[2][3]+=t.z;
    // }

    // //return the specified Atom with its position multiplied by the internal matrix

    // Atom getAtom(int i)
    // {
    //     Atom a = atoms[i];
    //     a.pos =  glm::vec3(model * glm::vec4(a.pos, 1.0));
    //     return a;
    // }
    // //return a vector of thez position of all atoms multiplied by the internal matrix
    // std::vector<float> getVertices()
    // {
    //     std::vector<float> vertices;

    //     glm::vec3 temp;

    //     glm::uint molSize = size();

    //     for (glm::uint i = 0; i < molSize; i ++)
    //     {
    //         temp = getAtom(i).pos;
    //         vertices.push_back(temp.x);
    //         vertices.push_back(temp.y);
    //         vertices.push_back(temp.z);
    //     }
        
    //     return vertices;
    // }

    /**
      * Donne la charge a n'importe quel point de l'espace autour de la molecule
      */
    float getCharge(glm::vec3 position)
    {
        float charge = 0.0f;
        int size = atoms.size();
        for(int i=0;i<size; i++)
        {
            float dist = glm::length(atoms[i].pos - position);
            //if (dist < 10.0f)
                charge += atoms[i].charge / dist;
        }
        return charge;
    }

    void getDimensions(float & minXres, float & maxXres, float & minYres, float & maxYres, float &minZres, float &maxZres)
    {
        float minX = atoms[0].pos.x;
        float minY = atoms[0].pos.y;
        float minZ = atoms[0].pos.z;
        float maxX = atoms[0].pos.x;
        float maxY = atoms[0].pos.y;
        float maxZ = atoms[0].pos.z;

        glm::uint NbAtoms = size();
        for(glm::uint i=0;i<NbAtoms;i++)
        {
            if(atoms[i].pos.x < minX)
                minX = atoms[i].pos.x;
            if(atoms[i].pos.y < minY)
                minY = atoms[i].pos.y;
            if(atoms[i].pos.z < minZ)
                minZ = atoms[i].pos.z;

            if(atoms[i].pos.x > maxX)
                maxX = atoms[i].pos.x;
            if(atoms[i].pos.y > maxY)
                maxY = atoms[i].pos.y;
            if(atoms[i].pos.z > maxZ)
                maxZ = atoms[i].pos.z;
        }

        // std::cout <<"X min: "<< minX << " max :"<<maxX << std::endl;
        // std::cout <<"Y min: "<< minY << " max :"<<maxY << std::endl;
        // std::cout <<"Z min: "<< minZ << " max :"<<maxZ << std::endl;
        minXres = minX;
        minYres = minY;
        minZres = minZ;
        maxXres = maxX;
        maxYres = maxY;
        maxZres = maxZ;
    }

};

#endif