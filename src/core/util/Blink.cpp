/*
 * Blink.cpp
 *
 *  Created on: 9 июл. 2020 г.
 *      Author: sadko
 */

#include <core/util/Blink.h>

namespace lsp
{
    Blink::Blink()
    {
        construct();
    }

    Blink::~Blink()
    {
        nCounter        = 0.0f;
        nTime           = 0.0f;
    }

    void Blink::construct()
    {
        nCounter        = 0.0f;
        nTime           = 0.0f;
        fOnValue        = 1.0f;
        fOffValue       = 0.0f;
        fTime           = 0.1f;
    }

    void Blink::dump(IStateDumper *v) const
    {
        v->write("nCounter", nCounter);
        v->write("nTime", nTime);
        v->write("fOnValue", fOnValue);
        v->write("fOffValue", fOffValue);
        v->write("fTime", fTime);
    }
}


