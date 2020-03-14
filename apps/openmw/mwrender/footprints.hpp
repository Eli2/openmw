#ifndef OPENMW_MWRENDER_FOOTPRINTS_H
#define OPENMW_MWRENDER_FOOTPRINTS_H

#include <osg/ref_ptr>

#include "../mwworld/ptr.hpp"

namespace osg
{
    class Group;
    class PositionAttitudeTransform;
}

namespace osgParticle
{
    class ParticleSystem;
}

namespace Resource
{
    class ResourceSystem;
}

namespace Fallback
{
    class Map;
}

namespace MWRender
{
    class Footprints
    {
        osg::ref_ptr<osg::Group> mParent;
        osg::ref_ptr<osgParticle::ParticleSystem> mParticleSystem;
        osg::ref_ptr<osg::PositionAttitudeTransform> mParticleNode;
        bool mEnabled;
    
    public:
        Footprints(osg::Group* parent, Resource::ResourceSystem* resourceSystem);
        ~Footprints();
        
        bool toggle();
        void enable();
        void disable();
        
        void spawnFootprint(const osg::Vec3f& pos, float angle, int foot);
    };
}

#endif
