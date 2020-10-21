//
// Created by tooster on 08.10.2020.
//

#ifndef PGK_MATERIAL_HPP
#define PGK_MATERIAL_HPP

#include <memory>
#include "Shader.hpp"

class Material {
    shared_ptr<Shader> usedShader;
};


#endif //PGK_MATERIAL_HPP
