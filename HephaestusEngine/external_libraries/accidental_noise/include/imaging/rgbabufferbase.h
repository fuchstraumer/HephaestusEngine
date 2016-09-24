#ifndef RGBABUFFERBASE_H
#define RGBABUFFERBASE_H

#include <memory>
#include "../imaging/rgbabufferbase.h"
#include "mapping.h"

namespace anl
{
    class CRGBABufferBase
    {
        public:
        CRGBABufferBase(){};
        virtual ~CRGBABufferBase(){}

        virtual void get(CArray2Drgba &out)=0;
    };
};

#endif
