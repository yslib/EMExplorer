#include <GL/glew.h>
#include "TF2DPrimitive.h"

TF2DPrimitive::TF2DPrimitive()
    : color(255, 255, 0, 255)
    , selected(false)
    , fuzziness(1.f)
    , cpSize(0.02f)
    , grabbed(-1)
    , scaleFactor(1.f)
{
}

TF2DPrimitive::TF2DPrimitive(Vector4f color_, float scaleFactor_)
    : color(color_)
    , selected(false)
    , fuzziness(1.f)
    , cpSize(0.02f)
    , grabbed(-1)
    , scaleFactor(scaleFactor_)
{
}

TF2DPrimitive::~TF2DPrimitive() 
{
}

float TF2DPrimitive::getScaleFactor() const
{
	return scaleFactor;
}

void TF2DPrimitive::setColor(const Vector4f& c) 
{
    color = c;
}

Vector4f TF2DPrimitive::getColor() const 
{
    return color;
}

void TF2DPrimitive::setFuzziness(float f) 
{
    fuzziness = f;
}

float TF2DPrimitive::getFuzziness() const 
{
    return fuzziness;
}

float TF2DPrimitive::getControlPointSize() const 
{
    return cpSize;
}

void TF2DPrimitive::select(Vector2f pos) 
{
    selected = true;
    selectControlPoint(pos);
}

void TF2DPrimitive::deselect() 
{
    selected = false;
    grabbed = -1;
}

void TF2DPrimitive::paintControlPoint(const Vector2f& v) 
{
    glTranslatef(v.x, v.y, 0.1f);
    Vector2f t;

    glBegin(GL_POLYGON);
    glColor4ub(150, 150, 150, 255);
    for (int i = 0; i < 20; ++i) {
        t.x = cosf((i / 20.f) * 2.f * MI_PI) * cpSize;
        t.y = sinf((i / 20.f) * 2.f * MI_PI) * cpSize;
        glVertex2f(t.x, t.y);
    }
    glEnd();

    glTranslatef(0.f, 0.f, 0.1f);

    glBegin(GL_LINE_LOOP);
    if (selected)
        glColor4ub(255, 255, 255, 255);
    else
        glColor4ub(128, 128, 128, 255);
    for (int i = 0; i < 20; ++i) {
        t.x = cosf((i / 20.f) * 2.f * MI_PI) * cpSize;
        t.y = sinf((i / 20.f) * 2.f * MI_PI) * cpSize;
		glVertex2f(t.x, t.y);
    }
    glEnd();

    glTranslatef(-v.x, -v.y, -0.2f);
}

void TF2DPrimitive::load(FILE *fp)
{
	fscanf(fp, "%f %f %f %f\n", &(color.x), &(color.y), &(color.z), &(color.w));
	fscanf(fp, "%f %f %f\n", &fuzziness, &cpSize, &scaleFactor);
}

void TF2DPrimitive::save(FILE *fp)
{
	fprintf(fp, "%f %f %f %f\n", color.x, color.y, color.z, color.w);
	fprintf(fp, "%f %f %f\n", fuzziness, cpSize, scaleFactor);
}


//-----------------------------------------------------------------------------

TransFuncQuad::TransFuncQuad()
    : TF2DPrimitive()
    , scaleCoords(true)
{
}

TransFuncQuad::TransFuncQuad(Vector2f center, float size, Vector4f col, float scaleFactor)
    : TF2DPrimitive(col, scaleFactor)
    , scaleCoords(true)
{
    size *= 0.5f;
    coords[0] = center + Vector2f(-size, -size);
    coords[1] = center + Vector2f( size, -size);
    coords[2] = center + Vector2f( size,  size);
    coords[3] = center + Vector2f(-size,  size);
}

TransFuncQuad::~TransFuncQuad() 
{
}

