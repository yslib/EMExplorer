#include "volume.h"


Volume::Volume(const void* data, int xSize, int ySize, int zSize, const VolumeFormat& fmt) :
	m_xSize(xSize)
	, m_ySize(ySize)
	, m_zSize(zSize)
	, m_fmt(fmt)
	, m_data(nullptr)
{

	int voxelChannel = 0;

	switch (m_fmt.fmt) {
		case VoxelFormat::Grayscale:voxelChannel = 1; break;
		case VoxelFormat::RGB:voxelChannel = 3; break;
		case VoxelFormat::RGBA:voxelChannel = 4; break;
	}
	size_t bytes = 0;
	switch(m_fmt.type) {
		case VoxelType::UInt8:
			m_data.reset(new unsigned char[xSize*ySize*zSize*voxelChannel]);
			bytes = xSize * ySize*zSize * sizeof(unsigned char)*voxelChannel; 
			break;
		case VoxelType::Float32:
			m_data.reset(new float[xSize*ySize*zSize*voxelChannel]);
			bytes = xSize * ySize*zSize * sizeof(float)*voxelChannel;
			break;
	}
	if(m_data != nullptr) {
		std::memcpy(m_data.get(), data, bytes);
	}
}



const static int faceIndex[] = { 1, 3, 7, 5, 0, 4, 6, 2, 2, 6, 7, 3, 0, 1, 5, 4, 4, 5, 7, 6, 0, 2, 3, 1 };


static const float xCoord = 1.0, yCoord = 1.0, zCoord = 1.0;
float positionVert[] = {
	0,0,0,
	xCoord, 0, 0 ,
	0, yCoord, 0 ,
	xCoord, yCoord, 0 ,
	0, 0, zCoord ,
	xCoord, 0, zCoord ,
	0, yCoord, zCoord ,
	xCoord, yCoord, zCoord ,
};

void SliceVolume::loadVolumeData() {

	if (m_gradCalc.hasData() == false)
		return;
	if (m_gradCalc.ready() == false)
		m_gradCalc.calcGradent();		//Time-consuming

	const auto d = m_gradCalc.data3();
	const auto z = zLength();
	const auto y = yLength();
	const auto x = xLength();

	auto fmt = format();



	m_gradientTexture.destroy();
	m_gradientTexture.setMagnificationFilter(QOpenGLTexture::Linear);
	m_gradientTexture.setMinificationFilter(QOpenGLTexture::Linear);
	m_gradientTexture.setWrapMode(QOpenGLTexture::ClampToEdge);
	m_gradientTexture.setSize(x, y, z);

	m_gradientTexture.setFormat(QOpenGLTexture::RGB8_UNorm);


	m_gradientTexture.allocateStorage();
	m_gradientTexture.setData(QOpenGLTexture::RGB, QOpenGLTexture::UInt8, d);

	m_volumeTexture.destroy();
	m_volumeTexture.setMagnificationFilter(QOpenGLTexture::Linear);
	m_volumeTexture.setMinificationFilter(QOpenGLTexture::Linear);
	m_volumeTexture.setWrapMode(QOpenGLTexture::ClampToEdge);
	m_volumeTexture.setSize(x, y, z);
	m_volumeTexture.setFormat(QOpenGLTexture::R16F);
	m_volumeTexture.allocateStorage();
	m_volumeTexture.setData(QOpenGLTexture::Red, QOpenGLTexture::UInt8, data());

}

SliceVolume::SliceVolume(const void* data, int xSize, int ySize, int zSize, const VolumeFormat& fmt):GPUVolume(data,xSize,ySize,zSize,fmt)
,m_fbo(nullptr)
,m_gradientTexture(QOpenGLTexture::Target3D)
,m_volumeTexture(QOpenGLTexture::Target3D)
,m_positionVBO(QOpenGLBuffer::VertexBuffer)
,m_positionEBO(QOpenGLBuffer::IndexBuffer)
,m_gradCalc(nullptr,nullptr,nullptr)
{
	initializeGLResources();

}

