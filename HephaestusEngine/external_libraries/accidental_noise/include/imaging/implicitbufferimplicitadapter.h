#ifndef IMPLICITBUFFERIMPLICITADAPTER_H
#define IMPLICITBUFFERIMPLICITADAPTER_H
#include "implicitbufferbase.h"
#include "../noise/implicitmodulebase.h"

namespace anl
{
    class CImplicitBufferImplicitAdapter : public CImplicitBufferBase
    {
    public:
        CImplicitBufferImplicitAdapter();
        CImplicitBufferImplicitAdapter(CImplicitModuleBase * src, int mapping=anl::SEAMLESS_NONE, SMappingRanges ranges=SMappingRanges(), bool use_z=false, ANLFloatType zvalue=0.0);
        ~CImplicitBufferImplicitAdapter();

        void setSource(CImplicitModuleBase * src);
        void setMapping(int mapping);
        void setRanges(SMappingRanges &ranges);
        void setUseZ(bool use_z);
        void setZ(ANLFloatType z);

        void get(CArray2Dd &out);
    private:
        CImplicitModuleBase * m_source;
        int m_mapping;
        SMappingRanges m_ranges;
        bool m_use_z;
        ANLFloatType m_z;
    };
};

#endif