void TransFuncQuad::setScaleFactor(float scaleFactor_) 
{
    scaleFactor = scaleFactor_;
    // adjust coords
    for (int i = 0; i < 4; ++i)
        coords[i].y = Clamp(coords[i].y / scaleFactor, 0.f, 1.f);
}

void TransFuncQuad::paint() 
{
    Vector2f coords[4];
    if (scaleCoords) {
        coords[0] = Vector2f(this->coords[0].x, scaleFactor * this->coords[0].y);
        coords[1] = Vector2f(this->coords[1].x, scaleFactor * this->coords[1].y);
        coords[2] = Vector2f(this->coords[2].x, scaleFactor * this->coords[2].y);
        coords[3] = Vector2f(this->coords[3].x, scaleFactor * this->coords[3].y);
    }
    else {
        coords[0] = this->coords[0];
        coords[1] = this->coords[1];
        coords[2] = this->coords[2];
        coords[3] = this->coords[3];
    }

    Vector2f center = coords[0] + coords[1] + coords[2] + coords[3];
    center /= 4.f;

    glTranslatef(0.f, 0.f, -0.5f);
    glBegin(GL_QUADS);

        for (int i = 1; i <= 4; ++i) {
            glColor4f(color.x, color.y, color.z, 0.f);
            glVertex2f(coords[i-1].x, coords[i-1].y);
            glVertex2f(coords[i%4].x, coords[i%4].y);
            glColor4f(color.x, color.y, color.z, color.w);
			Vector2f p1 = coords[i%4] * fuzziness + center * (1.f - fuzziness);
			Vector2f p2 = coords[i-1] * fuzziness + center * (1.f - fuzziness);
            glVertex2f(p1.x, p1.y);
            glVertex2f(p2.x, p2.y);
        }
		
		Vector2f p1 = coords[0] * fuzziness + center * (1.f - fuzziness);
		Vector2f p2 = coords[1] * fuzziness + center * (1.f - fuzziness);
		Vector2f p3 = coords[2] * fuzziness + center * (1.f - fuzziness);
		Vector2f p4 = coords[3] * fuzziness + center * (1.f - fuzziness);
        glVertex2f(p1.x, p1.y);
        glVertex2f(p2.x, p2.y);
        glVertex2f(p3.x, p3.y);
        glVertex2f(p4.x, p4.y);

    glEnd();
    glTranslatef(0.f, 0.f, 0.5f);
}

void TransFuncQuad::paintForSelection(unsigned char id)
{
    glBegin(GL_QUADS);
        glColor3ub(id, 123, 123);
        glVertex2f(coords[0].x, coords[0].y);
        glVertex2f(coords[1].x, coords[1].y);
        glVertex2f(coords[2].x, coords[2].y);
        glVertex2f(coords[3].x, coords[3].y);
    glEnd();
}

void TransFuncQuad::paintInEditor() 
{
    scaleCoords = false;
    paint();
    scaleCoords = true;

    glBegin(GL_LINE_LOOP);
        if (selected)
            glColor4ub(255, 255, 255, 255);
        else
            glColor4ub(128, 128, 128, 255);
        glVertex2f(coords[0].x, coords[0].y);
        glVertex2f(coords[1].x, coords[1].y);
        glVertex2f(coords[2].x, coords[2].y);
        glVertex2f(coords[3].x, coords[3].y);
    glEnd();

    paintControlPoint(coords[0]);
    paintControlPoint(coords[1]);
    paintControlPoint(coords[2]);
    paintControlPoint(coords[3]);
}

float TransFuncQuad::getClosestControlPointDist(Vector2f pos) 
{
    float min = Distance(pos, coords[0]);
    float d;
    for (int i = 1; i < 4; ++i) {
        d = Distance(pos, coords[i]);
        if (d < min)
            min = d;
    }
    return min;
}

