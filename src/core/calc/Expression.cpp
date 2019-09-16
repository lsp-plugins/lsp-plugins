/*
 * Expression.cpp
 *
 *  Created on: 16 сент. 2019 г.
 *      Author: sadko
 */

#include <core/calc/Expression.h>

namespace lsp
{
    namespace calc
    {
        
        Expression::Expression(Resolver *res)
        {
            pResolver       = res;
        }
        
        Expression::~Expression()
        {
            pResolver       = NULL;
        }
    
    } /* namespace calc */
} /* namespace lsp */
