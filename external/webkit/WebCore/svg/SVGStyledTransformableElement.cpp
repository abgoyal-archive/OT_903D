

#include "config.h"

#if ENABLE(SVG)
#include "SVGStyledTransformableElement.h"

#include "AffineTransform.h"
#include "Attr.h"
#include "MappedAttribute.h"
#include "RenderPath.h"
#include "SVGDocument.h"
#include "SVGStyledElement.h"
#include "SVGTransformList.h"

namespace WebCore {

SVGStyledTransformableElement::SVGStyledTransformableElement(const QualifiedName& tagName, Document* doc)
    : SVGStyledLocatableElement(tagName, doc)
    , SVGTransformable()
    , m_transform(SVGTransformList::create(SVGNames::transformAttr))
{
}

SVGStyledTransformableElement::~SVGStyledTransformableElement()
{
}

AffineTransform SVGStyledTransformableElement::getCTM() const
{
    return SVGTransformable::getCTM(this);
}

AffineTransform SVGStyledTransformableElement::getScreenCTM() const
{
    return SVGTransformable::getScreenCTM(this);
}

AffineTransform SVGStyledTransformableElement::animatedLocalTransform() const
{
    return m_supplementalTransform ? transform()->concatenate().matrix() * *m_supplementalTransform : transform()->concatenate().matrix();
}
    
AffineTransform* SVGStyledTransformableElement::supplementalTransform()
{
    if (!m_supplementalTransform)
        m_supplementalTransform.set(new AffineTransform());
    return m_supplementalTransform.get();
}

void SVGStyledTransformableElement::parseMappedAttribute(MappedAttribute* attr)
{
    if (SVGTransformable::isKnownAttribute(attr->name())) {
        SVGTransformList* localTransforms = transformBaseValue();
        if (!SVGTransformable::parseTransformAttribute(localTransforms, attr->value())) {
            ExceptionCode ec = 0;
            localTransforms->clear(ec);
        }
    } else 
        SVGStyledLocatableElement::parseMappedAttribute(attr);
}

void SVGStyledTransformableElement::synchronizeProperty(const QualifiedName& attrName)
{
    SVGStyledLocatableElement::synchronizeProperty(attrName);

    if (attrName == anyQName() || SVGTransformable::isKnownAttribute(attrName))
        synchronizeTransform();
}

bool SVGStyledTransformableElement::isKnownAttribute(const QualifiedName& attrName)
{
    return SVGTransformable::isKnownAttribute(attrName) ||
           SVGStyledLocatableElement::isKnownAttribute(attrName);
}

SVGElement* SVGStyledTransformableElement::nearestViewportElement() const
{
    return SVGTransformable::nearestViewportElement(this);
}

SVGElement* SVGStyledTransformableElement::farthestViewportElement() const
{
    return SVGTransformable::farthestViewportElement(this);
}

FloatRect SVGStyledTransformableElement::getBBox() const
{
    return SVGTransformable::getBBox(this);
}

RenderObject* SVGStyledTransformableElement::createRenderer(RenderArena* arena, RenderStyle*)
{
    // By default, any subclass is expected to do path-based drawing
    return new (arena) RenderPath(this);
}

Path SVGStyledTransformableElement::toClipPath() const
{
    Path pathData = toPathData();
    // FIXME: How do we know the element has done a layout?
    pathData.transform(animatedLocalTransform());
    return pathData;
}

}

#endif // ENABLE(SVG)