bool TransFuncQuad::selectControlPoint(Vector2f pos) 
{
    grabbed = -1;
    int n = 0;
    float min = Distance(pos, coords[0]);
    float d;
    for (int i = 1; i < 4; ++i) {
        d = Distance(pos, coords[i]);
        if (d < min) {
            min = d;
            n = i;
        }
    }
    if (min < cpSize) {
        grabbed = n;
        return true;
    }
    return false;
}

bool TransFuncQuad::move(Vector2f offset) 
{
    // only move the control point when one is grabbed
    if (grabbed > -1) {
        Vector2f temp = coords[grabbed] + offset;
        // do not move control point when it is outside of allowed region
        if ((temp.x < 0.f) || (temp.x > 1.f) ||
            (temp.y < 0.f) || (temp.y > 1.f))
        {
            return false;
        }
        else
            coords[grabbed] += offset;
    }
    else {
        for (int i = 0; i < 4; ++i) {
            Vector2f temp = coords[i] + offset;
            // do not move primitive when one point is outside of allowed region
            if ((temp.x < 0.f) || (temp.x > 1.f) ||
                (temp.y < 0.f) || (temp.y > 1.f))
            {
                return false;
            }
        }
        for (int i = 0; i < 4; ++i)
            coords[i] += offset;
    }

    return true;
}

void TransFuncQuad::load(FILE *fp)
{
	TF2DPrimitive::load(fp);
	fscanf(fp, "%f %f\n", &(coords[0].x), &(coords[0].y));
	fscanf(fp, "%f %f\n", &(coords[1].x), &(coords[1].y));
	fscanf(fp, "%f %f\n", &(coords[2].x), &(coords[2].y));
	fscanf(fp, "%f %f\n", &(coords[3].x), &(coords[3].y));
}

void TransFuncQuad::save(FILE *fp)
{
	TF2DPrimitive::save(fp);
	fprintf(fp, "%f %f\n", coords[0].x, coords[0].y);
	fprintf(fp, "%f %f\n", coords[1].x, coords[1].y);
	fprintf(fp, "%f %f\n", coords[2].x, coords[2].y);
	fprintf(fp, "%f %f\n", coords[3].x, coords[3].y);
}

//-----------------------------------------------------------------------------

TransFuncBanana::TransFuncBanana()
    : TF2DPrimitive()
    , steps(20)
    , scaleCoords(true)
{
}

TransFuncBanana::TransFuncBanana(Vector2f a, Vector2f b1, Vector2f b2, Vector2f c, Vector4f col, float scaleFactor)
    : TF2DPrimitive(col, scaleFactor)
    , steps(20)
    , scaleCoords(true)
{
    coords[0] = a;
    coords[1] = b1;
    coords[2] = b2;
    coords[3] = c;
}

TransFuncBanana::~TransFuncBanana() 
{
}

void TransFuncBanana::setScaleFactor(float scaleFactor) 
{
    scaleFactor = scaleFactor;
    // adjust coords
    for (int i = 0; i < 4; ++i)
        coords[i].y = Clamp(coords[i].y / scaleFactor, 0.f, 1.f);
}

void TransFuncBanana::paint() 
{
    glTranslatef(0.f, 0.f, -0.5f);
	glColor4f(color.x, color.y, color.z, color.w);
    paintInner();
    glTranslatef(0.f, 0.f, 0.5f);
}

void TransFuncBanana::paintForSelection(unsigned char id) 
{
    glColor3ub(id, 123, 123);
    float t;
    Vector2f v1, v2, t1, t2, t3, t4, tc;
    t1 = (2.f * coords[1]) - (0.5f * coords[0]) - (0.5f * coords[3]);
    t2 = (2.f * coords[2]) - (0.5f * coords[0]) - (0.5f * coords[3]);

    // fill the space between the two bezier curves:
    glBegin(GL_TRIANGLE_STRIP);
	glVertex2f(coords[0].x, coords[0].y);
    for (int i = 0; i < steps; ++i) {
        t = i / static_cast<float>(steps - 1);
        v1 = (((1 - t) * (1 - t)) * coords[0]) + ((2 * (1 - t) * t) * t1) + ((t * t) * coords[3]);
        v2 = (((1 - t) * (1 - t)) * coords[0]) + ((2 * (1 - t) * t) * t2) + ((t * t) * coords[3]);
        glVertex2f(v1.x, v1.y);
		glVertex2f(v2.x, v2.y);
    }
	glVertex2f(coords[3].x, coords[3].y);
    glEnd();
}

