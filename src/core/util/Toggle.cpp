/*
 * Toggle.cpp
 *
 *  Created on: 9 июл. 2020 г.
 *      Author: sadko
 */

#include <core/util/Toggle.h>

namespace lsp
{
    Toggle::Toggle()
    {
        construct();
    }

    Toggle::~Toggle()
    {
        fValue          = 0.0f;
        nState          = TRG_OFF;
    }

    void Toggle::construct()
    {
        fValue          = 0.0f;
        nState          = TRG_OFF;
    }

    void Toggle::dump(IStateDumper *v) const
    {
        v->write("fValue", fValue);
        v->write("nState", nState);
    }
}


