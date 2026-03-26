#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/BlendFunc>
#include <osg/PositionAttitudeTransform>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>

#include <cmath>

USE_OSGPLUGIN(osg2)
USE_SERIALIZER_WRAPPER_LIBRARY(osg)
USE_SERIALIZER_WRAPPER_LIBRARY(osgSim)
USE_COMPRESSOR_WRAPPER(ZLibCompressor)
USE_GRAPHICSWINDOW()

#define TRANS 1.0f
#define WHITE 1.0f, 1.0f, 1.0f
#define GRAY 0.3f, 0.3f, 0.3f
#define WIDTH 5.5
#define HEIGHT 2.5
#define EXTRUSION 1.0
#define FAN_VERT 4

int main(int argc, char** argv)
{
	const double W = WIDTH / 2.0; // Total width 5
	const double H = HEIGHT / 2.0; // Total height 2.5

	const int vert_size = 4 + 4 * 2 + 4 * (FAN_VERT + 1); // Rect + 4 extrusions + 4 fans surfaces (1 surface = 2 points)
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(vert_size);

	// Bottom (B) Top (T) Left (L) Right (R)
	// Inner rectangle
	(*vertices)[0].set(-W, 0.0, -H); // BL
	(*vertices)[1].set(W, 0.0, -H); // BR
	(*vertices)[2].set(W, 0.0, H); // TR
	(*vertices)[3].set(-W, 0.0, H); // TL

	// Extrusion bottom
	(*vertices)[4].set(-W, 0.0, -H - EXTRUSION); // L
	(*vertices)[5].set(W, 0.0, -H - EXTRUSION); // R

	// Extrusion right
	(*vertices)[6].set(W + EXTRUSION, 0.0, -H); // B
	(*vertices)[7].set(W + EXTRUSION, 0.0, H); // T

	// Extrusion top
	(*vertices)[8].set(-W, 0.0, H + EXTRUSION); // L
	(*vertices)[9].set(W, 0.0, H + EXTRUSION); // R

	// Extrusion left
	(*vertices)[10].set(-W - EXTRUSION, 0.0, -H); // B
	(*vertices)[11].set(-W - EXTRUSION, 0.0, H); // T

	int fans_start_idx = 12;
	auto makeExtrusionFan = [&](double start_angle, double corner_x, double corner_y, int fan_offset)
	{
		for (int i = 0; i <= FAN_VERT; i++)
		{
			double angle = start_angle + (i / static_cast<double>(FAN_VERT)) * (osg::PI / 2.0);
			(*vertices)[fans_start_idx + fan_offset + i].set(corner_x + cos(angle) * EXTRUSION, 0.0, corner_y + sin(angle) * EXTRUSION);
		}
	};

	makeExtrusionFan(osg::PI, -W, -H, 0); // BL
	makeExtrusionFan(3.0 * osg::PI / 2.0, W, -H, FAN_VERT + 1); // BR
	makeExtrusionFan(0.0, W, H, 2 * (FAN_VERT + 1)); // TR
	makeExtrusionFan(osg::PI / 2.0, -W, H, 3 * (FAN_VERT + 1)); // TL

	// Normals
	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0.0f, -1.0f, 0.0f));

	// Colors
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(vert_size);
	for (int i = 0; i < 4; i++)
	{
		(*colors)[i].set(WHITE, TRANS);
	}

	for (int i = 4; i < vertices->size(); i++)
	{
		(*colors)[i].set(WHITE, 0.0f);
	}

	// Base geometry
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
	geometry->setVertexArray(vertices.get());
	geometry->setNormalArray(normals.get());
	geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
	geometry->setColorArray(colors.get());
	geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	// Create the shapes
	osg::ref_ptr<osg::DrawElementsUInt> center = new osg::DrawElementsUInt(GL_QUADS, 4);
	(*center)[0] = 0;
	(*center)[1] = 1;
	(*center)[2] = 2;
	(*center)[3] = 3;
	geometry->addPrimitiveSet(center.get());

	osg::ref_ptr<osg::DrawElementsUInt> bottom = new osg::DrawElementsUInt(GL_QUADS, 4);
	(*bottom)[0] = 0;
	(*bottom)[1] = 1;
	(*bottom)[2] = 5;
	(*bottom)[3] = 4;
	geometry->addPrimitiveSet(bottom.get());

	osg::ref_ptr<osg::DrawElementsUInt> right = new osg::DrawElementsUInt(GL_QUADS, 4);
	(*right)[0] = 1;
	(*right)[1] = 2;
	(*right)[2] = 7;
	(*right)[3] = 6;
	geometry->addPrimitiveSet(right.get());

	osg::ref_ptr<osg::DrawElementsUInt> top = new osg::DrawElementsUInt(GL_QUADS, 4);
	(*top)[0] = 2;
	(*top)[1] = 3;
	(*top)[2] = 8;
	(*top)[3] = 9;
	geometry->addPrimitiveSet(top.get());

	osg::ref_ptr<osg::DrawElementsUInt> left = new osg::DrawElementsUInt(GL_QUADS, 4);
	(*left)[0] = 3;
	(*left)[1] = 0;
	(*left)[2] = 10;
	(*left)[3] = 11;
	geometry->addPrimitiveSet(left.get());

	auto makeFan = [&](int base_corner_idx, int start_offset)
	{
		osg::ref_ptr<osg::DrawElementsUInt> fan = new osg::DrawElementsUInt(GL_TRIANGLE_FAN, FAN_VERT + 2); // anchor corner + vertices
		(*fan)[0] = base_corner_idx; // The anchor
		for (int i = 0; i <= FAN_VERT; i++)
		{
			(*fan)[i + 1] = start_offset + i;
		}
		geometry->addPrimitiveSet(fan.get());
	};

	makeFan(0, fans_start_idx);
	makeFan(1, fans_start_idx + FAN_VERT + 1);
	makeFan(2, fans_start_idx + 2 * (FAN_VERT + 1));
	makeFan(3, fans_start_idx + 3 * (FAN_VERT + 1));

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(geometry.get());

	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(geode.get());

	// Transparancy
	osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
	blendFunc->setFunction( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	root->getOrCreateStateSet()->setAttributeAndModes(blendFunc);
	root->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	osgViewer::Viewer viewer;
	osg::DisplaySettings::instance()->setNumMultiSamples(4);
    viewer.setUpViewInWindow(100, 100, 800, 400);
	viewer.setSceneData(root.get());

	viewer.addEventHandler(new osgViewer::StatsHandler);

	osg::ref_ptr<osgGA::StateSetManipulator> statesetManipulator =
		new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet());
	viewer.addEventHandler(statesetManipulator.get());

	return viewer.run();
}