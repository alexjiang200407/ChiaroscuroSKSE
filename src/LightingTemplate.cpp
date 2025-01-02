#include "LightingTemplate.h"

LightingTemplateData::LightingTemplateData(PresetID id, std::string name, LightFlags flags) :
	Preset(PresetTID::kLightTemplate, id, name), flags(flags)
{
}

RE::ShadowSceneNode::LIGHT_CREATE_PARAMS LightingTemplateData::ToLightCreateParams() const
{
	using FLAGS = LightingTemplateData::Flags;
	RE::ShadowSceneNode::LIGHT_CREATE_PARAMS params{};

	params.affectLand      = flags.all(FLAGS::kAffectLand);
	params.affectWater     = flags.all(FLAGS::kAffectWater);
	params.depthBias       = depthBias;
	params.dynamic         = flags.all(FLAGS::kDynamic);
	params.falloff         = falloff;
	params.fov             = fov;
	params.lensFlareData   = nullptr;
	params.nearDistance    = nearDistance;
	params.neverFades      = flags.all(FLAGS::kNeverFades);
	params.portalStrict    = flags.all(FLAGS::kPortalStrict);
	params.restrictedNode  = nullptr;
	params.shadowLight     = flags.all(FLAGS::kCastsShadow);
	params.sceneGraphIndex = 0;

	return params;
}

LightingTemplateData::operator RE::ShadowSceneNode::LIGHT_CREATE_PARAMS() const
{
	return ToLightCreateParams();
}