void TransFuncBanana::paintInner()
{
    float t;
    Vector2f v1, v2, t1, t2, t3, t4, tc;

    Vector2f coords[4];
    if (scaleCoords) {
        coords[0] = Vector2f(this->coords[0].x, scaleFactor * this->coords[0].y);
        coords[1] = Vector2f(this->coords[1].x, scaleFactor * this->coords[1].y);
        coords[2] = Vector2f(this->coords[2].x, scaleFactor * this->coords[2].y);
        coords[3] = Vector2f(this->coords[3].x, scaleFactor * this->coords[3].y);
    }
    else {
        coords[0] = this->coords[0];
        coords[1] = this->coords[1];
        coords[2] = this->coords[2];
        coords[3] = this->coords[3];
    }

    t1 = (2.f * coords[1]) - (0.5f * coords[0]) - (0.5f * coords[3]);
    t2 = (2.f * coords[2]) - (0.5f * coords[0]) - (0.5f * coords[3]);

    tc = (t1 + t2) / 2.f;
    t3 = fuzziness * t1 + (1.f - fuzziness) * tc;
    t4 = fuzziness * t2 + (1.f - fuzziness) * tc;

    // fill the space between the two bezier curves:
    glBegin(GL_TRIANGLE_STRIP);
    glColor4f(color.x, color.y, color.z, color.w);
    glVertex2f(coords[0].x, coords[0].y);
    for (int i = 0; i < steps; ++i) {
        t = i / static_cast<float>(steps - 1);
        v1 = (((1 - t) * (1 - t)) * coords[0]) + ((2 * (1 - t) * t) * t1) + ((t * t) * coords[3]);
        v2 = (((1 - t) * (1 - t)) * coords[0]) + ((2 * (1 - t) * t) * t3) + ((t * t) * coords[3]);
        glColor4f(color.x, color.y, color.z, 0.f);
        glVertex2f(v1.x, v1.y);
        glColor4f(color.x, color.y, color.z, color.w);
        glVertex2f(v2.x, v2.y);
    }
    glVertex2f(coords[3].x, coords[3].y);

    glColor4f(color.x, color.y, color.z, color.w);
    glVertex2f(coords[0].x, coords[0].y);
    for (int i = 0; i < steps; ++i) {
        t = i / static_cast<float>(steps - 1);
        v1 = (((1 - t) * (1 - t)) * coords[0]) + ((2 * (1 - t) * t) * t3) + ((t * t) * coords[3]);
        v2 = (((1 - t) * (1 - t)) * coords[0]) + ((2 * (1 - t) * t) * t4) + ((t * t) * coords[3]);
        glVertex2f(v1.x, v1.y);
        glVertex2f(v2.x, v2.y);
    }
    glVertex2f(coords[3].x, coords[3].y);

    glVertex2f(coords[0].x, coords[0].y);
    for (int i = 0; i < steps; ++i) {
        t = i / static_cast<float>(steps - 1);
        v1 = (((1 - t) * (1 - t)) * coords[0]) + ((2 * (1 - t) * t) * t4) + ((t * t) * coords[3]);
        v2 = (((1 - t) * (1 - t)) * coords[0]) + ((2 * (1 - t) * t) * t2) + ((t * t) * coords[3]);
        glColor4f(color.x, color.y, color.z, color.w);
        glVertex2f(v1.x, v1.y);
        glColor4f(color.x, color.y, color.z, 0.f);
        glVertex2f(v2.x, v2.y);
    }
    glVertex2f(coords[3].x, coords[3].y);

    glEnd();
}

