#include "Box2D/Box2D.h"
#include "Box2D/Collision/b2Distance.h"

#include "cpgf/metadata/box2d/meta_b2Distance.h"

#include "cpgf/gmetapolicy.h"
#include "cpgf/goutmain.h"

using namespace cpgf;

namespace {

G_AUTO_RUN_BEFORE_MAIN()
{
    {
    GDefineMetaClass<void> _d = GDefineMetaClass<void>::define("box2d");
    buildMetaClass_Global_b2distance(0, _d, NULL, GMetaPolicyCopyAllConstReference());
    }
    

    {
    GDefineMetaClass<void> _ns = GDefineMetaClass<void>::define("box2d");
    GDefineMetaClass<b2DistanceInput> _d = GDefineMetaClass<b2DistanceInput>::declare("b2DistanceInput");
    _ns._class(_d);
    buildMetaClass_B2DistanceInput(0, _d, NULL, GMetaPolicyCopyAllConstReference());
    }
    

    {
    GDefineMetaClass<void> _ns = GDefineMetaClass<void>::define("box2d");
    GDefineMetaClass<b2DistanceOutput> _d = GDefineMetaClass<b2DistanceOutput>::declare("b2DistanceOutput");
    _ns._class(_d);
    buildMetaClass_B2DistanceOutput(0, _d, NULL, GMetaPolicyCopyAllConstReference());
    }
    

    {
    GDefineMetaClass<void> _ns = GDefineMetaClass<void>::define("box2d");
    GDefineMetaClass<b2DistanceProxy> _d = GDefineMetaClass<b2DistanceProxy>::declare("b2DistanceProxy");
    _ns._class(_d);
    buildMetaClass_B2DistanceProxy(0, _d, NULL, GMetaPolicyCopyAllConstReference());
    }
    

    {
    GDefineMetaClass<void> _ns = GDefineMetaClass<void>::define("box2d");
    GDefineMetaClass<b2SimplexCache> _d = GDefineMetaClass<b2SimplexCache>::declare("b2SimplexCache");
    _ns._class(_d);
    buildMetaClass_B2SimplexCache(0, _d, NULL, GMetaPolicyCopyAllConstReference());
    }
    

}

} // unnamed namespace
