#include "footprints.hpp"

#include <iomanip>

#include <osg/PolygonOffset>
#include <osg/Texture2D>
#include <osg/Material>
#include <osg/Depth>
#include <osg/PositionAttitudeTransform>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>

#include <components/misc/rng.hpp>
#include <components/nifosg/controller.hpp>
#include <components/resource/imagemanager.hpp>
#include <components/resource/resourcesystem.hpp>
#include <components/resource/scenemanager.hpp>
#include <components/fallback/fallback.hpp>
#include <components/sceneutil/vismask.hpp>
#include <components/shader/shadervisitor.hpp>

#include "../mwbase/world.hpp"
#include "../mwbase/environment.hpp"

#include "../mwmechanics/actorutil.hpp"

namespace MWRender
{

Footprints::Footprints(osg::Group *parent, Resource::ResourceSystem* resourceSystem)
    : mParent(parent)
{
    int footprintDurationS = 60;

    mParticleSystem = new osgParticle::ParticleSystem;

    mParticleSystem->setParticleAlignment(osgParticle::ParticleSystem::FIXED);
    mParticleSystem->setAlignVectorX(osg::Vec3f(1,0,0));
    mParticleSystem->setAlignVectorY(osg::Vec3f(0,1,0));

    osgParticle::Particle& particleTemplate = mParticleSystem->getDefaultParticleTemplate();
    particleTemplate.setSizeRange(osgParticle::rangef(15, 15));
    particleTemplate.setColorRange(osgParticle::rangev4(osg::Vec4f(1,1,1,1), osg::Vec4f(1,1,1,1)));
    //particleTemplate.setAlphaRange(osgParticle::rangef(1.f, 0.f));
    particleTemplate.setAngularVelocity(osg::Vec3f(0,0,0));
    particleTemplate.setLifeTime(footprintDurationS);

    osg::ref_ptr<osgParticle::ParticleSystemUpdater> updater(new osgParticle::ParticleSystemUpdater);
    updater->addParticleSystem(mParticleSystem);

    mParticleNode = new osg::PositionAttitudeTransform;
    mParticleNode->setName("Footprints Root");
    mParticleNode->addChild(updater);
    mParticleNode->addChild(mParticleSystem);
    mParticleNode->setNodeMask(SceneUtil::Mask_Effect);

    osg::ref_ptr<osg::Texture2D> tex0 (
        new osg::Texture2D(resourceSystem->getImageManager()->getImage(
            "textures/footprint/footprint.png")));
        
    tex0->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    tex0->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    
    
    osg::ref_ptr<osg::StateSet> stateset (new osg::StateSet);
    stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
    stateset->setTextureAttributeAndModes(0, tex0, osg::StateAttribute::ON);
    
    osg::ref_ptr<osg::Depth> depth (new osg::Depth);
    depth->setWriteMask(false);
    stateset->setAttributeAndModes(depth, osg::StateAttribute::ON);

    osg::ref_ptr<osg::PolygonOffset> polygonOffset (new osg::PolygonOffset);
    polygonOffset->setUnits(-1);
    polygonOffset->setFactor(-1);
    stateset->setAttributeAndModes(polygonOffset, osg::StateAttribute::ON);
    
    stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);
    stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

    mParticleNode->setStateSet(stateset);

    resourceSystem->getSceneManager()->recreateShaders(mParticleNode);
    
    mParent->addChild(mParticleNode);
}

Footprints::~Footprints()
{
    mParent->removeChild(mParticleNode);
}

bool Footprints::toggle()
{
    if (mEnabled)
        disable();
    else
        enable();

    return mEnabled;
}

void Footprints::enable()
{
    mEnabled = true;
}

void Footprints::disable()
{
    mEnabled = false;
    for (int i=0; i<mParticleSystem->numParticles(); ++i)
        mParticleSystem->destroyParticle(i);
}

void Footprints::spawnFootprint(const osg::Vec3f& pos, float angle, int foot)
{
    if(!mEnabled)
        return;

    //Log(Debug::Info) << "Step: " << pos << " Rot: " << angle << " Foot: " << foot;
    
    osgParticle::Particle * p = mParticleSystem->createParticle(nullptr);
    p->setPosition(pos);
    p->setAngle(osg::Vec3f(0,0, angle + osg::DegreesToRadians(180.f)));
}

}