void TransFuncBanana::paintInEditor()
{
    scaleCoords = false;
    paint();
    scaleCoords = true;

    float t;
    Vector2f v, t1, t2;
    t1 = (2.f * coords[1]) - (0.5f * coords[0]) - (0.5f * coords[3]);
    t2 = (2.f * coords[2]) - (0.5f * coords[0]) - (0.5f * coords[3]);

    // draw outer line of double bezier curve:
    glBegin(GL_LINE_LOOP);
        if (selected)
            glColor4ub(255, 255, 255, 255);
        else
            glColor4ub(128, 128, 128, 255);

        for (int i = 0; i < steps; ++i) {
            t = i / static_cast<float>(steps - 1);
            v = (((1 - t) * (1 - t)) * coords[0]) + ((2 * (1 - t) * t) * t1) + ((t * t) * coords[3]);
            glVertex2f(v.x, v.y);
        }

        for (int i = 0; i < steps; ++i) {
            t = 1.f - (i / static_cast<float>(steps - 1));
            v = (((1 - t) * (1 - t)) * coords[0]) + ((2 * (1 - t) * t) * t2) + ((t * t) * coords[3]);
            glVertex2f(v.x, v.y);
        }
    glEnd();

    paintControlPoint(coords[0]);
    paintControlPoint(coords[1]);
    paintControlPoint(coords[2]);
    paintControlPoint(coords[3]);
}

float TransFuncBanana::getClosestControlPointDist(Vector2f pos)
{
    float min = Distance(pos, coords[0]);
    float d;
    for (int i = 1; i < 4; ++i) {
        d = Distance(pos, coords[i]);
        if (d < min)
            min = d;
    }
    return min;
}

bool TransFuncBanana::selectControlPoint(Vector2f pos) 
{
    grabbed = -1;
    int n = 0;
    float min = Distance(pos, coords[0]);
    float d;
    for (int i = 1; i < 4; ++i) {
        d = Distance(pos, coords[i]);
        if (d < min) {
            min = d;
            n = i;
        }
    }
    if (min < cpSize) {
        grabbed = n;
        return true;
    }
    return false;
}

bool TransFuncBanana::move(Vector2f offset) 
{
    if (grabbed > -1) {
        Vector2f temp = coords[grabbed] + offset;
        // do not move control point when it is outside of allowed region
        if ((temp.x < 0.f) || (temp.x > 1.f) ||
            (temp.y < 0.f) || (temp.y > 1.f))
        {
            return false;
        }
        else
            coords[grabbed] += offset;
    }
    else {
        for (int i = 0; i < 4; ++i) {
            Vector2f temp = coords[i] + offset;
            //do not move primitive when one point is outside of allowed region
            if ((temp.x < 0.f) || (temp.x > 1.f) ||
                (temp.y < 0.f) || (temp.y > 1.f))
            {
                return false;
            }
        }
        for (int i = 0; i < 4; ++i)
            coords[i] += offset;
    }

    return true;
}

void TransFuncBanana::load(FILE *fp)
{
	TF2DPrimitive::load(fp);
	fscanf(fp, "%f %f\n", &(coords[0].x), &(coords[0].y));
	fscanf(fp, "%f %f\n", &(coords[1].x), &(coords[1].y));
	fscanf(fp, "%f %f\n", &(coords[2].x), &(coords[2].y));
	fscanf(fp, "%f %f\n", &(coords[3].x), &(coords[3].y));
}

void TransFuncBanana::save(FILE *fp)
{
	TF2DPrimitive::save(fp);
	fprintf(fp, "%f %f\n", coords[0].x, coords[0].y);
	fprintf(fp, "%f %f\n", coords[1].x, coords[1].y);
	fprintf(fp, "%f %f\n", coords[2].x, coords[2].y);
	fprintf(fp, "%f %f\n", coords[3].x, coords[3].y);
}