bool SliceVolume::initializeGLResources() {

	if(initializeOpenGLFunctions() == false)
		return false;
	// Initialize Front and back face texture
	m_positionShader.reset(new PositionShader);
	m_positionShader->link();
	Q_ASSERT_X(m_positionShader->isLinked(), "VolumeWidget::initializeGL", "positionShader linking failed.");
	m_positionVAO.create();
	Q_ASSERT_X(m_positionVAO.isCreated(), "VolumeWidget::initializeGL", "VAO is not created.");
	QOpenGLVertexArrayObject::Binder positionVAOBinder(&m_positionVAO);
	m_positionVBO.create();
	m_positionVBO.bind();
	m_positionVBO.allocate(positionVert, sizeof(positionVert));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<void*>(0));	//vertex
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<void*>(0));	//texture coord
	m_positionShader->bind();
	m_positionEBO.create();
	m_positionEBO.bind();
	m_positionEBO.allocate(faceIndex, sizeof(faceIndex));
	m_positionVBO.release();

	// Initilize ray casting shader
	m_currentShader.reset(new RayCastingShader);
	m_currentShader->link();
	Q_ASSERT_X(m_currentShader->isLinked(), "VolumeWidget::initializeGL", "currentShader linking failed.");
	m_rayCastingTextureVAO.create();
	QOpenGLVertexArrayObject::Binder binder(&m_rayCastingTextureVAO);
	m_rayCastingTextureVBO.create();
	m_rayCastingTextureVBO.bind();
	m_currentShader->bind();
	m_rayCastingTextureVBO.allocate(8 * sizeof(GLfloat));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), reinterpret_cast<void*>(0));
	m_rayCastingTextureVBO.release();

	m_sliceShader.reset(new SliceShader());
	m_sliceShader->link();

	m_topSliceVAO.create();
	QOpenGLVertexArrayObject::Binder binder1(&m_topSliceVAO);
	m_topSliceVBO.create();
	m_topSliceVBO.bind();
	m_topSliceVBO.allocate(4 * 3 * sizeof(float) + 4 * 2 * sizeof(float));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D) + sizeof(QVector2D), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QVector3D) + sizeof(QVector2D), reinterpret_cast<void*>(3 * sizeof(float)));

	m_rightSliceVAO.create();
	QOpenGLVertexArrayObject::Binder binder2(&m_rightSliceVAO);
	m_rightSliceVBO.create();
	m_rightSliceVBO.bind();
	m_rightSliceVBO.allocate(4 * 3 * sizeof(float) + 4 * 2 * sizeof(float));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D) + sizeof(QVector2D), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QVector3D) + sizeof(QVector2D), reinterpret_cast<void*>(3 * sizeof(float)));

	m_frontSliceVAO.create();
	QOpenGLVertexArrayObject::Binder binder3(&m_frontSliceVAO);
	m_frontSliceVBO.create();
	m_frontSliceVBO.bind();
	m_frontSliceVBO.allocate(4 * 3 * sizeof(float) + 4 * 2 * sizeof(float));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D) + sizeof(QVector2D), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QVector3D) + sizeof(QVector2D), reinterpret_cast<void*>(3 * sizeof(float)));

}

void SliceVolume::destoryGLResources() 
{
	m_positionVAO.destroy();
	m_positionVBO.destroy();
	m_positionEBO.destroy();
	m_rayCastingTextureVAO.destroy();
	m_rayCastingTextureVBO.destroy();

	m_topSliceVAO.destroy();
	m_topSliceVBO.destroy();
	m_rightSliceVAO.destroy();
	m_rightSliceVBO.destroy();
	m_frontSliceVAO.destroy();
	m_frontSliceVBO.destroy();

	m_gradientTexture.destroy();
	m_fbo.reset();
}
bool SliceVolume::render() {
	
}
